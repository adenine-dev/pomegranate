#pragma once

#include "base.hpp"

#include <utility>

#include "component.hpp"

namespace pom {
    class Type {
    public:
        Type() = default;

        template <Component... Cs> [[nodiscard]] static Type fromPack()
        {
            Type t;
            t.components = { getComponentMetadata<Cs>()... };
            t.sort();
            return t;
        }

        template <Component C> [[nodiscard]] Type add() const
        {
            Type newType = { components };
            newType.components.push_back(getComponentMetadata<C>());
            newType.sort();
            return newType;
        }

        [[nodiscard]] Type add(const ComponentMetadata& added) const
        {
            Type newType = { components };
            newType.components.push_back(added);
            newType.sort();
            return newType;
        }

        template <Component C> [[nodiscard]] Type remove() const
        {
            Type newType = { components };
            newType.components.erase(std::find(newType.components.begin(), newType.components.end(), componentId<C>()));
            return newType;
        }

        template <Component C> [[nodiscard]] i32 indexOf() const
        {
            auto it = binaryFind(components.cbegin(),
                                 components.cend(),
                                 ComponentMetadata { .id = componentId<C>() },
                                 [](const ComponentMetadata& a, const ComponentMetadata& b) { return a.id < b.id; });
            if (it == components.cend())
                return -1;

            return it - components.cbegin();
        }

        template <Component C> [[nodiscard]] bool contains() const
        {
            return indexOf<C>() != -1;
        }

        [[nodiscard]] usize size() const
        {
            return components.size();
        }

        [[nodiscard]] const ComponentMetadata& operator[](usize idx) const
        {
            return components[idx];
        }

        [[nodiscard]] auto begin() const
        {
            return components.cbegin();
        }

        [[nodiscard]] auto end() const
        {
            return components.cend();
        }

    private:
        friend std::ostream& operator<<(std::ostream& os, const Type& type);

        Type(std::vector<ComponentMetadata> data) : components(std::move(data))
        {
        }

        void sort()
        {
            std::sort(components.begin(), components.end(), [](ComponentMetadata& a, ComponentMetadata& b) {
                return a.id < b.id;
            });
        }

        std::vector<ComponentMetadata> components;
    };

    inline std::ostream& operator<<(std::ostream& os, const Type& type)
    {
        if (type.components.empty()) {
            os << "NullType []";
        } else {
            os << "Type [ ";
            for (usize i = 0; i < type.size() - 1; i++)
                os << type[i].name << ", ";
            os << type.components.back().name << " ]";
        }
        return os;
    }
} // namespace pom