#pragma once

#include "base.hpp"

#include <unordered_map>

#include "archetype.hpp"
#include "component.hpp"
#include "view.hpp"

namespace pom {
    class POM_API Store {
    public:
        Store();
        ~Store();

        Entity createEntity();
        void destroyEntity(Entity entity);

        [[nodiscard]] bool exists(Entity entity) const;

        [[nodiscard]] const Type& getType(Entity entity) const;

        template <Component C> bool hasComponent(Entity entity)
        {
            auto recordPair = records.find(entity);
            POM_ASSERT(recordPair != records.end(), "Cannot check components of nonexistent entity ", entity);
            return recordPair->second.archetype->getType().contains<C>();
        }

        template <Component C> C& getComponent(Entity entity)
        {
            auto recordPair = records.find(entity);
            POM_ASSERT(recordPair != records.end(), "Cannot get component of nonexistent entity ", entity);
            return recordPair->second.archetype->getComponent<C>(recordPair->second.idx);
        }

        template <Component C> C& addComponent(Entity entity)
        {
            auto recordPair = records.find(entity);
            POM_ASSERT(recordPair != records.end(), "Cannot add component to nonexistent entity ", entity);
            POM_ASSERT(!recordPair->second.archetype->getType().contains<C>(),
                       "entity",
                       entity,
                       " already has component ",
                       typeName<C>());

            Record oldRecord = recordPair->second;
            Archetype* nextArchetype = oldRecord.archetype->addEdges.get(componentId<C>());
            if (nextArchetype == nullptr) {
                Type newType = oldRecord.archetype->type.add<C>();

                // we don't call `createChildArchetype` here to create the canonical path for the archetype, then we
                // assign it in this add table.
                nextArchetype = oldRecord.archetype->addEdges.add(componentId<C>(), findOrCreateArchetype(newType));
            }

            Archetype* archetype = nextArchetype;
            Record record = archetype->addEntity(entity);
            records[entity] = record;
            usize i1 = 0;
            usize i2 = 0;
            usize i = -1;
            for (const ComponentMetadata& component : archetype->type) {
                if (component.id != componentId<C>()) {
                    memcpy((byte*)archetype->componentBuffers[i1].data + (component.size * record.idx),
                           (byte*)oldRecord.archetype->componentBuffers[i2].data + (component.size * oldRecord.idx),
                           component.size);
                    i2++;
                } else
                    i = i1;

                i1++;
            }

            oldRecord.archetype->removeEntity(oldRecord.idx);

            return ((C*)(archetype->componentBuffers[i].data))[record.idx];
        }

        template <Component C> void removeComponent(Entity entity)
        {
            auto recordPair = records.find(entity);
            POM_ASSERT(recordPair != records.end(), "Cannot remove component from nonexistent entity ", entity);
            POM_ASSERT(recordPair->second.archetype->getType().contains<C>(),
                       "entity ",
                       entity,
                       " does not have component ",
                       typeName<C>());

            Record oldRecord = recordPair->second;
            Archetype* nextArchetype = oldRecord.archetype->removeEdges.get(componentId<C>());
            if (nextArchetype == nullptr) {
                Type newType = oldRecord.archetype->getType().remove<C>();

                // we don't call `createChildArchetype` here to create the canonical path for the archetype, then we
                // assign it in this add table.
                nextArchetype = oldRecord.archetype->removeEdges.add(componentId<C>(), findOrCreateArchetype(newType));
            }

            oldRecord = records[entity];

            Archetype* archetype = nextArchetype;
            Record record = records[entity] = archetype->addEntity(entity);
            usize i1 = 0;
            usize i2 = 0;
            for (const ComponentMetadata& component : oldRecord.archetype->type) {
                if (component.id != componentId<C>()) {
                    memcpy((byte*)archetype->componentBuffers[i1].data + (component.size * record.idx),
                           (byte*)oldRecord.archetype->componentBuffers[i2].data + (component.size * oldRecord.idx),
                           component.size);
                    i1++;
                }
                i2++;
            }

            oldRecord.archetype->removeEntity(oldRecord.idx);
        }

        // FIXME: adding or removing components in a view breaks stuff
        template <Component... Cs> View<Cs...> view()
        {
            return View<Cs...>(findOrCreateArchetype<Cs...>());
        }

    private:
        template <Component... Cs> Archetype* findOrCreateArchetype()
        {
            return findOrCreateArchetype(Type::fromPack<Cs...>());
        }

        Archetype* findOrCreateArchetype(const Type& type);
        Archetype* createChildArchetype(Archetype* archetype, const ComponentMetadata& added);

        [[nodiscard]] inline Archetype* getNullArchetype()
        {
            return archetypes[0];
        }

        friend class Archetype;
        std::unordered_map<Entity, Record> records;
        std::vector<Archetype*> archetypes;
    };
} // namespace pom