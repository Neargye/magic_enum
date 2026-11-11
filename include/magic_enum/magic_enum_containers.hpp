//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  version 0.9.8
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.
// Copyright (c) 2022 - 2023 Bela Schaum <schaumb@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef NEARGYE_MAGIC_ENUM_CONTAINERS_HPP
#define NEARGYE_MAGIC_ENUM_CONTAINERS_HPP

#include "magic_enum.hpp"
#if MAGIC_ENUM_VERSION_MAJOR != 0 || MAGIC_ENUM_VERSION_MINOR != 9 || MAGIC_ENUM_VERSION_PATCH != 8
#  error "magic_enum headers of different versions are mixed. Check the include paths."
#endif

#ifndef MAGIC_ENUM_USE_STD_MODULE
#  include <initializer_list>
#  include <iosfwd>
#  include <iterator>
#  include <string_view>
#endif

#if !defined(MAGIC_ENUM_USE_STD_MODULE) && __has_include(<bit>) && (__cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L))
#  include <bit>
#endif

#if (!defined(__cpp_lib_bitops) || (__cpp_lib_bitops < 201907L)) && defined(_MSC_VER) && !defined(__clang__)
#  include <intrin.h>
#  pragma intrinsic(_BitScanForward)
#  pragma intrinsic(_BitScanReverse)
#  ifdef _WIN64
#    pragma intrinsic(_BitScanForward64)
#    pragma intrinsic(_BitScanReverse64)
#  endif
#endif

#if !defined(MAGIC_ENUM_NO_EXCEPTION) && (defined(__cpp_exceptions) || defined(__EXCEPTIONS) || defined(_CPPUNWIND))
#  ifndef MAGIC_ENUM_USE_STD_MODULE
#    include <stdexcept>
#  endif
#  define MAGIC_ENUM_CONTAINERS_THROW(...) throw (__VA_ARGS__)
#else
#  ifndef MAGIC_ENUM_USE_STD_MODULE
#    include <cstdlib>
#  endif
#  define MAGIC_ENUM_CONTAINERS_THROW(...) std::abort()
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 28020) // MSVC analyzer loses bounds of constexpr sorting buffers.
#  pragma warning(disable : 4459 6244) // Local names can match global names in user code.
#endif

namespace magic_enum::containers {

namespace detail {

using magic_enum::detail::case_insensitive;
using magic_enum::detail::enum_value_equal;
using magic_enum::detail::subtype_v;

template <typename T, typename = void>
inline constexpr bool is_transparent_v{};

template <typename T>
inline constexpr bool is_transparent_v<T, std::void_t<typename T::is_transparent>>{true};

template <typename T1, typename T2>
constexpr bool equal(T1&& t1, T2&& t2) {
  auto first1 = t1.begin();
  auto last1 = t1.end();
  auto first2 = t2.begin();
  auto last2 = t2.end();

  for (; first1 != last1; ++first1, ++first2) {
    if (first2 == last2 || !(*first1 == *first2)) {
      return false;
    }
  }
  return first2 == last2;
}

template <typename Cmp = std::less<>, typename T1, typename T2>
constexpr bool lexicographical_compare(T1&& t1, T2&& t2, Cmp&& cmp = {}) {
  auto first1 = t1.begin();
  auto last1 = t1.end();
  auto first2 = t2.begin();
  auto last2 = t2.end();

  // Same as std::lexicographical_compare, which is constexpr only since C++20.
  for (; (first1 != last1) && (first2 != last2); ++first1, (void)++first2) {
    if (cmp(*first1, *first2)) {
      return true;
    }
    if (cmp(*first2, *first1)) {
      return false;
    }
  }
  return (first1 == last1) && (first2 != last2);
}

template <typename T>
constexpr std::size_t popcount(T x) noexcept {
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L
  return static_cast<std::size_t>(std::popcount(x));
#else
  std::size_t c = 0;
  while (x > 0) {
    x &= x - 1;
    ++c;
  }
  return c;
#endif
}

// Same as std::partition_point, which is constexpr only since C++20.
template <typename T, typename Pred>
constexpr const T* partition_point(const T* first, const T* last, Pred pred) {
  auto count = last - first;
  while (count > 0) {
    const auto step = count / 2;
    if (pred(first[step])) {
      first += step + 1;
      count -= step + 1;
    } else {
      count = step;
    }
  }
  return first;
}

template <typename T, typename K, typename Cmp>
constexpr const T* lower_bound(const T* first, const T* last, const K& key, Cmp&& comp) {
  return detail::partition_point(first, last, [&](const T& value) { return comp(value, key); });
}

template <typename T, typename K, typename Cmp>
constexpr const T* upper_bound(const T* first, const T* last, const K& key, Cmp&& comp) {
  return detail::partition_point(first, last, [&](const T& value) { return !comp(key, value); });
}

template <typename T, typename K, typename Cmp>
constexpr auto equal_range(const T* first, const T* last, const K& key, Cmp&& comp) {
  const auto lower = detail::lower_bound(first, last, key, comp);
  return std::pair{lower, detail::upper_bound(lower, last, key, comp)};
}

template <typename E, typename Cmp>
MAGIC_ENUM_CONSTEVAL bool is_default_order() noexcept {
  if constexpr (!std::is_void_v<E> && !std::is_void_v<Cmp>) {
    constexpr auto& values = enum_values<E>();
    constexpr Cmp cmp{};
    for (std::size_t i = 1; i < values.size(); ++i) {
      if (cmp(values[i], values[i - 1])) {
        return false;
      }
    }
  }
  return true;
}

template <typename E = void, typename Cmp = void, typename = void>
class indexing {
  [[nodiscard]] static constexpr auto get_indices() noexcept {
    constexpr auto& values = enum_values<E>();
    constexpr auto count = values.size();
    const auto* const values_data = values.data();

    // Sorted position to enum_values index.
    std::array<std::size_t, count> order{};
    auto* const order_data = order.data();

    for (std::size_t i = 0; i < count; ++i) {
      order_data[i] = i;
    }

    constexpr Cmp cmp{};

    // Stable merge sort preserves enum_values order for equivalent keys.
    std::array<std::size_t, count> buffer{};
    auto* const buffer_data = buffer.data();
    for (std::size_t width = 1; width < count; width *= 2) {
      for (std::size_t lo = 0; lo < count; lo += 2 * width) {
        const std::size_t mid = lo + width < count ? lo + width : count;
        const std::size_t hi = mid + width < count ? mid + width : count;
        std::size_t a = lo;
        std::size_t b = mid;
        std::size_t k = lo;
        while (a < mid && b < hi) {
          buffer_data[k++] = cmp(values_data[order_data[b]], values_data[order_data[a]]) ? order_data[b++] : order_data[a++];
        }
        while (a < mid) {
          buffer_data[k++] = order_data[a++];
        }
        while (b < hi) {
          buffer_data[k++] = order_data[b++];
        }
      }
      order = buffer;
    }

    std::array<E, count> sorted_values{};
    // enum_values index to sorted position.
    std::array<std::size_t, count> positions{};
    auto* const positions_data = positions.data();
    auto* const sorted_data = sorted_values.data();
    for (std::size_t i = 0; i < count; ++i) {
      positions_data[order_data[i]] = i;
      sorted_data[i] = values_data[order_data[i]];
    }

    struct indices_t {
      std::array<E, count> values;
      std::array<std::size_t, count> positions;
    };
    return indices_t{sorted_values, positions};
  }

  static constexpr auto indices = get_indices();

 public:
  [[nodiscard]] static constexpr const E* begin() noexcept { return indices.values.data(); }

