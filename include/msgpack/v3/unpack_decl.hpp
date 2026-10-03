//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2018 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGPACK_V3_UNPACK_DECL_HPP
#define MSGPACK_V3_UNPACK_DECL_HPP

#include "msgpack/v2/unpack_decl.hpp"

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v3) {
/// @endcond

using v2::unpack_reference_func;

using v2::unpack_error;
using v2::parse_error;
using v2::insufficient_bytes;
using v2::size_overflow;
using v2::array_size_overflow;
using v2::map_size_overflow;
using v2::str_size_overflow;
using v2::bin_size_overflow;
using v2::ext_size_overflow;
using v2::depth_size_overflow;
using v2::unpack_limit;

namespace detail {

using v2::detail::unpack_user;

using v2::detail::unpack_uint8;
using v2::detail::unpack_uint16;
using v2::detail::unpack_uint32;
using v2::detail::unpack_uint64;

using v2::detail::unpack_int8;
using v2::detail::unpack_int16;
using v2::detail::unpack_int32;
using v2::detail::unpack_int64;

using v2::detail::unpack_float;
using v2::detail::unpack_double;

using v2::detail::unpack_nil;

using v2::detail::unpack_true;
using v2::detail::unpack_false;

using v2::detail::unpack_array;
using v2::detail::unpack_array_item;
using v2::detail::unpack_map;
using v2::detail::unpack_map_item;
using v2::detail::unpack_str;
using v2::detail::unpack_bin;
using v2::detail::unpack_ext;

using  v2::detail::unpack_stack;

using v2::detail::init_count;
using v2::detail::decr_count;
using v2::detail::incr_count;

using v2::detail::get_count;

using v2::detail::fix_tag;

using v2::detail::value;

using v2::detail::load;

} // detail


using v2::unpacked;
using v2::unpacker;
using v2::basic_unpacker;

namespace detail {

using v2::detail::unpack_imp;

} // detail

/// Unpack msgpack::object from a buffer.
/**
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param off The offset position of the buffer. It is read and overwritten.
 * @param referenced If the unpacked object contains reference of the buffer, then set as true, otherwise false.
 * @param f A judging function that msgpack::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msgpack::object.
 *
 * @return object_handle that contains unpacked data.
 *
 */
msgpack::object_handle unpack(
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    msgpack::unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR, msgpack::unpack_limit const& limit = unpack_limit());

/// Unpack msgpack::object from a buffer.
/**
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param off The offset position of the buffer. It is read and overwritten.
 * @param f A judging function that msgpack::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msgpack::object.
 *
 * @return object_handle that contains unpacked data.
 *
 */
msgpack::object_handle unpack(
    const char* data, std::size_t len, std::size_t& off,
    msgpack::unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR, msgpack::unpack_limit const& limit = unpack_limit());

/// Unpack msgpack::object from a buffer.
/**
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param referenced If the unpacked object contains reference of the buffer, then set as true, otherwise false.
 * @param f A judging function that msgpack::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msgpack::object.
 *
 * @return object_handle that contains unpacked data.
 *
 */
msgpack::object_handle unpack(
    const char* data, std::size_t len, bool& referenced,
    msgpack::unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR, msgpack::unpack_limit const& limit = unpack_limit());

/// Unpack msgpack::object from a buffer.
/**
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param f A judging function that msgpack::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msgpack::object.
 *
 * @return object_handle that contains unpacked data.
 *
 */
msgpack::object_handle unpack(
    const char* data, std::size_t len,
    msgpack::unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR, msgpack::unpack_limit const& limit = unpack_limit());


/// Unpack msgpack::object from a buffer.
/**
 * @param result The object_handle that contains unpacked data.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param off The offset position of the buffer. It is read and overwritten.
 * @param referenced If the unpacked object contains reference of the buffer, then set as true, otherwise false.
 * @param f A judging function that msgpack::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msgpack::object.
 *
 *
 */
void unpack(
    msgpack::object_handle& result,
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    msgpack::unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR, msgpack::unpack_limit const& limit = unpack_limit());

/// Unpack msgpack::object from a buffer.
/**
 * @param result The object_handle that contains unpacked data.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param off The offset position of the buffer. It is read and overwritten.
 * @param f A judging function that msgpack::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msgpack::object.
 *
 *
 */
void unpack(
    msgpack::object_handle& result,
    const char* data, std::size_t len, std::size_t& off,
    msgpack::unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR, msgpack::unpack_limit const& limit = unpack_limit());

/// Unpack msgpack::object from a buffer.
/**
 * @param result The object_handle that contains unpacked data.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param referenced If the unpacked object contains reference of the buffer, then set as true, otherwise false.
 * @param f A judging function that msgpack::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msgpack::object.
 *
 *
 */
void unpack(
    msgpack::object_handle& result,
    const char* data, std::size_t len, bool& referenced,
    msgpack::unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR, msgpack::unpack_limit const& limit = unpack_limit());

/// Unpack msgpack::object from a buffer.
/**
 * @param result The object_handle that contains unpacked data.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param f A judging function that msgpack::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msgpack::object.
 *
 *
 */
void unpack(
    msgpack::object_handle& result,
    const char* data, std::size_t len,
    msgpack::unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR, msgpack::unpack_limit const& limit = unpack_limit());

/// Unpack msgpack::object from a buffer.
/**
 * @param z The msgpack::zone that is used as a memory of unpacked msgpack objects.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param off The offset position of the buffer. It is read and overwritten.
 * @param referenced If the unpacked object contains reference of the buffer, then set as true, otherwise false.
 * @param f A judging function that msgpack::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msgpack::object.
 *
 * @return msgpack::object that contains unpacked data.
 *
 */
msgpack::object unpack(
    msgpack::zone& z,
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    msgpack::unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR, msgpack::unpack_limit const& limit = unpack_limit());

/// Unpack msgpack::object from a buffer.
/**
 * @param z The msgpack::zone that is used as a memory of unpacked msgpack objects.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param off The offset position of the buffer. It is read and overwritten.
 * @param f A judging function that msgpack::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msgpack::object.
 *
 * @return msgpack::object that contains unpacked data.
 *
 */
msgpack::object unpack(
    msgpack::zone& z,
    const char* data, std::size_t len, std::size_t& off,
    msgpack::unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR, msgpack::unpack_limit const& limit = unpack_limit());

/// Unpack msgpack::object from a buffer.
/**
 * @param z The msgpack::zone that is used as a memory of unpacked msgpack objects.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param referenced If the unpacked object contains reference of the buffer, then set as true, otherwise false.
 * @param f A judging function that msgpack::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msgpack::object.
 *
 * @return msgpack::object that contains unpacked data.
 *
 */
msgpack::object unpack(
    msgpack::zone& z,
    const char* data, std::size_t len, bool& referenced,
    msgpack::unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR, msgpack::unpack_limit const& limit = unpack_limit());

/// Unpack msgpack::object from a buffer.
/**
 * @param z The msgpack::zone that is used as a memory of unpacked msgpack objects.
 * @param data The pointer to the buffer.
 * @param len The length of the buffer.
 * @param f A judging function that msgpack::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msgpack::object.
 *
 * @return msgpack::object that contains unpacked data.
 *
 */
msgpack::object unpack(
    msgpack::zone& z,
    const char* data, std::size_t len,
    msgpack::unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR, msgpack::unpack_limit const& limit = unpack_limit());


/// @cond
/// Unpack msgpack::object from an iterator range.
/**
 * Unpacks one msgpack::object from the byte range [begin, end).
 * Any input iterator over byte-sized values is accepted. The range does not
 * need to be contiguous nor random access.
 *
 * On success `begin` is advanced to the position just after the unpacked
 * object, so the function can be called repeatedly to unpack consecutive
 * objects from the same range. The range is never read beyond the end of the
 * unpacked object, which also holds for single-pass input iterators.
 *
 * If the range is contiguous (a pointer, or a contiguous iterator when
 * std::contiguous_iterator is available), the data is unpacked in place with
 * no copy, exactly like unpack(data, len, off). Otherwise the bytes are copied
 * into an internal buffer first, and the resulting object never references
 * the caller's range.
 *
 * @param begin The beginning of the range. It is advanced past the object.
 * @param end The end of the range.
 * @param referenced Set to true if the unpacked object references the
 *                   caller's range (only possible for contiguous ranges),
 *                   otherwise false.
 * @param f A judging function that msgpack::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msgpack::object.
 *
 * @return object_handle that contains unpacked data.
 *
 * @throw msgpack::insufficient_bytes if the range ends before the object is
 *        complete.
 *
 * On error `begin` is left unchanged for contiguous and forward (multi-pass)
 * ranges, like `off` in unpack(data, len, off). For a single-pass input
 * iterator the bytes already read cannot be given back; on a parse error the
 * range has been consumed up to and including the offending byte.
 */
template <typename Iterator>
msgpack::object_handle unpack(
    Iterator& begin, Iterator end, bool& referenced,
    unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR, unpack_limit const& limit = unpack_limit());

/// Unpack msgpack::object from an iterator range.
/**
 * See unpack(Iterator& begin, Iterator end, bool& referenced, ...).
 *
 * @param begin The beginning of the range. It is advanced past the object.
 * @param end The end of the range.
 * @param f A judging function that msgpack::object refer to the buffer.
 * @param user_data This parameter is passed to f.
 * @param limit The size limit information of msgpack::object.
 *
 * @return object_handle that contains unpacked data.
 */
template <typename Iterator>
msgpack::object_handle unpack(
    Iterator& begin, Iterator end,
    unpack_reference_func f = MSGPACK_NULLPTR, void* user_data = MSGPACK_NULLPTR, unpack_limit const& limit = unpack_limit());

}  // MSGPACK_API_VERSION_NAMESPACE(v3)
/// @endcond

}  // namespace msgpack

#endif // MSGPACK_V3_UNPACK_DECL_HPP
