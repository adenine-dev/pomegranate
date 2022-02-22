#include "base.hpp"

#include "archetype.hpp"
#include "scene/ecs/store.hpp"

namespace pom {
    Archetype* EdgeTable::get(ComponentId id)
    {
        auto edge = binaryFind(edges.begin(), edges.end(), id);
        if (edge == edges.end())
            return nullptr;

        return edge->archetype;
    }

    Archetype* EdgeTable::add(ComponentId id, Archetype* archetype)
    {
        if (get(id) == nullptr) {
            edges.push_back({ id, archetype });
            std::sort(edges.begin(), edges.end());
        }
        return archetype;
    }

    Archetype::Archetype(Store* store, const Type& type) : store(store), type(type)
    {
        componentBuffers.reserve(type.size());

        for (const ComponentMetadata& component : type) {
            componentBuffers.push_back({
                .data = malloc(INITIAL_LENGTH * component.size),
                .allocated = INITIAL_LENGTH,
                .length = 0,
            });
        }
    }

    Archetype::~Archetype()
    {
        for (auto componentStorage : componentBuffers) {
            free(componentStorage.data);
        }
    }

    Record Archetype::addEntity(Entity entity)
    {
        for (usize i = 0; i < type.size(); i++) {
            if (componentBuffers[i].allocated == componentBuffers[i].length) {
                componentBuffers[i].allocated *= 2;
                componentBuffers[i].data
                    = realloc(componentBuffers[i].data, componentBuffers[i].allocated * type[i].size);
            }

            ++componentBuffers[i].length;
        }

        entities.push_back(entity);
        return {
            .archetype = this,
            .idx = size() - 1,
        };
    }

    void Archetype::removeEntity(usize idx)
    {
        POM_ASSERT(idx < size(), "index ", idx, " does not exist in archetype that has size ", size());
        for (usize i = 0; i < type.size(); i++) {
            memcpy((byte*)componentBuffers[i].data + (idx * type[i].size),
                   (byte*)componentBuffers[i].data + ((--componentBuffers[i].length) * type[i].size),
                   type[i].size);
        }

        if (idx != size() - 1) {
            entities[idx] = entities.back();
            store->records[entities[idx]].idx = idx;
        }
        entities.pop_back();
    }

} // namespace pom