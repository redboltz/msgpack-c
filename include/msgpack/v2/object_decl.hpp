//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGPACK_V2_OBJECT_DECL_HPP
#define MSGPACK_V2_OBJECT_DECL_HPP

#include "msgpack/v1/object_decl.hpp"

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v2) {
/// @endcond

using v1::object_handle;

namespace detail {

using v1::detail::add_ext_type_size;

} // namespace detail

using v1::aligned_zone_size;

using v1::clone;

namespace detail {

using v1::detail::packer_serializer;

} // namespace detail

// Adaptor functor specialization to object
namespace adaptor {

using v1::adaptor::convert;

using v1::adaptor::pack;

using v1::adaptor::object_with_zone;

} // namespace adaptor


// obsolete
using v1::define;

using v1::operator==;

using v1::operator!=;

using v1::operator<<;

// obsolete
using v1::convert;
using v1::pack;

// obsolete
using v1::pack_copy;

/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v2)
/// @endcond

}  // namespace msgpack

#endif // MSGPACK_V2_OBJECT_DECL_HPP
