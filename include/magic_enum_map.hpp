#ifndef MAGIC_ENUM_MAP_HPP
#define MAGIC_ENUM_MAP_HPP

#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <map>

#include "magic_enum.hpp"

namespace magic_enum
{
    // helper struct to check if value is an iterable
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
    /**
     * @brief Maps every enum entry in E to a unique value T
     * 
     * @tparam E enum type used as key
     * @tparam T mapped value type
     * @tparam Map template class used as map data structure
     * @tparam MapArgs optional arguments passed for map class
     */
    template <
        class E,
        class T,
        template <class...> class Map,
        class... MapArgs>
    class enum_map
    {
        static_assert(std::is_enum_v<E>, "magic_enum::enum_map requires enum type");

    public:
        // type used to map values
        typedef Map<E, T, MapArgs...> map_type;
        // iterator of map
        typedef typename map_type::iterator iterator;
        // const_iterator of map
        typedef typename map_type::const_iterator const_iterator;

        // type used by map to store entry pairs
        typedef typename map_type::value_type value_type;
        // type T
        typedef typename map_type::mapped_type mapped_type;
        // type E
        typedef typename map_type::key_type enum_type;
        
        /**
         * @brief Constructs a new enum map object, creating 
         * an entry for each possible enum value. 
         */
        enum_map()
        {
            for(size_t i = 0; i < enum_count<E>(); i++)
                _map[enum_value<E>(i)];
        };

        /**
         * @brief Constructs a new enum map object, initializing 
         * entries using passed args. 
         * 
         * @tparam Args parameter pack consisting of type T 
         * convertible values 
         * @param args entries of type T 
         */
        template <
            typename... Args,
            typename = typename std::enable_if<(true && ... && std::is_convertible_v<Args, T>), void>::type>
        enum_map(Args &&...args) : enum_map()
        {
            insert(begin(), std::array<T, sizeof...(Args)>{{args...}});
        }

        /**
         * @brief Constructs a new enum map object, initializing 
         * entries using passed args. 
         * 
         * @tparam Args parameter pack consisting of type T 
         * convertible values 
         * @param args entries of type T 
         */
        template <
            typename... Args,
            typename = typename std::enable_if<(true && ... && std::is_convertible_v<Args, T>), void>::type>
        enum_map(const Args &...args) : enum_map()
        {
            insert(begin(), std::array<T, sizeof...(Args)>{{args...}});
        }

        /**
         * @brief Constructs a new enum map object, initializing
         * entries using values from an iterable. 
         * 
         * @tparam V iterable containing type T 
         * @param values iterable container 
         */
        template <typename V,
                  typename = typename std::enable_if<(is_iterable<V>::value && (std::is_convertible_v<typename std::iterator_traits<typename V::iterator>::value_type, T> || std::is_convertible_v<typename V::iterator::value_type, value_type>)), void>::type>
        enum_map(const V &values) : enum_map()
        {
            insert(begin(), values);
        }

        /**
         * @brief Constructs a new enum map object, initializing 
         * entries using an initializer list. 
         *
         * @param l initializer list of type Map<E, T, MapArgs...>::value_type 
         */
        enum_map(std::initializer_list<value_type> l) : enum_map()
        {
            for (const value_type &v : l)
                _map[v.first] = v.second;
        }

        /**
         * @brief Returns an iterator to the first element of the map. 
         *
         * @return const_iterator, Map<E,T,MapArgs...>::const_iterator 
         */
        const_iterator begin() const { return _map.begin(); }
        /**
         * @brief Returns an iterator to the first element of the map.
         *
         * @return iterator, Map<E,T,MapArgs...>::iterator 
         */
        iterator begin() { return _map.begin(); }

        /**
         * @brief Returns an iterator to the element following the last 
         * element of the map. 
         *
         * @return const_iterator, Map<E,T,MapArgs...>::const_iterator 
         */
        const_iterator end() const { return _map.end(); }
        /**
         * @brief Returns an iterator to the element following the last 
         * element of the map. 
         *
         * @return iterator, Map<E, T, MapArgs...>::iterator 
         */
        iterator end() { return _map.end(); }

        /**
         * @brief Finds an element with key equivalent to enum value. 
         * 
         * @param e enum value 
         * @return const_iterator, Map<E,T,MapArgs...>::const_iterator  
         */
        const_iterator find(const E &e) const { return _map.find(e); }
        /**
         * @brief Finds an element with key equivalent to enum value. 
         * 
         * @param e enum value 
         * @return iterator, Map<E,T,MapArgs...>::iterator 
         */
        iterator find(const E &e) { return _map.find(e); }
        
        /**
         * @brief Returns a reference to the mapped value of the element 
         * with key equivalent to enum value. 
         * 
         * @param e enum value 
         * @return const T&, reference to mapped value 
         */
        const T &at(const E &e) const { return _map.at(e); }
        /**
         * @brief Returns a reference to the mapped value of the element
         * with key equivalent to enum value.
         *
         * @param e enum value
         * @return T&, reference to mapped value
         */
        T &at(const E &e) { return _map.at(e); }

        /**
         * @brief Inserts elements from a parameter pack args, begining at 
         * iterator begin and inserting for the number of passed args (will overwrite).
         *
         * @tparam Args parameter pack consisting of type T 
         * convertible values 
         * @param begin iterator specifying where to begin insertion 
         * @param args entries of type T 
         * @return iterator pointing to entry immediately 
         * after the last inserted value 
         */
        template <typename... Args,
                  typename = typename std::enable_if<(true && ... && std::is_convertible_v<Args, T>), void>::type>
        iterator insert(iterator begin, const Args &...args)
        {
            return insert(begin, std::array<T, sizeof...(Args)>{{args...}});
        }

        /**
         * @brief Inserts elements from an iterable, begining at iterator
         * begin and inserting for the length of values (will overwrite).
         *
         * @tparam V iterable containing type T
         * @param begin iterator specifying where to begin insertion 
         * @param values iterable container
         * @return iterator pointing to entry immediately 
         * after the last inserted value 
         */
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

        /**
         * @brief Inserts elements from an iterable directly to 
         * specified enum value (will overwrite).
         * 
         * @tparam V iterable containing type std::pair<const E, T>
         * @param values iterable container 
         * @return iterator pointing to last inserted element
         */
        template <typename V,
                  typename = typename std::enable_if<(is_iterable<V>::value && std::is_convertible_v<typename V::iterator::value_type, value_type>), void>::type>
        iterator insert(const V &values)
        {   
            //? Currently dependent on std::pair with first and
            //? and second. Needs to be more generic
            auto v_it = values.begin();
            for (; v_it != values.end(); v_it++)
                _map[v_it->first] = v_it->second;

            v_it--;
            return find(v_it->first);
        }

        /**
         * @brief Returns a reference to the value that is mapped to 
         * a key equivalent to enum value. 
         *
         * @param e enum value 
         * @return T&, reference to mapped value 
         */
        T &operator[](const E &e) { return _map[e]; }
        /**
         * @brief Returns a reference to the value that is mapped to
         * a key equivalent to enum value.
         *
         * @param e enum vale
         * @return T&, reference to mapped value
         */
        T &operator[](E &&e) { return _map[e]; }

    private:
        // underlying data structure used to map values
        map_type _map;
    };
}

#endif // MAGIC_ENUM_MAP_HPP