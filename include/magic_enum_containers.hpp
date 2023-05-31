//  __  __             _        ______                          _____
// |  \/  |           (_)      |  ____|                        / ____|_     _
// | \  / | __ _  __ _ _  ___  | |__   _ __  _   _ _ __ ___   | |   _| |_ _| |_
// | |\/| |/ _` |/ _` | |/ __| |  __| | '_ \| | | | '_ ` _ \  | |  |_   _|_   _|
// | |  | | (_| | (_| | | (__  | |____| | | | |_| | | | | | | | |____|_|   |_|
// |_|  |_|\__,_|\__, |_|\___| |______|_| |_|\__,_|_| |_| |_|  \_____|
//                __/ | https://github.com/Neargye/magic_enum
//               |___/  version 0.9.1
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2023 Daniil Goncharov <neargye@gmail.com>.
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

#include <stdexcept>

#include "magic_enum.hpp"

namespace magic_enum::containers {

namespace detail {

template <typename T, typename = void>
[[maybe_unused]] constexpr static bool is_transparent_v{};

template <typename T>
constexpr static bool is_transparent_v<T, std::void_t<typename T::is_transparent>>{true};

template <typename Eq = std::equal_to<>, typename T1, typename T2>
constexpr bool equal(T1&& t1, T2&& t2, Eq&& eq = {}) {
  auto first1 = t1.begin();
  auto last1 = t1.end();
  auto first2 = t2.begin();
  auto last2 = t2.end();

  for (; first1 != last1; ++first1, ++first2) {
    if (first2 == last2 || !eq(*first1, *first2)) {
      return false;
    }
  }
  return first2 == last2;
}

template <typename Cmp = std::less<>, typename T1, typename T2>
constexpr bool lexicographical_compare(T1&& t1, T2&& t2, Cmp&& cmp = {}) noexcept {
  auto first1 = t1.begin();
  auto last1 = t1.end();
  auto first2 = t2.begin();
  auto last2 = t2.end();

  // copied from std::lexicographical_compare
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
  std::size_t c = 0;
  while (x > 0) {
    c += x & 1;
    x >>= 1;
  }
  return c;
}

template <typename Cmp = std::less<>, typename ForwardIt, typename E>
constexpr ForwardIt lower_bound(ForwardIt first, ForwardIt last, E&& e, Cmp&& comp = {}) {
  auto count = std::distance(first, last);

  for (auto it = first; count > 0;) {
    auto step = count / 2;
    std::advance(it, step);
    if (comp(*it, e)) {
      first = ++it;
      count -= step + 1;
    } else {
      count = step;
    }
  }
  return first;
}

template <typename Cmp = std::less<>, typename BidirIt, typename E>
constexpr auto equal_range(BidirIt begin, BidirIt end, E&& e, Cmp&& comp = {}) {
  const auto first = lower_bound(begin, end, e, comp);
  return std::pair{first, lower_bound(std::make_reverse_iterator(end), std::make_reverse_iterator(first), e, [&comp](auto&& lhs, auto&& rhs) { return comp(rhs, lhs); }).base()};
}

template <typename E = void, typename Cmp = std::less<E>, typename = void>
struct indexing {
  [[nodiscard]] constexpr static auto get_indices() noexcept {
    // reverse result index mapping
    std::array<std::size_t, enum_count<E>()> rev_res{};

    // std::iota
    for (std::size_t i = 0; i < enum_count<E>(); ++i) {
      rev_res[i] = i;
    }

    constexpr auto orig_values = enum_values<E>();
    constexpr Cmp cmp{};

    // ~std::sort
    for (std::size_t i = 0; i < enum_count<E>(); ++i) {
      for (std::size_t j = i + 1; j < enum_count<E>(); ++j) {
        if (cmp(orig_values[rev_res[j]], orig_values[rev_res[i]])) {
          auto tmp = rev_res[i];
          rev_res[i] = rev_res[j];
          rev_res[j] = tmp;
        }
      }
    }

    std::array<E, enum_count<E>()> sorted_values{};
    // reverse the sorted indices
    std::array<std::size_t, enum_count<E>()> res{};
    for (std::size_t i = 0; i < enum_count<E>(); ++i) {
      res[rev_res[i]] = i;
      sorted_values[i] = orig_values[rev_res[i]];
    }

    return std::pair{sorted_values, res};
  }

  constexpr static inline std::array<E, enum_count<E>()> values = get_indices().first;
  constexpr static inline const std::array<E, enum_count<E>()>* values_v = &values;
  constexpr static inline std::array<std::size_t, enum_count<E>()> reindex = get_indices().second;

