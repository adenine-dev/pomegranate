#pragma once

#include "base.hpp"

// FIXME: hash func for component id.
#define ECS_COMPONENT() static constexpr ::pom::ComponentId ecsComponentId = __COUNTER__

namespace pom {
    using ComponentId = u64;
    using Entity = ComponentId;

    struct ComponentMetadata {
        ComponentId id;
        const char* name;
        size_t size;
        size_t align;

        bool operator==(const ComponentId& other) const
        {
            return other == id;
        }

        bool operator==(const ComponentMetadata& other) const
        {
            return other.id == id; // NOTE: assumes a consistently constructed componet.
        }
    };

    // FIXME: this only needs to be trivial, eventually type hash.
    template <typename T> concept Component = std::is_trivially_copyable<T>::value&& requires
    {
        std::same_as<decltype(T::ecsComponentId), ComponentId>;
    };

    template <Component C> ComponentId componentId()
    {
        // FIXME: replace with type hash
        return C::ecsComponentId;
    }

    template <Component C> constexpr ComponentMetadata getComponentMetadata()
    {
        return {
            .id = componentId<C>(),
            .name = typeName<C>(),
            .size = sizeof(C),
            .align = alignof(C),
        };
    }

} // namespace pom