#pragma once

#include "base.hpp"

#include "pch.hpp"

namespace pom {
    struct SlotmapStorageTypesU64 {
        static constexpr usize PAGE_SIZE = 4096;
        static constexpr u16 MAX_GENERATION = UINT16_MAX;

        struct Key {
            u64 _ : 16, generation : 16, page : 32 - std::bit_width(PAGE_SIZE), meta : std::bit_width(PAGE_SIZE);
        };

        struct Meta {
            u64 active : 1, generation : 16, index : std::bit_width(PAGE_SIZE);
        };

        struct FreeIdx {
            u32 page : 32 - std::bit_width(PAGE_SIZE), meta : std::bit_width(PAGE_SIZE);
        };
    };

    /// Slotmaps are similar to hashmaps however you don't get to decide on a key, instead the key is a type that
    /// guarantees true O(1) lookup, insertion and removal.
    template <typename T, typename Storage = SlotmapStorageTypesU64, usize minFreeListSize = 32>
    class Slotmap {
    public:
        using Key = Storage::Key;

        constexpr Slotmap() : pages({new Page()}) {}

        constexpr ~Slotmap() {
            for (auto p : pages)
                delete p;
        }

        /// Construct a new T in place and returns the key to that data.
        template <class... Args>
        [[nodiscard]] constexpr Key emplace(Args &&...args) {
            if (freeList.empty() || freeList.size() <= minFreeListSize) {
                if (pages.back()->used == Storage::PAGE_SIZE)
                    pages.emplace_back(new Page{});

                std::construct_at<T>(&pages.back()->data[pages.back()->used], std::forward<Args>(args)...);
                pages.back()->metadata[pages.back()->used] = {1, 1, pages.back()->used};

                return {
                    ._ = 0,
                    .generation = 1,
                    .page = pages.size() - 1,
                    .meta = pages.back()->used++, // NOTE: postincrement here.
                };
            } else {
                const FreeIdx idx = freeList.front();
                freeList.pop_front();
                Meta &meta = pages[idx.page]->metadata[idx.meta];
                assert(!meta.active, "");
                meta.active = true;
                std::construct_at<T>(&pages[idx.page]->data[meta.index], std::forward<Args>(args)...);

                return {
                    ._ = 0,
                    .generation = meta.generation,
                    .page = idx.page,
                    .meta = meta.index, // NOTE: postincrement here.
                };
            }
        }

        /// Returns true if the key is valid.
        constexpr bool has(Key k) const {
            return k.page < pages.size() && k.generation == pages[k.page]->metadata[k.meta].generation;
        }

        /// Returns a reference to the data associated with the key if it exists.
        /// @warning This asserts that the key is valid, if it is not, this will fail catastrophically.
        /// Use tryGet() or check with has().
        constexpr T &get(Key k) {
            assert(has(k), "Trying to get data with an invalid key, maybe use `tryGet`?");

            return pages[k.page]->data[pages[k.page]->metadata[k.meta].index];
        }

        /// Returns a pointer to the data associated with the key if it exists, or `nullptr` if it does not.
        constexpr T *tryGet(Key k) {
            return has(k) ? &pages[k.page]->data[pages[k.page]->metadata[k.meta].index] : nullptr;
        }

        /// Removes the data associated with the key if it exists, if it does not it does nothing. This invalidates the
        /// key.
        constexpr void remove(Key k) {
            if (has(k)) {
                pages[k.page]->metadata[k.meta].active = false;
                pages[k.page]->data[pages[k.page]->metadata[k.meta].index].~T();
                if (pages[k.page]->metadata[k.meta].generation != Storage::MAX_GENERATION) {
                    ++pages[k.page]->metadata[k.meta].generation;
                    freeList.push_back({.page = (u32)k.page, .meta = (u32)k.meta});
                }
            }
        }

    private:
        using FreeIdx = Storage::FreeIdx;
        using Meta = Storage::Meta;

        struct Page {
            union { // union to prevent the constructor/destructor of T from being called while still having the type
                    // safety of an array.
                u8 _doNotUse = 0;
                T data[Storage::PAGE_SIZE];
            };

            Meta metadata[Storage::PAGE_SIZE] = {};
            u16 used = 0;

            constexpr ~Page() {
                for (u16 i = 0; i < used; i++)
                    if (metadata[i].active)
                        data[metadata[used].index].~T();
            }
        };

        std::vector<Page *> pages;
        std::deque<FreeIdx> freeList;
    };

} // namespace pom