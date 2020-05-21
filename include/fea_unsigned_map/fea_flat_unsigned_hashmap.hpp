/*
BSD 3-Clause License

Copyright (c) 2020, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include <algorithm>
#include <cassert>
#include <numeric>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

/*
This is a more traditional-ish "hash map".

Its special characteristics are :
	- Like the unsigned_map, your keys must be unique unsigned numbers.
	- WARNING : The map does not support key collisions (no hashing).
	- Unlike unsigned_map, you may use as big of keys as you want. The key
		container doesn't grow as big as N.
	- Values are packed, so you may iterate values quickly (the map supports
		.data()).
	- Note : This map doesn't follow the c++ standard apis very closely, as
		iterators are on value_type, not pair<key_type, value_type>.
*/


namespace fea {
namespace detail {
template <class T>
inline constexpr std::conditional_t<!std::is_move_constructible<T>::value
				&& std::is_copy_constructible<T>::value,
		const T&, T&&>
flathashmap_maybe_move(T& arg) noexcept {
	return std::move(arg);
}

// https://stackoverflow.com/questions/30052316/find-next-prime-number-algorithm
template <class T>
bool is_prime(T number) {
	if (number == 2 || number == 3)
		return true;

	if (number % 2 == 0 || number % 3 == 0)
		return false;

	T divisor = 6;
	while (divisor * divisor - 2 * divisor + 1 <= number) {

		if (number % (divisor - 1) == 0)
			return false;

		if (number % (divisor + 1) == 0)
			return false;

		divisor += 6;
	}

	return true;
}

template <class T>
T next_prime(T a) {
	switch (a) {
	case 14: {
		return 17;
	} break;
	case 34: {
		return 37;
	} break;
	case 74: {
		return 79;
	} break;
	case 158: {
		return 163;
	} break;
	case 326: {
		return 331;
	} break;
	case 662: {
		return 673;
	} break;
	case 1346: {
		return 1361;
	} break;
	case 2722: {
		return 2729;
	} break;
	case 5458: {
		return 5471;
	} break;
	case 10942: {
		return 10949;
	} break;
	case 21898: {
		return 21911;
	} break;
	case 43822: {
		return 43853;
	} break;
	case 87706: {
		return 87719;
	} break;
	case 175438: {
		return 175447;
	} break;
	case 350894: {
		return 350899;
	} break;
	case 701798: {
		return 701819;
	} break;
	case 1403638: {
		return 1403641;
	} break;
	case 2807282: {
		return 2807303;
	} break;
	case 5614606: {
		return 5614657;
	} break;
	default: {
		while (!is_prime(++a)) {
		}
		return a;
	} break;
	}
}
} // namespace detail


template <class Key, class T>
struct flat_unsigned_hashmap {
	static_assert(std::is_unsigned<Key>::value,
			"unsigned_map : key must be unsigned integer");

	using key_type = Key;
	using mapped_type = T;
	using value_type = mapped_type;
	using size_type = std::size_t;
	using pos_type = Key;
	using difference_type = std::ptrdiff_t;

	using allocator_type = typename std::vector<value_type>::allocator_type;

	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = typename std::allocator_traits<allocator_type>::pointer;
	using const_pointer =
			typename std::allocator_traits<allocator_type>::const_pointer;

	using iterator = typename std::vector<value_type>::iterator;
	using const_iterator = typename std::vector<value_type>::const_iterator;
	using local_iterator = iterator;
	using const_local_iterator = const_iterator;

	// Don't make sense
	// using hasher = std::hash<key_type>;
	// using key_equal = std::equal_to<key_type>;
	// using node_type;
	// using insert_return_type;


	// Constructors, destructors and assignement

	flat_unsigned_hashmap()
			: _lookup(std::vector<std::pair<key_type, pos_type>>(
					init_count(), { {}, pos_sentinel() })) {
		// Starts with 7 elements to optimize some calls.
		// Uses prime modulo sizes.
	}
	flat_unsigned_hashmap(const flat_unsigned_hashmap&) = default;
	flat_unsigned_hashmap(flat_unsigned_hashmap&&) = default;
	flat_unsigned_hashmap& operator=(const flat_unsigned_hashmap&) = default;
	flat_unsigned_hashmap& operator=(flat_unsigned_hashmap&&) = default;

	explicit flat_unsigned_hashmap(size_t reserve_count)
			: flat_unsigned_hashmap() {
		_lookup.reserve(reserve_count);
		_reverse_lookup.reserve(reserve_count);
		_values.reserve(reserve_count);
	}
	explicit flat_unsigned_hashmap(
			size_t key_reserve_count, size_t value_reserve_count)
			: flat_unsigned_hashmap() {
		_lookup.reserve(key_reserve_count);
		_reverse_lookup.reserve(value_reserve_count);
		_values.reserve(value_reserve_count);
	}

