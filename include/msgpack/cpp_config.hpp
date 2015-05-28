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
#ifndef MSGPACK_CPP_CONFIG_HPP
#define MSGPACK_CPP_CONFIG_HPP

#include "msgpack/versioning.hpp"

#if !defined(MSGPACK_USE_CPP03)
  // If MSVC would support C++11 completely,
  // then 'defined(_MSC_VER)' would replace with
  // '_MSC_VER < XXXX'
# if (__cplusplus < 201103L) || defined(_MSC_VER)
#  define MSGPACK_USE_CPP03
# endif
#endif // MSGPACK_USE_CPP03

#if __cplusplus < 201103L

#if !defined(nullptr)
#  if _MSC_VER < 1600
#    define nullptr (0)
#  endif
#endif

#endif // __cplusplus < 201103L

#include <v1/msgpack/cpp_config_decl.hpp>
#include <v2/msgpack/cpp_config.hpp>
#include <v1/msgpack/cpp_config.hpp>


#endif // MSGPACK_CPP_CONFIG_HPP
