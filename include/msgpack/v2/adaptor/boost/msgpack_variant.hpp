//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGPACK_V2_TYPE_BOOST_MSGPACK_VARIANT_HPP
#define MSGPACK_V2_TYPE_BOOST_MSGPACK_VARIANT_HPP

#if defined(MSGPACK_USE_BOOST)

#include "msgpack/v2/adaptor/boost/msgpack_variant_decl.hpp"
#include "msgpack/v2/adaptor/adaptor_base_decl.hpp"

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v2) {
/// @endcond

namespace type {

template <typename STR, typename BIN, typename EXT>
struct basic_variant :
        v1::type::basic_variant<STR, BIN, EXT>,
        private boost::totally_ordered<basic_variant<STR, BIN, EXT> > {
    basic_variant() {}
    template <typename T>
    basic_variant(T const& t):v1::type::basic_variant<STR, BIN, EXT>(t) {}
};

template <typename STR, typename BIN, typename EXT>
inline bool operator<(basic_variant<STR, BIN, EXT> const& lhs, basic_variant<STR, BIN, EXT> const& rhs) {
    return
        static_cast<typename basic_variant<STR, BIN, EXT>::base const&>(lhs) <
        static_cast<typename basic_variant<STR, BIN, EXT>::base const&>(rhs);
}

template <typename STR, typename BIN, typename EXT>
inline bool operator==(basic_variant<STR, BIN, EXT> const& lhs, basic_variant<STR, BIN, EXT> const& rhs) {
    return
        static_cast<typename basic_variant<STR, BIN, EXT>::base const&>(lhs) ==
        static_cast<typename basic_variant<STR, BIN, EXT>::base const&>(rhs);
}

} // namespace type

namespace adaptor {

#if !defined (MSGPACK_USE_CPP03)

template <typename STR, typename BIN, typename EXT>
struct as<type::basic_variant<STR, BIN, EXT> > : v1::adaptor::as<v1::type::basic_variant<STR, BIN, EXT> > {
};

#endif // !defined (MSGPACK_USE_CPP03)

template <typename STR, typename BIN, typename EXT>
struct convert<type::basic_variant<STR, BIN, EXT> > : convert<v1::type::basic_variant<STR, BIN, EXT> > {
};

template <typename STR, typename BIN, typename EXT>
struct pack<type::basic_variant<STR, BIN, EXT> > : pack<v1::type::basic_variant<STR, BIN, EXT> > {
};

template <typename STR, typename BIN, typename EXT>
struct object<type::basic_variant<STR, BIN, EXT> > : object<v1::type::basic_variant<STR, BIN, EXT> > {
};

template <typename STR, typename BIN, typename EXT>
struct object_with_zone<type::basic_variant<STR, BIN, EXT> > : object_with_zone<v1::type::basic_variant<STR, BIN, EXT> > {
};

} // namespace adaptor



/// @cond
} // MSGPACK_API_VERSION_NAMESPACE(v2)
/// @endcond

} // namespace msgpack

#endif // MSGPACK_USE_BOOST
#endif // MSGPACK_V2_TYPE_BOOST_MSGPACK_VARIANT_HPP
