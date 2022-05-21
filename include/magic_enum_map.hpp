#ifndef _MAGIC_ENUM_MAP_HPP_
#define _MAGIC_ENUM_MAP_HPP_

#include <yvals_core.h>
#include <iterator>
#include <map>

#include "magic_enum.hpp"

namespace map_utils {
    template <class _Mty, class = void>
    struct map_reference
    {
        using type = typename _Mty::value_type &;
    };

    template <class _Mty>
    struct map_reference<_Mty, std::void_t<typename _Mty::reference>>
    {
        using type = typename _Mty::reference;
    };

    template <class _Mty>
    using map_reference_t = typename map_reference<_Mty>::type;

    template <class _Mty, class = void>
    struct map_const_reference
    {
        using type = typename const map_reference_t<_Mty> &;
    };

    template <class _Mty>
    struct map_const_reference<_Mty, std::void_t<typename _Mty::const_reference>>
    {
        using type = typename _Mty::const_reference;
    };

    template <class _Mty>
    using map_const_reference_t = typename map_const_reference<_Mty>::type;

    template <class _Mty>
    struct map_pointer
    {
        using type = typename std::_Get_pointer_type<_Mty>::type;
    };

    template <class _Mty>
    using map_pointer_t = typename map_pointer<_Mty>::type;

    template <class _Mty>
    struct map_const_pointer
    {
        using type = typename std::_Get_const_pointer_type<_Mty>::type;
    };

    template <class _Mty>
    using map_const_pointer_t = typename map_const_pointer<_Mty>::type;

    template <class _Mty, class = void>
    struct map_reverse_iterator
    {
        using type = std::reverse_iterator<typename _Mty::iterator>;
    };

    template <class _Mty>
    struct map_reverse_iterator<_Mty, std::void_t<typename _Mty::reverse_iterator>>
    {
        using type = typename _Mty::reverse_iterator;
    };

    template <class _Mty>
    using map_reverse_iterator_t = typename map_reverse_iterator<_Mty>::type;

    template <class _Mty, class = void>
    struct map_const_reverse_iterator
    {
        using type = std::reverse_iterator<typename _Mty::const_iterator>;
    };

    template <class _Mty>
    struct map_const_reverse_iterator<_Mty, std::void_t<typename _Mty::const_reverse_iterator>>
    {
        using type = typename _Mty::const_reverse_iterator;
    };

    template <class _Mty>
    using map_const_reverse_iterator_t = typename map_const_reverse_iterator<_Mty>::type;

    template <typename T, typename = void>
    struct is_iterable : std::false_type
    {
    };

    template <typename T>
    struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())),
                                      decltype(std::end(std::declval<T>()))>> : std::true_type
    {
    };

    template <typename T>
    constexpr bool is_iterable_v = is_iterable<T>::value;
};

namespace magic_enum
{
    template <typename _Mty, typename = void>
    struct _Is_valid_map : std::false_type
    {
    };

    template <typename _Mty>
    struct _Is_valid_map<_Mty, std::void_t<typename _Mty::key_type,
                                           typename _Mty::mapped_type,
                                           typename _Mty::value_type,
                                           typename _Mty::iterator,
                                           typename _Mty::const_iterator>> : std::true_type
    {
    };

    template <typename _Mty>
    constexpr bool _Is_valid_map_v = _Is_valid_map<_Mty>::value;

    template <class _Ety, class _Ty, template <class...> class _Map, class... _Map_Args>
    class enum_map
    {
    public:
        using map_type = _Map<_Ety, _Ty, _Map_Args...>;
        static_assert(_Is_valid_map_v<map_type>, "map type provided is not valid");
        static_assert(std::is_enum_v<_Ety>, "magic_enum::enum_map requires enum type");

        using enum_type = typename map_type::key_type;
        using mapped_type = typename map_type::mapped_type;
        using value_type = typename map_type::value_type;

        using pointer = map_utils::map_pointer_t<map_type>;
        using const_pointer = map_utils::map_const_pointer_t<map_type>;
        using reference = map_utils::map_reference_t<map_type>;
        using const_reference = map_utils::map_const_reference_t<map_type>;

        using iterator = typename map_type::iterator;
        using const_iterator = typename map_type::const_iterator;
        using reverse_iterator = map_utils::map_reverse_iterator_t<map_type>;
        using const_reverse_iterator = map_utils::map_const_reverse_iterator_t<map_type>;

