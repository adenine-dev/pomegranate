#pragma once

#include "base.hpp"

#include <unordered_map>

#include "archetype.hpp"
#include "component.hpp"

namespace pom {
    /// @addtogroup ecs
    /// @{

    template <Component... Cs> requires(are_distinct<Cs...>) class View;

    /// @brief Creates and contains all entity and component data.
    class POM_API Store {
    public:
        Store();
        ~Store();

        /// Returns a new entity with no components attached to it.
        /// @note Entities are only unique within the `Store` they were created in, and they may be reused after an
        /// entity has been destroyed.
        Entity createEntity();

        /// @brief Destroys an entity and all of its components.
        /// @note After destroying the entity, the entity id may be used again.
        void destroyEntity(Entity entity);

        /// @brief Returns true if the entity exists in the store, false otherwise.
        [[nodiscard]] bool exists(Entity entity) const;

        /// @brief Returns the Type of an entity.
        /// @warning This crashes if the entity does not exist in this store, use Store::exists to check if an entity
        /// exists in the store.
        [[nodiscard]] const Type& getType(Entity entity) const;

        /// @brief Returns true if the entity contains the component C
        /// @warning This crashes if the entity does not exist in this store, use Store::exists to check if an entity
        /// exists in the store.
        template <Component C> bool hasComponent(Entity entity)
        {
            auto recordPair = records.find(entity);
            POM_ASSERT(recordPair != records.end(), "Cannot check components of nonexistent entity ", entity);
            return recordPair->second.archetype->getType().contains<C>();
        }

        /// @brief Returns a mutable reference to the entity's component `C`.
        /// @warning This crashes if the entity does not exist in this store or if the entity does no have the
        /// component, use Store::exists to check if an entity exists in the store, or `tryGetComponent` to
        /// conditionally get the component.
        template <Component C> C& getComponent(Entity entity)
        {
            auto recordPair = records.find(entity);
            POM_ASSERT(recordPair != records.end(), "Cannot get component of nonexistent entity ", entity);
            return recordPair->second.archetype->getComponent<C>(recordPair->second.idx);
        }

        /// @brief Adds a component to the entity. Returns a mutable reference to the entity's component `C`.
        /// @warning This crashes if the entity does not exist in this store or if the entity already has the
        /// component, use Store::exists to check if an entity exists in the store.
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

        /// @brief Adds a component to the entity. Returns a mutable reference to the entity's component `C`.
        /// @warning This crashes if the entity does not exist in this store or if the entity does not have the
        /// component, use Store::exists to check if an entity exists in the store.
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

        /// Returns a view into the store with the requested components.
        /// @warning moving an entity while iterating over a view invalidates that view.
        template <Component... Cs> requires(are_distinct<Cs...>) [[nodiscard]] View<Cs...> view()
        {
            POM_PROFILE_FUNCTION();
            return View<Cs...>(this);
        }

    private:
        template <Component... Cs> requires(are_distinct<Cs...>) Archetype* findOrCreateArchetype()
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
        template <Component... Cs> requires(are_distinct<Cs...>) friend class View;

        std::unordered_map<Entity, Record> records;
        std::vector<Archetype*> archetypes;
    };

    /// @}
} // namespace pom