  [[nodiscard]] constexpr inline optional<std::size_t> operator()(E val) const noexcept {
    if (auto opt = enum_index(val)) {
      return reindex[*opt];
    }
    return {};
  }
};

template <typename E, typename Less>
struct indexing<E, Less, std::enable_if_t<std::is_enum_v<std::decay_t<E>> && (std::is_same_v<Less, std::less<E>> || std::is_same_v<Less, std::less<>>)>> {
  constexpr static inline const std::array<E, enum_count<E>()>* values_v = &enum_values<E>();
  [[nodiscard]] constexpr inline optional<std::size_t> operator()(E val) const noexcept { return enum_index(val); }
};

template <typename Less>
struct indexing<void, Less, void> {
  using is_transparent = std::true_type;
  template <typename E>
  [[nodiscard]] constexpr inline optional<std::size_t> operator()(E val) const noexcept {
    constexpr indexing<E, Less> ix{};
    return ix(val);
  }
};

template <typename E = void, typename OP = std::less<>, typename = void>
struct name_sort_impl {
  [[nodiscard]] constexpr inline bool operator()(E e1, E e2) const noexcept { return OP{}(enum_name(e1), enum_name(e2)); }
};

template <typename OP>
struct name_sort_impl<void, OP> {
  using is_transparent = std::true_type;
  template <typename S = OP, typename = void>
  struct FullCmp : S {};

  template <typename S>
  struct FullCmp<S, std::enable_if_t<!std::is_invocable_v<S, string_view, string_view> && std::is_invocable_v<S, char_type, char_type>>> {
    [[nodiscard]] constexpr inline bool operator()(string_view s1, string_view s2) const noexcept { return lexicographical_compare<S>(s1, s2); }
  };

  template <typename E1, typename E2>
  [[nodiscard]] constexpr inline std::enable_if_t<
      // at least one of need to be an enum type
      (std::is_enum_v<std::decay_t<E1>> || std::is_enum_v<std::decay_t<E2>>)&&
      // if both is enum, only accept if the same enum
      (!std::is_enum_v<std::decay_t<E1>> || !std::is_enum_v<std::decay_t<E2>> || std::is_same_v<E1, E2>)&&
      // is invocable with comparator
      (std::is_invocable_r_v<bool, FullCmp<>, std::conditional_t<std::is_enum_v<std::decay_t<E1>>, string_view, E1>,
                             std::conditional_t<std::is_enum_v<std::decay_t<E2>>, string_view, E2>>),
      bool>
  operator()(E1 e1, E2 e2) const noexcept {
    using D1 = std::decay_t<E1>;
    using D2 = std::decay_t<E2>;
    constexpr FullCmp<> cmp{};

    if constexpr (std::is_enum_v<D1> && std::is_enum_v<D2>) {
      return cmp(enum_name(e1), enum_name(e2));
    } else if constexpr (std::is_enum_v<D1>) {
      return cmp(enum_name(e1), e2);
    } else /* if constexpr (std::is_enum_v<D2>) */ {
      return cmp(e1, enum_name(e2));
    }
  }
};

struct raw_access_t {};

template <typename Parent, typename Iterator, typename Getter, typename Predicate>
struct FilteredIterator {
  Parent parent;
  Iterator first;
  Iterator last;
  Iterator current;
  Getter getter;
  Predicate predicate;

  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = std::remove_reference_t<std::invoke_result_t<Getter, Parent, Iterator>>;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type*;
  using reference = value_type&;

  constexpr FilteredIterator() noexcept = default;
  constexpr FilteredIterator(const FilteredIterator&) = default;
  constexpr FilteredIterator& operator=(const FilteredIterator&) = default;
  constexpr FilteredIterator(FilteredIterator&&) noexcept = default;
  constexpr FilteredIterator& operator=(FilteredIterator&&) noexcept = default;

  template <typename OtherParent, typename OtherIterator,
            typename = std::enable_if_t<std::is_convertible_v<OtherParent, Parent> && std::is_convertible_v<OtherIterator, Iterator>>*>
  constexpr explicit FilteredIterator(const FilteredIterator<OtherParent, OtherIterator, Getter, Predicate>& other)
      : parent(other.parent), first(other.first), last(other.last), current(other.current), getter(other.getter), predicate(other.predicate) {}

  ~FilteredIterator() = default;

  constexpr FilteredIterator(Parent p, Iterator begin, Iterator end, Iterator curr, Getter getter = {}, Predicate pred = {})
      : parent(p), first(std::move(begin)), last(std::move(end)), current(std::move(curr)), getter{std::move(getter)}, predicate{std::move(pred)} {
    if (current == first && !predicate(parent, current)) {
      ++*this;
    }
  }

  [[nodiscard]] constexpr reference operator*() const { return getter(parent, current); }

  [[nodiscard]] constexpr pointer operator->() const { return std::addressof(**this); }

