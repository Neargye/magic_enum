#ifndef MAGIC_ENUM_MAP_HPP
#define MAGIC_ENUM_MAP_HPP

#include <initializer_list>
#include <iterator>
#include <utility>
#include <type_traits>
#include <map>

#include <iostream>

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
            insert(begin(), args...);
        }

        /**
         * @brief Constructs a new enum map object, initializing
         * entries using values from an iterable. 
         * 
         * @tparam V iterable containing type T 
         * @param values iterable container 
         */
        template <typename V,
                  typename = typename std::enable_if<
                    (is_iterable<V>::value && 
                    (std::is_convertible_v<typename std::iterator_traits<typename V::iterator>::value_type, T> ||
                     std::is_convertible_v<typename std::iterator_traits<typename V::iterator>::value_type, value_type>)), void>>
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
         * @brief Inserts a value at an iterator if the
         * iterator does not exceed the map
         *
         * @param it where to insert
         * @param value value to insert
         * @return iterator pointing to inserted entry
         */
        iterator insert(iterator it, T &&value)
        {
            if (it != end())
                std::get<T>(*it) = std::forward<T>(value);
            return it;
        }

        /**
         * @brief Inserts elements from a parameter pack args, begining at a
         * specified iterator and inserting for the number of passed args 
         * (will overwrite).
         *
         * @tparam Args parameter pack consisting of type T 
         * convertible values 
         * @param it where to begin insertion 
         * @param args entries of type T 
         * @return iterator pointing to entry immediately after the last inserted value 
         */
        template <typename... Args,
                  typename = typename std::enable_if<(true && ... && std::is_convertible_v<Args, T>), void>::type>
        iterator insert(iterator it, Args &&...args)
        {
            return insert(it, {args...});
        }

        /**
         * @brief Inserts elements from an iterable, begining at a specified
         * iterator and inserting for the length of values (will overwrite).
         *
         * @tparam V iterable containing type T
         * @param it where to insert
         * @param values values to insert
         * @return iterator pointing to entry immediately
         * after the last inserted value
         */
        template <typename V>
        typename std::enable_if<(is_iterable<V>::value && std::is_convertible_v<typename std::iterator_traits<typename V::iterator>::value_type, T>), iterator>::type
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

        /**
         * @brief Inserts elements from an initializer list of T, begining at
         * a specified iterator and inserting for the length of the list (will 
         * overwrite).
         *
         * @param it where to begin insertion 
         * @param list initializer list of type T 
         * @return iterator pointing to entry immediately after the last 
         * inserted value 
         */
        iterator insert(iterator it, std::initializer_list<T> &&list)
        {   
            auto l_it = list.begin();
            while(l_it != list.end() && it != end()) {
                insert(it, T(*l_it));
                it++; l_it++;
            }

            return it;
        }

        /**
         * @brief Inserts value into map
         *
         * @param value value to insert
         * @return iterator pointing to inserted entry 
         */
        iterator insert(value_type &&value)
        {
            iterator it = find(std::get<const E>(value));
            return insert(it, std::get<T>(value));
        }

        /**
         * @brief Inserts elements from a parameter pack args, inserting for the number of passed args 
         * (will overwrite).
         *
         * @tparam Args parameter pack consisting of type T
         * convertible values
         * @param it where to begin insertion
         * @param args entries of type T
         * @return iterator pointing to entry immediately after the last inserted value
         */
        template <typename... Args,
                  typename = typename std::enable_if<(true && ... && std::is_convertible_v<Args, value_type>), void>::type>
        iterator insert(Args &&...args)
        {
            return insert({args...});
        }

        /**
         * @brief Inserts elements from an iterable, begining at a specified
         * iterator and inserting for the length of values (will overwrite).
         *
         * @tparam V iterable containing type T
         * @param it where to insert
         * @param values values to insert
         * @return iterator pointing to entry immediately
         * after the last inserted value
         */
        template <typename V>
        typename std::enable_if<(is_iterable<V>::value && std::is_convertible_v<typename std::iterator_traits<typename V::iterator>::value_type, value_type>), iterator>::type
        insert(iterator it, const V &values)
        {   
            (void)it;
            return insert(values);
        }

        /**
         * @brief Inserts elements from an initializer list of T, begining at
         * a specified iterator and inserting for the length of the list (will
         * overwrite).
         *
         * @param list initializer list of type value_type
         * @return iterator pointing to the last inserted entry
         */
        iterator insert(std::initializer_list<value_type> &&list)
        {
            iterator it = end();
            for(auto l_it = list.begin(); l_it != list.end(); l_it++)
                it = insert(value_type(*l_it));

            return it;
        }

        /**
         * @brief Inserts elements from an iterable directly to specified enum 
         * value (will overwrite).
         * 
         * @tparam V iterable containing type value_type
         * @param values values to insert
         * @return iterator pointing to last inserted element 
         */
        template <typename V,
                  typename = typename std::enable_if<(is_iterable<V>::value && std::is_convertible_v<typename V::iterator::value_type, value_type>), void>::type>
        iterator insert(const V &values)
        {   
            iterator it = end();
            for (auto v_it = values.begin(); v_it != values.end(); v_it++)
                it = insert(value_type(*v_it));

            return it;
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

        /**
         * @brief Returns a reference to the mapped value of the element
         * with key equivalent to enum value.
         *
         * @param e enum value
         * @return const T&, reference to mapped value
         */
        const T &operator[](const E &e) const { return _map.at(e); }

    private:
        // underlying data structure used to map values
        map_type _map;
    };
}

#endif // MAGIC_ENUM_MAP_HPP