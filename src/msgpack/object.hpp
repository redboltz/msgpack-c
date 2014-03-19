//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2010 FURUHASHI Sadayuki
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
#ifndef MSGPACK_OBJECT_HPP
#define MSGPACK_OBJECT_HPP

#include "object.h"
#include "pack.hpp"
#include <string.h>
#include <stdexcept>
#include <typeinfo>
#include <limits>
#include <ostream>
#include <memory>
#include "cpp_config.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/move/move.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits.hpp>

namespace msgpack {

class type_error : public std::bad_cast { };

struct free_deleter {
	template <typename T>
	void operator()(T* p) const {
		::free(p);
	}
};


namespace type {

	enum object_type {
		NIL                     = MSGPACK_OBJECT_NIL,
		BOOLEAN                 = MSGPACK_OBJECT_BOOLEAN,
		POSITIVE_INTEGER        = MSGPACK_OBJECT_POSITIVE_INTEGER,
		NEGATIVE_INTEGER        = MSGPACK_OBJECT_NEGATIVE_INTEGER,
		DOUBLE                  = MSGPACK_OBJECT_DOUBLE,
		STR                     = MSGPACK_OBJECT_STR,
		BIN                     = MSGPACK_OBJECT_BIN,
		ARRAY                   = MSGPACK_OBJECT_ARRAY,
		MAP                     = MSGPACK_OBJECT_MAP
	};
} // type

struct object;
struct object_kv;

struct object_array {
	uint32_t size;
	object* ptr;
};

struct object_map {
	uint32_t size;
	object_kv* ptr;
};

struct object_str {
	uint32_t size;
	boost::shared_ptr<const char> ptr;
};

inline bool operator==(object_str const& lhs, object_str const& rhs) {
	return lhs.size == rhs.size && memcmp(lhs.ptr.get(), rhs.ptr.get(), lhs.size) == 0;
}

struct object_bin {
	uint32_t size;
	boost::shared_ptr<const char> ptr;
};

inline bool operator==(object_bin const& lhs, object_bin const& rhs) {
	return lhs.size == rhs.size && memcmp(lhs.ptr.get(), rhs.ptr.get(), lhs.size) == 0;
}

struct object {
	BOOST_COPYABLE_AND_MOVABLE(object);
public:
	union union_type {
		bool boolean;
		uint64_t u64;
		int64_t  i64;
		double   dec;
		object_array array;
		object_map map;
		int strbuf[sizeof(object_str) / sizeof(int)];
		int binbuf[sizeof(object_bin) / sizeof(int)];
		union_type() {}
		~union_type() {}
	};
	type::object_type type;
	union_type via;
	object_str& str() {
		return *reinterpret_cast<object_str*>(via.strbuf);
	}
	object_str const& str() const {
		return *reinterpret_cast<object_str const*>(via.strbuf);
	}
	object_bin& bin() {
		return *reinterpret_cast<object_bin*>(via.binbuf);
	}
	object_bin const& bin() const {
		return *reinterpret_cast<object_bin const*>(via.binbuf);
	}

	bool is_nil() const { return type == type::NIL; }

	template <typename T>
	T as() const;

	template <typename T>
	void convert(T& v) const;
	template <typename T>
	void convert(T* v) const;

	object();

	object(msgpack_object o);

	template <typename T>
	explicit object(const T& v);


	object(object const&);
	//object(BOOST_RV_REF(object) other) throw();
	object(BOOST_RV_REF(object) other) throw() :type(other.type) {
		switch(type) {
		case type::NIL:
			break;

		case type::BOOLEAN:
			via.boolean = other.via.boolean;
			break;

		case type::POSITIVE_INTEGER:
			via.u64 = other.via.u64;
			break;

		case type::NEGATIVE_INTEGER:
			via.i64 = other.via.i64;
			break;

		case type::DOUBLE:
			via.dec = other.via.dec;
			break;

		case type::STR: {
			object_str* str = new(via.strbuf) object_str();
			str->size = other.str().size;
			str->ptr.swap(other.str().ptr);
		} break;

		case type::BIN: {
			object_bin* bin = new(via.binbuf) object_bin();
			bin->size = other.bin().size;
			bin->ptr.swap(other.bin().ptr);
		} break;

		case type::ARRAY:
			via.array.size = other.via.array.size;
			via.array.ptr = other.via.array.ptr;
			other.via.array.size = 0;
			other.via.array.ptr = nullptr;
			break;

		case type::MAP:
			via.map.size = other.via.map.size;
			via.map.ptr = other.via.map.ptr;
			other.via.array.size = 0;
			other.via.map.ptr = nullptr;
			break;

		default:
			break;
		}
	}