  [[nodiscard]] static constexpr const E* end() noexcept { return indices.values.data() + indices.values.size(); }

  [[nodiscard]] static constexpr const E* it(std::size_t i) noexcept { return indices.values.data() + i; }

  [[nodiscard]] static constexpr optional<std::size_t> at(E val) noexcept {
    if (auto i = enum_index(val)) {
      return indices.positions[*i];
    }
    return {};
  }
};

template <typename E, typename Cmp>
class indexing<E, Cmp, std::enable_if_t<std::is_enum_v<E> && is_default_order<E, Cmp>()>> {
  static constexpr auto& values = enum_values<E>();

 public:
  [[nodiscard]] static constexpr const E* begin() noexcept { return values.data(); }

  [[nodiscard]] static constexpr const E* end() noexcept { return values.data() + values.size(); }

  [[nodiscard]] static constexpr const E* it(std::size_t i) noexcept { return values.data() + i; }

  [[nodiscard]] static constexpr optional<std::size_t> at(E val) noexcept { return enum_index(val); }
};

template <typename Cmp>
struct indexing<void, Cmp, void> {
  using is_transparent = std::true_type;

  template <typename E>
  [[nodiscard]] static constexpr optional<std::size_t> at(E val) noexcept {
    return indexing<E, Cmp>::at(val);
  }
};

template <typename E = void, typename Cmp = std::less<>>
struct name_sort_impl {
  [[nodiscard]] constexpr bool operator()(E e1, E e2) const { return Cmp{}(enum_name(e1), enum_name(e2)); }
};

template <typename Cmp>
struct name_sort_impl<void, Cmp> {
  using is_transparent = std::true_type;

  template <typename C = Cmp, typename = void>
  struct FullCmp : C {};

  template <typename C>
  struct FullCmp<C, std::enable_if_t<!std::is_invocable_v<const C&, string_view, string_view> && std::is_invocable_v<C&, const char_type&, const char_type&>>> {
    [[nodiscard]] constexpr bool operator()(string_view s1, string_view s2) const { return lexicographical_compare<C>(s1, s2); }
  };

  template <typename T>
  [[nodiscard]] static constexpr decltype(auto) cmp_arg(T&& value) {
    if constexpr (std::is_enum_v<std::decay_t<T>>) {
      return enum_name(value);
    } else if constexpr (std::is_constructible_v<string_view, T>) {
      return string_view{std::forward<T>(value)};
    } else {
      return std::forward<T>(value);
    }
  }

  template <typename T>
  using cmp_arg_t = decltype(cmp_arg(std::declval<T>()));

  template <typename E1, typename E2>
  [[nodiscard]] constexpr std::enable_if_t<
      (std::is_enum_v<std::decay_t<E1>> || std::is_enum_v<std::decay_t<E2>>) &&
      (!std::is_enum_v<std::decay_t<E1>> || !std::is_enum_v<std::decay_t<E2>> || std::is_same_v<std::decay_t<E1>, std::decay_t<E2>>) &&
      std::is_invocable_r_v<bool, const FullCmp<>&, cmp_arg_t<E1>, cmp_arg_t<E2>>,
      bool>
  operator()(E1&& e1, E2&& e2) const {
    constexpr FullCmp<> cmp{};
    return cmp(cmp_arg(std::forward<E1>(e1)), cmp_arg(std::forward<E2>(e2)));
  }
};

struct raw_access_t {};

template <typename Parent, typename Iterator, typename Predicate>
struct FilteredIterator {
  Parent parent;
  Iterator first;
  Iterator current;
  Predicate predicate;

  using iterator_category = std::bidirectional_iterator_tag;
  using reference = typename std::iterator_traits<Iterator>::reference;
  using value_type = std::remove_cv_t<std::remove_reference_t<reference>>;
  using difference_type = std::ptrdiff_t;
  using pointer = std::add_pointer_t<std::remove_reference_t<reference>>;

  constexpr FilteredIterator() noexcept = default;

  constexpr FilteredIterator(Parent p, Iterator begin, Iterator curr, Predicate pred = {})
      : parent(p), first(std::move(begin)), current(std::move(curr)), predicate{std::move(pred)} {
    if (current == first && !predicate(parent, current)) {
      ++*this;
    }
  }

  [[nodiscard]] constexpr reference operator*() const { return *current; }

  [[nodiscard]] constexpr pointer operator->() const { return std::addressof(**this); }

  constexpr FilteredIterator& operator++() {
    current = predicate.next(parent, first, current);
    return *this;
  }

  [[nodiscard]] constexpr FilteredIterator operator++(int) {
    FilteredIterator cp = *this;
    ++*this;
    return cp;
  }

  constexpr FilteredIterator& operator--() {
    do {
      --current;
    } while (current != first && !predicate(parent, current));
    return *this;
  }

  [[nodiscard]] constexpr FilteredIterator operator--(int) {
    FilteredIterator cp = *this;
    --*this;
    return cp;
  }

  [[nodiscard]] friend constexpr bool operator==(const FilteredIterator& lhs, const FilteredIterator& rhs) { return lhs.current == rhs.current; }

