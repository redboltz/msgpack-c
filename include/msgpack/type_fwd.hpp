//
// MessagePack for C++ static resolution routine
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

#ifndef TYPE_FWD_HPP
#define TYPE_FWD_HPP

#include "msgpack/adaptor/int_fwd.hpp"
#include "msgpack/adaptor/bool_fwd.hpp"
#include "msgpack/adaptor/char_ptr_fwd.hpp"
#include "msgpack/adaptor/deque_fwd.hpp"
#include "msgpack/adaptor/fixint_fwd.hpp"
#include "msgpack/adaptor/float_fwd.hpp"
#include "msgpack/adaptor/int_fwd.hpp"
#include "msgpack/adaptor/list_fwd.hpp"
#include "msgpack/adaptor/map_fwd.hpp"
#include "msgpack/adaptor/msgpack_tuple_fwd.hpp"
#include "msgpack/adaptor/nil_fwd.hpp"
#include "msgpack/adaptor/pair_fwd.hpp"
#include "msgpack/adaptor/raw_fwd.hpp"
#include "msgpack/adaptor/set_fwd.hpp"
#include "msgpack/adaptor/string_fwd.hpp"
#include "msgpack/adaptor/vector_fwd.hpp"
#include "msgpack/adaptor/vector_bool_fwd.hpp"
#include "msgpack/adaptor/vector_char_fwd.hpp"

#if defined(MSGPACK_USE_CPP03)

#include "msgpack/adaptor/tr1/unordered_map_fwd.hpp"
#include "msgpack/adaptor/tr1/unordered_set_fwd.hpp"

#else  // defined(MSGPACK_USE_CPP03)

#include "adaptor/cpp11/array_fwd.hpp"
#include "adaptor/cpp11/array_char_fwd.hpp"
#include "adaptor/cpp11/forward_list_fwd.hpp"
#include "adaptor/cpp11/tuple_fwd.hpp"
#include "adaptor/cpp11/unordered_map_fwd.hpp"
#include "adaptor/cpp11/unordered_set_fwd.hpp"

#endif // defined(MSGPACK_USE_CPP03)

#endif // TYPE_FWD_HPP
