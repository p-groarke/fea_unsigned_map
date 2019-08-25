#include <algorithm>
#include <array>
#include <bench_util/bench_util.h>
#include <cstdio>
#include <fea_unsigned_map/fea_unsigned_map.hpp>
#include <map>
#include <string>
#include <unordered_map>

#if defined(NDEBUG)
constexpr size_t num_keys = 10'000'000;
#else
constexpr size_t num_keys = 1'000'000;
#endif

struct small_obj {
	float x{ 42 };
	float y{ 42 };
	float z{ 42 };
};

struct big_obj {
	big_obj() {
		data.fill(42);
	}
	~big_obj() {
	}
	big_obj(const big_obj& other) {
		data = other.data;
	}
	big_obj& operator=(const big_obj& other) {
		if (&other == this)
			return *this;

		data = other.data;
		return *this;
	}
	big_obj(big_obj&&) = delete;
	big_obj& operator=(big_obj&&) = delete;

	std::array<uint8_t, 1024> data{};
};

void benchmarks(const std::vector<size_t>& keys) {
	std::array<char, 128> title;
	title.fill('\0');

	bench::suite suite;

	// Containers
	std::map<size_t, small_obj> map_small;
	std::unordered_map<size_t, small_obj> unordered_map_small;
	fea::unsigned_map<size_t, small_obj> unsigned_map_small;

	std::map<size_t, big_obj> map_big;
	std::unordered_map<size_t, big_obj> unordered_map_big;
	fea::unsigned_map<size_t, big_obj> unsigned_map_big;


	// Preheat
	for (size_t i = 0; i < keys.size(); ++i) {
		map_small.insert({ keys[i], { float(i), float(i), float(i) } });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unordered_map_small.insert(
				{ keys[i], { float(i), float(i), float(i) } });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unsigned_map_small.insert(
				{ keys[i], { float(i), float(i), float(i) } });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		map_big.insert({ keys[i], {} });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unordered_map_big.insert({ keys[i], {} });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unsigned_map_big.insert({ keys[i], {} });
	}
	printf("Num unique keys : %zu\n\n", map_small.size());
	// printf("%zu\n", unordered_map_small.size());
	// printf("%zu\n", unsigned_map_small.size());

	// printf("%f\n", map_small[rand() % num_keys].x);
	// printf("%f\n", unordered_map_small[rand() % num_keys].x);
	// printf("%f\n", unsigned_map_small[rand() % num_keys].x);
	// printf("%u\n", map_big[rand() % num_keys].);
	// printf("%u\n", unordered_map_big[rand() % num_keys].x);
	// printf("%u\n", unsigned_map_big[rand() % num_keys].x);

	// Bench : clear small
	title.fill('\0');
	std::snprintf(
			title.data(), title.size(), "Clear %zu small objects", keys.size());
	suite.title(title.data());

	suite.benchmark("std::map clear", [&]() { map_small.clear(); });
	suite.benchmark(
			"std::unordered_map clear", [&]() { unordered_map_small.clear(); });
	suite.benchmark(
			"fea::unsigned_map clear", [&]() { unsigned_map_small.clear(); });
	suite.print();
	suite.clear();

	// Bench : clear big
	title.fill('\0');
	std::snprintf(
			title.data(), title.size(), "Clear %zu big objects", keys.size());
	suite.title(title.data());

	suite.benchmark("std::map clear", [&]() { map_big.clear(); });
	suite.benchmark(
			"std::unordered_map clear", [&]() { unordered_map_big.clear(); });
	suite.benchmark(
			"fea::unsigned_map clear", [&]() { unsigned_map_big.clear(); });
	suite.print();
	suite.clear();


	// Bench : insert small_obj
	title.fill('\0');
	std::snprintf(title.data(), title.size(), "Insert %zu small objects",
			keys.size());
	suite.title(title.data());

	suite.benchmark("std::map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			map_small.insert({ keys[i], { float(i), float(i), float(i) } });
		}
	});
	suite.benchmark("std::unordered_map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unordered_map_small.insert(
					{ keys[i], { float(i), float(i), float(i) } });
		}
	});
	suite.benchmark("fea::unsigned_map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unsigned_map_small.insert(
					{ keys[i], { float(i), float(i), float(i) } });
		}
	});
	suite.print();
	suite.clear();
	map_small.clear();
	unordered_map_small.clear();
	unsigned_map_small.clear();


	// Bench : insert big_obj
	title.fill('\0');
	std::snprintf(
			title.data(), title.size(), "Insert %zu big objects", keys.size());
	suite.title(title.data());

	suite.benchmark("std::map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			map_big.insert({ keys[i], {} });
		}
	});
	suite.benchmark("std::unordered_map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unordered_map_big.insert({ keys[i], {} });
		}
	});
	suite.benchmark("fea::unsigned_map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unsigned_map_big.insert({ keys[i], {} });
		}
	});
	suite.print();
	suite.clear();
	map_big.clear();
	unordered_map_big.clear();
	unsigned_map_big.clear();


	// Bench : insert small_obj reserves
	printf("\n");
	title.fill('\0');
	std::snprintf(title.data(), title.size(),
			"Insert %zu small objects after reserve", keys.size());
	suite.title(title.data());

	unordered_map_small.reserve(keys.size());
	unsigned_map_small.reserve(keys.size());

	suite.benchmark("std::map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			map_small.insert({ keys[i], {} });
		}
	});
	suite.benchmark("std::unordered_map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unordered_map_small.insert({ keys[i], {} });
		}
	});
	suite.benchmark("fea::unsigned_map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unsigned_map_small.insert({ keys[i], {} });
		}
	});
	suite.print();
	suite.clear();
	map_big.clear();
	unordered_map_small.clear();
	unsigned_map_small.clear();


	// Bench : insert big_obj reserves
	title.fill('\0');
	std::snprintf(title.data(), title.size(),
			"Insert %zu big objects after reserve", keys.size());
	suite.title(title.data());

	unordered_map_big.reserve(keys.size());
	unsigned_map_big.reserve(keys.size());

	suite.benchmark("std::map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			map_big.insert({ keys[i], {} });
		}
	});
	suite.benchmark("std::unordered_map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unordered_map_big.insert({ keys[i], {} });
		}
	});
	suite.benchmark("fea::unsigned_map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unsigned_map_big.insert({ keys[i], {} });
		}
	});
	suite.print();
	suite.clear();
	map_big.clear();
	unordered_map_big.clear();
	unsigned_map_big.clear();


	// Bench : Iterate and assign value small_obj
	for (size_t i = 0; i < keys.size(); ++i) {
		map_small.insert({ keys[i], { float(i), float(i), float(i) } });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unordered_map_small.insert(
				{ keys[i], { float(i), float(i), float(i) } });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unsigned_map_small.insert(
				{ keys[i], { float(i), float(i), float(i) } });
	}

	title.fill('\0');
	std::snprintf(title.data(), title.size(),
			"Iterate %zu small objects and assign value",
			unsigned_map_small.size());
	suite.title(title.data());

	suite.benchmark("std::map iterate & assign", [&]() {
		for (auto& p : map_small) {
			p.second.y = float(rand() % 100);
		}
	});
	suite.benchmark("std::unordered_map iterate & assign", [&]() {
		for (auto& p : unordered_map_small) {
			p.second.y = float(rand() % 100);
		}
	});
	suite.benchmark("fea::unsigned_map iterate & assign", [&]() {
		for (auto& p : unsigned_map_small) {
			p.second.y = float(rand() % 100);
		}
	});
	suite.print();
	suite.clear();

	map_small.clear();
	unordered_map_small.clear();
	unsigned_map_small.clear();


	// Bench : Iterate and assign value big_obj
	for (size_t i = 0; i < keys.size(); ++i) {
		map_big.insert({ keys[i], {} });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unordered_map_big.insert({ keys[i], {} });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unsigned_map_big.insert({ keys[i], {} });
	}

	title.fill('\0');
	std::snprintf(title.data(), title.size(),
			"Iterate %zu big objects and assign value",
			unsigned_map_big.size());
	suite.title(title.data());

	suite.benchmark("std::map iterate & assign", [&]() {
		for (auto& p : map_big) {
			p.second.data.fill(rand() % 100);
		}
	});
	suite.benchmark("std::unordered_map iterate & assign", [&]() {
		for (auto& p : unordered_map_big) {
			p.second.data.fill(rand() % 100);
		}
	});
	suite.benchmark("fea::unsigned_map iterate & assign", [&]() {
		for (auto& p : unsigned_map_big) {
			p.second.data.fill(rand() % 100);
		}
	});
	suite.print();
	suite.clear();

	map_big.clear();
	unordered_map_big.clear();
	unsigned_map_big.clear();
}