	~object();

	template <typename T>
	typename boost::disable_if<boost::is_same<T, object>, object>::type& operator=(const T& v);
#if 0
	object& operator=(BOOST_COPY_ASSIGN_REF(object));
	object& operator=(BOOST_RV_REF(object)) throw();
#endif
	object& operator=(BOOST_COPY_ASSIGN_REF(object) other) {
		std::cout << "Copy Assign  called" << std::endl;
	}

	object& operator=(BOOST_RV_REF(object) other) throw() {
		type = other.type;
		switch(type) {
		case type::NIL:
			break;

		case type::BOOLEAN:
			via.boolean = other.via.boolean;
			break;

		case type::POSITIVE_INTEGER:
			via.u64 = other.via.u64;
			break;

		case type::NEGATIVE_INTEGER:
			via.i64 = other.via.i64;
			break;

		case type::DOUBLE:
			via.dec = other.via.dec;
			break;

		case type::STR: {
			object_str* str = new(via.strbuf) object_str();
			str->size = other.str().size;
			str->ptr.swap(other.str().ptr);
		} break;

		case type::BIN: {
			object_bin* bin = new(via.binbuf) object_bin();
			bin->size = other.bin().size;
			bin->ptr.swap(other.bin().ptr);
		} break;

		case type::ARRAY:
			via.array.size = other.via.array.size;
			via.array.ptr = other.via.array.ptr;
			other.via.array.size = 0;
			other.via.array.ptr = nullptr;
			break;

		case type::MAP:
			via.map.size = other.via.map.size;
			via.map.ptr = other.via.map.ptr;
			other.via.map.size = 0;
			other.via.map.ptr = nullptr;
			break;

		default:
			break;
		}
		return *this;
	}

	operator msgpack_object() const;

private:
	struct implicit_type;

public:
	implicit_type convert() const;
};

bool operator==(const object& x, const object& y);

struct object_kv {
	BOOST_COPYABLE_AND_MOVABLE(object_kv);
public:
	object_kv() {}
	object_kv(BOOST_RV_REF(object) k, BOOST_RV_REF(object) v):key(boost::move(k)), val(boost::move(v)) {}
	object_kv(object_kv const&) {
		std::cout << "kv Copy ctor is called" << std::endl;
	}
	object_kv(BOOST_RV_REF(object_kv) other):key(boost::move(other.key)), val(boost::move(other.val)) {}
	~object_kv() {}
	object_kv& operator=(BOOST_COPY_ASSIGN_REF(object_kv)) {
		std::cout << "kv Copy assign is called" << std::endl;
	}
	object_kv& operator=(BOOST_RV_REF(object_kv) other) {
		key = boost::move(other.key);
		val = boost::move(other.val);
	}
	object key;
	object val;
};

inline bool operator==(object_kv const& lhs, object_kv const& rhs) {
	return lhs.key == rhs.key && lhs.val == rhs.val;
}

struct object::implicit_type {
	implicit_type(object const& o) : obj(o) { }
	~implicit_type() { }

