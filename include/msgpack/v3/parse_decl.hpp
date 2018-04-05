//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2018-2017 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGPACK_V3_PARSE_DECL_HPP
#define MSGPACK_V3_PARSE_DECL_HPP

#include "msgpack/v2/parse_decl.hpp"

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v3) {
/// @endcond

namespace detail {

using v2::detail::context;

} // detail

using v2::parser;
using v2::parse;

namespace detail {

using v2::detail::parse_helper;
using v2::detail::parse_imp;

} // detail

/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v3)
/// @endcond

}  // namespace msgpack


#endif // MSGPACK_V3_PARSE_DECL_HPP