  constexpr FilteredIterator& operator++() {
    do {
      ++current;
    } while (current != last && !predicate(parent, current));
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

} // namespace detail

template <typename E = void>
using name_less [[maybe_unused]] = detail::name_sort_impl<E>;
template <typename E = void>
using name_greater [[maybe_unused]] = detail::name_sort_impl<E, std::greater<>>;

using name_less_ci [[maybe_unused]] = detail::name_sort_impl<void, magic_enum::detail::case_insensitive<std::less<>>>;
using name_greater_ci [[maybe_unused]] = detail::name_sort_impl<void, magic_enum::detail::case_insensitive<std::greater<>>>;

template <typename E = void>
using default_indexing = detail::indexing<E>;

template <typename Cmp = std::less<>>
using comparator_indexing [[maybe_unused]] = detail::indexing<void, Cmp>;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                          ARRAY                                                            //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename E, typename V, typename Index = default_indexing<E>>
struct array {
  static_assert(std::is_enum_v<E>);
  static_assert(std::is_trivially_constructible_v<Index>);
  static_assert(enum_count<E>() == 0 || Index{}(enum_values<E>().front())); // check Index is constexpr

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

  constexpr reference at(E pos) {
    if (auto index = index_type{}(pos)) {
      return a[*index];
    }
    throw std::out_of_range("enum array::at: unrecognized position");
  }

  constexpr const_reference at(E pos) const {
    if (auto index = index_type{}(pos)) {
      return a[*index];
    }
    throw std::out_of_range("enum array::at: unrecognized position");
  }

  [[nodiscard]] constexpr reference operator[](E pos) noexcept { return a[*index_type{}(pos)]; }

  [[nodiscard]] constexpr const_reference operator[](E pos) const noexcept { return a[*index_type{}(pos)]; }

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

  [[nodiscard]] constexpr iterator rbegin() noexcept { return a.rbegin(); }

  [[nodiscard]] constexpr const_iterator rbegin() const noexcept { return a.rbegin(); }

  [[nodiscard]] constexpr const_iterator crbegin() const noexcept { return a.crbegin(); }

  [[nodiscard]] constexpr iterator rend() noexcept { return a.rend(); }

  [[nodiscard]] constexpr const_iterator rend() const noexcept { return a.rend(); }

  [[nodiscard]] constexpr const_iterator crend() const noexcept { return a.crend(); }

  [[nodiscard]] constexpr bool empty() const noexcept { return a.empty(); }

  [[nodiscard]] constexpr size_type size() const noexcept { return a.size(); }

  [[nodiscard]] constexpr size_type max_size() const noexcept { return a.max_size(); }

  constexpr void fill(const V& value) {
    for (auto& v : a) {
      v = value;
    }
  }

  constexpr void swap(array& other) noexcept(std::is_nothrow_swappable_v<V>) {
    for (std::size_t i{}; i < a.size(); ++i) {
      auto v = std::move(other.a[i]);
      other.a[i] = std::move(a[i]);
      a[i] = std::move(v);
    }
  }

  [[nodiscard]] friend constexpr bool operator==(const array& a1, const array& a2) { return detail::equal(a1, a2); }

  [[nodiscard]] friend constexpr bool operator!=(const array& a1, const array& a2) { return !detail::equal(a1, a2); }

  [[nodiscard]] friend constexpr bool operator<(const array& a1, const array& a2) { return detail::lexicographical_compare(a1, a2); }

  [[nodiscard]] friend constexpr bool operator<=(const array& a1, const array& a2) { return !detail::lexicographical_compare(a2, a1); }

  [[nodiscard]] friend constexpr bool operator>(const array& a1, const array& a2) { return detail::lexicographical_compare(a2, a1); }

  [[nodiscard]] friend constexpr bool operator>=(const array& a1, const array& a2) { return !detail::lexicographical_compare(a1, a2); }

  container_type a;
};

namespace detail {

template <typename E, typename T, std::size_t N, std::size_t... I>
constexpr array<E, std::remove_cv_t<T>> to_array_impl(T (&a)[N], std::index_sequence<I...>) {
  return {{a[I]...}};
}

template <typename E, typename T, std::size_t N, std::size_t... I>
constexpr array<E, std::remove_cv_t<T>> to_array_impl(T(&&a)[N], std::index_sequence<I...>) {
  return {{std::move(a[I])...}};
}

} // namespace detail

template <typename E, typename T, std::size_t N>
constexpr std::enable_if_t<(enum_count<E>() == N), array<E, std::remove_cv_t<T>>> to_array(T (&a)[N]) {
  return detail::to_array_impl<E>(a, std::make_index_sequence<N>{});
}

template <typename E, typename T, std::size_t N>
constexpr std::enable_if_t<(enum_count<E>() == N), array<E, std::remove_cv_t<T>>> to_array(T(&&a)[N]) {
  return detail::to_array_impl<E>(std::move(a), std::make_index_sequence<N>{});
}

template <typename E, typename... Ts>
constexpr std::enable_if_t<(enum_count<E>() == sizeof...(Ts)), array<E, std::remove_cv_t<std::common_type_t<Ts...>>>> make_array(Ts&&... ts) {
  return {{std::forward<Ts>(ts)...}};
}

inline constexpr detail::raw_access_t raw_access{};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                         BITSET                                                            //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename E, typename Index = default_indexing<E>>
class bitset {
  static_assert(std::is_enum_v<E>);
  static_assert(std::is_trivially_constructible_v<Index>);
  static_assert(enum_count<E>() == 0 || Index{}(enum_values<E>().front())); // check Index is constexpr