	template <typename T>
	operator T() { return obj.as<T>(); }

private:
	object const& obj;
};

// deconvert operator
template <typename T>
inline void operator<< (object& o, const T& v)
{
	v.msgpack_object(&o);
}

inline bool operator==(const object& x, const object& y)
{
	if(x.type != y.type) { return false; }

	switch(x.type) {
	case type::NIL:
		return true;

	case type::BOOLEAN:
		return x.via.boolean == y.via.boolean;

	case type::POSITIVE_INTEGER:
		return x.via.u64 == y.via.u64;

	case type::NEGATIVE_INTEGER:
		return x.via.i64 == y.via.i64;

	case type::DOUBLE:
		return x.via.dec == y.via.dec;

	case type::STR:
		return x.str().size == y.str().size &&
			memcmp(x.str().ptr.get(), y.str().ptr.get(), x.str().size) == 0;

	case type::BIN:
		return x.bin().size == y.bin().size &&
			memcmp(x.bin().ptr.get(), y.bin().ptr.get(), x.bin().size) == 0;

	case type::ARRAY:
		if(x.via.array.size != y.via.array.size) {
			return false;
		} else if(x.via.array.size == 0) {
			return true;
		} else {
			object* px = x.via.array.ptr;
			object* const pxend = x.via.array.ptr + x.via.array.size;
			object* py = y.via.array.ptr;
			do {
				if(!(*px == *py)) {
					return false;
				}
				++px;
				++py;
			} while(px < pxend);
			return true;
		}

	case type::MAP:
		if(x.via.map.size != y.via.map.size) {
			return false;
		} else if(x.via.map.size == 0) {
			return true;
		} else {
			object_kv* px = x.via.map.ptr;
			object_kv* const pxend = x.via.map.ptr + x.via.map.size;
			object_kv* py = y.via.map.ptr;
			do {
				if(!(px->key == py->key) || !(px->val == py->val)) {
					return false;
				}
				++px;
				++py;
			} while(px < pxend);
			return true;
		}

	default:
		return false;
	}
}


// obsolete
template <typename Type>
class define : public Type {
public:
	typedef Type msgpack_type;
	typedef define<Type> define_type;

	define() {}
	define(const msgpack_type& v) : msgpack_type(v) {}

	template <typename Packer>
	void msgpack_pack(Packer& o) const
	{
		o << static_cast<const msgpack_type&>(*this);
	}