	// todo : kv iterators
	// template <class InputIt>
	// flat_unsigned_hashmap(InputIt first, InputIt last)
	//		: flat_unsigned_hashmap() {
	//	// TODO : benchmark and potentially optimize
	//	for (auto it = first; it != last; ++it) {
	//		insert(*it);
	//	}
	//}

	explicit flat_unsigned_hashmap(
			std::initializer_list<std::pair<key_type, value_type>> init)
			: flat_unsigned_hashmap() {
		// TODO : benchmark and potentially optimize
		for (const std::pair<key_type, value_type>& kv : init) {
			insert(kv.first, kv.second);
		}
	}


	// Iterators

	// returns an iterator to the beginning
	iterator begin() noexcept {
		return _values.begin();
	}
	const_iterator begin() const noexcept {
		return _values.begin();
	}
	const_iterator cbegin() const noexcept {
		return _values.cbegin();
	}

	// returns an iterator to the end (one past last)
	iterator end() noexcept {
		return _values.end();
	}
	const_iterator end() const noexcept {
		return _values.end();
	}
	const_iterator cend() const noexcept {
		return _values.cend();
	}


	// Capacity

	// checks whether the container is empty
	bool empty() const noexcept {
		return _values.empty();
	}

	// returns the number of elements
	size_type size() const noexcept {
		return _values.size();
	}

	// returns the maximum possible number of elements
	size_type max_size() const noexcept {
		// -1 due to sentinel
		return pos_sentinel() - 1;
	}

	// reserves storage
	void reserve(size_type new_cap) {
		_lookup.reserve(new_cap);
		_reverse_lookup.reserve(new_cap);
		_values.reserve(new_cap);
	}

	// returns the number of elements that can be held in currently
	// allocated storage
	size_type capacity() const noexcept {
		return _values.capacity();
	}

	// reduces memory usage by freeing unused memory
	void shrink_to_fit() {
		_lookup.shrink_to_fit();
		_reverse_lookup.shrink_to_fit();
		_values.shrink_to_fit();
	}


	// Modifiers

	// clears the contents
	void clear() noexcept {
		_lookup = std::vector<std::pair<key_type, pos_type>>(
				init_count(), { {}, pos_sentinel() });
		_reverse_lookup.clear();
		_values.clear();
	}

	// inserts elements or nodes
	std::pair<iterator, bool> insert(key_type key, const value_type& value) {
		return minsert(key, value);
	}
	std::pair<iterator, bool> insert(key_type key, value_type&& value) {
		return minsert(key, detail::flathashmap_maybe_move(value));
	}

	// todo : pair iterators
	// template <class InputIt>
	// void insert(InputIt first, InputIt last) {
	//	// TODO : benchmark and potentially optimize
	//	for (auto it = first; it != last; ++it) {
	//		insert(*it);
	//	}
	//}
	void insert(std::initializer_list<std::pair<key_type, value_type>> ilist) {
		// TODO : benchmark and potentially optimize
		for (const std::pair<key_type, value_type>& kv : ilist) {
			insert(kv.first, kv.second);
		}
	}

	// inserts an element or assigns to the current element if the key
	// already exists
	std::pair<iterator, bool> insert_or_assign(
			key_type key, const value_type& value) {
		return minsert(key, value, true);
	}
	std::pair<iterator, bool> insert_or_assign(
			key_type key, value_type&& value) {
		return minsert(key, detail::flathashmap_maybe_move(value), true);
	}

	// constructs element in-place
	template <class... Args>
	std::pair<iterator, bool> emplace(key_type key, Args&&... args) {
		// Standard emplace behavior doesn't apply. Use try_emplace.
		return try_emplace(key, std::forward<Args>(args)...);
	}

	// inserts in-place if the key does not exist, does nothing if the key
	// exists
	template <class... Args>
	std::pair<iterator, bool> try_emplace(key_type key, Args&&... args) {
		if (load_factor() > _max_load_factor) {
			size_type new_size = _lookup.size() * 2;
			rehash(new_size);
		}

		auto lookup_it = find_first_slot(key);
		if (lookup_it->second != pos_sentinel()) {
			// Found valid key.
			return { _values.begin() + lookup_it->second, false };
		}

		pos_type new_pos = pos_type(_values.size());
		_values.emplace_back(std::forward<Args>(args)...);
		_reverse_lookup.push_back(key);
		lookup_it->first = key;
		lookup_it->second = new_pos;

		assert(_reverse_lookup.size() == _values.size());
		return { begin() + new_pos, true };
	}