  using base_type = std::conditional_t<enum_count<E>() <= 8,  std::uint_least8_t,
                    std::conditional_t<enum_count<E>() <= 16, std::uint_least16_t,
                    std::conditional_t<enum_count<E>() <= 32, std::uint_least32_t,
                                                              std::uint_least64_t>>>;

  constexpr static std::size_t bits_per_base = sizeof(base_type) * 8;
  constexpr static std::size_t base_type_count = (enum_count<E>() > 0 ? (enum_count<E>() - 1) / bits_per_base + 1 : 0);
  constexpr static std::size_t not_interested = base_type_count * bits_per_base - enum_count<E>();
  constexpr static base_type last_value_max = (base_type{1} << (bits_per_base - not_interested)) - 1;

  template <typename parent_t = bitset*>
  class reference_impl {
    friend class bitset;

    parent_t parent;
    std::size_t num_index;
    base_type bit_index;

    constexpr reference_impl(parent_t parent, std::size_t ix) noexcept : reference_impl(parent, std::pair{ix / bits_per_base, base_type{1} << (ix % bits_per_base)}) {}

    constexpr reference_impl(parent_t parent, std::pair<std::size_t, base_type> ix) noexcept : parent(parent), num_index(std::get<0>(ix)), bit_index(std::get<1>(ix)) {}

   public:
    constexpr reference_impl& operator=(bool v) noexcept {
      if (v) {
        parent->a[num_index] |= bit_index;
      } else {
        parent->a[num_index] &= ~bit_index;
      }
      return *this;
    }

    constexpr reference_impl& operator=(const reference_impl& v) noexcept {
      if (this == &v) {
        return *this;
      }
      *this = static_cast<bool>(v);
      return *this;
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept { return (parent->a[num_index] & bit_index) > 0; }

    [[nodiscard]] constexpr bool operator~() const noexcept { return !static_cast<bool>(*this); }

    constexpr reference_impl& flip() noexcept {
      *this = ~*this;
      return *this;
    }
  };

  template <typename T>
  [[nodiscard]] constexpr T to_(detail::raw_access_t) const {
    T res{};
    T flag{1};
    for (std::size_t i{}; i < size(); ++i, flag <<= 1) {
      if (const_reference{this, i}) {
        if (i >= sizeof(T) * 8) {
          throw std::overflow_error("cannot represent enum in this type");
        }
        res |= flag;
      }
    }
    return res;
  }

 public:
  using index_type = Index;
  using container_type = std::array<base_type, base_type_count>;
  using reference = reference_impl<>;
  using const_reference = reference_impl<const bitset*>;

  constexpr explicit bitset(detail::raw_access_t = raw_access) noexcept : a{{}} {}

  constexpr explicit bitset(detail::raw_access_t, unsigned long long val) : a{{}} {
    unsigned long long bit{1};
    for (std::size_t i{}; i < (sizeof(val) * 8); ++i, bit <<= 1) {
      if ((val & bit) > 0) {
        if (i >= enum_count<E>()) {
          throw std::out_of_range("enum bitset::constructor: Upper bit set in raw number");
        }

        reference{this, i} = true;
      }
    }
  }

  constexpr explicit bitset(detail::raw_access_t, string_view sv, string_view::size_type pos = 0, string_view::size_type n = string_view::npos, char_type zero = '0', char_type one = '1')
      : a{{}} {
    std::size_t i{};
    for (auto c : sv.substr(pos, n)) {
      if (c == one) {
        if (i >= enum_count<E>()) {
          throw std::out_of_range("enum bitset::constructor: Upper bit set in raw string");
        }
        reference{this, i} = true;
      } else if (c != zero) {
        throw std::invalid_argument("enum bitset::constructor: unrecognized character in raw string");
      }
      ++i;
    }
  }

  constexpr explicit bitset(detail::raw_access_t, const char_type* str, std::size_t n = ~std::size_t{}, char_type zero = '0', char_type one = '1')
      : bitset(std::string_view{str, (std::min)(std::char_traits<char_type>::length(str), n)}, 0, n, zero, one) {}

