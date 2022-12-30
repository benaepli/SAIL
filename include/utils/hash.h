#pragma once
#include <cstdint>
#include <functional>

namespace sail::utils
{
    inline void hashCombine(size_t& seed) {}

    template<typename T, typename... Rest>
    inline void hashCombine(size_t& seed, const T& v, Rest... rest)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        hashCombine(seed, rest...);
    }
}  // namespace sail::utils

#define SAIL_HASH_FUNCTION(type, function) \
    namespace std \
    { \
        template<> \
        struct hash<type> \
        { \
            auto operator()(const type& val) const -> size_t \
            { \
                return function(val); \
            } \
        }; \
    }

#define SAIL_MAKE_HASHABLE(type, ...) \
    SAIL_HASH_FUNCTION(type, \
                       [](const type& val) -> size_t \
                       { \
                           size_t seed = 0; \
                           sail::utils::hashCombine(seed, __VA_ARGS__); \
                           return seed; \
                       })

#define SAIL_HASH_DECLARATION(type) \
    namespace std \
    { \
        template<> \
        struct hash<type> \
        { \
            auto operator()(const type& val) const -> size_t; \
        }; \
    }

#define SAIL_HASH_DEFINITION(type, function) \
    namespace std \
    { \
        auto hash<type>::operator()(const type& val) const -> size_t \
        { \
            return function(val); \
        } \
    }

#define SAIL_MAKE_HASHABLE_DEFINITION(type, ...) \
    SAIL_HASH_DEFINITION(type, \
                         [](const type& val) -> size_t \
                         { \
                             size_t seed = 0; \
                             sail::utils::hashCombine(seed, __VA_ARGS__); \
                             return seed; \
                         })