	void msgpack_unpack(object const& o)
	{
		o >> static_cast<msgpack_type&>(*this);
	}
};


inline object& operator>> (object const& o, object& v)
{
	v = o;
	return v;
}

// convert operator
template <typename T>
inline T& operator>> (object const& o, T& v)
{
	v.msgpack_unpack(o.convert());
	return v;
}

namespace detail {
template <typename Stream, typename T>
struct packer_serializer {
	static packer<Stream>& pack(packer<Stream>& o, const T& v) {
		v.msgpack_pack(o);
		return o;
	}
};
} // detail

// serialize operator
template <typename Stream, typename T>
inline packer<Stream>& operator<< (packer<Stream>& o, const T& v)
{
	return detail::packer_serializer<Stream, T>::pack(o, v);
}


template <typename T>
inline bool operator==(const object& x, const T& y)
{
	try {
		return x == object(y);
	} catch (msgpack::type_error&) {
		return false;
	}
}

inline bool operator!=(const object& x, const object& y)
{ return !(x == y); }

template <typename T>
inline bool operator==(const T& y, const object x)
{ return x == y; }

template <typename T>
inline bool operator!=(const object& x, const T& y)
{ return !(x == y); }

template <typename T>
inline bool operator!=(const T& y, const object& x)
{ return x != y; }


inline object::implicit_type object::convert() const
{
	return implicit_type(*this);
}

template <typename T>
inline void object::convert(T& v) const
{
	*this >> v;
}

template <typename T>
inline void object::convert(T* v) const
{
	convert(*v);
}

template <typename T>
inline T object::as() const
{
	T v;
	convert(v);
	return v;
}

inline object::object()
{
	type = type::NIL;
}

inline
object::object(object const& other) {
	std::cout << "Copy Ctor called" << std::endl;
}

inline
object::~object() {
	switch(type) {
	case type::STR:
		str().ptr.~shared_ptr<const char>();
		break;

	case type::BIN:
		bin().ptr.~shared_ptr<const char>();
		break;

	case type::ARRAY:
		for (size_t idx = 0; idx < via.array.size; ++idx) {
			(via.array.ptr + idx)->~object();
		}
		::free(via.array.ptr);
		break;

	case type::MAP:
		for (size_t idx = 0; idx < via.map.size; ++idx) {
			(via.map.ptr + idx)->~object_kv();
		}
		::free(via.map.ptr);
		break;

	default:
		break;
	}
}

#if 0
inline
object& object::operator=(BOOST_COPY_ASSIGN_REF(object) other) {
	std::cout << "Copy Assign  called" << std::endl;
}
#endif

template <typename T>
inline object::object(const T& v)
{
	*this << v;
}

template <typename T>
inline
typename boost::disable_if<boost::is_same<T, object>, object>::type& object::operator=(const T& v)
{
	*this = object(v);
	return *this;
}


inline object::object(msgpack_object o)
{
	// FIXME beter way?
	::memcpy(this, &o, sizeof(o));
}


inline void operator<< (object& o, msgpack_object v)
{
	// FIXME beter way?
	::memcpy(&o, &v, sizeof(v));
}

inline object::operator msgpack_object() const
{
	// FIXME beter way?
	msgpack_object obj;
	::memcpy(&obj, this, sizeof(obj));
	return obj;
}


// obsolete
template <typename T>
inline void convert(T& v, object const& o)
{
	o.convert(v);
}

// obsolete
template <typename Stream, typename T>
inline void pack(packer<Stream>& o, const T& v)
{
	o.pack(v);
}

// obsolete
template <typename Stream, typename T>
inline void pack_copy(packer<Stream>& o, T v)
{
	pack(o, v);
}

template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& o, const object& v)
{
	switch(v.type) {
	case type::NIL:
		o.pack_nil();
		return o;

	case type::BOOLEAN:
		if(v.via.boolean) {
			o.pack_true();
		} else {
			o.pack_false();
		}
		return o;

	case type::POSITIVE_INTEGER:
		o.pack_uint64(v.via.u64);
		return o;

	case type::NEGATIVE_INTEGER:
		o.pack_int64(v.via.i64);
		return o;

	case type::DOUBLE:
		o.pack_double(v.via.dec);
		return o;

	case type::STR:
		o.pack_str(v.str().size);
		o.pack_str_body(v.str().ptr.get(), v.str().size);
		return o;

	case type::BIN:
		o.pack_bin(v.bin().size);
		o.pack_bin_body(v.bin().ptr.get(), v.bin().size);
		return o;

	case type::ARRAY:
		o.pack_array(v.via.array.size);
		for(object* p(v.via.array.ptr),
				* const pend(v.via.array.ptr + v.via.array.size);
			p < pend; ++p) {
			o << *p;
		}
		return o;

	case type::MAP:
		o.pack_map(v.via.map.size);
		for(object_kv* p(v.via.map.ptr),
				* const pend(v.via.map.ptr + v.via.map.size);
			p < pend; ++p) {
			o << p->key;
			o << p->val;
		}
		return o;

	default:
		throw type_error();
	}
}

std::ostream& operator<< (std::ostream& s, const object& o)
{
	switch(o.type) {
	case type::NIL:
		s << "nil";
		break;

	case type::BOOLEAN:
		s << (o.via.boolean ? "true" : "false");
		break;

	case type::POSITIVE_INTEGER:
		s << o.via.u64;
		break;

	case type::NEGATIVE_INTEGER:
		s << o.via.i64;
		break;

	case type::DOUBLE:
		s << o.via.dec;
		break;

	case type::STR:
		(s << '"').write(o.str().ptr.get(), o.str().size) << '"';
		break;

	case type::BIN:
		(s << '"').write(o.bin().ptr.get(), o.bin().size) << '"';
		break;

	case type::ARRAY:
		s << "[";
		if(o.via.array.size != 0) {
			object* p(o.via.array.ptr);
			s << *p;
			++p;
			for(object* const pend(o.via.array.ptr + o.via.array.size);
				p < pend; ++p) {
				s << ", " << *p;
			}
		}
		s << "]";
		break;

	case type::MAP:
		s << "{";
		if(o.via.map.size != 0) {
			object_kv* p(o.via.map.ptr);
			s << p->key << "=>" << p->val;
			++p;
			for(object_kv* const pend(o.via.map.ptr + o.via.map.size);
				p < pend; ++p) {
				s << ", " << p->key << "=>" << p->val;
			}
		}
		s << "}";
		break;

	default:
		// FIXME
		s << "#<UNKNOWN " << (uint16_t)o.type << ">";
	}
}

}  // namespace msgpack

#include "msgpack/type.hpp"

#endif /* msgpack/object.hpp */