  constexpr bitset(std::initializer_list<E> starters) : a{{}} {
    if constexpr (magic_enum::detail::subtype_v<E> == magic_enum::detail::enum_subtype::flags) {
      for (auto& f : starters) {
        *this |= bitset(f);
      }
    } else {
      for (auto& f : starters) {
        set(f);
      }
    }
  }
  template <typename V, std::enable_if_t<std::is_same_v<V, E> && magic_enum::detail::subtype_v<V> == magic_enum::detail::enum_subtype::flags, int> = 0>
  constexpr explicit bitset(V starter) : a{{}} {
    auto u = enum_underlying(starter);
    for (E v : enum_values<E>()) {
      if (auto ul = enum_underlying(v); (ul & u) != 0) {
        u &= ~ul;
        (*this)[v] = true;
      }
    }
    if (u != 0) {
      throw std::invalid_argument("enum bitset::constructor: unrecognized enum value in flag");
    }
  }

  template <typename Cmp = std::equal_to<>>
  constexpr explicit bitset(string_view sv, Cmp&& cmp = {}, char_type sep = '|') {
    for (std::size_t to{}; (to = magic_enum::detail::find(sv, sep)) != string_view::npos; sv.remove_prefix(to + 1)) {
      if (auto v = enum_cast<E>(sv.substr(0, to), cmp)) {
        set(v);
      } else {
        throw std::invalid_argument("enum bitset::constructor: unrecognized enum value in string");
      }
    }
    if (!sv.empty()) {
      if (auto v = enum_cast<E>(sv, cmp)) {
        set(v);
      } else {
        throw std::invalid_argument("enum bitset::constructor: unrecognized enum value in string");
      }
    }
  }

  [[nodiscard]] friend constexpr bool operator==(const bitset& lhs, const bitset& rhs) noexcept { return detail::equal(lhs.a, rhs.a); }

  [[nodiscard]] friend constexpr bool operator!=(const bitset& lhs, const bitset& rhs) noexcept { return !detail::equal(lhs.a, rhs.a); }

  [[nodiscard]] constexpr bool operator[](E pos) const noexcept { return static_cast<bool>(const_reference(this, *index_type{}(pos))); }

  [[nodiscard]] constexpr reference operator[](E pos) noexcept { return reference{this, *index_type{}(pos)}; }

  constexpr bool test(E pos) const {
    if (auto ix = index_type{}(pos)) {
      return static_cast<bool>(const_reference(this, *ix));
    }
    throw std::out_of_range("enum bitset::test: unrecognized position");
  }

  [[nodiscard]] constexpr bool all() const noexcept {
    if constexpr (base_type_count == 0) {
      return true;
    }

    for (std::size_t i{}; i < base_type_count - (not_interested > 0); ++i) {
      auto check = ~a[i];
      if (check) {
        return false;
      }
    }

    if constexpr (not_interested > 0) {
      return a[base_type_count - 1] == last_value_max;
    }
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
    std::size_t c{};
    for (auto& v : a) {
      c += detail::popcount(v);
    }
    return c;
  }

  [[nodiscard]] constexpr std::size_t size() const noexcept { return enum_count<E>(); }

  [[nodiscard]] constexpr std::size_t max_size() const noexcept { return enum_count<E>(); }

  constexpr bitset& operator&=(const bitset& other) noexcept {
    for (std::size_t i{}; i < base_type_count; ++i) {
      a[i] &= other.a[i];
    }
    return *this;
  }

  constexpr bitset& operator|=(const bitset& other) noexcept {
    for (std::size_t i{}; i < base_type_count; ++i) {
      a[i] |= other.a[i];
    }
    return *this;
  }

  constexpr bitset& operator^=(const bitset& other) noexcept {
    for (std::size_t i{}; i < base_type_count; ++i) {
      a[i] ^= other.a[i];
    }
    return *this;
  }

  [[nodiscard]] constexpr bitset operator~() const noexcept {
    bitset res;
    for (std::size_t i{}; i < base_type_count - (not_interested > 0); ++i) {
      res.a[i] = ~a[i];
    }

    if constexpr (not_interested > 0) {
      res.a[base_type_count - 1] = ~a[base_type_count - 1] & last_value_max;
    }
    return res;
  }

  constexpr bitset& set() noexcept {
    for (std::size_t i{}; i < base_type_count - (not_interested > 0); ++i) {
      a[i] = ~base_type{};
    }

    if constexpr (not_interested > 0) {
      a[base_type_count - 1] = last_value_max;
    }
    return *this;
  }

  constexpr bitset& set(E pos, bool value = true) {
    if (auto ix = index_type{}(pos)) {
      reference{this, *ix} = value;
      return *this;
    }
    throw std::out_of_range("enum bitset::set: unrecognized position");
  }

  constexpr bitset& reset() noexcept { return *this = bitset{}; }

  constexpr bitset& reset(E pos) {
    if (auto ix = index_type{}(pos)) {
      reference{this, *ix} = false;
      return *this;
    }
    throw std::out_of_range("enum bitset::reset: unrecognized position");
  }

  constexpr bitset& flip() noexcept { return *this = ~*this; }

  [[nodiscard]] friend constexpr bitset operator&(const bitset& lhs, const bitset& rhs) noexcept {
    bitset cp = lhs;
    cp &= rhs;
    return cp;
  }

