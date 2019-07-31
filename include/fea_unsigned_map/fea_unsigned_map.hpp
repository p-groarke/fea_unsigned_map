#pragma once
#include <cassert>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <tuple>
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
namespace detail {

// std::apply in c++17
template <class Func, class Tuple, size_t... I>
inline constexpr void apply_imp(
		Func&& func, Tuple&& tup, std::index_sequence<I...>) {
	std::forward<Func>(func)(std::get<I>(std::forward<Tuple>(tup))...);
}
template <class Func, class Tuple>
inline constexpr void apply(Func&& func, Tuple&& tup) {
	apply_imp(std::forward<Func>(func), std::forward<Tuple>(tup),
			std::make_index_sequence<std::tuple_size<
					std::remove_reference<Tuple>::type>::value>{});
}
} // namespace detail

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

	// Don't make sense
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
		_reverse_lookup.reserve(reserve_count);
		_values.reserve(reserve_count);
	}
	explicit unsigned_map(
			size_t key_reserve_count, size_t value_reserve_count) {
		_value_indexes.reserve(key_reserve_count);
		_reverse_lookup.reserve(value_reserve_count);
		_values.reserve(value_reserve_count);
	}

	// template <class InputIt>
	// unsigned_map(InputIt first, InputIt last);

	unsigned_map(std::initializer_list<value_type> init) {
		for (const value_type& kv : init) {
			insert(kv);
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

	// returns underlying packed data
	const std::vector<T>& data() const {
		return _values;
	}
	std::vector<T>& data() {
		return _values;
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
		return _value_indexes.max_size() - 1;
	}


	// Modifiers

	// clears the contents
	void clear() noexcept {
		_value_indexes.clear();
		_reverse_lookup.clear();
		_values.clear();
	}

	// inserts elements or nodes
	std::pair<iterator, bool> insert(const value_type& value) {
		return minsert(value.first, value.second);
	}
	std::pair<iterator, bool> insert(value_type&& value) {
		return minsert(value.first, std::move(value.second));
	}
	template <class InputIt>
	void insert(InputIt first, InputIt last);
	void insert(std::initializer_list<value_type> ilist) {
		for (const value_type& kv : ilist) {
			insert(kv);
		}
	}

	// inserts an element or assigns to the current element if the key already
	// exists
	template <class M>
	std::pair<iterator, bool> insert_or_assign(key_type k, M&& obj) {
		return minsert(k, std::forward<M>(obj), true);
	}

	// constructs element in-place
	template <class... Args>
	std::pair<iterator, bool> emplace(Args&&... args) {
		Key k = std::get<0>(std::forward_as_tuple(args...));
		iterator it = find(k);
		if (it != end()) {
			return { it, false };
		}

		resize_indexes_if_needed(k);

		_value_indexes[size_t(k)] = _values.size();
		_reverse_lookup.push_back(k);
		_values.emplace_back(std::get<1>(std::forward_as_tuple(args...)));

		assert(_reverse_lookup.size() == _values.size());
		return { std::prev(_values.end()), true };
	}
	template <class... KeyArgs, class... ObjArgs>
	std::pair<iterator, bool> emplace(std::piecewise_construct_t,
			std::tuple<KeyArgs&&...> key_args,
			std::tuple<ObjArgs&&...> obj_args) {

		Key k;
		detail::apply([&](auto&& key, auto&&...) { k = key; }, key_args);
		iterator it = find(k);

		if (it != end()) {
			return { it, false };
		}

		resize_indexes_if_needed(k);

		_value_indexes[size_t(k)] = _values.size();
		_reverse_lookup.push_back(k);
		detail::apply(
				[this](auto&&... args) {
					_values.emplace_back(std::forward<decltype(args)>(args)...);
				},
				obj_args);

		assert(_reverse_lookup.size() == _values.size());
		return { std::prev(_values.end()), true };
	}

	// inserts in-place if the key does not exist, does nothing if the key
	// exists
	template <class... Args>
	std::pair<iterator, bool> try_emplace(Args&&... args) {
		// Standard emplace behavior doesn't apply, always use try_emplace
		// behavior.
		return emplace(std::forward<Args>(args)...);
	}
	template <class... KeyArgs, class... ObjArgs>
	std::pair<iterator, bool> try_emplace(std::piecewise_construct_t,
			std::tuple<KeyArgs&&...> key_args,
			std::tuple<ObjArgs&&...> obj_args) {
		return emplace(std::piecewise_construct, key_args, obj_args);
	}

	// erases elements
	iterator erase(const_iterator pos) {
		size_t idx = std::distance(_values.cbegin(), pos);
		erase(_reverse_lookup[idx]);

		if (idx >= _reverse_lookup.size())
			return end();

		return find(_reverse_lookup[idx]);
	}
	iterator erase(const_iterator first, const_iterator last) {
		size_t start = std::distance(_values.cbegin(), first);
		size_t finish = std::distance(_values.cbegin(), last) + 1; // inclusive

		std::vector<key_type> keys(std::next(_reverse_lookup.begin(), start),
				std::next(_reverse_lookup.begin(), finish));

		for (key_type k : keys) {
			erase(k);
		}

		if (start >= _reverse_lookup.size())
			return end();
		return find(_reverse_lookup[start]);
	}
	size_type erase(key_type k) {
		iterator it = find(k);
		if (it == end()) {
			return 0;
		}

		iterator last_it = std::prev(end());
		_value_indexes[size_t(k)] = key_sentinel();

		// No need for swap, object is already at end.
		if (last_it == it) {
			_values.pop_back();
			_reverse_lookup.pop_back();

			assert(_reverse_lookup.size() == _values.size());
			return 1;
		}

		// swap & pop
		size_t value_idx = std::distance(_values.begin(), it);
		auto reverse_it = std::next(_reverse_lookup.begin(), value_idx);
		key_type last_key = _reverse_lookup.back();

		std::swap(*it, _values.back());
		_values.pop_back();
		std::swap(*reverse_it, _reverse_lookup.back());
		_reverse_lookup.pop_back();
		_value_indexes[size_t(last_key)] = value_idx;

		assert(_reverse_lookup.size() == _values.size());
		return 1;
	}

	// swaps the contents
	void swap(unsigned_map& other) noexcept {
		_value_indexes.swap(other._value_indexes);
		_reverse_lookup.swap(other._reverse_lookup);
		_values.swap(other._values);
	}


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

		return std::next(begin(), _value_indexes[size_t(k)]);
	}
	const_iterator find(key_type k) const {
		if (!contains(k)) {
			return end();
		}

		return std::next(begin(), _value_indexes[size_t(k)]);
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
		return { it, std::next(it) };
	}
	std::pair<const_iterator, const_iterator> equal_range(key_type k) const {
		const_iterator it = find(k);
		if (it == end()) {
			return { it, it };
		}
		return { it, std::next(it) };
	}


	// Non-member functions

	//	compares the values in the unordered_map
	template <class Key, class T>
	friend bool operator==(
			const unsigned_map<Key, T>& lhs, const unsigned_map<Key, T>& rhs);
	template <class Key, class T>
	friend bool operator!=(
			const unsigned_map<Key, T>& lhs, const unsigned_map<Key, T>& rhs);

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

	template <class M>
	std::pair<iterator, bool> minsert(
			key_type k, M&& obj, bool assign_found = false) {
		iterator it = find(k);
		if (it != end()) {
			if (assign_found) {
				*it = std::forward<M>(obj);
			}
			return { it, false };
		}

		resize_indexes_if_needed(k);

		_value_indexes[size_t(k)] = _values.size();
		_reverse_lookup.push_back(k);
		_values.push_back(std::forward<M>(obj));

		assert(_reverse_lookup.size() == _values.size());
		return { std::prev(_values.end()), true };
	}

	std::vector<size_type> _value_indexes;
	std::vector<key_type> _reverse_lookup;
	std::vector<mapped_type> _values;
};

template <class Key, class T>
inline bool operator==(
		const unsigned_map<Key, T>& lhs, const unsigned_map<Key, T>& rhs) {
	if (lhs.size() != rhs.size())
		return false;

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
inline void swap(fea::unsigned_map<Key, T>& lhs,
		fea::unsigned_map<Key, T>& rhs) noexcept {
	lhs.swap(rhs);
}
} // namespace std