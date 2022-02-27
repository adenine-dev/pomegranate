#pragma once

#include "base.hpp"

// FIXME: hash func for component id.
// NOTE: fix Component id docs when you change this
#define ECS_COMPONENT() static constexpr ::pom::ComponentId ecsComponentId = __COUNTER__

namespace pom {
    /// @addtogroup ecs
    /// @{

    /// @brief Each component has a unique id, consistent across all executions of the application.
    /// This id is given by @ref componentId<C>()
    using ComponentId = u64;

    /// Each entity is represented as a unique id
    using Entity = ComponentId;

    /// @brief Contains metadata for a given Component.
    /// This data can be customized by specializing @ref getComponentMetadata<C>() for a given type C.
    struct ComponentMetadata {
        /// The unique id of a component. By default the result of @ref componentId<C>()
        ComponentId id;
        /// @brief The human readable  name of a component. By default the result of @ref typeName<T>() which is the
        /// stringified name of the component structure.
        const char* name;
        /// The size in bytes of a component, should *always* be `sizeof(C)`.
        usize size;
        /// The alignment in bytes of a component, should *always* be `alignof(C)`.
        usize align;

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
    template <typename T>
    concept Component = std::is_trivially_copyable<T>::value&& std::is_trivially_destructible<T>::value&& requires
    {
        std::same_as<decltype(T::ecsComponentId), ComponentId>;
    };

    /// Returns the component id of a given component, this should always be the same in a given build of the app.
    template <Component C> constexpr ComponentId componentId()
    {
        // FIXME: replace with type hash
        return C::ecsComponentId;
    }

    /// Returns metadata for a component, this can be specialized to customize its output, see ComponentMetadata
    template <Component C> constexpr ComponentMetadata getComponentMetadata()
    {
        return {
            .id = componentId<C>(),
            .name = typeName<C>(),
            .size = sizeof(C),
            .align = alignof(C),
        };
    }

    /// @}

} // namespace pom