  [[nodiscard]] friend constexpr bool operator!=(const FilteredIterator& lhs, const FilteredIterator& rhs) { return lhs.current != rhs.current; }
};

template <typename T>
constexpr int countr_zero(T x) noexcept {
  constexpr int digits = std::numeric_limits<T>::digits;
  if (x == 0) {
    return digits;
  }
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L
  return std::countr_zero(x);
#elif defined(_MSC_VER) && !defined(__clang__)
  // MSVC _BitScan* intrinsics cannot run at compile time, so constant evaluation uses a plain loop.
#  if _MSC_VER >= 1925
  if (!__builtin_is_constant_evaluated()) {
    unsigned long index = 0;
    if constexpr (sizeof(T) <= sizeof(unsigned long)) {
      _BitScanForward(&index, static_cast<unsigned long>(x));
    } else {
#    ifdef _WIN64
      _BitScanForward64(&index, static_cast<unsigned __int64>(x));
#    else
      if (!_BitScanForward(&index, static_cast<unsigned long>(x))) {
        _BitScanForward(&index, static_cast<unsigned long>(x >> 32));
        index += 32;
      }
#    endif
    }
    return static_cast<int>(index);
  }
#  endif
  for (int i = 0; i < digits; ++i) {
    if (((x >> i) & 1) != 0) {
      return i;
    }
  }
  return digits;
#else
  if constexpr (sizeof(T) <= sizeof(unsigned int)) {
    return __builtin_ctz(static_cast<unsigned int>(x));
  } else if constexpr (sizeof(T) <= sizeof(unsigned long)) {
    return __builtin_ctzl(static_cast<unsigned long>(x));
  } else {
    return __builtin_ctzll(static_cast<unsigned long long>(x));
  }
#endif
}

template <typename T>
constexpr int countl_zero(T x) noexcept {
  constexpr int digits = std::numeric_limits<T>::digits;
  if (x == 0) {
    return digits;
  }
#if defined(__cpp_lib_bitops) && __cpp_lib_bitops >= 201907L
  return std::countl_zero(x);
#elif defined(_MSC_VER) && !defined(__clang__)
  // MSVC _BitScan* intrinsics cannot run at compile time, so constant evaluation uses a plain loop.
#  if _MSC_VER >= 1925
  if (!__builtin_is_constant_evaluated()) {
    unsigned long index = 0;
    if constexpr (sizeof(T) <= sizeof(unsigned long)) {
      _BitScanReverse(&index, static_cast<unsigned long>(x));
    } else {
#    ifdef _WIN64
      _BitScanReverse64(&index, static_cast<unsigned __int64>(x));
#    else
      if (_BitScanReverse(&index, static_cast<unsigned long>(x >> 32))) {
        index += 32;
      } else {
        _BitScanReverse(&index, static_cast<unsigned long>(x));
      }
#    endif
    }
    return digits - static_cast<int>(index) - 1;
  }
#  endif
  for (int i = digits - 1; i >= 0; --i) {
    if (((x >> i) & 1) != 0) {
      return digits - 1 - i;
    }
  }
  return digits;
#else
  // Subtract the leading zeros introduced by promotion to the intrinsic's argument type.
  if constexpr (sizeof(T) <= sizeof(unsigned int)) {
    return __builtin_clz(static_cast<unsigned int>(x)) - (std::numeric_limits<unsigned int>::digits - digits);
  } else if constexpr (sizeof(T) <= sizeof(unsigned long)) {
    return __builtin_clzl(static_cast<unsigned long>(x)) - (std::numeric_limits<unsigned long>::digits - digits);
  } else {
    return __builtin_clzll(static_cast<unsigned long long>(x)) - (std::numeric_limits<unsigned long long>::digits - digits);
  }
#endif
}

template <typename T>
constexpr int bit_width(T x) noexcept {
#if defined(__cpp_lib_int_pow2) && __cpp_lib_int_pow2 >= 202002L
  // Before LWG 3656, std::bit_width returns T.
  return static_cast<int>(std::bit_width(x));
#else
  return std::numeric_limits<T>::digits - countl_zero(x);
#endif
}

template <typename E, typename Index>
MAGIC_ENUM_CONSTEVAL bool valid_indexing() noexcept {
  constexpr std::size_t count = enum_count<E>();
  if constexpr (count == 0) {
    return false;
  } else {
    std::array<bool, count> used_indices{};
    for (const auto value : enum_values<E>()) {
      const auto index = Index::at(value);
      if (!index || *index >= count || used_indices[*index]) {
        return false;
      }
      used_indices[*index] = true;
    }
    return true;
  }
}

// set keeps its keys in a bitset, so its index keeps a named zero as a key.
template <typename Index>
struct key_indexing : Index {};

// Positions of a bitset. A flags bitset holds only bits and skips a named zero, array and set keep it as a key.
template <typename E, typename Index, bool Keys = false>
struct bit_domain {
  static constexpr bool skip_zero = !Keys && is_flags_enum_v<E> && static_cast<bool>(enum_index(static_cast<E>(0)));

  static constexpr std::size_t zero = skip_zero ? *Index::at(static_cast<E>(0)) : 0;

  static constexpr std::size_t count = enum_count<E>() - (skip_zero ? 1 : 0);

  [[nodiscard]] static constexpr bool is_skipped(E value) noexcept { return skip_zero && enum_value_equal(value, 0); }

  [[nodiscard]] static constexpr optional<std::size_t> at(E value) noexcept {
    if constexpr (skip_zero) {
      if (const auto i = Index::at(value); i && *i != zero) {
        return *i > zero ? *i - 1 : *i;
      }
      return {};
    } else {
      return Index::at(value);
    }
  }

  [[nodiscard]] static constexpr auto it(std::size_t i) noexcept {
    if constexpr (skip_zero) {
      return Index::it(i < zero ? i : i + 1);
    } else {
      return Index::it(i);
    }
  }
};

template <typename E, typename Index>
struct bit_domain<E, key_indexing<Index>> : bit_domain<E, Index, true> {};

// Bind the enum type to provide iteration for transparent indexing.
template <typename E, typename Cmp>
struct bit_domain<E, indexing<void, Cmp>> : bit_domain<E, indexing<E, Cmp>> {};

} // namespace magic_enum::containers::detail

template <typename E = void>
using name_less = detail::name_sort_impl<E>;

template <typename E = void>
using name_greater = detail::name_sort_impl<E, std::greater<>>;

using name_less_case_insensitive = detail::name_sort_impl<void, detail::case_insensitive<std::less<>>>;

using name_greater_case_insensitive = detail::name_sort_impl<void, detail::case_insensitive<std::greater<>>>;

template <typename E = void>
using default_indexing = detail::indexing<E>;

template <typename Cmp = std::less<>>
using comparator_indexing = detail::indexing<void, Cmp>;

// Array of V indexed by enum values.
template <typename E, typename V, typename Index = default_indexing<E>>
struct array {
  static_assert(std::is_enum_v<E>, "magic_enum::containers::array requires enum type.");
  static_assert(detail::valid_indexing<E, Index>(), "magic_enum::containers::array requires non-empty reflected enum and valid indexing.");

  using index_type = Index;
  using container_type = std::array<V, enum_count<E>()>;

  using value_type = typename container_type::value_type;
  using size_type = typename container_type::size_type;
  using difference_type = typename container_type::difference_type;
  using reference = typename container_type::reference;
  using const_reference = typename container_type::const_reference;
  using pointer = typename container_type::pointer;
  using const_pointer = typename container_type::const_pointer;
  using iterator = typename container_type::iterator;
  using const_iterator = typename container_type::const_iterator;
  using reverse_iterator = typename container_type::reverse_iterator;
  using const_reverse_iterator = typename container_type::const_reverse_iterator;

  constexpr reference at(E pos) { return a[checked_index(pos)]; }

  constexpr const_reference at(E pos) const { return a[checked_index(pos)]; }

  [[nodiscard]] constexpr reference operator[](E pos) {
    auto i = index_type::at(pos);
    return MAGIC_ENUM_ASSERT(i && *i < a.size()), a[*i];
  }

  [[nodiscard]] constexpr const_reference operator[](E pos) const {
    auto i = index_type::at(pos);
    return MAGIC_ENUM_ASSERT(i && *i < a.size()), a[*i];
  }

  [[nodiscard]] constexpr reference front() noexcept { return a.front(); }

  [[nodiscard]] constexpr const_reference front() const noexcept { return a.front(); }

  [[nodiscard]] constexpr reference back() noexcept { return a.back(); }

  [[nodiscard]] constexpr const_reference back() const noexcept { return a.back(); }

  [[nodiscard]] constexpr pointer data() noexcept { return a.data(); }

  [[nodiscard]] constexpr const_pointer data() const noexcept { return a.data(); }

  [[nodiscard]] constexpr iterator begin() noexcept { return a.begin(); }

  [[nodiscard]] constexpr const_iterator begin() const noexcept { return a.begin(); }

  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return a.cbegin(); }

  [[nodiscard]] constexpr iterator end() noexcept { return a.end(); }

  [[nodiscard]] constexpr const_iterator end() const noexcept { return a.end(); }

  [[nodiscard]] constexpr const_iterator cend() const noexcept { return a.cend(); }

