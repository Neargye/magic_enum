#ifndef MAGIC_ENUM_MAP_HPP
#define MAGIC_ENUM_MAP_HPP

#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <map>

#include "magic_enum.hpp"

namespace magic_enum
{
    using std::begin;
    using std::end;

    template <typename C>
    struct is_iterable
    {
        typedef long false_type;
        typedef char true_type;

        template <class T>
        static false_type check(...);
        template <class T>
        static true_type check(int,
                               typename T::const_iterator = C().end());

        const static bool value = sizeof(check<C>(0)) == sizeof(true_type);
    };

    template <
        class E,
        class T,
        template <class...> class Map,
        class... MapArgs>
    class enum_map
    {
        static_assert(std::is_enum_v<E>, "magic_enum::enum_map requires enum type");

    public:
        typedef Map<E, T, MapArgs...> map_type;
        typedef typename map_type::iterator iterator;
        typedef typename map_type::const_iterator const_iterator;
        typedef typename map_type::value_type value_type;
        typedef typename map_type::mapped_type mapped_type;
        typedef typename map_type::key_type enum_type;

        T &operator[](const E &e) { return _map[e]; }
        T &operator[](E &&e) { return _map[e]; }

        enum_map()
        {
            using D = std::decay_t<E>;
            detail::for_each<D>([this](auto e){ (*this)[e]; }, std::make_index_sequence<detail::count_v<D>>{});
        };

        template <
            typename... Args,
            typename = typename std::enable_if<(true && ... && std::is_convertible_v<Args, T>), void>::type>
        enum_map(Args &&...args) : enum_map()
        {
            insert(begin(), std::array<T, sizeof...(Args)>{{args...}});
        }

        template <
            typename... Args,
            typename = typename std::enable_if<(true && ... && std::is_convertible_v<Args, T>), void>::type>
        enum_map(const Args &...args) : enum_map()
        {
            insert(begin(), std::array<T, sizeof...(Args)>{{args...}});
        }

        template <typename V,
                  typename = typename std::enable_if<(is_iterable<V>::value && (std::is_convertible_v<typename std::iterator_traits<typename V::iterator>::value_type, T> || std::is_convertible_v<typename V::iterator::value_type, value_type>)), void>::type>
        enum_map(const V &values) : enum_map()
        {
            insert(begin(), values);
        }

        enum_map(std::initializer_list<value_type> l) : enum_map()
        {
            for (const value_type &v : l)
                _map[v.first] = v.second;
        }

        const_iterator begin() const { return _map.begin(); }
        const_iterator end() const { return _map.end(); }
        iterator begin() { return _map.begin(); }
        iterator end() { return _map.end(); }

        iterator find(const E &e) { return _map.find(e); }
        const_iterator find(const E &e) const { return _map.find(e); }

        T &at(const E &e) { return _map.at(e); }
        const T &at(const E &e) const { return _map.at(e); }

        template <typename... Args,
                  typename = typename std::enable_if<(true && ... && std::is_convertible_v<Args, T>), void>::type>
        iterator insert(iterator begin, Args &&...args)
        {
            return insert(begin, std::array<T, sizeof...(Args)>{{args...}});
        }

        template <typename V,
                  typename = typename std::enable_if<(is_iterable<V>::value && std::is_convertible_v<typename std::iterator_traits<typename V::iterator>::value_type, T>), void>::type>
        iterator insert(iterator begin, const V &values)
        {
            auto v_it = values.begin();
            while (v_it != values.end() && begin != end())
            {
                begin->second = *v_it;
                begin++;
                v_it++;
            }

            return begin;
        }

        template <typename V,
                  typename = typename std::enable_if<(is_iterable<V>::value && std::is_convertible_v<typename V::iterator::value_type, value_type>), void>::type>
        iterator insert(const V &values)
        {
            auto v_it = values.begin();
            for (; v_it != values.end(); v_it++)
                _map[v_it->first] = v_it->second;

            v_it--;
            return _map.find(v_it->first);
        }

    private:
        map_type _map;
    };
}

#endif // MAGIC_ENUM_MAP_HPP