        enum_map()
        {
            for (size_t i = 0; i < enum_count<enum_type>(); i++)
                _map[enum_value<enum_type>(i)];
        }

        template <
            typename... Args,
            typename = typename std::enable_if<(true && ... && std::is_convertible_v<Args, mapped_type>), void>::type>
        enum_map(Args &&...args) : enum_map()
        {
            insert(begin(), args...);
        }

        template <typename V,
                  typename = typename std::enable_if<
                      (map_utils::is_iterable_v<V> &&
                       (std::is_convertible_v<typename std::iterator_traits<typename V::iterator>::value_type, mapped_type> ||
                        std::is_convertible_v<typename std::iterator_traits<typename V::iterator>::value_type, value_type>)),
                      void>>
        enum_map(const V &values) : enum_map()
        {
            insert(begin(), values);
        }

        enum_map(std::initializer_list<value_type> l) : enum_map()
        {
            for (const value_type &v : l)
                _map[v.first] = v.second;
        }

        iterator begin() { return std::begin(_map); }
        const_iterator begin() const { return std::begin(_map); }

        iterator end() { return std::end(_map); }
        const_iterator end() const { return std::end(_map); }

        iterator find(const enum_type &e)
        {   
            return std::find_if(begin(), end(), [e](const value_type &v)
                                { return v.first == e; });
        }
        const_iterator find(const enum_type &e) const {
            return std::find_if(begin(), end(), [e](const value_type &v)
                                { return v.first == e; });
        }

        iterator insert(iterator it, mapped_type &&value)
        {
            if (it != end())
                std::get<mapped_type>(*it) = std::forward<mapped_type>(value);
            return it;
        }

        template <typename... Args,
                  typename = typename std::enable_if<(true && ... && std::is_convertible_v<Args, mapped_type>), void>::type>
        iterator insert(iterator it, Args &&...args)
        {
            return insert(it, {args...});
        }

        template <typename V>
        typename std::enable_if<(map_utils::is_iterable_v<V> && std::is_convertible_v<typename std::iterator_traits<typename V::iterator>::value_type, mapped_type>), iterator>::type
        insert(iterator it, const V &values)
        {
            auto v_it = values.begin();
            while (v_it != values.end() && it != end())
            {
                insert(it, *v_it);
                it++;
                v_it++;
            }

            return it;
        }

        iterator insert(iterator it, std::initializer_list<mapped_type> &&list)
        {
            auto l_it = list.begin();
            while(l_it != list.end() && it != end()) {
                insert(it, mapped_type(*l_it));
                it++; l_it++;
            }

            return it;
        }

        iterator insert(value_type &&value)
        {
            iterator it = find(std::get<const enum_type>(value));
            return insert(it, std::get<mapped_type>(value));
        }

        template <typename... Args,
                  typename = typename std::enable_if<(true && ... && std::is_convertible_v<Args, value_type>), void>::type>
        iterator insert(Args &&...args)
        {
            return insert({args...});
        }

        template <typename V>
        typename std::enable_if<(map_utils::is_iterable<V>::value && std::is_convertible_v<typename std::iterator_traits<typename V::iterator>::value_type, value_type>), iterator>::type
        insert(iterator it, const V &values)
        {
            (void)it;
            return insert(values);
        }

        iterator insert(std::initializer_list<value_type> &&list)
        {
            iterator it = end();
            for(auto l_it = list.begin(); l_it != list.end(); l_it++)
                it = insert(value_type(*l_it));

            return it;
        }

        template <typename V,
                  typename = typename std::enable_if<(map_utils::is_iterable<V>::value && std::is_convertible_v<typename V::iterator::value_type, value_type>), void>::type>
        iterator insert(const V &values)
        {
            iterator it = end();
            for (auto v_it = values.begin(); v_it != values.end(); v_it++)
                it = insert(value_type(*v_it));

            return it;
        }

        mapped_type &at(const enum_type &e) { return (*find(e)).second; }
        const mapped_type &at(const enum_type &e) const { return (*find(e)).second; }
       
        mapped_type &operator[](const enum_type &e) { return at(e); }
        const mapped_type &operator[](const enum_type &e) const { return at(e); }
        mapped_type &operator[](enum_type &&e) { return at(e); }

    private:
        map_type _map;
    };
}

#endif // _MAGIC_ENUM_MAP_HPP_