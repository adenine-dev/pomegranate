#pragma once

#include "base.hpp"

#include "archetype.hpp"
#include "component.hpp"

namespace pom {
    class Archetype;

    template <Component... Cs> class View {
    private:
        class iterator {
        public:
            using difference_type = std::ptrdiff_t; // meaningless, just for the iterator concept.
            using value_type = std::tuple<Entity, typename std::add_lvalue_reference<Cs>::type...>;
            using pointer = value_type*;
            using reference = value_type&;
            using iterator_category = std::forward_iterator_tag;

            explicit iterator(Archetype* a, usize idx) :
                frontier(a ? std::vector<Archetype*> { a } : std::vector<Archetype*> {}),
                currentArchetypeIdx(a ? 0 : SIZE_MAX), currentIdx(idx)
            {
            }

            iterator& operator++()
            {
                POM_PROFILE_FUNCTION();

                if (++currentIdx >= frontier[currentArchetypeIdx]->size()) {
                    currentIdx = 0;
                    do {
                        for (auto& edge : frontier[currentArchetypeIdx]->addEdges.edges) {
                            if (std::find(frontier.begin(), frontier.end(), edge.archetype) == frontier.end()) {
                                frontier.push_back(edge.archetype);
                            }
                        }

                        if (++currentArchetypeIdx >= frontier.size()) {
                            currentArchetypeIdx = SIZE_MAX;
                            currentIdx = SIZE_MAX;
                            break;
                        }
                    } while (frontier[currentArchetypeIdx]->size() == 0);
                }

                return *this;
            }

            const iterator operator++(int)
            {
                POM_PROFILE_FUNCTION();

                View::iterator retval = *this;
                ++(*this);
                return retval;
            }

            inline bool operator==(iterator other) const
            {
                return currentArchetypeIdx == other.currentArchetypeIdx && currentIdx == other.currentIdx;
            }

            inline bool operator!=(iterator other) const
            {
                return !(*this == other);
            }

            value_type operator*() const
            {
                POM_PROFILE_FUNCTION();

                // FIXME: don't use get component, store references to component buffers directly
                return value_type(frontier[currentArchetypeIdx]->entities[currentIdx],
                                  frontier[currentArchetypeIdx]->getComponent<Cs>(currentIdx)...);
            }

        private:
            std::vector<Archetype*> frontier; // TODO: can this be done without a vector here?
            usize currentArchetypeIdx;
            usize currentIdx;
        };

    public:
        View(Archetype* startArchetype) : startArchetype(startArchetype)
        {
        }

        iterator begin()
        {
            return iterator(startArchetype, 0);
        }

        iterator end()
        {
            return iterator(nullptr, SIZE_MAX);
        }

        Archetype* startArchetype;
    };
} // namespace pom
