//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2009 FURUHASHI Sadayuki
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
#ifndef MSGPACK_TYPE_INT_HPP
#define MSGPACK_TYPE_INT_HPP

#include "msgpack/object.hpp"
#include <limits>

namespace msgpack {


namespace type {
namespace detail {
	template <typename T, bool Signed>
	struct convert_integer_sign;

	template <typename T>
	struct convert_integer_sign<T, true> {
		struct integer_sign_visitor : boost::static_visitor<T>
		{
			T operator()(uint64_t v) const {
				if (v > static_cast<uint64_t>(std::numeric_limits<T>::max())) {
					throw type_error();
				}
				return static_cast<T>(v);
			}
			T operator()(int64_t v) const {
				if (v < static_cast<int64_t>(std::numeric_limits<T>::min())) {
					throw type_error();
				}
				return static_cast<T>(v);
			}
			template <typename U>
			T operator()(U const&) const {
				throw type_error();
				return 0;
			}
		};
		static inline T convert(object const& o) {
			return boost::apply_visitor(integer_sign_visitor(), o.via);
		}
	};

	template <typename T>
	struct convert_integer_sign<T, false> {
		static inline T convert(object const& o) {
			uint64_t const* pv = boost::get<uint64_t>(&o.via);
			if (!pv) throw type_error();
			if (*pv > static_cast<uint64_t>(std::numeric_limits<T>::max())) {
				throw type_error();
			}
			return static_cast<T>(*pv);
		}
	};

	template <typename T>
	struct is_signed {
		static const bool value = std::numeric_limits<T>::is_signed;
	};

	template <typename T>
	static inline T convert_integer(object const& o)
	{
		return detail::convert_integer_sign<T, is_signed<T>::value>::convert(o);
	}

	template <bool Signed>
	struct pack_char_sign;

	template <>
	struct pack_char_sign<true> {
		template <typename Stream>
		static inline packer<Stream>& pack(packer<Stream>& o, char v) {
			o.pack_int8(v); return o;
		}
	};

	template <>
	struct pack_char_sign<false> {
		template <typename Stream>
		static inline packer<Stream>& pack(packer<Stream>& o, char v) {
			o.pack_uint8(v); return o;
		}
	};

	template <typename Stream>
	static inline packer<Stream>& pack_char(packer<Stream>& o, char v) {
		return pack_char_sign<is_signed<char>::value>::pack(o, v);
	}

	template <bool Signed>
	struct object_char_sign;

	template <>
	struct object_char_sign<true> {
		static inline void make(object& o, char v) {
			v < 0 ? o.via = static_cast<int64_t>(v)
				  : o.via = static_cast<uint64_t>(v);
		}
	};

	template <>
	struct object_char_sign<false> {
		static inline void make(object& o, char v) {
			o.via = static_cast<uint64_t>(v);
		}
	};

	static inline void object_char(object& o, char v) {
		return object_char_sign<is_signed<char>::value>::make(o, v);
	}

}  // namespace detail
}  // namespace type

inline char& operator>> (object const& o, char& v)
	{ v = type::detail::convert_integer<char>(o); return v; }


inline signed char& operator>> (object const& o, signed char& v)
	{ v = type::detail::convert_integer<signed char>(o); return v; }

inline signed short& operator>> (object const& o, signed short& v)
	{ v = type::detail::convert_integer<signed short>(o); return v; }

inline signed int& operator>> (object const& o, signed int& v)
	{ v = type::detail::convert_integer<signed int>(o); return v; }

inline signed long& operator>> (object const& o, signed long& v)
	{ v = type::detail::convert_integer<signed long>(o); return v; }

inline signed long long& operator>> (object const& o, signed long long& v)
	{ v = type::detail::convert_integer<signed long long>(o); return v; }


inline unsigned char& operator>> (object const& o, unsigned char& v)
	{ v = type::detail::convert_integer<unsigned char>(o); return v; }

inline unsigned short& operator>> (object const& o, unsigned short& v)
	{ v = type::detail::convert_integer<unsigned short>(o); return v; }

inline unsigned int& operator>> (object const& o, unsigned int& v)
	{ v = type::detail::convert_integer<unsigned int>(o); return v; }

inline unsigned long& operator>> (object const& o, unsigned long& v)
	{ v = type::detail::convert_integer<unsigned long>(o); return v; }

inline unsigned long long& operator>> (object const& o, unsigned long long& v)
	{ v = type::detail::convert_integer<unsigned long long>(o); return v; }


template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, char v)
	{ return type::detail::pack_char(o, v); }


template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, signed char v)
	{ o.pack_int8(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, signed short v)
	{ o.pack_short(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, signed int v)
	{ o.pack_int(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, signed long v)
	{ o.pack_long(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, signed long long v)
	{ o.pack_long_long(v); return o; }


template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, unsigned char v)
	{ o.pack_uint8(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, unsigned short v)
	{ o.pack_unsigned_short(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, unsigned int v)
	{ o.pack_unsigned_int(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, unsigned long v)
	{ o.pack_unsigned_long(v); return o; }

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, unsigned long long v)
	{ o.pack_unsigned_long_long(v); return o; }


inline void operator<< (object& o, char v)
	{ type::detail::object_char(o, v); }


inline void operator<< (object& o, signed char v)
	{ v < 0 ? o.via = static_cast<int64_t>(v) : o.via = static_cast<uint64_t>(v); }

inline void operator<< (object& o, signed short v)
	{ v < 0 ? o.via = static_cast<int64_t>(v) : o.via = static_cast<uint64_t>(v); }


inline void operator<< (object& o, signed int v)
	{ v < 0 ? o.via = static_cast<int64_t>(v) : o.via = static_cast<uint64_t>(v); }

inline void operator<< (object& o, signed long v)
	{ v < 0 ? o.via = static_cast<int64_t>(v) : o.via = static_cast<uint64_t>(v); }

inline void operator<< (object& o, signed long long v)
	{ v < 0 ? o.via = static_cast<int64_t>(v) : o.via = static_cast<uint64_t>(v); }


inline void operator<< (object& o, unsigned char v)
	{ o.via = static_cast<uint64_t>(v); }

inline void operator<< (object& o, unsigned short v)
	{ o.via = static_cast<uint64_t>(v); }

inline void operator<< (object& o, unsigned int v)
	{ o.via = static_cast<uint64_t>(v); }

inline void operator<< (object& o, unsigned long v)
	{ o.via = static_cast<uint64_t>(v); }

inline void operator<< (object& o, unsigned long long v)
	{ o.via = static_cast<uint64_t>(v); }

}  // namespace msgpack

#endif /* msgpack/type/int.hpp */

