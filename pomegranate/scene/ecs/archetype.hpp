#pragma once

#include "base.hpp"

#include "type.hpp"
#include "util/misc.hpp"

namespace pom {
    /// @addtogroup ecs
    /// @{

    class Store;
    class Archetype;

    /// @private
    struct Record {
        Archetype* archetype;
        usize idx;
    };

    /// @private
    struct Edge {
        ComponentId id;
        Archetype* archetype;

        constexpr bool operator==(ComponentId other) const
        {
            return id == other;
        }

        constexpr bool operator<(ComponentId other) const
        {
            return id < other;
        }

        constexpr bool operator<=(ComponentId other) const
        {
            return id <= other;
        }

        constexpr bool operator>(ComponentId other) const
        {
            return id > other;
        }

        constexpr bool operator>=(ComponentId other) const
        {
            return id >= other;
        }

        constexpr bool operator==(const Edge& other) const
        {
            return id == other.id && archetype == other.archetype;
        }

        constexpr bool operator<(const Edge& other) const
        {
            return id < other.id;
        }

        constexpr bool operator<=(const Edge& other) const
        {
            return id <= other.id;
        }

        constexpr bool operator>(const Edge& other) const
        {
            return id > other.id;
        }

        constexpr bool operator>=(const Edge& other) const
        {
            return id >= other.id;
        }
    };

    /// @private
    struct POM_API EdgeTable {
        std::vector<Edge> edges;

        Archetype* get(ComponentId id);

        Archetype* add(ComponentId id, Archetype* archetype);
    };

    /// @private
    class POM_API Archetype {
    public:
        Archetype(Store* store, const Type& type);
        ~Archetype();

        template <Component C> [[nodiscard]] C& getComponent(usize idx)
        {
            POM_PROFILE_FUNCTION();

            const i32 i = type.indexOf<C>();
            POM_ASSERT(i != -1,
                       "Requested component `",
                       typeName<C>(),
                       " (",
                       componentId<C>(),
                       ")` does not exist on entity.");

            return ((C*)componentBuffers[i].data)[idx];
        }

        template <Component C> [[nodiscard]] C* tryGetComponent(usize idx)
        {
            POM_PROFILE_FUNCTION();

            i32 i = type.indexOf<C>();
            if (i != -1)
                return nullptr;

            return &((C*)componentBuffers[i].data)[idx];
        }

        [[nodiscard]] inline usize size() const
        {
            return entities.size();
        }

        [[nodiscard]] inline const Type& getType() const
        {
            return type;
        }

        Record addEntity(Entity entity);
        void removeEntity(usize idx);

    private:
        friend class Store;
        template <Component... Cs> requires(are_distinct<Cs...>) friend class View;

        

        template <Component... Cs> [[nodiscard]] constexpr auto getQueryIndicies()
        {
            return (usize[std::tuple_size<std::tuple<Cs...>>::value]) { (usize)type.indexOf<Cs>()... };
        }

        constexpr static const usize INITIAL_LENGTH = 4;

        struct ComponentStorage {
            void* data;
            usize allocated;
            usize length;
        };

        Store* store;
        const Type type;
        EdgeTable addEdges;
        EdgeTable removeEdges;
        std::vector<ComponentStorage> componentBuffers;
        std::vector<Entity> entities;
    };

    /// @}
} // namespace pom