  [[nodiscard]] constexpr reverse_iterator rbegin() noexcept { return a.rbegin(); }

  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return a.rbegin(); }

  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return a.crbegin(); }

  [[nodiscard]] constexpr reverse_iterator rend() noexcept { return a.rend(); }

  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return a.rend(); }

  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return a.crend(); }

  [[nodiscard]] constexpr bool empty() const noexcept { return a.empty(); }

  [[nodiscard]] constexpr size_type size() const noexcept { return a.size(); }

  [[nodiscard]] constexpr size_type max_size() const noexcept { return a.max_size(); }

  constexpr void fill(const V& value) {
    for (auto& v : a) {
      v = value;
    }
  }

  constexpr void swap(array& other) noexcept(std::is_nothrow_move_constructible_v<V> && std::is_nothrow_move_assignable_v<V>) {
    for (std::size_t i = 0; i < a.size(); ++i) {
      auto v = std::move(other.a[i]);
      other.a[i] = std::move(a[i]);
      a[i] = std::move(v);
    }
  }

  [[nodiscard]] friend constexpr bool operator==(const array& a1, const array& a2) { return detail::equal(a1, a2); }

  [[nodiscard]] friend constexpr bool operator!=(const array& a1, const array& a2) { return !(a1 == a2); }

  [[nodiscard]] friend constexpr bool operator<(const array& a1, const array& a2) { return detail::lexicographical_compare(a1, a2); }

  [[nodiscard]] friend constexpr bool operator<=(const array& a1, const array& a2) { return !(a2 < a1); }

  [[nodiscard]] friend constexpr bool operator>(const array& a1, const array& a2) { return a2 < a1; }

  [[nodiscard]] friend constexpr bool operator>=(const array& a1, const array& a2) { return !(a1 < a2); }

  container_type a;

 private:
  constexpr std::size_t checked_index(E pos) const {
    if (auto index = index_type::at(pos); index && *index < a.size()) {
      return *index;
    }
    MAGIC_ENUM_CONTAINERS_THROW(std::out_of_range("magic_enum::containers::array::at: Unrecognized position"));
  }
};

namespace detail {

template <typename E, typename T, std::size_t N, std::size_t... J>
constexpr array<E, std::remove_cv_t<T>> to_array_impl(T(&a)[N], std::index_sequence<J...>) {
  return {{a[J]...}};
}

template <typename E, typename T, std::size_t N, std::size_t... J>
constexpr array<E, std::remove_cv_t<T>> to_array_impl(T(&&a)[N], std::index_sequence<J...>) {
  return {{std::move(a[J])...}};
}

} // namespace magic_enum::containers::detail

template <typename E, typename T, std::size_t N>
[[nodiscard]] constexpr std::enable_if_t<(enum_count<E>() == N), array<E, std::remove_cv_t<T>>> to_array(T(&a)[N]) {
  return detail::to_array_impl<E>(a, std::make_index_sequence<N>{});
}

template <typename E, typename T, std::size_t N>
[[nodiscard]] constexpr std::enable_if_t<(enum_count<E>() == N), array<E, std::remove_cv_t<T>>> to_array(T(&&a)[N]) {
  return detail::to_array_impl<E>(std::move(a), std::make_index_sequence<N>{});
}

template <typename E, typename... Ts>
[[nodiscard]] constexpr std::enable_if_t<(enum_count<E>() == sizeof...(Ts)), array<E, std::remove_cv_t<std::common_type_t<Ts...>>>> make_array(Ts&&... ts) {
  return {{std::forward<Ts>(ts)...}};
}

inline constexpr detail::raw_access_t raw_access{};

template <typename E, typename Cmp>
class set;

// Bitset indexed by enum values.
template <typename E, typename Index = default_indexing<E>>
class bitset {
  static_assert(std::is_enum_v<E>, "magic_enum::containers::bitset requires enum type.");
  static_assert(detail::valid_indexing<E, Index>(), "magic_enum::containers::bitset requires non-empty reflected enum and valid indexing.");

  using domain = detail::bit_domain<E, Index>;
  static_assert(enum_count<E>() == 0 || domain::count > 0, "magic_enum::containers::bitset requires at least one flag bit, a named zero has no bit.");

  using base_type = std::conditional_t<domain::count <= 8,  std::uint_least8_t,
                    std::conditional_t<domain::count <= 16, std::uint_least16_t,
                    std::conditional_t<domain::count <= 32, std::uint_least32_t,
                                                            std::uint_least64_t>>>;

  static constexpr std::size_t bits_per_base = sizeof(base_type) * 8;
  static constexpr std::size_t base_type_count = (domain::count > 0 ? (domain::count - 1) / bits_per_base + 1 : 0);
  static constexpr std::size_t unused_bits = base_type_count * bits_per_base - domain::count;
  static constexpr base_type last_word_mask = [] {
    if constexpr (unused_bits == 0) {
      return (std::numeric_limits<base_type>::max)();
    } else {
      return static_cast<base_type>((base_type{1} << (bits_per_base - unused_bits)) - 1);
    }
  }();

  [[nodiscard]] static constexpr base_type bit_mask(std::size_t index) noexcept {
    return static_cast<base_type>(base_type{1} << (index % bits_per_base));
  }

  [[nodiscard]] static constexpr base_type least_significant_bit(base_type value) noexcept {
    MAGIC_ENUM_ASSERT(value != 0);
    return bit_mask(static_cast<std::size_t>(detail::countr_zero(value)));
  }

  template <typename parent_t = bitset*>
  class reference_impl {
    friend class bitset;

    parent_t parent;
    std::size_t word_index;
    base_type mask;

    constexpr reference_impl(parent_t p, std::size_t i) noexcept : parent(p), word_index(base_type_count == 1 ? 0 : i / bits_per_base), mask(bit_mask(i)) {}

   public:
    constexpr reference_impl(const reference_impl&) noexcept = default;

    constexpr reference_impl& operator=(bool v) noexcept {
      if (v) {
        parent->a[word_index] |= mask;
      } else {
        parent->a[word_index] &= ~mask;
      }
      return *this;
    }

    constexpr reference_impl& operator=(const reference_impl& v) noexcept { return *this = static_cast<bool>(v); }

    [[nodiscard]] constexpr operator bool() const noexcept { return (parent->a[word_index] & mask) > 0; }

    [[nodiscard]] constexpr bool operator~() const noexcept { return !static_cast<bool>(*this); }

    constexpr reference_impl& flip() noexcept { return *this = ~*this; }
  };

  template <typename T>
  [[nodiscard]] constexpr T to_(detail::raw_access_t) const {
    if constexpr (std::numeric_limits<T>::digits < std::numeric_limits<base_type>::digits) {
      if (a[0] > static_cast<base_type>((std::numeric_limits<T>::max)())) {
        MAGIC_ENUM_CONTAINERS_THROW(std::overflow_error("magic_enum::containers::bitset::to: Cannot represent enum in this type"));
      }
    }
    // Skip the loop for a single word, otherwise MSVC /analyze warns that it never runs.
    if constexpr (base_type_count > 1) {
      for (std::size_t i = 1; i < base_type_count; ++i) {
        if (a[i] != 0) {
          MAGIC_ENUM_CONTAINERS_THROW(std::overflow_error("magic_enum::containers::bitset::to: Cannot represent enum in this type"));
        }
      }
    }
    return static_cast<T>(a[0]);
  }

  template <typename parent_t = bitset*>
  class iterator_impl {
    friend class bitset;

    parent_t parent = nullptr;
    std::size_t word_index = 0;
    base_type mask = 0;

   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = E;
    using difference_type = std::ptrdiff_t;
    using pointer = const E*;
    using reference = const E&;

    constexpr iterator_impl() noexcept = default;

    template <typename OtherParent, typename = std::enable_if_t<std::is_convertible_v<OtherParent, parent_t>>>
    constexpr iterator_impl(const iterator_impl<OtherParent>& other) noexcept
        : parent(other.parent), word_index(other.word_index), mask(other.mask) {}

   private:
    template <typename OtherParent>
    friend class iterator_impl;

    constexpr iterator_impl(parent_t p, std::size_t i) noexcept : iterator_impl(p, i / bits_per_base, bit_mask(i)) {}

    constexpr iterator_impl(parent_t p, std::size_t num, base_type bit) noexcept : parent(p), word_index(num), mask(bit) {}

