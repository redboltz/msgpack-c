//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2015 MIZUKI Hirata
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

#ifndef MSGPACK_V1_ITERATOR_DECL_HPP
#define MSGPACK_V1_ITERATOR_DECL_HPP

#if !defined(MSGPACK_USE_CPP03)

#include <msgpack/object_fwd.hpp>

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond

object_kv* begin(object_map &map);
const object_kv* begin(const object_map &map);
object_kv* end(object_map &map);
const object_kv* end(const object_map &map);

object* begin(object_array &array);
const object* begin(const object_array &array);
object* end(object_array &array);
const object* end(const object_array &array);

/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msgpack

#endif // !defined(MSGPACK_USE_CPP03)

#endif // MSGPACK_V1_ITERATOR_DECL_HPP
