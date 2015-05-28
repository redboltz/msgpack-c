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

#ifndef MSGPACK_V2_OBJECT_FWD_HPP
#define MSGPACK_V2_OBJECT_FWD_HPP

#include "msgpack/versioning.hpp"

namespace msgpack {

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


} // namespace msgpack

#endif // MSGPACK_V2_OBJECT_FWD_HPP
