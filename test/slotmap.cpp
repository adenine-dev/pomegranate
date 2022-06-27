#include "doctest.h"

#include "pomegranate.hpp"

TEST_CASE("Slotmap basic usage") {
    pom::Slotmap<f32> slotmap;
    const auto key = slotmap.emplace(3.f);

    CHECK(slotmap.has(key));
    CHECK(slotmap.get(key) == 3.f);

    CHECK(*slotmap.tryGet(key) == 3.f);
    slotmap.remove(key);
    CHECK(!slotmap.has(key));
    CHECK(slotmap.tryGet(key) == nullptr);
}

u32 ctorCount = 0;
u32 dtorCount = 0;

TEST_CASE("BUG: Slotmap member lifetime is handled correctly") {
    struct X {
        X() { ++ctorCount; }

        ~X() { ++dtorCount; }
    };

    {
        pom::Slotmap<X> sm;
        auto k = sm.emplace();
        sm.emplace();
        auto k1 = sm.emplace();
        sm.remove(k1);

        CHECK(ctorCount == 3);
        CHECK(dtorCount == 1);
    }

    CHECK(ctorCount == 3);
    CHECK(dtorCount == 3);
}

TEST_CASE("Slotmap handles multiple pages correctly") {
    const u32 count = 5000;
    pom::Slotmap<u32> sm;
    pom::Slotmap<u32>::Key keys[count];

    for (u32 i = 0; i < count; i++) {
        keys[i] = sm.emplace(i);
    }

    for (u32 i = 0; i < count; i++) {
        CHECK(sm.get(keys[i]) == i);
    }
}

TEST_CASE("Slotmap handles generations and free list minimum size correctly") {
    pom::Slotmap<u32, pom::SlotmapStorageTypesU64, 40> sm;

    auto k = sm.emplace(42);
    sm.remove(k);

    decltype(k) k1;
    u32 n = 0;
    while (true) {
        k1 = sm.emplace(1);
        if (k1.meta != k.meta || k1.page != k.page) {
            sm.remove(k1);
            ++n;
        } else
            break;
    }

    CHECK(!sm.has(k));
    CHECK(sm.get(k1) == 1);
    CHECK(n == 40); // There should be 40 elements in the free list before the first one is reused.
}