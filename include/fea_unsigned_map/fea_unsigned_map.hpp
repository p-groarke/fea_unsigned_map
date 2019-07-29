#pragma once
#include <functional>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

// TODO : Actual iterators that return pairs (or equivalent). This will enable
// support for range ctors/inserts etc.

// Notes :
// - The container doesn't use const key_type& as the value of a key will
// always be smaller or equally sized to a reference.
// - Doesn't provide hint apis.

namespace fea {
template <class Key, class T>
struct unsigned_map {
	static_assert(std::is_unsigned<Key>::value,
			"number_map : key must be unsigned integer");

	using key_type = Key;
	using mapped_type = T;
	using value_type = std::pair<const key_type, mapped_type>;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	using allocator_type = typename std::vector<T>::allocator_type;

	using reference = value_type&;
	using const_reference = const reference;
	using pointer = typename std::allocator_traits<allocator_type>::pointer;
	using const_pointer =
			typename std::allocator_traits<allocator_type>::const_pointer;

	using iterator = typename std::vector<T>::iterator;
	using const_iterator = typename std::vector<T>::const_iterator;
	using local_iterator = iterator;
	using const_local_iterator = const_iterator;

	// Don't make sense?
	// using hasher = std::hash<key_type>;
	// using key_equal = std::equal_to<key_type>;
	// using node_type;
	// using insert_return_type;

	// Constructors, destructors and assignement

	unsigned_map() = default;
	unsigned_map(const unsigned_map&) = default;
	unsigned_map(unsigned_map&&) = default;
	unsigned_map& operator=(const unsigned_map&) = default;
	unsigned_map& operator=(unsigned_map&&) = default;

	explicit unsigned_map(size_t reserve_count) {
		_value_indexes.reserve(reserve_count);
		_values.reserve(reserve_count);
	}
	explicit unsigned_map(
			size_t key_reserve_count, size_t value_reserve_count) {
		_value_indexes.reserve(key_reserve_count);
		_values.reserve(value_reserve_count);
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
		return _value_indexes.max_size();
	}

	// Modifiers

	// clears the contents
	void clear() noexcept {
		_value_indexes.clear();
		_values.clear();
	}

	// inserts elements or nodes
	std::pair<iterator, bool> insert(const value_type& value) {
		iterator it = find(value.first);
		if (it != end()) {
			return { it, false };
		}

		resize_indexes_if_needed(value.first);

		_value_indexes[size_t(value.first)] = size();
		_values.push_back(value.second);

		return { std::next(begin(), size() - 1), true };
	}
	std::pair<iterator, bool> insert(value_type&& value) {
		iterator it = find(value.first);
		if (it != end()) {
			return { it, false };
		}

		resize_indexes_if_needed(value.first);

		_value_indexes[size_t(value.first)] = size();
		_values.push_back(std::move(value.second));

		return { std::next(begin(), size() - 1), true };
	}

	// inserts an element or assigns to the current element if the key already
	// exists
	template <class M>
	std::pair<iterator, bool> insert_or_assign(key_type k, M&& obj) {
		iterator it = find(k);
		if (it != end()) {
			operator[](k) = std::forward<M>(obj);
			return { it, false };
		}

		resize_indexes_if_needed(value.first);

		_value_indexes[size_t(value.first)] = size();
		_values.push_back(std::forward<M>(obj));

		return { std::next(begin(), size() - 1), true };
	}

	// emplace
	// constructs element in-place

	// emplace_hint
	//	constructs elements in-place using a hint

	// try_emplace
	// inserts in-place if the key does not exist, does nothing if the key
	// exists

	// erase
	// erases elements

	// swap
	// swaps the contents (public member function)

	// extract
	// extracts nodes from the container

	// merge
	//	splices nodes from another container

	// Lookup

	// access specified element with bounds checking
	mapped_type& at(key_type k) {
		return const_cast<mapped_type&>(
				static_cast<const unsigned_map*>(this)->at(k));
	}
	const mapped_type& at(key_type k) const {
		const_iterator it = find(k);
		if (it == end()) {
			throw std::out_of_range{ "unsigned_map : value doesn't exist" };
		}

		return *it;
	}

