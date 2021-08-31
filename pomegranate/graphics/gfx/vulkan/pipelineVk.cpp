#include "pch.hpp"

#include "pipelineVk.hpp"

#include "gfxVk.hpp"
#include "instanceVk.hpp"
#include "pipelineLayoutVk.hpp"
#include "renderPassVk.hpp"
#include "shaderVk.hpp"

namespace pom::gfx {
    PipelineVk::PipelineVk(InstanceVk* instance,
                           RenderPassVk* renderPass,
                           ShaderVk* shader,
                           GraphicsPipelineState state,
                           std::initializer_list<VertexBinding> vertexBindings,
                           PipelineLayoutVk* pipelineLayout) :
        instance(instance)
    {
        std::vector<VkVertexInputBindingDescription> vertexBindingDescs;
        vertexBindingDescs.reserve(vertexBindings.size());
        std::vector<VkVertexInputAttributeDescription> vertexAttribDescs;

        for (const auto& binding : vertexBindings) {
            u32 totalAttribSize = 0;
            for (auto attrib : binding.attribs) {
                vertexAttribDescs.push_back({
                    .location = attrib.location,
                    .binding = binding.binding,
                    .format = toVkFormat(attrib.format),
                    .offset = attrib.offset == -1 ? totalAttribSize : attrib.offset,
                });
                totalAttribSize += sizeofFormat(attrib.format);
            }

            vertexBindingDescs.push_back({
                .binding = binding.binding,
                .stride = binding.stride == -1 ? totalAttribSize : binding.stride,
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX // TODO: have this be configurable
            });
        }

        VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = static_cast<u32>(vertexBindingDescs.size()),
            .pVertexBindingDescriptions = vertexBindingDescs.data(),
            .vertexAttributeDescriptionCount = static_cast<u32>(vertexAttribDescs.size()),
            .pVertexAttributeDescriptions = vertexAttribDescs.data(),
        };

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = toVkPrimitiveTopology(state.topology),
            .primitiveRestartEnable = state.primativeRestart ? VK_TRUE : VK_FALSE,
        };

        VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = toVkCullMode(state.cullMode),
            .frontFace = state.frontIsClockwise ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.f,
            .depthBiasClamp = 0.f,
            .depthBiasSlopeFactor = 0.f,
            .lineWidth = 1.f,
        };

        VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,
        };

        // TODO: blend modes
        // mix based on opacity
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask
            = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };

        VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachment,
            .blendConstants = { 0.f, 0.f, 0.f, 0.f },
        };

        // NOTE: Currently viewport things will always be dynamic for command buffer stuff as well as general usability.
        VkPipelineViewportStateCreateInfo viewportCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = 1,
            .pViewports = nullptr,
            .scissorCount = 1,
            .pScissors = nullptr,
        };

        VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .dynamicStateCount = 2,
            .pDynamicStates = dynamicStates,
        };

        VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stageCount = static_cast<u32>(shader->getNumModules()),
            .pStages = shader->getShaderStagesVk(),
            .pVertexInputState = &vertexInputCreateInfo,
            .pInputAssemblyState = &inputAssemblyCreateInfo,
            .pTessellationState = nullptr,
            .pViewportState = &viewportCreateInfo,
            .pRasterizationState = &rasterizationCreateInfo,
            .pMultisampleState = &multisampleCreateInfo,
            .pDepthStencilState = nullptr,
            .pColorBlendState = &colorBlendCreateInfo,
            .pDynamicState = &dynamicStateCreateInfo,
            .layout = pipelineLayout->getVkPipelineLayout(),
            .renderPass = renderPass->getHandle(),
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
        };

        POM_CHECK_VK(vkCreateGraphicsPipelines(instance->getVkDevice(),
                                               VK_NULL_HANDLE,
                                               1,
                                               &pipelineCreateInfo,
                                               nullptr,
                                               &pipeline),
                     "Failed to create graphics pipeline");
    }

    PipelineVk::~PipelineVk()
    {
        vkDestroyPipeline(instance->getVkDevice(), pipeline, nullptr);
    }
} // namespace pom::gfx