int main(int, char**) {
	srand(static_cast<unsigned int>(
			std::chrono::system_clock::now().time_since_epoch().count()));


	// Random keys.
	std::vector<size_t> keys;
	keys.reserve(num_keys);
	for (size_t i = 0; i < num_keys; ++i) {
		keys.push_back(rand() % num_keys);
	}

	std::array<char, 128> title;
	title.fill('\0');
	std::snprintf(title.data(), title.size(),
			"Benchmark using %zu random keys, with duplicates", num_keys);
	bench::title(title.data());

	benchmarks(keys);


	// Linear keys, 0 to N
	keys.clear();
	for (size_t i = 0; i < num_keys / 2; ++i) {
		keys.push_back(i);
	}

	printf("\n\n");
	title.fill('\0');
	std::snprintf(title.data(), title.size(),
			"Benchmark using linear keys, 0 to %zu, no duplicates",
			num_keys / 2);
	bench::title(title.data());

	benchmarks(keys);


	// Linear keys, N to 0
	keys.clear();
	for (long long i = long long(num_keys / 2 - 1); i >= 0; --i) {
		keys.push_back(size_t(i));
	}

	printf("\n\n");
	title.fill('\0');
	std::snprintf(title.data(), title.size(),
			"Benchmark using linear keys, %zu to 0, no duplicates",
			num_keys / 2);
	bench::title(title.data());

	benchmarks(keys);

	return 0;
}