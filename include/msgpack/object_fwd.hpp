//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2014 FURUHASHI Sadayuki and KONDO Takatoshi
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//

#ifndef MSGPACK_OBJECT_FWD_HPP
#define MSGPACK_OBJECT_FWD_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/zone.hpp"
#include "msgpack/object.h"

#include <typeinfo>

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace type {
    enum object_type {
        NIL                 = MSGPACK_OBJECT_NIL,
        BOOLEAN             = MSGPACK_OBJECT_BOOLEAN,
        POSITIVE_INTEGER    = MSGPACK_OBJECT_POSITIVE_INTEGER,
        NEGATIVE_INTEGER    = MSGPACK_OBJECT_NEGATIVE_INTEGER,
        FLOAT               = MSGPACK_OBJECT_FLOAT,
#if defined(MSGPACK_USE_LEGACY_NAME_AS_FLOAT)
        DOUBLE              = MSGPACK_OBJECT_DOUBLE, // obsolete
#endif // MSGPACK_USE_LEGACY_NAME_AS_FLOAT
        STR                 = MSGPACK_OBJECT_STR,
        BIN                 = MSGPACK_OBJECT_BIN,
        ARRAY               = MSGPACK_OBJECT_ARRAY,
        MAP                 = MSGPACK_OBJECT_MAP,
        EXT                 = MSGPACK_OBJECT_EXT
    };
} // namespace type

struct object;
struct object_kv;

struct object_array;
struct object_map;

struct object_str;
struct object_bin;
struct object_ext;
class type_error;
/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v1)
/// @endcond

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v2) {
/// @endcond
namespace type {
using v1::type::object_type;
using v1::type::NIL;
using v1::type::BOOLEAN;
using v1::type::POSITIVE_INTEGER;
using v1::type::NEGATIVE_INTEGER;
using v1::type::FLOAT;
#if defined(MSGPACK_USE_LEGACY_NAME_AS_FLOAT)
using v1::type::DOUBLE;
#endif // MSGPACK_USE_LEGACY_NAME_AS_FLOAT
using v1::type::STR;
using v1::type::BIN;
using v1::type::ARRAY;
using v1::type::MAP;
using v1::type::EXT;
} // namespace type

using v1::object;
using v1::object_kv;

using v1::object_array;
using v1::object_map;

using v1::object_str;
using v1::object_bin;
using v1::object_ext;

using v1::type_error;
/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v2)
/// @endcond


/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond

struct object_array {
    uint32_t size;
    msgpack::object* ptr;
};

struct object_map {
    uint32_t size;
    msgpack::object_kv* ptr;
};

struct object_str {
    uint32_t size;
    const char* ptr;
};

struct object_bin {
    uint32_t size;
    const char* ptr;
};

struct object_ext {
    int8_t type() const { return ptr[0]; }
    const char* data() const { return &ptr[1]; }
    uint32_t size;
    const char* ptr;
};

struct object {
    union union_type {
        bool boolean;
        uint64_t u64;
        int64_t  i64;
#if defined(MSGPACK_USE_LEGACY_NAME_AS_FLOAT)
        double   dec; // obsolete
#endif // MSGPACK_USE_LEGACY_NAME_AS_FLOAT
        double   f64;
        msgpack::object_array array;
        msgpack::object_map map;
        msgpack::object_str str;
        msgpack::object_bin bin;
        msgpack::object_ext ext;
    };

    msgpack::type::object_type type;
    union_type via;

    bool is_nil() const { return type == msgpack::type::NIL; }

    template <typename T>
    T as() const;

    template <typename T>
    T& convert(T& v) const;
    template <typename T>
    T* convert(T* v) const;

    object();

    object(const msgpack_object& o);

    template <typename T>
    explicit object(const T& v);

    template <typename T>
    object(const T& v, msgpack::zone& z);

    // obsolete
    template <typename T>
    object(const T& v, msgpack::zone* z);

    template <typename T>
    object& operator=(const T& v);

    operator msgpack_object() const;

    struct with_zone;

private:
    struct implicit_type;

public:
    implicit_type convert() const;
};

class type_error : public std::bad_cast { };

struct object_kv {
    msgpack::object key;
    msgpack::object val;
};

struct object::with_zone : msgpack::object {
    with_zone(msgpack::zone& zone) : zone(zone) { }
    msgpack::zone& zone;
private:
    with_zone();
};

/// @cond
} // MSGPACK_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msgpack

#endif // MSGPACK_OBJECT_FWD_HPP