    [[nodiscard]] static constexpr iterator_impl begin(parent_t p) noexcept {
      for (std::size_t word_index = 0; word_index < base_type_count; ++word_index) {
        if (p->a[word_index] > 0) {
          return iterator_impl(p, word_index, least_significant_bit(p->a[word_index]));
        }
      }
      return end(p);
    }

    [[nodiscard]] static constexpr iterator_impl end(parent_t p) noexcept { return iterator_impl(p, domain::count); }

   public:
    [[nodiscard]] constexpr reference operator*() const noexcept { return *domain::it(word_index * bits_per_base + static_cast<std::size_t>(detail::countr_zero(mask))); }

    [[nodiscard]] constexpr pointer operator->() const noexcept { return std::addressof(**this); }

    constexpr iterator_impl& operator++() noexcept {
      const auto lower_bits = static_cast<base_type>((mask << 1) - 1);
      auto remaining_bits = static_cast<base_type>(parent->a[word_index] & static_cast<base_type>(~lower_bits));
      while (remaining_bits == 0 && ++word_index < base_type_count) {
        remaining_bits = parent->a[word_index];
      }
      if (word_index >= base_type_count) {
        return *this = end(parent);
      }
      mask = least_significant_bit(remaining_bits);
      return *this;
    }

    [[nodiscard]] constexpr iterator_impl operator++(int) noexcept {
      iterator_impl cp = *this;
      ++*this;
      return cp;
    }

    constexpr iterator_impl& operator--() noexcept {
      base_type search_mask = 0;
      if (word_index >= base_type_count) {
        word_index = base_type_count - 1;
        search_mask = last_word_mask;
      } else if (word_index == base_type_count - 1 && mask > last_word_mask) {
        search_mask = last_word_mask;
      } else {
        search_mask = static_cast<base_type>(mask - 1);
      }

      auto remaining_bits = static_cast<base_type>(parent->a[word_index] & search_mask);
      while (remaining_bits == 0 && word_index != 0) {
        remaining_bits = parent->a[--word_index];
      }
      if (remaining_bits == 0) {
        word_index = (std::numeric_limits<std::size_t>::max)();
        mask = static_cast<base_type>(base_type{1} << (bits_per_base - 1));
        return *this;
      }
      mask = static_cast<base_type>(base_type{1} << (detail::bit_width(remaining_bits) - 1));
      return *this;
    }

    [[nodiscard]] constexpr iterator_impl operator--(int) noexcept {
      iterator_impl cp = *this;
      --*this;
      return cp;
    }

    [[nodiscard]] friend constexpr bool operator==(const iterator_impl& lhs, const iterator_impl& rhs) { return lhs.parent == rhs.parent && lhs.word_index == rhs.word_index && lhs.mask == rhs.mask; }

    [[nodiscard]] friend constexpr bool operator!=(const iterator_impl& lhs, const iterator_impl& rhs) { return !(lhs == rhs); }
  };

 public:
  using index_type = Index;
  using container_type = std::array<base_type, base_type_count>;
  using reference = reference_impl<>;
  using const_reference = reference_impl<const bitset*>;
  using iterator = iterator_impl<>;
  using const_iterator = iterator_impl<const bitset*>;

  constexpr bitset() noexcept : a{{}} {}

  constexpr explicit bitset(detail::raw_access_t) noexcept : a{{}} {}

  constexpr explicit bitset(detail::raw_access_t, unsigned long long val) : a{{}} {
    if constexpr (domain::count < std::numeric_limits<unsigned long long>::digits) {
      if ((val >> domain::count) != 0) {
        MAGIC_ENUM_CONTAINERS_THROW(std::out_of_range("magic_enum::containers::bitset::constructor: Upper bit set in raw number"));
      }
    }
    a[0] = static_cast<base_type>(val);
  }

  constexpr explicit bitset(detail::raw_access_t, string_view sv, string_view::size_type pos = 0, string_view::size_type n = string_view::npos, char_type zero = char_type{'0'}, char_type one = char_type{'1'})
      : a{{}} {
    if (pos > sv.size()) {
      MAGIC_ENUM_CONTAINERS_THROW(std::out_of_range("magic_enum::containers::bitset::constructor: Position is past the end of raw string"));
    }
    std::size_t i = 0;
    for (auto c : sv.substr(pos, n)) {
      if (c == one) {
        if (i >= domain::count) {
          MAGIC_ENUM_CONTAINERS_THROW(std::out_of_range("magic_enum::containers::bitset::constructor: Upper bit set in raw string"));
        }
        reference{this, i} = true;
      } else if (c != zero) {
        MAGIC_ENUM_CONTAINERS_THROW(std::invalid_argument("magic_enum::containers::bitset::constructor: Unrecognized character in raw string"));
      }
      ++i;
    }
  }

  // A template, so a literal 0 selects the integer constructor.
  template <typename C, std::enable_if_t<std::is_same_v<C, char_type>, int> = 0>
  constexpr explicit bitset(detail::raw_access_t, const C* str, std::size_t n = ~std::size_t{0}, char_type zero = char_type{'0'}, char_type one = char_type{'1'})
      : bitset(detail::raw_access_t{}, n == string_view::npos ? string_view{str} : string_view{str, n}, 0, n, zero, one) {}

  // bitset(0) would otherwise read a null string.
  bitset(std::nullptr_t) = delete;

  constexpr bitset(std::initializer_list<E> starters) : a{{}} {
    for (const auto f : starters) {
      if constexpr (is_flags_enum_v<E>) {
        *this |= bitset(f);
      } else {
        set(f);
      }
    }
  }

  template <typename V, std::enable_if_t<std::is_same_v<V, E> && is_flags_enum_v<V>, int> = 0>
  constexpr explicit bitset(V starter) : a{{}} {
    auto u = enum_underlying(starter);
    for (E v : enum_values<E>()) {
      if (u == 0) {
        break;
      }
      if (auto ul = enum_underlying(v); (ul & u) != 0) {
        u ^= ul;
        (*this)[v] = true;
      }
    }
    if (u != 0) {
      MAGIC_ENUM_CONTAINERS_THROW(std::invalid_argument("magic_enum::containers::bitset::constructor: Unrecognized enum value in flag"));
    }
  }

  template <typename Cmp = std::equal_to<>>
  constexpr explicit bitset(string_view sv, char_type sep = char_type{'|'}, Cmp&& cmp = {}) : a{{}} {
    if (!set_names(sv, sep, cmp)) {
      MAGIC_ENUM_CONTAINERS_THROW(std::invalid_argument("magic_enum::containers::bitset::constructor: Unrecognized enum value in string"));
    }
  }

  [[nodiscard]] friend constexpr bool operator==(const bitset& lhs, const bitset& rhs) noexcept { return detail::equal(lhs.a, rhs.a); }

  [[nodiscard]] friend constexpr bool operator!=(const bitset& lhs, const bitset& rhs) noexcept { return !(lhs == rhs); }

  [[nodiscard]] constexpr bool operator[](E pos) const {
    auto i = domain::at(pos);
    MAGIC_ENUM_ASSERT(i && *i < size());
    return test_at(*i);
  }

  [[nodiscard]] constexpr reference operator[](E pos) {
    auto i = domain::at(pos);
    MAGIC_ENUM_ASSERT(i && *i < size());
    return reference{this, *i};
  }

  [[nodiscard]] constexpr iterator begin() noexcept { return iterator::begin(this); }

