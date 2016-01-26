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
#include "msgpack/adaptor/adaptor_base.hpp"

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

// obsolete
template <typename Type>
class define;

bool operator==(const msgpack::object& x, const msgpack::object& y);

template <typename T>
bool operator==(const msgpack::object& x, const T& y);

bool operator!=(const msgpack::object& x, const msgpack::object& y);

template <typename T>
bool operator==(const T& y, const msgpack::object& x);

template <typename T>
bool operator!=(const msgpack::object& x, const T& y);

template <typename T>
bool operator!=(const T& y, const msgpack::object& x);

void operator<< (msgpack::object& o, const msgpack_object& v);

// obsolete
template <typename T>
void convert(T& v, msgpack::object const& o);

// obsolete
template <typename Stream, typename T>
void pack(msgpack::packer<Stream>& o, const T& v);

// obsolete
template <typename Stream, typename T>
void pack_copy(msgpack::packer<Stream>& o, T v);

template <typename Stream>
msgpack::packer<Stream>& operator<< (msgpack::packer<Stream>& o, const msgpack::object& v);

template <typename Stream>
msgpack::packer<Stream>& operator<< (msgpack::packer<Stream>& o, const msgpack::object::with_zone& v);

std::ostream& operator<< (std::ostream& s, const msgpack::object& o);

/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v2)
/// @endcond

}  // namespace msgpack

#endif // MSGPACK_V2_OBJECT_DECL_HPP
