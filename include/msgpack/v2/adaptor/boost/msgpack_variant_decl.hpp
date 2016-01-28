//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGPACK_V2_TYPE_BOOST_MSGPACK_VARIANT_DECL_HPP
#define MSGPACK_V2_TYPE_BOOST_MSGPACK_VARIANT_DECL_HPP

#if defined(MSGPACK_USE_BOOST)

#include "msgpack/v1/adaptor/boost/msgpack_variant_decl.hpp"

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v2) {
/// @endcond

namespace type {

template <typename STR, typename BIN, typename EXT>
struct basic_variant;

typedef basic_variant<std::string, std::vector<char>, ext> variant;
typedef basic_variant<
#if (BOOST_VERSION / 100000) >= 1 && ((BOOST_VERSION / 100) % 1000) >= 53
    boost::string_ref,
#else  // (BOOST_VERSION / 100000) >= 1 && ((BOOST_VERSION / 100) % 1000) >= 53
    std::string,
#endif // (BOOST_VERSION / 100000) >= 1 && ((BOOST_VERSION / 100) % 1000) >= 53
    raw_ref, ext_ref> variant_ref;

} // namespace type

/// @cond
} // MSGPACK_API_VERSION_NAMESPACE(v2)
/// @endcond

} // namespace msgpack

#endif // MSGPACK_USE_BOOST
#endif // MSGPACK_V2_TYPE_BOOST_MSGPACK_VARIANT_DECL_HPP