	// erases elements
	void erase(const_iterator pos) {
		size_t idx = std::distance(_values.cbegin(), pos);
		erase(_reverse_lookup[idx]);
	}
	void erase(const_iterator first, const_iterator last) {
		size_t first_idx = std::distance(_values.cbegin(), first);
		size_t last_idx = std::distance(_values.cbegin(), last);

		std::vector<key_type> to_erase;
		to_erase.reserve(last_idx - first_idx);
		for (auto it = _reverse_lookup.begin() + first_idx;
				it != _reverse_lookup.begin() + last_idx; ++it) {
			to_erase.push_back(*it);
		}

		for (key_type& k : to_erase) {
			erase(k);
		}
	}
	size_type erase(key_type k) {
		auto lookup_it = find_first_slot(k);

		if (lookup_it->second == pos_sentinel()) {
			return 0;
		}

		if (lookup_it->second == _values.size() - 1) {
			// No need for swap, object is already at end.
			lookup_it->first = {};
			lookup_it->second = pos_sentinel();
			_reverse_lookup.pop_back();
			_values.pop_back();
			assert(_values.size() == _reverse_lookup.size());

			return 1;
		}

		// todo : Better way than doing a key search? could be slow
		key_type last_key = _reverse_lookup.back();
		auto last_lookup_it = find_first_slot(last_key);

		// set new pos on last element.
		last_lookup_it->second = lookup_it->second;

		// invalidate erased lookup
		lookup_it->first = {};
		lookup_it->second = pos_sentinel();

		// "swap" the elements
		_values[last_lookup_it->second]
				= detail::flathashmap_maybe_move(_values.back());
		_reverse_lookup[last_lookup_it->second] = last_key;

		// delete last
		_values.pop_back();
		_reverse_lookup.pop_back();

		assert(_values.size() == _reverse_lookup.size());
		return 1;
	}

	// swaps the contents
	void swap(flat_unsigned_hashmap& other) noexcept {
		_lookup.swap(other._lookup);
		_reverse_lookup.swap(other._reverse_lookup);
		_values.swap(other._values);
	}


	// Lookup
	// direct access to the underlying vector
	const value_type* data() const noexcept {
		return _values.data();
	}
	value_type* data() noexcept {
		return _values.data();
	}

	// access specified element with bounds checking
	const mapped_type& at(key_type k) const {
		const_iterator it = find(k);
		if (it == end()) {
			throw std::out_of_range{ "unsigned_map : value doesn't exist" };
		}

		return *it;
	}
	mapped_type& at(key_type k) {
		return const_cast<mapped_type&>(
				static_cast<const flat_unsigned_hashmap*>(this)->at(k));
	}

	// access specified element without any bounds checking
	const mapped_type& at_unchecked(key_type k) const {
		const_iterator it = find(k);
		return *it;
	}
	mapped_type& at_unchecked(key_type k) {
		return const_cast<mapped_type&>(
				static_cast<const flat_unsigned_hashmap*>(this)->at_unchecked(
						k));
	}

	// access or insert specified element
	mapped_type& operator[](key_type k) {
		iterator it = find(k);
		if (it != end()) {
			return *it;
		}

		return *insert(k, {}).first;
	}

	// returns the number of elements matching specific key (which is 1 or 0,
	// since there are no duplicates)
	size_type count(key_type k) const {
		if (contains(k))
			return 1;

		return 0;
	}

	// finds element with specific key
	const_iterator find(key_type k) const {
		auto lookup_it = find_first_slot(k);
		if (lookup_it->second == pos_sentinel()) {
			return end();
		}

		assert(lookup_it->first == k);
		assert(lookup_it->second < _values.size());
		assert(lookup_it->second < _reverse_lookup.size());

		return begin() + lookup_it->second;
	}
	iterator find(key_type k) {
		auto lookup_it = find_first_slot(k);
		if (lookup_it->second == pos_sentinel()) {
			return end();
		}

		assert(lookup_it->first == k);
		assert(lookup_it->second < _values.size());
		assert(lookup_it->second < _reverse_lookup.size());

		return begin() + lookup_it->second;
	}

	// checks if the container contains element with specific key
	bool contains(key_type k) const {
		return find(k) != end();
	}


	// Hash policy

	// returns average number of elements per bucket
	float load_factor() const noexcept {
		return _values.size() / float(_lookup.size());
	}

	float max_load_factor() const noexcept {
		return _max_load_factor;
	}
	void max_load_factor(float ml) noexcept {
		_max_load_factor = ml;
	}

