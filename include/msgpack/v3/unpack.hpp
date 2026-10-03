//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2018 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGPACK_V3_UNPACK_HPP
#define MSGPACK_V3_UNPACK_HPP

#include "msgpack/unpack_decl.hpp"
#include "msgpack/parse.hpp"
#include "msgpack/create_object_visitor.hpp"

#include <iterator>
#include <string>

// Detect std::contiguous_iterator / std::to_address through the library
// feature-test macros (reported by <version>), rather than by compiler or
// __cplusplus version, so that partial C++20 implementations are handled
// correctly. When unavailable, only raw pointers are treated as contiguous.
#if defined(__has_include)
#  if __has_include(<version>)
#    include <version>
#  endif
#endif
#if defined(__cpp_lib_concepts) && (__cpp_lib_concepts >= 202002L) \
    && defined(__cpp_lib_to_address) && (__cpp_lib_to_address >= 201711L)
#  include <concepts>
#  include <memory>
#  define MSGPACK_HAS_CONTIGUOUS_ITERATOR 1
#endif

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v3) {
/// @endcond

inline msgpack::object_handle unpack(
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    msgpack::unpack_reference_func f, void* user_data,
    msgpack::unpack_limit const& limit
)
{
    msgpack::object obj;
    msgpack::unique_ptr<msgpack::zone> z(new msgpack::zone);
    referenced = false;
    parse_return ret = detail::unpack_imp(
        data, len, off, *z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case PARSE_SUCCESS:
        return msgpack::object_handle(obj, msgpack::move(z));
    case PARSE_EXTRA_BYTES:
        return msgpack::object_handle(obj, msgpack::move(z));
    default:
        break;
    }
    return msgpack::object_handle();
}

inline msgpack::object_handle unpack(
    const char* data, std::size_t len, std::size_t& off,
    msgpack::unpack_reference_func f, void* user_data,
    msgpack::unpack_limit const& limit)
{
    bool referenced;
    return msgpack::v3::unpack(data, len, off, referenced, f, user_data, limit);
}

inline msgpack::object_handle unpack(
    const char* data, std::size_t len, bool& referenced,
    msgpack::unpack_reference_func f, void* user_data,
    msgpack::unpack_limit const& limit)
{
    std::size_t off = 0;
    return msgpack::v3::unpack(data, len, off, referenced, f, user_data, limit);
}

inline msgpack::object_handle unpack(
    const char* data, std::size_t len,
    msgpack::unpack_reference_func f, void* user_data,
    msgpack::unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    return msgpack::v3::unpack(data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    msgpack::object_handle& result,
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    msgpack::unpack_reference_func f, void* user_data,
    msgpack::unpack_limit const& limit)
{
    msgpack::object obj;
    msgpack::unique_ptr<msgpack::zone> z(new msgpack::zone);
    referenced = false;
    parse_return ret = detail::unpack_imp(
        data, len, off, *z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case PARSE_SUCCESS:
        result.set(obj);
        result.zone() = msgpack::move(z);
        return;
    case PARSE_EXTRA_BYTES:
        result.set(obj);
        result.zone() = msgpack::move(z);
        return;
    default:
        return;
    }
}

inline void unpack(
    msgpack::object_handle& result,
    const char* data, std::size_t len, std::size_t& off,
    msgpack::v3::unpack_reference_func f, void* user_data,
            msgpack::unpack_limit const& limit)
{
    bool referenced;
    msgpack::v3::unpack(result, data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    msgpack::object_handle& result,
    const char* data, std::size_t len, bool& referenced,
    msgpack::unpack_reference_func f, void* user_data,
    msgpack::unpack_limit const& limit)
{
    std::size_t off = 0;
    msgpack::v3::unpack(result, data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    msgpack::object_handle& result,
    const char* data, std::size_t len,
    msgpack::unpack_reference_func f, void* user_data,
    msgpack::unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    msgpack::v3::unpack(result, data, len, off, referenced, f, user_data, limit);
}


inline msgpack::object unpack(
    msgpack::zone& z,
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    msgpack::unpack_reference_func f, void* user_data,
    msgpack::unpack_limit const& limit)
{
    msgpack::object obj;
    referenced = false;
    parse_return ret = detail::unpack_imp(
        data, len, off, z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case PARSE_SUCCESS:
        return obj;
    case PARSE_EXTRA_BYTES:
        return obj;
    default:
        break;
    }
    return obj;
}

inline msgpack::object unpack(
    msgpack::zone& z,
    const char* data, std::size_t len, std::size_t& off,
    msgpack::unpack_reference_func f, void* user_data,
    msgpack::unpack_limit const& limit)
{
    bool referenced;
    return msgpack::v3::unpack(z, data, len, off, referenced, f, user_data, limit);
}

inline msgpack::object unpack(
    msgpack::zone& z,
    const char* data, std::size_t len, bool& referenced,
    msgpack::unpack_reference_func f, void* user_data,
    msgpack::unpack_limit const& limit)
{
    std::size_t off = 0;
    return msgpack::v3::unpack(z, data, len, off, referenced, f, user_data, limit);
}

inline msgpack::object unpack(
    msgpack::zone& z,
    const char* data, std::size_t len,
    msgpack::unpack_reference_func f, void* user_data,
    msgpack::unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    return msgpack::v3::unpack(z, data, len, off, referenced, f, user_data, limit);
}

/// @cond
namespace detail {

// Iterator-range unpack dispatch tags.
struct unpack_iter_contiguous_tag {};
struct unpack_iter_forward_tag {};
struct unpack_iter_input_tag {};

// A range is unpacked in place (no copy) only when its iterator is contiguous
// and its value type is one byte wide.
template <typename Iterator>
struct unpack_iter_is_contiguous {
#if defined(MSGPACK_HAS_CONTIGUOUS_ITERATOR)
    static const bool value =
        std::contiguous_iterator<Iterator>
        && sizeof(typename std::iterator_traits<Iterator>::value_type) == 1;
#else
    static const bool value =
        msgpack::is_pointer<Iterator>::value
        && sizeof(typename std::iterator_traits<Iterator>::value_type) == 1;
#endif
};

template <typename Iterator>
inline const char* unpack_iter_address(Iterator it) {
#if defined(MSGPACK_HAS_CONTIGUOUS_ITERATOR)
    return reinterpret_cast<const char*>(std::to_address(it));
#else
    return reinterpret_cast<const char*>(it);
#endif
}

// Select the strategy: contiguous beats the iterator category; otherwise
// forward (multi-pass) iterators and single-pass input iterators differ in
// whether the range may be re-read, which decides how exact positioning is
// achieved.
template <typename Iterator, bool Contiguous>
struct unpack_iter_select;

template <typename Iterator>
struct unpack_iter_select<Iterator, true> {
    typedef unpack_iter_contiguous_tag type;
};

template <typename Category>
struct unpack_iter_category_tag {
    typedef unpack_iter_input_tag type;
};
template <>
struct unpack_iter_category_tag<std::forward_iterator_tag> {
    typedef unpack_iter_forward_tag type;
};
template <>
struct unpack_iter_category_tag<std::bidirectional_iterator_tag> {
    typedef unpack_iter_forward_tag type;
};
template <>
struct unpack_iter_category_tag<std::random_access_iterator_tag> {
    typedef unpack_iter_forward_tag type;
};

template <typename Iterator>
struct unpack_iter_select<Iterator, false> {
    typedef typename unpack_iter_category_tag<
        typename std::iterator_traits<Iterator>::iterator_category
    >::type type;
};

template <typename Iterator>
inline void unpack_iter_advance(Iterator& it, std::size_t n) {
    std::advance(
        it,
        static_cast<typename std::iterator_traits<Iterator>::difference_type>(n));
}

// Contiguous: unpack in place through the existing (data, len, off) path.
template <typename Iterator>
inline msgpack::object_handle unpack_iter(
    Iterator& begin, Iterator end, bool& referenced,
    unpack_reference_func f, void* user_data, unpack_limit const& limit,
    unpack_iter_contiguous_tag)
{
    if (begin == end) {
        throw msgpack::insufficient_bytes(std::string("insufficient bytes"));
    }
    const char* data = unpack_iter_address(begin);
    std::size_t len = static_cast<std::size_t>(std::distance(begin, end));
    std::size_t off = 0;
    // Qualify to the same API version: an unqualified call is ambiguous
    // because ADL on unpack_limit also finds v1::unpack.
    msgpack::object_handle oh = v3::unpack(data, len, off, referenced, f, user_data, limit);
    unpack_iter_advance(begin, off);
    return oh;
}

// Forward (multi-pass): copy chunks into an unpacker, then rewind `begin` by
// the number of bytes that actually belonged to the object. Bytes read past
// the object are simply re-read by the next call. On any error `begin` is
// restored to where it started, like `off` in unpack(data, len, off), so the
// caller can e.g. retry after insufficient_bytes once more data is available.
template <typename Iterator>
inline msgpack::object_handle unpack_iter(
    Iterator& begin, Iterator end, bool& referenced,
    unpack_reference_func f, void* user_data, unpack_limit const& limit,
    unpack_iter_forward_tag)
{
    referenced = false;
    unpacker u(f, user_data, MSGPACK_UNPACKER_RESERVE_SIZE, limit);
    Iterator start = begin;
    std::size_t fed = 0;
    msgpack::object_handle oh;
    try {
        for (;;) {
            if (u.next(oh)) break;
            if (begin == end) {
                throw msgpack::insufficient_bytes(std::string("insufficient bytes"));
            }
            u.reserve_buffer(MSGPACK_UNPACKER_RESERVE_SIZE);
            char* buf = u.buffer();
            std::size_t n = 0;
            while (n < MSGPACK_UNPACKER_RESERVE_SIZE && begin != end) {
                buf[n++] = static_cast<char>(*begin);
                ++begin;
            }
            u.buffer_consumed(n);
            fed += n;
        }
    }
    catch (...) {
        begin = start;
        throw;
    }
    std::size_t consumed = fed - u.nonparsed_size();
    begin = start;
    unpack_iter_advance(begin, consumed);
    return oh;
}

// Input (single-pass): the range cannot be re-read, so feed one byte at a
// time; the unpacker then stops exactly at the end of the object and `begin`
// is never advanced past it. On a parse error the offending byte has
// necessarily been consumed already (it had to be read to be judged), which
// is the minimum possible for a single-pass source and cannot be undone.
template <typename Iterator>
inline msgpack::object_handle unpack_iter(
    Iterator& begin, Iterator end, bool& referenced,
    unpack_reference_func f, void* user_data, unpack_limit const& limit,
    unpack_iter_input_tag)
{
    referenced = false;
    unpacker u(f, user_data, MSGPACK_UNPACKER_RESERVE_SIZE, limit);
    msgpack::object_handle oh;
    for (;;) {
        if (u.next(oh)) return oh;
        if (begin == end) {
            throw msgpack::insufficient_bytes(std::string("insufficient bytes"));
        }
        u.reserve_buffer(1);
        *u.buffer() = static_cast<char>(*begin);
        ++begin;
        u.buffer_consumed(1);
    }
}

} // namespace detail

template <typename Iterator>
inline msgpack::object_handle unpack(
    Iterator& begin, Iterator end, bool& referenced,
    unpack_reference_func f, void* user_data, unpack_limit const& limit)
{
    typedef typename detail::unpack_iter_select<
        Iterator, detail::unpack_iter_is_contiguous<Iterator>::value
    >::type tag;
    return detail::unpack_iter(begin, end, referenced, f, user_data, limit, tag());
}

template <typename Iterator>
inline msgpack::object_handle unpack(
    Iterator& begin, Iterator end,
    unpack_reference_func f, void* user_data, unpack_limit const& limit)
{
    bool referenced;
    return v3::unpack(begin, end, referenced, f, user_data, limit);
}

}  // MSGPACK_API_VERSION_NAMESPACE(v3)
/// @endcond

}  // namespace msgpack


#endif // MSGPACK_V3_UNPACK_HPP