  [[nodiscard]] friend constexpr bitset operator|(const bitset& lhs, const bitset& rhs) noexcept {
    bitset cp = lhs;
    cp |= rhs;
    return cp;
  }

  [[nodiscard]] friend constexpr bitset operator^(const bitset& lhs, const bitset& rhs) noexcept {
    bitset cp = lhs;
    cp ^= rhs;
    return cp;
  }

  template <typename V = E>
  [[nodiscard]] constexpr explicit operator std::enable_if_t<magic_enum::detail::subtype_v<V> == magic_enum::detail::enum_subtype::flags, E>() const {
    E res{};
    for (auto& e : enum_values<E>()) {
      if (test(e)) {
        res |= e;
      }
    }
    return res;
  }

  [[nodiscard]] string to_string(char_type sep = '|') const {
    string name;

    for (auto& e : enum_values<E>()) {
      if (test(e)) {
        if (!name.empty()) {
          name.append(1, sep);
        }
        auto n = enum_name(e);
        name.append(n.data(), n.size());
      }
    }
    return name;
  }

  [[nodiscard]] string to_string(detail::raw_access_t, char_type zero = '0', char_type one = '1') const {
    string name;
    name.reserve(size());
    for (std::size_t i{}; i < size(); ++i) {
      name.append(1, const_reference{this, i} ? one : zero);
    }
    return name;
  }

  [[nodiscard]] constexpr unsigned long long to_ullong(detail::raw_access_t raw) const { return to_<unsigned long long>(raw); }

  [[nodiscard]] constexpr unsigned long long to_ulong(detail::raw_access_t raw) const { return to_<unsigned long>(raw); }

  friend std::ostream& operator<<(std::ostream& o, const bitset& bs) { return o << bs.to_string(); }

  friend std::istream& operator>>(std::istream& i, bitset& bs) {
    string s;
    if (i >> s; !s.empty()) {
      bs = bitset(string_view{s});
    }
    return i;
  }

 private:
  container_type a;
};

template <typename V, int = 0>
explicit bitset(V starter) -> bitset<V>;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                           SET                                                             //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <typename E, typename CExprLess = std::less<E>>
class set {
  using index_type = detail::indexing<E, CExprLess>;
  struct Getter {
    constexpr const E& operator()(const set*, const E* p) const noexcept { return *p; }
  };
  struct Predicate {
    constexpr bool operator()(const set* h, const E* e) const noexcept { return h->a[*e]; }
  };

 public:
  using container_type = bitset<E, index_type>;
  using key_type = E;
  using value_type = E;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using key_compare = CExprLess;
  using value_compare = CExprLess;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = detail::FilteredIterator<const set*, const E*, Getter, Predicate>;
  using const_iterator = detail::FilteredIterator<const set*, const E*, Getter, Predicate>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  constexpr set() noexcept = default;

  template <typename InputIt>
  constexpr set(InputIt first, InputIt last) {
    while (first != last) {
      insert(*first++);
    }
  }

  constexpr set(std::initializer_list<E> ilist) {
    for (auto e : ilist) {
      insert(e);
    }
  }
  template <typename V, std::enable_if_t<std::is_same_v<V, E> && magic_enum::detail::subtype_v<V> == magic_enum::detail::enum_subtype::flags, int> = 0>
  constexpr explicit set(V starter) {
    auto u = enum_underlying(starter);
    for (E v : enum_values<E>()) {
      if ((enum_underlying(v) & u) != 0) {
        insert(v);
      }
    }
  }

  constexpr set(const set&) noexcept = default;
  constexpr set(set&&) noexcept = default;

  constexpr set& operator=(const set&) noexcept = default;
  constexpr set& operator=(set&&) noexcept = default;
  constexpr set& operator=(std::initializer_list<E> ilist) {
    for (auto e : ilist) {
      insert(e);
    }
  }

  constexpr const_iterator begin() const noexcept {
    return const_iterator{this, &(*index_type::values_v->begin()), &(*index_type::values_v->end()), &(*index_type::values_v->begin())};
  }

  constexpr const_iterator end() const noexcept {
    return const_iterator{this, &(*index_type::values_v->begin()), &(*index_type::values_v->end()), &(*index_type::values_v->end())};
  }

  constexpr const_iterator cbegin() const noexcept { return begin(); }

  constexpr const_iterator cend() const noexcept { return end(); }

  constexpr const_reverse_iterator rbegin() const noexcept { return {end()}; }

  constexpr const_reverse_iterator rend() const noexcept { return {begin()}; }

  constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  [[nodiscard]] constexpr bool empty() const noexcept { return s == 0; }

  [[nodiscard]] constexpr size_type size() const noexcept { return s; }

  [[nodiscard]] constexpr size_type max_size() const noexcept { return a.max_size(); }