	// access or insert specified element
	mapped_type& operator[](key_type k) {
		iterator it = find(k);
		if (it != end()) {
			return *it;
		}

		return *insert({ k, {} }).first;
	}

	// returns the number of elements matching specific key (which is 1 or 0,
	// since there are no duplicates)
	size_type count(key_type k) const {
		if (contains(k))
			return 1;

		return 0;
	}

	// finds element with specific key
	iterator find(key_type k) {
		if (!contains(k)) {
			return end();
		}

		return begin() + _value_indexes[size_t(k)];
	}
	const_iterator find(key_type k) const {
		if (!contains(k)) {
			return end();
		}

		return begin() + _value_indexes[size_t(k)];
	}

	// checks if the container contains element with specific key
	bool contains(key_type k) const {
		if (size_t(k) >= _value_indexes.size())
			return false;

		if (_value_indexes[size_t(k)] == key_sentinel())
			return false;

		return true;
	}

	// returns range of elements matching a specific key (in this case, 1 or 0
	// elements)
	std::pair<iterator, iterator> equal_range(key_type k) {
		iterator it = find(k);
		if (it == end()) {
			return { it, it };
		}
		return { it, std::next(it, 1) };
	}
	std::pair<const_iterator, const_iterator> equal_range(key_type k) const {
		const_iterator it = find(k);
		if (it == end()) {
			return { it, it };
		}
		return { it, std::next(it, 1) };
	}

	// Non-member functions

	//	compares the values in the unordered_map
	template <class Key, class T>
	friend bool operator==(
			const unsigned_map<Key, T>& lhs, const unsigned_map<Key, T>& rhs);
	template <class Key, class T>
	friend bool operator!=(
			const unsigned_map<Key, T>& lhs, const unsigned_map<Key, T>& rhs);

	// std::swap(std::unordered_map)
	//	specializes the std::swap algorithm
	// erase_if(std::unordered_map)
	//	Erases all elements satisfying specific criteria

private:
	constexpr size_type key_sentinel() const noexcept {
		return (std::numeric_limits<size_type>::max)();
	}

	void resize_indexes_if_needed(key_type k) {
		if (size_t(k) < _value_indexes.size()) {
			return;
		}

		if (size_t(k) == key_sentinel()) {
			throw std::out_of_range{ "unsigned_map : maximum size reached\n" };
		}

		_value_indexes.resize(size_t(k) + 1, key_sentinel());
	}

	// o(n)
	// size_type valid_key_size() const {
	//	size_type ret = 0;
	//	for (size_type key : _value_indexes) {
	//		if (key == key_sentinel())
	//			continue;
	//		++ret;
	//	}
	//	return ret;
	//}

	std::vector<size_type> _value_indexes;
	std::vector<mapped_type> _values;
};

template <class Key, class T>
inline bool operator==(
		const unsigned_map<Key, T>& lhs, const unsigned_map<Key, T>& rhs) {
	if (lhs.size() != rhs.size())
		return false;
	// if (lhs.valid_key_size() != rhs.valid_key_size()) // slower?
	//	return false;

	using size_type = typename unsigned_map<Key, T>::size_type;
	for (size_type i = 0; i < lhs._value_indexes.size(); ++i) {
		if (lhs._value_indexes[i] == lhs.key_sentinel())
			continue;

		// Key doesn't exist in rhs, not equal.
		if (!rhs.contains(Key(i)))
			return false;

		if (*lhs.find(Key(i)) != *rhs.find(Key(i)))
			return false;
	}
	return true;
}
template <class Key, class T>
inline bool operator!=(
		const unsigned_map<Key, T>& lhs, const unsigned_map<Key, T>& rhs) {
	return !operator==(lhs, rhs);
}

} // namespace fea

namespace std {
template <class Key, class T>
inline void swap(
		fea::unsigned_map<Key, T>& lhs, fea::unsigned_map<Key, T>& rhs) {
}
} // namespace std