//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2015 FURUHASHI Sadayuki and KONDO Takatoshi
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

#ifndef MSGPACK_V1_OBJECT_FWD_DECL_HPP
#define MSGPACK_V1_OBJECT_FWD_DECL_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/object.h"

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


} // namespace msgpack

#endif // MSGPACK_V1_OBJECT_FWD_DECL_HPP