  constexpr void clear() noexcept {
    a.reset();
    s = 0;
  }

  constexpr std::pair<iterator, bool> insert(const value_type& value) noexcept {
    if (auto i = index_type{}(value)) {
      typename container_type::reference ref = a[value];
      bool r = !ref;
      if (r) {
        ref = true;
        ++s;
      }

      return {iterator{this, &(*index_type::values_v->begin()), &(*index_type::values_v->end()), &(*index_type::values_v)[*i]}, r};
    }
    return {end(), false};
  }

  constexpr std::pair<iterator, bool> insert(value_type&& value) noexcept { return insert(value); }

  constexpr iterator insert(const_iterator, const value_type& value) noexcept { return insert(value).first; }

  constexpr iterator insert(const_iterator hint, value_type&& value) noexcept { return insert(hint, value); }

  template <typename InputIt>
  constexpr void insert(InputIt first, InputIt last) noexcept {
    while (first != last) {
      insert(*first++);
    }
  }

  constexpr void insert(std::initializer_list<value_type> ilist) noexcept {
    for (auto v : ilist) {
      insert(v);
    }
  }

  template <typename... Args>
  constexpr std::pair<iterator, bool> emplace(Args&&... args) noexcept {
    return insert({std::forward<Args>(args)...});
  }

  template <typename... Args>
  constexpr iterator emplace_hint(const_iterator, Args&&... args) noexcept {
    return emplace(std::forward<Args>(args)...).first;
  }

  constexpr iterator erase(const_iterator pos) noexcept {
    erase(*pos++);
    return pos;
  }

  constexpr iterator erase(const_iterator first, const_iterator last) noexcept {
    while ((first = erase(first)) != last) {
      ;
    }
    return first;
  }

  constexpr size_type erase(const key_type& key) noexcept {
    typename container_type::reference ref = a[key];
    bool res = ref;
    if (res) {
      --s;
    }
    ref = false;
    return res;
  }

  template <typename K, typename KC = key_compare>
  constexpr std::enable_if_t<detail::is_transparent_v<KC>, size_type> erase(K&& x) noexcept {
    size_type c{};
    for (auto [first, last] = detail::equal_range(index_type::values_v->begin(), index_type::values_v->end(), x, key_compare{}); first != last;) {
      c += erase(*first++);
    }
    return c;
  }

  void swap(set& other) noexcept {
    set cp = *this;
    *this = other;
    other = cp;
  }

  [[nodiscard]] constexpr size_type count(const key_type& key) const noexcept { return index_type{}(key) && a[key]; }

  template <typename K, typename KC = key_compare>
  [[nodiscard]] constexpr std::enable_if_t<detail::is_transparent_v<KC>, size_type> count(const K& x) const {
    size_type c{};
    for (auto [first, last] = detail::equal_range(index_type::values_v->begin(), index_type::values_v->end(), x, key_compare{}); first != last; ++first) {
      c += count(*first);
    }
    return c;
  }

  [[nodiscard]] constexpr const_iterator find(const key_type& key) const noexcept {
    if (auto i = index_type{}(key); i && a.test(key))
      return const_iterator{this, index_type::values_v->begin(), index_type::values_v->end(), &(*index_type::values_v)[*i]};
    return end();
  }

  template <typename K, typename KC = key_compare>
  [[nodiscard]] constexpr std::enable_if_t<detail::is_transparent_v<KC>, const_iterator> find(const K& x) const {
    for (auto [first, last] = detail::equal_range(index_type::values_v->begin(), index_type::values_v->end(), x, key_compare{}); first != last; ++first) {
      if (a.test(*first)) {
        return find(*first);
      }
    }
    return end();
  }

  [[nodiscard]] constexpr bool contains(const key_type& key) const noexcept { return count(key); }

  template <typename K, typename KC = key_compare>
  [[nodiscard]] constexpr std::enable_if_t<detail::is_transparent_v<KC>, bool> contains(const K& x) const noexcept {
    return count(x) > 0;
  }

  [[nodiscard]] constexpr std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const noexcept { return {lower_bound(key), upper_bound(key)}; }

  template <typename K, typename KC = key_compare>
  [[nodiscard]] constexpr std::enable_if_t<detail::is_transparent_v<KC>, std::pair<const_iterator, const_iterator>> equal_range(const K& x) const noexcept {
    return {lower_bound(x), upper_bound(x)};
  }

  [[nodiscard]] constexpr const_iterator lower_bound(const key_type& key) const noexcept {
    if (auto i = index_type{}(key)) {
      auto it = const_iterator{this, index_type::values_v->begin(), index_type::values_v->end(), &(*index_type::values_v)[*i]};
      return a.test(key) ? it : std::next(it);
    }
    return end();
  }

