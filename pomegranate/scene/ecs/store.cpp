#include "pch.hpp"

#include "store.hpp"

namespace pom {
    Store::Store()
    {
        POM_PROFILE_FUNCTION();

        archetypes.push_back(new Archetype(this, {}));
    }

    Store::~Store()
    {
        POM_PROFILE_FUNCTION();

        for (Archetype* a : archetypes)
            delete a;
    }

    Entity Store::createEntity()
    {
        POM_PROFILE_FUNCTION();

        // TODO: replace this with a free list to reuse component ids.
        static Entity counter = NULL_ENTITY; // NOTE: 0 is reserved

        Entity ret = ++counter;
        records.insert({ ret, archetypes[0]->addEntity(ret) });
        return ret;
    }

    void Store::destroyEntity(Entity entity)
    {
        POM_PROFILE_FUNCTION();

        auto recordPair = records.find(entity);
        POM_ASSERT(recordPair != records.end(), "Cannot destroy nonexistent entity ", entity);
        recordPair->second.archetype->removeEntity(recordPair->second.idx);
        records.erase(recordPair);
    }

    [[nodiscard]] bool Store::exists(Entity entity) const
    {
        POM_PROFILE_FUNCTION();

        return records.contains(entity);
    }

    [[nodiscard]] const Type& Store::getType(Entity entity) const
    {
        POM_PROFILE_FUNCTION();

        auto recordPair = records.find(entity);
        POM_ASSERT(recordPair != records.end(), "Cannot get type of nonexistent entity ", entity);
        return recordPair->second.archetype->getType();
    }

    void Store::addParent(Entity parent, Entity child)
    {
        POM_PROFILE_FUNCTION();

        addComponent(child, getComponentMetadata(CHILDOF | parent));
    }

    void* Store::addComponent(Entity entity, const ComponentMetadata& componentMetadata)
    {
        auto recordPair = records.find(entity);
        POM_ASSERT(recordPair != records.end(), "Cannot add component to nonexistent entity ", entity);
        POM_ASSERT(!recordPair->second.archetype->getType().contains(componentMetadata.id),
                   "entity",
                   entity,
                   " already has component ",
                   componentMetadata.name);

        Record oldRecord = recordPair->second;
        Archetype* nextArchetype = oldRecord.archetype->addEdges.get(componentMetadata.id);
        if (nextArchetype == nullptr) {
            Type newType = oldRecord.archetype->type.add(componentMetadata);

            // we don't call `createChildArchetype` here to create the canonical path for the archetype, then we
            // assign it in this add table.
            nextArchetype = oldRecord.archetype->addEdges.add(componentMetadata.id, findOrCreateArchetype(newType));
        }

        Archetype* archetype = nextArchetype;
        Record record = archetype->addEntity(entity);
        records[entity] = record;
        usize i1 = 0;
        usize i2 = 0;
        usize i = -1;
        for (const ComponentMetadata& component : archetype->type) {
            if (component.id != componentMetadata.id) {
                memcpy((byte*)archetype->componentBuffers[i1].data + (component.size * record.idx),
                       (byte*)oldRecord.archetype->componentBuffers[i2].data + (component.size * oldRecord.idx),
                       component.size);
                i2++;
            } else {
                // zero initialize new data.
                memset((byte*)archetype->componentBuffers[i1].data + (component.size * record.idx), 0, component.size);
                i = i1;
            }

            i1++;
        }

        oldRecord.archetype->removeEntity(oldRecord.idx);

        return ((byte*)(archetype->componentBuffers[i].data)) + (componentMetadata.size * record.idx);
    }

    void Store::removeComponent(Entity entity, const ComponentMetadata& componentMetadata)
    {
        auto recordPair = records.find(entity);
        POM_ASSERT(recordPair != records.end(), "Cannot remove component from nonexistent entity ", entity);
        POM_ASSERT(recordPair->second.archetype->getType().contains(componentMetadata.id),
                   "entity ",
                   entity,
                   " does not have component ",
                   componentMetadata.name);

        Record oldRecord = recordPair->second;
        Archetype* nextArchetype = oldRecord.archetype->removeEdges.get(componentMetadata.id);
        if (nextArchetype == nullptr) {
            Type newType = oldRecord.archetype->getType().remove(componentMetadata);

            // we don't call `createChildArchetype` here to create the canonical path for the archetype, then we
            // assign it in this add table.
            nextArchetype = oldRecord.archetype->removeEdges.add(componentMetadata.id, findOrCreateArchetype(newType));
        }

        oldRecord = records[entity];

        Archetype* archetype = nextArchetype;
        Record record = records[entity] = archetype->addEntity(entity);
        usize i1 = 0;
        usize i2 = 0;
        for (const ComponentMetadata& component : oldRecord.archetype->type) {
            if (component.id != componentMetadata.id) {
                memcpy((byte*)archetype->componentBuffers[i1].data + (component.size * record.idx),
                       (byte*)oldRecord.archetype->componentBuffers[i2].data + (component.size * oldRecord.idx),
                       component.size);
                i1++;
            }
            i2++;
        }

        oldRecord.archetype->removeEntity(oldRecord.idx);
    }

    Archetype* Store::createChildArchetype(Archetype* archetype, const ComponentMetadata& added)
    {
        POM_PROFILE_FUNCTION();

        Type newType = archetype->type.add(added);
        auto* newArchetype = new Archetype(this, newType);
        archetypes.push_back(newArchetype);
        return archetype->addEdges.add(added.id, newArchetype);
    }

    Archetype* Store::findOrCreateArchetype(const Type& type)
    {
        POM_PROFILE_FUNCTION();

        Archetype* cur = getNullArchetype();
        for (const ComponentMetadata& component : type) {
            Archetype* next = cur->addEdges.get(component.id);
            if (next == nullptr) {
                cur = createChildArchetype(cur, component);
            } else {
                cur = next;
            }
        }

        return cur;
    }
} // namespace pom