  [[nodiscard]] constexpr const_iterator begin() const noexcept { return const_iterator::begin(this); }

  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return const_iterator::begin(this); }

  [[nodiscard]] constexpr iterator end() noexcept { return iterator::end(this); }

  [[nodiscard]] constexpr const_iterator end() const noexcept { return const_iterator::end(this); }

  [[nodiscard]] constexpr const_iterator cend() const noexcept { return const_iterator::end(this); }

  [[nodiscard]] constexpr const_iterator find(E pos) const noexcept {
    if (auto i = domain::at(pos); i && *i < size() && test_at(*i)) {
      return const_iterator(this, *i);
    }
    return end();
  }

  [[nodiscard]] constexpr iterator find(E pos) noexcept {
    if (auto i = domain::at(pos); i && *i < size() && test_at(*i)) {
      return iterator(this, *i);
    }
    return end();
  }

  [[nodiscard]] constexpr bool test(E pos) const {
    return test_at(checked_index(pos, "magic_enum::containers::bitset::test: Unrecognized position"));
  }

  [[nodiscard]] constexpr bool all() const noexcept {
    for (std::size_t i = 0; i + 1 < base_type_count; ++i) {
      if (a[i] != (std::numeric_limits<base_type>::max)()) {
        return false;
      }
    }
    return a[base_type_count - 1] == last_word_mask;
  }

  [[nodiscard]] constexpr bool any() const noexcept {
    for (auto& v : a) {
      if (v > 0) {
        return true;
      }
    }
    return false;
  }

  [[nodiscard]] constexpr bool none() const noexcept { return !any(); }

  [[nodiscard]] constexpr std::size_t count() const noexcept {
    std::size_t c = 0;
    for (auto& v : a) {
      c += detail::popcount(v);
    }
    return c;
  }

  [[nodiscard]] constexpr std::size_t size() const noexcept { return domain::count; }

  [[nodiscard]] constexpr std::size_t max_size() const noexcept { return domain::count; }

  constexpr bitset& operator&=(const bitset& other) noexcept {
    for (std::size_t i = 0; i < base_type_count; ++i) {
      a[i] &= other.a[i];
    }
    return *this;
  }

  constexpr bitset& operator|=(const bitset& other) noexcept {
    for (std::size_t i = 0; i < base_type_count; ++i) {
      a[i] |= other.a[i];
    }
    return *this;
  }

  constexpr bitset& operator^=(const bitset& other) noexcept {
    for (std::size_t i = 0; i < base_type_count; ++i) {
      a[i] ^= other.a[i];
    }
    return *this;
  }

  [[nodiscard]] constexpr bitset operator~() const noexcept { return bitset(*this).flip(); }

  constexpr bitset& set() noexcept {
    for (std::size_t i = 0; i + 1 < base_type_count; ++i) {
      a[i] = (std::numeric_limits<base_type>::max)();
    }
    a[base_type_count - 1] = last_word_mask;
    return *this;
  }

  constexpr bitset& set(E pos, bool value = true) {
    reference{this, checked_index(pos, "magic_enum::containers::bitset::set: Unrecognized position")} = value;
    return *this;
  }

  constexpr bitset& reset() noexcept { return *this = bitset{}; }

  constexpr bitset& reset(E pos) { return set(pos, false); }

  constexpr bitset& flip() noexcept {
    for (auto& value : a) {
      value = static_cast<base_type>(~value);
    }
    a[base_type_count - 1] &= last_word_mask;
    return *this;
  }

  constexpr bitset& flip(E pos) {
    reference{this, checked_index(pos, "magic_enum::containers::bitset::flip: Unrecognized position")}.flip();
    return *this;
  }

  [[nodiscard]] friend constexpr bitset operator&(const bitset& lhs, const bitset& rhs) noexcept { return bitset(lhs) &= rhs; }

  [[nodiscard]] friend constexpr bitset operator|(const bitset& lhs, const bitset& rhs) noexcept { return bitset(lhs) |= rhs; }

  [[nodiscard]] friend constexpr bitset operator^(const bitset& lhs, const bitset& rhs) noexcept { return bitset(lhs) ^= rhs; }

  template <typename V = E>
  [[nodiscard]] constexpr explicit operator std::enable_if_t<is_flags_enum_v<V>, E>() const {
    underlying_type_t<E> res = 0;
    for (const auto value : *this) {
      res |= enum_underlying(value);
    }
    return static_cast<E>(res);
  }

  template <typename String = string>
  [[nodiscard]] constexpr String to_string(char_type sep = char_type{'|'}) const {
    static_assert(std::is_same_v<typename String::value_type, char_type>, "magic_enum::containers::bitset::to_string requires String::value_type to be magic_enum::char_type.");
    String name;

    for (const auto& e : enum_values<E>()) {
      if (!domain::is_skipped(e) && test(e)) {
        if (!name.empty()) {
          name.append(1, sep);
        }
        auto n = enum_name(e);
        name.append(n.data(), n.size());
      }
    }
    if constexpr (domain::skip_zero) {
      if (none()) {
        const auto n = enum_name(static_cast<E>(0));
        name.append(n.data(), n.size());
      }
    }
    return name;
  }

  template <typename String = string>
  [[nodiscard]] constexpr String to_string(detail::raw_access_t, char_type zero = char_type{'0'}, char_type one = char_type{'1'}) const {
    static_assert(std::is_same_v<typename String::value_type, char_type>, "magic_enum::containers::bitset::to_string requires String::value_type to be magic_enum::char_type.");
    String name;
    name.reserve(size());
    for (std::size_t i = 0; i < size(); ++i) {
      name.append(1, const_reference{this, i} ? one : zero);
    }
    return name;
  }

  [[nodiscard]] constexpr unsigned long long to_ullong(detail::raw_access_t raw) const { return to_<unsigned long long>(raw); }

  [[nodiscard]] constexpr unsigned long to_ulong(detail::raw_access_t raw) const { return to_<unsigned long>(raw); }

  template <typename Traits>
  friend std::basic_ostream<char_type, Traits>& operator<<(std::basic_ostream<char_type, Traits>& o, const bitset& bs) {
    const auto s = bs.to_string();
    return o << std::basic_string_view<char_type, Traits>{s.data(), s.size()};
  }

  template <typename Traits>
  friend std::basic_istream<char_type, Traits>& operator>>(std::basic_istream<char_type, Traits>& i, bitset& bs) {
    std::basic_string<char_type, Traits> s;
    if (i >> s) {
      bitset result;
      if (result.set_names(string_view{s.data(), s.size()})) {
        bs = result;
      } else {
        i.setstate(std::basic_istream<char_type, Traits>::failbit);
      }
    }
    return i;
  }

 private:
  template <typename Cmp = std::equal_to<>>
  constexpr bool set_names(string_view sv, char_type sep = char_type{'|'}, Cmp&& cmp = {}) {
    // Avoid GCC C++26 wrong-code with find/remove_prefix, see #467.
    for (std::size_t first = 0; first < sv.size();) {
      auto last = first;
      while (last < sv.size() && sv[last] != sep) {
        ++last;
      }
      if (const auto v = enum_cast<E, detail::subtype_v<E>, Cmp&>(sv.substr(first, last - first), cmp)) {
        // A zero flag is neutral, not a position.
        const bool zero_flag = is_flags_enum_v<E> && enum_underlying(*v) == 0;
        if (!zero_flag) {
          set(*v);
        }
      } else {
        return false;
      }
      first = (last < sv.size()) ? last + 1 : last;
    }
    return true;
  }

  // Qualified, because nvcc with an MSVC host finds the set() member first.
  template <typename, typename>
  friend class containers::set;

  // Index of the first set bit at or after i, or the bit count.
  [[nodiscard]] constexpr std::size_t next_set_bit(std::size_t i) const noexcept {
    for (std::size_t n = i / bits_per_base; n < base_type_count; ++n) {
      auto word = a[n];
      if (n == i / bits_per_base) {
        word = static_cast<base_type>(word & static_cast<base_type>(static_cast<base_type>(~base_type{0}) << (i % bits_per_base)));
      }
      if (word != 0) {
        return n * bits_per_base + static_cast<std::size_t>(detail::countr_zero(word));
      }
    }
    return domain::count;
  }

  [[nodiscard]] constexpr reference reference_at(std::size_t i) noexcept {
    MAGIC_ENUM_ASSERT(i < size());
    return reference{this, i};
  }

  // A set position is its bit, so iteration skips the index lookup.
  [[nodiscard]] constexpr bool test_at(std::size_t i) const noexcept { return (a[base_type_count == 1 ? 0 : i / bits_per_base] & bit_mask(i)) != 0; }

  constexpr std::size_t checked_index(E pos, [[maybe_unused]] const char* what) const {
    if (auto i = domain::at(pos); i && *i < size()) {
      return *i;
    }
    MAGIC_ENUM_CONTAINERS_THROW(std::out_of_range(what));
  }

  container_type a;
};