  template <typename K, typename KC = key_compare>
  [[nodiscard]] constexpr std::enable_if_t<detail::is_transparent_v<KC>, const_iterator> lower_bound(const K& x) const noexcept {
    auto [first, last] = detail::equal_range(index_type::values_v->begin(), index_type::values_v->end(), x, key_compare{});
    return first != last ? lower_bound(*first) : end();
  }

  [[nodiscard]] constexpr const_iterator upper_bound(const key_type& key) const noexcept {
    if (auto i = index_type{}(key)) {
      return std::next(const_iterator{this, index_type::values_v->begin(), index_type::values_v->end(), &(*index_type::values_v)[*i]});
    }
    return end();
  }

  template <typename K, typename KC = key_compare>
  [[nodiscard]] constexpr std::enable_if_t<detail::is_transparent_v<KC>, const_iterator> upper_bound(const K& x) const noexcept {
    auto [first, last] = detail::equal_range(index_type::values_v->begin(), index_type::values_v->end(), x, key_compare{});
    return first != last ? upper_bound(*std::prev(last)) : end();
  }

  [[nodiscard]] constexpr key_compare key_comp() const { return {}; }

  [[nodiscard]] constexpr value_compare value_comp() const { return {}; }

  [[nodiscard]] constexpr friend bool operator==(const set& lhs, const set& rhs) noexcept { return lhs.a == rhs.a; }

  [[nodiscard]] constexpr friend bool operator!=(const set& lhs, const set& rhs) noexcept { return lhs.a != rhs.a; }

  [[nodiscard]] constexpr friend bool operator<(const set& lhs, const set& rhs) noexcept {
    if (lhs.s < rhs.s) {
      return true;
    }
    if (rhs.s < lhs.s) {
      return false;
    }

    for (auto& e : *index_type::values_v) {
      if (auto c = rhs.contains(e); c != lhs.contains(e)) {
        return c;
      }
    }
    return false;
  }

  [[nodiscard]] constexpr friend bool operator<=(const set& lhs, const set& rhs) noexcept { return !(rhs < lhs); }

  [[nodiscard]] constexpr friend bool operator>(const set& lhs, const set& rhs) noexcept { return rhs < lhs; }

  [[nodiscard]] constexpr friend bool operator>=(const set& lhs, const set& rhs) noexcept { return !(lhs < rhs); }

  template <typename Pred>
  size_type erase_if(Pred pred) {
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
  //...

 private:
  container_type a;
  std::size_t s{};
};

template <typename V, int = 0>
explicit set(V starter) -> set<V>;

} // namespace magic_enum::containers

namespace std {

template <auto I, typename E, typename V, typename Index>
constexpr std::enable_if_t<(std::is_integral_v<decltype(I)> && I < magic_enum::enum_count<E>()), V&> get(magic_enum::containers::array<E, V, Index>& a) noexcept {
  return a.a[I];
}

template <auto I, typename E, typename V, typename Index>
constexpr std::enable_if_t<(std::is_integral_v<decltype(I)> && I < magic_enum::enum_count<E>()), V&&> get(magic_enum::containers::array<E, V, Index>&& a) noexcept {
  return std::move(a.a[I]);
}

template <auto I, typename E, typename V, typename Index>
constexpr std::enable_if_t<(std::is_integral_v<decltype(I)> && I < magic_enum::enum_count<E>()), const V&> get(const magic_enum::containers::array<E, V, Index>& a) noexcept {
  return a.a[I];
}

template <auto I, typename E, typename V, typename Index>
constexpr std::enable_if_t<(std::is_integral_v<decltype(I)> && I < magic_enum::enum_count<E>()), const V&&> get(const magic_enum::containers::array<E, V, Index>&& a) noexcept {
  return std::move(a.a[I]);
}

template <auto Enum, typename E, typename V, typename Index>
constexpr std::enable_if_t<std::is_same_v<decltype(Enum), E> && magic_enum::enum_contains(Enum), V&> get(magic_enum::containers::array<E, V, Index>& a) noexcept {
  return a[Enum];
}

template <auto Enum, typename E, typename V, typename Index>
constexpr std::enable_if_t<std::is_same_v<decltype(Enum), E> && magic_enum::enum_contains(Enum), V&&> get(magic_enum::containers::array<E, V, Index>&& a) noexcept {
  return std::move(a[Enum]);
}

template <auto Enum, typename E, typename V, typename Index>
constexpr std::enable_if_t<std::is_same_v<decltype(Enum), E> && magic_enum::enum_contains(Enum), const V&> get(const magic_enum::containers::array<E, V, Index>& a) noexcept {
  return a[Enum];
}

template <auto Enum, typename E, typename V, typename Index>
constexpr std::enable_if_t<std::is_same_v<decltype(Enum), E> && magic_enum::enum_contains(Enum), const V&&> get(const magic_enum::containers::array<E, V, Index>&& a) noexcept {
  return std::move(a[Enum]);
}

} // namespace std

#endif // NEARGYE_MAGIC_ENUM_CONTAINERS_HPP
