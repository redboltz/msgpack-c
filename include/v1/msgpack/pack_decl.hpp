//
// MessagePack for C++ serializing routine
//
// Copyright (C) 2008-2013 FURUHASHI Sadayuki and KONDO Takatoshi
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
#ifndef MSGPACK_V1_PACK_DECL_HPP
#define MSGPACK_V1_PACK_DECL_HPP

#include "msgpack/versioning.hpp"

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond
template <typename Stream>
class packer;
template <typename Stream, typename T>
void pack(Stream& s, const T& v);
template <typename T>
char take8_8(T d);
template <typename T>
char take8_16(T d);
template <typename T>
char take8_32(T d);
template <typename T>
char take8_64(T d);

/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msgpack

#endif // MSGPACK_V1_PACK_DECL_HPP