	void rehash(size_type count) {
		count = detail::next_prime(count);
		assert(detail::is_prime(count));

		std::vector<std::pair<key_type, pos_type>> new_lookup(
				count, { {}, pos_sentinel() });

		for (const std::pair<key_type, pos_type>& ks : _lookup) {
			if (ks.second == pos_sentinel()) {
				continue;
			}

			// new lookup position
			size_type bucket_pos = ks.first % count;

			// find first free slot
			auto it = std::find_if(new_lookup.begin() + bucket_pos,
					new_lookup.end(),
					[](const std::pair<key_type, pos_type>& search) {
						return search.second == pos_sentinel();
					});

			// No free slot after bucket_pos, find one from beginning.
			if (it == new_lookup.end()) {
				it = std::find_if(new_lookup.begin(),
						new_lookup.begin() + bucket_pos,
						[&](const std::pair<key_type, pos_type>& search) {
							return search.second == pos_sentinel();
						});

				// Something is really screwed up.
				assert(it != new_lookup.end());
			}

			// creates new lookup, assigns the existing element pos
			it->first = ks.first;
			it->second = ks.second;
		}
		_lookup = std::move(new_lookup);
	}


	// Non-member functions

	//	compares the values in the unordered_map
	template <class K, class U>
	friend bool operator==(const flat_unsigned_hashmap<K, U>& lhs,
			const flat_unsigned_hashmap<K, U>& rhs);
	template <class K, class U>
	friend bool operator!=(const flat_unsigned_hashmap<K, U>& lhs,
			const flat_unsigned_hashmap<K, U>& rhs);

private:
	static constexpr pos_type pos_sentinel() noexcept {
		return (std::numeric_limits<pos_type>::max)();
	}

	static constexpr size_type init_count() noexcept {
		return 7;
	}

	auto find_first_slot(key_type key) const {
		size_type search_pos = key % _lookup.size();

		auto it = std::find_if(_lookup.begin() + search_pos, _lookup.end(),
				[&](const std::pair<key_type, pos_type>& search) {
					return search.first == key
							|| search.second == pos_sentinel();
				});
		// No free slot after search_pos, find one from beginning.
		if (it == _lookup.end()) {
			it = std::find_if(_lookup.begin(), _lookup.begin() + search_pos,
					[&](const std::pair<key_type, pos_type>& search) {
						return search.first == key
								|| search.second == pos_sentinel();
					});

			// Something is really screwed up.
			assert(it != _lookup.end());
		}
		return it;
	}
	auto find_first_slot(key_type key) {
		size_type search_pos = key % _lookup.size();
		auto it = std::find_if(_lookup.begin() + search_pos, _lookup.end(),
				[&](const std::pair<key_type, pos_type>& search) {
					return search.first == key
							|| search.second == pos_sentinel();
				});

		// No free slot after search_pos, find one from beginning.
		if (it == _lookup.end()) {
			it = std::find_if(_lookup.begin(), _lookup.begin() + search_pos,
					[&](const std::pair<key_type, pos_type>& search) {
						return search.first == key
								|| search.second == pos_sentinel();
					});

			// Something is really screwed up.
			assert(it != _lookup.begin() + search_pos);
		}
		return it;
	}

	key_type reverse_lookup(size_type idx) const {
		assert(idx < _reverse_lookup.size());
		return _reverse_lookup[idx];
	}

	template <class M>
	std::pair<iterator, bool> minsert(
			key_type key, M&& value, bool assign_found = false) {
		if (load_factor() > _max_load_factor) {
			size_type new_size = _lookup.size() * 2;
			rehash(new_size);
		}

		auto lookup_it = find_first_slot(key);
		if (lookup_it->second != pos_sentinel()) {
			// Found valid key.

			auto it = _values.begin() + lookup_it->second;
			if (assign_found) {
				*it = std::forward<M>(value);
			}
			return { it, false };
		}

		pos_type new_pos = pos_type(_values.size());
		_values.push_back(std::forward<M>(value));
		_reverse_lookup.push_back(key);
		lookup_it->first = key;
		lookup_it->second = new_pos;

		assert(_reverse_lookup.size() == _values.size());
		return { begin() + new_pos, true };
	}


	// max load is at 50% because we want many holes to minimize the insert N.
	// todo : confirm with benchmarks
	float _max_load_factor = 0.50f;
	std::vector<std::pair<key_type, pos_type>> _lookup;
	std::vector<key_type> _reverse_lookup; // used in erase
	std::vector<value_type> _values;
};

template <class Key, class T>
inline bool operator==(const flat_unsigned_hashmap<Key, T>& lhs,
		const flat_unsigned_hashmap<Key, T>& rhs) {
	if (lhs.size() != rhs.size())
		return false;

	for (size_t i = 0; i < lhs.size(); ++i) {
		Key k = lhs.reverse_lookup(i);
		auto it = rhs.find(k);
		if (it == rhs.end()) {
			return false;
		}

		if (*it != lhs._values[i]) {
			return false;
		}
	}

	return true;
}
template <class Key, class T>
inline bool operator!=(const flat_unsigned_hashmap<Key, T>& lhs,
		const flat_unsigned_hashmap<Key, T>& rhs) {
	return !operator==(lhs, rhs);
}
} // namespace fea
