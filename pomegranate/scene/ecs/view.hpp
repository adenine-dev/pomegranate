#pragma once

#include "base.hpp"

#include "archetype.hpp"
#include "component.hpp"
#include "store.hpp"

namespace pom {
    /// @brief An iterator into a store that sees all entities with the passed Components.
    template <Component... Cs> requires(are_distinct<Cs...>) class View {
    private:
        class iterator {
        public:
            using difference_type = std::ptrdiff_t; // meaningless, just for the iterator concept.
            using value_type = std::tuple<Entity, typename std::add_lvalue_reference<Cs>::type...>;
            using pointer = value_type*;
            using reference = value_type&;
            using iterator_category = std::forward_iterator_tag;

            explicit iterator(std::vector<Archetype*>* archetypes, usize aidx, usize idx) :
                archetypes(archetypes), currentArchetypeIdx(aidx), currentIdx(idx),
                indicies(aidx == SIZE_MAX ? std::array<i32, std::tuple_size<std::tuple<Cs...>>::value>()
                                          : std::array<i32, std::tuple_size<std::tuple<Cs...>>::value> {
                                              (*archetypes)[aidx]->getType().indexOf<Cs>()... })
            {
                POM_PROFILE_FUNCTION();
            }

            iterator& operator++()
            {
                POM_PROFILE_FUNCTION();

                if (++currentIdx >= (*archetypes)[currentArchetypeIdx]->size()) {
                    currentIdx = 0;
                    do {
                        if (++currentArchetypeIdx >= archetypes->size()) {
                            currentArchetypeIdx = SIZE_MAX;
                            currentIdx = SIZE_MAX;
                            return *this;
                        }
                    } while ((*archetypes)[currentArchetypeIdx]->size() == 0
                             || !(*archetypes)[currentArchetypeIdx]->getType().match<Cs...>());
                    indicies = { (*archetypes)[currentArchetypeIdx]->getType().indexOf<Cs>()... };
                }

                return *this;
            }

            iterator operator++(int)
            {
                POM_PROFILE_FUNCTION();

                View::iterator retval = *this;
                ++(*this);
                return retval;
            }

            inline bool operator==(iterator other) const
            {
                POM_PROFILE_FUNCTION();
                return archetypes == other.archetypes && currentArchetypeIdx == other.currentArchetypeIdx
                       && currentIdx == other.currentIdx;
            }

            inline bool operator!=(iterator other) const
            {
                POM_PROFILE_FUNCTION();
                return archetypes != other.archetypes || currentArchetypeIdx != other.currentArchetypeIdx
                       || currentIdx != other.currentIdx;
            }

            value_type operator*()
            {
                POM_PROFILE_FUNCTION();
                // if ((*archetypes)[currentArchetypeIdx]->size() == 0)
                //     operator++();

                return value_type((*archetypes)[currentArchetypeIdx]->entities[currentIdx],
                                  ((Cs*)(*archetypes)[currentArchetypeIdx]
                                       ->componentBuffers[indicies[TupleIndex<Cs, std::tuple<Cs...>>::value]]
                                       .data)[currentIdx]...);
            }

        private:
            std::vector<Archetype*>* archetypes;
            usize currentArchetypeIdx;
            usize currentIdx;
            std::array<i32, std::tuple_size<std::tuple<Cs...>>::value> indicies;
        };

    public:
        View(Store* store)
        {
            if (store)
                // TODO: cache/generate these in store
                for (auto* a : store->archetypes) {
                    if (a->getType().match<Cs...>())
                        archetypes.push_back(a);
                }
        }

        iterator begin()
        {
            if (archetypes.empty())
                return end();
            for (u32 i = 0; i < archetypes.size(); i++)
                if (archetypes[i]->size() != 0 && archetypes[i]->getType().match<Cs...>())
                    return iterator(&archetypes, i, 0);

            return end();
        }

        iterator end()
        {
            return iterator(&archetypes, SIZE_MAX, SIZE_MAX);
        }

        std::vector<Archetype*> archetypes;
    };
} // namespace pom