template <typename V, std::enable_if_t<is_flags_enum_v<V>, int> = 0>
explicit bitset(V starter) -> bitset<V>;

// Set of enum values stored as a bitset.
template <typename E, typename Cmp = std::less<E>>
class set {
  static_assert(std::is_enum_v<E>, "magic_enum::containers::set requires enum type.");
  static_assert(detail::valid_indexing<E, detail::indexing<E, Cmp>>(), "magic_enum::containers::set requires a non-empty reflected enum.");

  using index_type = detail::indexing<E, Cmp>;

  struct Predicate {
    constexpr bool operator()(const set* h, const E* e) const noexcept { return h->a.test_at(static_cast<std::size_t>(e - index_type::begin())); }

    constexpr const E* next(const set* h, const E* first, const E* e) const noexcept {
      return first + h->a.next_set_bit(static_cast<std::size_t>(e - first) + 1);
    }
  };

 public:
  using container_type = bitset<E, detail::key_indexing<index_type>>;
  using key_type = E;
  using value_type = E;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using key_compare = Cmp;
  using value_compare = Cmp;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = detail::FilteredIterator<const set*, const E*, Predicate>;
  using const_iterator = iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  constexpr set() noexcept = default;

  template <typename InputIt, typename = typename std::iterator_traits<InputIt>::iterator_category>
  constexpr set(InputIt first, InputIt last) { insert(first, last); }

  constexpr set(std::initializer_list<E> ilist) { insert(ilist); }

  constexpr set(const set&) noexcept = default;
  constexpr set(set&&) noexcept = default;

  constexpr set& operator=(const set&) noexcept = default;
  constexpr set& operator=(set&&) noexcept = default;

  // Preserve the contents if validation throws.
  constexpr set& operator=(std::initializer_list<E> ilist) { return *this = set(ilist); }

  // A zero mask yields an empty set, even when zero is named.
  template <typename V = E, std::enable_if_t<std::is_same_v<V, E> && is_flags_enum_v<V>, int> = 0>
  [[nodiscard]] static constexpr set from_mask(E mask) {
    set r;
    r.a = container_type(mask);
    r.s = r.a.count();
    return r;
  }

  [[nodiscard]] constexpr const_iterator begin() const noexcept { return make_iterator(index_type::begin()); }

  [[nodiscard]] constexpr const_iterator end() const noexcept { return make_iterator(index_type::end()); }

  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }

  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }

  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator{begin()}; }

  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  [[nodiscard]] constexpr bool empty() const noexcept { return s == 0; }

  [[nodiscard]] constexpr size_type size() const noexcept { return s; }

  [[nodiscard]] constexpr size_type max_size() const noexcept { return a.max_size(); }

  constexpr void clear() noexcept {
    a.reset();
    s = 0;
  }

  constexpr std::pair<iterator, bool> insert(const value_type& value) {
    if (auto i = index_type::at(value)) {
      auto ref = a.reference_at(*i);
      const bool res = !ref;
      if (res) {
        ref = true;
        ++s;
      }

      return {make_iterator(index_type::it(*i)), res};
    }
    MAGIC_ENUM_CONTAINERS_THROW(std::invalid_argument("magic_enum::containers::set::insert: Unrecognized enum value"));
  }

  constexpr std::pair<iterator, bool> insert(value_type&& value) { return insert(value); }

  constexpr iterator insert(const_iterator, const value_type& value) { return insert(value).first; }

  constexpr iterator insert(const_iterator hint, value_type&& value) { return insert(hint, value); }

  template <typename InputIt>
  constexpr void insert(InputIt first, InputIt last) {
    while (first != last) {
      insert(*first++);
    }
  }

  constexpr void insert(std::initializer_list<value_type> ilist) { insert(ilist.begin(), ilist.end()); }

  template <typename... Args, std::enable_if_t<std::is_constructible_v<value_type, Args&&...>, int> = 0>
  constexpr std::pair<iterator, bool> emplace(Args&&... args) {
    return insert(value_type(std::forward<Args>(args)...));
  }

  template <typename... Args, std::enable_if_t<std::is_constructible_v<value_type, Args&&...>, int> = 0>
  constexpr iterator emplace_hint(const_iterator, Args&&... args) {
    return emplace(std::forward<Args>(args)...).first;
  }

  constexpr iterator erase(const_iterator pos) noexcept {
    const auto i = static_cast<std::size_t>(std::addressof(*pos) - index_type::begin());
    ++pos;
    a.reference_at(i) = false;
    --s;
    return pos;
  }

  constexpr iterator erase(const_iterator first, const_iterator last) noexcept {
    while (first != last) {
      first = erase(first);
    }
    return first;
  }

  constexpr size_type erase(const key_type& key) noexcept {
    if (const auto i = index_type::at(key)) {
      return erase_at(*i);
    }
    return 0;
  }

  template <typename K, typename KC = key_compare>
  constexpr std::enable_if_t<detail::is_transparent_v<KC> && !std::is_same_v<std::decay_t<K>, key_type>, size_type> erase(K&& x) {
    size_type c = 0;
    for (auto [first, last] = detail::equal_range(index_type::begin(), index_type::end(), x, key_compare{}); first != last; ++first) {
      c += erase_at(static_cast<std::size_t>(first - index_type::begin()));
    }
    return c;
  }

  void swap(set& other) noexcept {
    std::swap(a, other.a);
    std::swap(s, other.s);
  }

  [[nodiscard]] constexpr size_type count(const key_type& key) const noexcept { return a.find(key) != a.end(); }

  template <typename K, typename KC = key_compare>
  [[nodiscard]] constexpr std::enable_if_t<detail::is_transparent_v<KC>, size_type> count(const K& x) const {
    size_type c = 0;
    for (auto [first, last] = detail::equal_range(index_type::begin(), index_type::end(), x, key_compare{}); first != last; ++first) {
      c += a.test_at(static_cast<std::size_t>(first - index_type::begin()));
    }
    return c;
  }

  [[nodiscard]] constexpr const_iterator find(const key_type& key) const noexcept {
    if (auto i = index_type::at(key); i && a.test_at(*i)) {
      return make_iterator(index_type::it(*i));
    }
    return end();
  }

  template <typename K, typename KC = key_compare>
  [[nodiscard]] constexpr std::enable_if_t<detail::is_transparent_v<KC>, const_iterator> find(const K& x) const {
    for (auto [first, last] = detail::equal_range(index_type::begin(), index_type::end(), x, key_compare{}); first != last; ++first) {
      if (a.test_at(static_cast<std::size_t>(first - index_type::begin()))) {
        return make_iterator(first);
      }
    }
    return end();
  }

  [[nodiscard]] constexpr bool contains(const key_type& key) const noexcept { return count(key) > 0; }

  template <typename K, typename KC = key_compare>
  [[nodiscard]] constexpr std::enable_if_t<detail::is_transparent_v<KC>, bool> contains(const K& x) const {
    return find(x) != end();
  }

 private:
  constexpr size_type erase_at(std::size_t i) noexcept {
    if (!a.test_at(i)) {
      return 0;
    }
    a.reference_at(i) = false;
    --s;
    return 1;
  }

  [[nodiscard]] constexpr const_iterator make_iterator(const E* pos) const noexcept {
    return const_iterator{this, index_type::begin(), pos};
  }

  [[nodiscard]] constexpr const_iterator iterator_at_or_after(const E* it) const noexcept {
    const auto i = a.next_set_bit(static_cast<std::size_t>(it - index_type::begin()));
    return make_iterator(index_type::begin() + i);
  }

 public:
  [[nodiscard]] constexpr std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
    return {lower_bound(key), upper_bound(key)};
  }

  template <typename K, typename KC = key_compare>
  [[nodiscard]] constexpr std::enable_if_t<detail::is_transparent_v<KC>, std::pair<const_iterator, const_iterator>> equal_range(const K& x) const {
    return {lower_bound(x), upper_bound(x)};
  }

  [[nodiscard]] constexpr const_iterator lower_bound(const key_type& key) const {
    return iterator_at_or_after(detail::lower_bound(index_type::begin(), index_type::end(), key, key_compare{}));
  }

  template <typename K, typename KC = key_compare>
  [[nodiscard]] constexpr std::enable_if_t<detail::is_transparent_v<KC>, const_iterator> lower_bound(const K& x) const {
    return iterator_at_or_after(detail::lower_bound(index_type::begin(), index_type::end(), x, key_compare{}));
  }

  [[nodiscard]] constexpr const_iterator upper_bound(const key_type& key) const {
    return iterator_at_or_after(detail::upper_bound(index_type::begin(), index_type::end(), key, key_compare{}));
  }

  template <typename K, typename KC = key_compare>
  [[nodiscard]] constexpr std::enable_if_t<detail::is_transparent_v<KC>, const_iterator> upper_bound(const K& x) const {
    return iterator_at_or_after(detail::upper_bound(index_type::begin(), index_type::end(), x, key_compare{}));
  }

  [[nodiscard]] constexpr key_compare key_comp() const { return {}; }

  [[nodiscard]] constexpr value_compare value_comp() const { return {}; }

  [[nodiscard]] friend constexpr bool operator==(const set& lhs, const set& rhs) noexcept { return lhs.a == rhs.a; }

  [[nodiscard]] friend constexpr bool operator!=(const set& lhs, const set& rhs) noexcept { return lhs.a != rhs.a; }

  [[nodiscard]] friend constexpr bool operator<(const set& lhs, const set& rhs) { return detail::lexicographical_compare(lhs, rhs); }

  [[nodiscard]] friend constexpr bool operator<=(const set& lhs, const set& rhs) { return !(rhs < lhs); }

  [[nodiscard]] friend constexpr bool operator>(const set& lhs, const set& rhs) { return rhs < lhs; }

  [[nodiscard]] friend constexpr bool operator>=(const set& lhs, const set& rhs) { return !(lhs < rhs); }

  template <typename Pred>
  constexpr size_type erase_if(Pred pred) {
    auto old_size = size();
    for (auto i = begin(), last = end(); i != last;) {
      if (pred(*i)) {
        i = erase(i);
      } else {
        ++i;
      }
    }
    return old_size - size();
  }

 private:
  container_type a;
  std::size_t s = 0;
};

