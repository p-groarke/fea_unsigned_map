#include <fea_unsigned_map/fea_unsigned_map.hpp>
#include <gtest/gtest.h>
#include <unordered_map>

namespace {
struct test {
	size_t val = 42;
};
bool operator==(const test& lhs, const test& rhs) {
	return lhs.val == rhs.val;
}
bool operator!=(const test& lhs, const test& rhs) {
	return !operator==(lhs, rhs);
}

TEST(unsigned_map, basics) {
	constexpr size_t small_num = 10;

	fea::unsigned_map<size_t, test> map{ small_num };

	for (size_t i = 0; i < small_num; ++i) {
		map.insert({ i, { i } });
	}
	fea::unsigned_map<size_t, test> map2{ map };
	fea::unsigned_map<size_t, test> map_ded{ map };
	fea::unsigned_map<size_t, test> map3{ std::move(map_ded) };

	for (size_t i = 0; i < small_num; ++i) {
		EXPECT_EQ(map[i], test{ i });
		EXPECT_EQ(map.at(i), test{ i });
		EXPECT_EQ(*map.find(i), test{ i });

		EXPECT_EQ(map2[i], test{ i });
		EXPECT_EQ(map2.at(i), test{ i });
		EXPECT_EQ(*map2.find(i), test{ i });

		EXPECT_EQ(map3[i], test{ i });
		EXPECT_EQ(map3.at(i), test{ i });
		EXPECT_EQ(*map3.find(i), test{ i });
	}

	// std::unordered_map<size_t, test> compare_map;
	// decltype(compare_map)::allocator_type;
	// decltype(compare_map)::hasher;
	// decltype(compare_map)::key_equal;
	// auto it = compare_map.begin();
}

} // namespace

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
