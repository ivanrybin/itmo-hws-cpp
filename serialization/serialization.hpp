/*
	SFINAE standard types (de)serialization: 
	int, double, pair, string, vector, map.

	Ivan Rybin 2020.
*/

#include <iostream>
#include <cassert>
#include <sstream>

template <typename T>
struct is_pair {
    static constexpr bool value = false;
};

template <typename Key, typename V>
struct is_pair<std::pair<Key, V>> {
    static constexpr bool value = true;
};

template<typename T, typename = void>
struct is_container {
    static constexpr bool value = false;
};

template<typename T>
struct is_container<T, std::void_t<typename T::iterator, typename T::const_iterator>> {
    static constexpr bool value = true;
};

template <typename T, typename = void>
struct is_map {
    static constexpr bool value = false;
};

template <typename T>
struct is_map<T, std::void_t<typename T::key_type, typename T::mapped_type>> {
    static constexpr bool value = true;
};

template <typename T>
using pod_void = std::enable_if_t<std::is_pod<T>::value, void>;

template <typename T>
using pair_void = std::enable_if_t<is_pair<T>::value, void>;

template <typename T>
using iter_void = std::enable_if_t<is_container<T>::value && !is_map<T>::value, void>;

template <typename T>
using map_void = std::enable_if_t<is_container<T>::value && is_map<T>::value, void>;

template <typename T>
using bad_void = std::enable_if_t<!is_container<T>::value && !std::is_pod<T>::value && !is_pair<T>::value && !is_map<T>::value, void>;

template <typename T>
constexpr pod_void<T> serialize(std::ostream& os, T const& obj);

template <typename T>
constexpr pair_void<T> serialize(std::ostream& os, T const& obj);

template <typename T>
constexpr iter_void<T> serialize(std::ostream& os, T const& obj);

template <typename T>
constexpr map_void<T> serialize(std::ostream& os, T const& obj);

template <typename T>
constexpr bad_void<T> serialize(std::ostream& os, T const& obj);

template <typename T>
constexpr pod_void<T> deserialize(std::ostream& is, T& obj);

template <typename T>
constexpr pair_void<T> deserialize(std::ostream& is, T& obj);

template <typename T>
constexpr iter_void<T> deserialize(std::ostream& is, T& obj);

template <typename T>
constexpr map_void<T> deserialize(std::ostream& is, T& obj);

template <typename T>
constexpr bad_void<T> deserialize(std::ostream& is, T& obj);

template <typename T>
constexpr pod_void<T> serialize(std::ostream& os, T const& obj) {
    size_t size = sizeof(obj);
    os.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
    os.write(reinterpret_cast<const char*>(&obj), sizeof(obj));
}

template <typename T>
constexpr pair_void<T> serialize(std::ostream& os, T const& obj) {
    serialize(os, obj.first);
    serialize(os, obj.second);
}

template <typename T>
constexpr iter_void<T> serialize(std::ostream& os, T const& obj) {
    size_t size = obj.size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
    for (const auto& it : obj) {
        serialize(os, it);
    }
}

template <typename T>
constexpr map_void<T> serialize(std::ostream& os, T const& obj) {
    size_t size = obj.size();
    os.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
    for (const auto& it : obj) {
        serialize(os, it);
    }
}

template <typename T>
constexpr bad_void<T> serialize(std::ostream& os, T const& obj) {
    static_assert(std::is_pod<T>::value, "Type is not serializable");
}

template <typename T>
constexpr pod_void<T> deserialize(std::istream& is, T& obj) {
    size_t size = 0;
    is.read(reinterpret_cast<char *>(&size), sizeof(size));
    is.read(reinterpret_cast<char *>(&obj), sizeof(obj));
}

template <typename T>
constexpr iter_void<T> deserialize(std::istream& is, T& obj) {
    size_t size = 0;
    is.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    obj = T(size, typename T::value_type());
    for (auto& it : obj) {
        deserialize(is, it);
    }
}

template <typename T>
constexpr pair_void<T> deserialize(std::istream& is, T& obj) {
    obj = T();
    deserialize(is, obj.first);
    deserialize(is, obj.second);
}

template <typename T>
constexpr map_void<T> deserialize(std::istream& is, T& obj) {
    size_t size = 0;
    is.read(reinterpret_cast<char *>(&size), sizeof(size_t));
    std::pair<typename T::key_type, typename T::mapped_type> p;
    for (size_t i = 0; i < size; ++i) {
        deserialize(is, p);
        obj.insert(p);
    }
}

template <typename T>
constexpr bad_void<T> deserialize(std::istream& is, T& obj) {
    static_assert(std::is_pod<T>::value, "Type is not deserializable");
}