template <auto J, typename E, typename V, typename Index>
[[nodiscard]] constexpr std::enable_if_t<(std::is_integral_v<decltype(J)> && J >= 0 && J < enum_count<E>()), V&> get(array<E, V, Index>& a) noexcept {
  return a.a[J];
}

template <auto J, typename E, typename V, typename Index>
[[nodiscard]] constexpr std::enable_if_t<(std::is_integral_v<decltype(J)> && J >= 0 && J < enum_count<E>()), V&&> get(array<E, V, Index>&& a) noexcept {
  return std::move(a.a[J]);
}

template <auto J, typename E, typename V, typename Index>
[[nodiscard]] constexpr std::enable_if_t<(std::is_integral_v<decltype(J)> && J >= 0 && J < enum_count<E>()), const V&> get(const array<E, V, Index>& a) noexcept {
  return a.a[J];
}

template <auto J, typename E, typename V, typename Index>
[[nodiscard]] constexpr std::enable_if_t<(std::is_integral_v<decltype(J)> && J >= 0 && J < enum_count<E>()), const V&&> get(const array<E, V, Index>&& a) noexcept {
  return std::move(a.a[J]);
}

template <auto Enum, typename E, typename V, typename Index>
[[nodiscard]] constexpr std::enable_if_t<std::is_same_v<decltype(Enum), E> && enum_contains(Enum), V&> get(array<E, V, Index>& a) {
  return a[Enum];
}

template <auto Enum, typename E, typename V, typename Index>
[[nodiscard]] constexpr std::enable_if_t<std::is_same_v<decltype(Enum), E> && enum_contains(Enum), V&&> get(array<E, V, Index>&& a) {
  return std::move(a[Enum]);
}

template <auto Enum, typename E, typename V, typename Index>
[[nodiscard]] constexpr std::enable_if_t<std::is_same_v<decltype(Enum), E> && enum_contains(Enum), const V&> get(const array<E, V, Index>& a) {
  return a[Enum];
}

template <auto Enum, typename E, typename V, typename Index>
[[nodiscard]] constexpr std::enable_if_t<std::is_same_v<decltype(Enum), E> && enum_contains(Enum), const V&&> get(const array<E, V, Index>&& a) {
  return std::move(a[Enum]);
}

} // namespace magic_enum::containers

template <typename E, typename Index>
struct std::hash<magic_enum::containers::bitset<E, Index>> {
  std::size_t operator()(const magic_enum::containers::bitset<E, Index>& bs) const noexcept {
    using domain = magic_enum::containers::detail::bit_domain<E, Index>;
    if constexpr (domain::count <= sizeof(unsigned long long) * 8) {
      return std::hash<unsigned long long>{}(bs.to_ullong(magic_enum::containers::raw_access));
    } else {
      unsigned long long low_bits = 0;
      std::size_t seed = 0;
      bool has_high_bits = false;
      for (const auto member : bs) {
        const auto index = domain::at(member);
        MAGIC_ENUM_ASSERT(index);
        if (*index < sizeof(low_bits) * 8) {
          low_bits |= 1ULL << *index;
        } else {
          if (!has_high_bits) {
            seed = std::hash<unsigned long long>{}(low_bits);
            has_high_bits = true;
          }
          const auto index_hash = std::hash<std::size_t>{}(*index);
          seed ^= index_hash + std::size_t{0x9e3779b9U} + (seed << 6) + (seed >> 2);
        }
      }
      return has_high_bits ? seed : std::hash<unsigned long long>{}(low_bits);
    }
  }
};

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif

#undef MAGIC_ENUM_CONTAINERS_THROW

#endif // NEARGYE_MAGIC_ENUM_CONTAINERS_HPP
