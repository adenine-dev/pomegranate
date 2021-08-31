#include "pch.hpp"

#include "pipelineLayoutVk.hpp"

#include "bufferVk.hpp"
#include "gfxVk.hpp"
#include "instanceVk.hpp"
#include "textureVk.hpp"

namespace pom::gfx {
    PipelineLayoutVk::PipelineLayoutVk(InstanceVk* instance, std::initializer_list<Descriptor> descriptors) :
        instance(instance)
    {
        std::unordered_map<DescriptorType, u32> descriptorTypes;
        std::unordered_map<u32, std::vector<VkDescriptorSetLayoutBinding>> descriptorSetBindings;

        for (const auto& descriptor : descriptors) {
            descriptorTypes[descriptor.type] += descriptor.count;
            POM_DEBUG(toVkShaderStageFlags(descriptor.stages));
            descriptorSetBindings[descriptor.set].push_back({
                .binding = descriptor.binding,
                .descriptorType = toVkDescriptorType(descriptor.type),
                .descriptorCount = descriptor.count,
                .stageFlags = toVkShaderStageFlags(descriptor.stages),
                .pImmutableSamplers = nullptr,
            });
        }

        VkDescriptorPoolSize poolSizes[] = {
            {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = POM_MAX_FRAMES_IN_FLIGHT * descriptorTypes[DescriptorType::UNIFORM_BUFFER],
            },
            {
                .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = POM_MAX_FRAMES_IN_FLIGHT * descriptorTypes[DescriptorType::COMBINED_TEXTURE_SAMPLER],
            },
        };

        VkDescriptorPoolCreateInfo poolCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            // NOTE: idk how to fix it but this is prob dumb to use
            .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
            // FIXME: more than 2 descriptor sets will be bound.
            .maxSets = static_cast<u32>(POM_MAX_FRAMES_IN_FLIGHT * descriptorSetBindings.size()),
            .poolSizeCount = 2,
            .pPoolSizes = poolSizes,
        };

        POM_CHECK_VK(vkCreateDescriptorPool(instance->device, &poolCreateInfo, nullptr, &pool),
                     "could not create descriptor pool.");

        std::vector<VkDescriptorSetLayout> descSetLayouts(descriptorSetBindings.size());
        for (u32 i = 0; i < descriptorSetBindings.size(); i++) {
            VkDescriptorSetLayoutCreateInfo descSetLayoutCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .bindingCount = static_cast<u32>(descriptorSetBindings[i].size()),
                .pBindings = descriptorSetBindings[i].data(),
            };

            POM_CHECK_VK(vkCreateDescriptorSetLayout(instance->device,
                                                     &descSetLayoutCreateInfo,
                                                     nullptr,
                                                     &descriptorSetLayouts[i]),
                         "Failed to create descriptor set layout.");

            descSetLayouts[i] = descriptorSetLayouts[i];
        }

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = static_cast<u32>(descSetLayouts.size()),
            .pSetLayouts = descSetLayouts.data(),
            .pushConstantRangeCount = 0, // TODO: push constants
            .pPushConstantRanges = nullptr,
        };

        POM_CHECK_VK(vkCreatePipelineLayout(instance->device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout),
                     "failed to create pipeline");
    }

    PipelineLayoutVk::~PipelineLayoutVk()
    {
        vkDestroyPipelineLayout(instance->device, pipelineLayout, nullptr);
        vkDestroyDescriptorPool(instance->device, pool, nullptr);
        for (auto& layout : descriptorSetLayouts) {
            vkDestroyDescriptorSetLayout(instance->device, layout.second, nullptr);
        }
    }

} // namespace pom::gfx