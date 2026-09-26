/**
 * @file test_ids.cpp
 * @brief Tests for ID types and handles
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "core/handles/ids.h"
#include "core/testing/test.h"

POKO_TEST(Handles, IDGenerator_Generate) {
    using namespace poko::core;

    IDGenerator gen(1);
    uint64_t id1 = gen.generate();
    uint64_t id2 = gen.generate();

    POKO_ASSERT_TRUE(id1 != id2);
    POKO_ASSERT_EQ(1ULL, id1);
    POKO_ASSERT_EQ(2ULL, id2);
}

POKO_TEST(Handles, IDGenerator_Reserve) {
    using namespace poko::core;

    IDGenerator gen(1);
    POKO_ASSERT_TRUE(gen.reserve(100));
    POKO_ASSERT_EQ(100ULL, gen.get_max());

    uint64_t id = gen.generate();
    POKO_ASSERT_EQ(101ULL, id);
}

POKO_TEST(Handles, Handle_IsValid) {
    using namespace poko::core;

    InstanceHandle handle;
    POKO_ASSERT_FALSE(handle.is_valid());

    InstanceHandle valid(1, 1);
    POKO_ASSERT_TRUE(valid.is_valid());
}

POKO_TEST(Handles, Handle_Equality) {
    using namespace poko::core;

    InstanceHandle h1(1, 1);
    InstanceHandle h2(1, 1);
    InstanceHandle h3(1, 2);

    POKO_ASSERT_TRUE(h1 == h2);
    POKO_ASSERT_FALSE(h1 == h3);
}

POKO_TEST(Handles, StrongID_Comparison) {
    using namespace poko::core;

    TypedInstanceID id1(InstanceIDUnderlying(1));
    TypedInstanceID id2(InstanceIDUnderlying(2));

    POKO_ASSERT_TRUE(id1 < id2);
    POKO_ASSERT_FALSE(id1 == id2);
    POKO_ASSERT_TRUE(id1 != id2);
}

int main() {
    return poko::core::testing::run_all_tests();
}
