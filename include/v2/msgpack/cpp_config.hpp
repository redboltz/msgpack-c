//
// MessagePack for C++ C++03/C++11 Adaptation
//
// Copyright (C) 2015 KONDO Takatoshi
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
#ifndef MSGPACK_V2_CPP_CONFIG_HPP
#define MSGPACK_V2_CPP_CONFIG_HPP

#include "msgpack/versioning.hpp"

#if __cplusplus < 201103L

#include <memory>

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v2) {
/// @endcond

    using v1::unique_ptr;
    using v1::move;
    using v1::enable_if;

/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v2)
/// @endcond

}  // namespace msgpack

#else  // __cplusplus < 201103L

#include <memory>
#include <tuple>

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v2) {
/// @endcond

    // unique_ptr
    using std::unique_ptr;
    // using std::make_unique; // since C++14
    using std::hash;

    // utility
    using std::move;
    using std::swap;
    using std::enable_if;

/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v2)
/// @endcond

}  // namespace msgpack

#endif // __cplusplus < 201103L

#endif // MSGPACK_V2_CPP_CONFIG_HPP
