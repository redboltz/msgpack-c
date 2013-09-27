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
#include "zone.hpp"
#include <string.h>
#include <stdexcept>
#include <typeinfo>
#include <limits>
#include <ostream>

#include "equal.hpp" // boost equal

namespace msgpack {


class type_error : public std::bad_cast { };


namespace type {
	enum object_type {
		NIL				= MSGPACK_OBJECT_NIL,
		BOOLEAN			= MSGPACK_OBJECT_BOOLEAN,
		POSITIVE_INTEGER	= MSGPACK_OBJECT_POSITIVE_INTEGER,
		NEGATIVE_INTEGER	= MSGPACK_OBJECT_NEGATIVE_INTEGER,
		DOUBLE				= MSGPACK_OBJECT_DOUBLE,
		RAW				= MSGPACK_OBJECT_RAW,
		ARRAY				= MSGPACK_OBJECT_ARRAY,
		MAP				= MSGPACK_OBJECT_MAP
	};
}

template <typename ForwardIterator>
struct object;

template <typename ForwardIterator>
struct object_kv;

template <typename ForwardIterator>
struct object_array {
	uint32_t size;
	object<ForwardIterator>* ptr;
};

template <typename ForwardIterator>
struct object_map {
	uint32_t size;
	object_kv<ForwardIterator>* ptr;
};

template <typename ForwardIterator>
struct object_raw {
	ForwardIterator begin;
	ForwardIterator end;
};

template <typename ForwardIterator = const char *>
struct object {
	union union_type {
		bool boolean;
		uint64_t u64;
		int64_t  i64;
		double   dec;
		object_array<ForwardIterator> array;
		object_map<ForwardIterator> map;
		object_raw<ForwardIterator> raw;
		object_raw<ForwardIterator> ref;  // obsolete
	};

	type::object_type type;
	union_type via;

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

	template <typename T>
	object(const T& v, zone* z);

	template <typename T>
	object& operator=(const T& v);

	operator msgpack_object() const;

	struct with_zone;

private:
	struct implicit_type;

public:
	implicit_type convert() const;
};

template <typename ForwardIterator>
struct object_kv {
	object<ForwardIterator> key;
	object<ForwardIterator> val;
};

template <typename ForwardIterator>
struct object<ForwardIterator>::with_zone : object<ForwardIterator> {
	with_zone(msgpack::zone* zone) : zone(zone) { }
	msgpack::zone* zone;
private:
	with_zone();
};

template <typename ForwardIterator>
struct object<ForwardIterator>::implicit_type {
	implicit_type(object const& o) : obj(o) { }
	~implicit_type() { }

	template <typename T>
	operator T() { return obj.as<T>(); }

private:
	object<ForwardIterator> const& obj;
};


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

	template <typename ForwardIterator>
	void msgpack_unpack(object<ForwardIterator> const& o)
	{
		o >> static_cast<msgpack_type&>(*this);
	}
};


template <typename Stream>
template <typename T>
inline packer<Stream>& packer<Stream>::pack(const T& v)
{
	*this << v;
	return *this;
}

template <typename ForwardIterator>
inline object<ForwardIterator>& operator>> (object<ForwardIterator> const& o, object<ForwardIterator>& v)
{
	v = o;
	return v;
}

// convert operator
template <typename T, typename ForwardIterator>
inline T& operator>> (object<ForwardIterator> const& o, T& v)
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
}

// serialize operator
template <typename Stream, typename T>
inline packer<Stream>& operator<< (packer<Stream>& o, const T& v)
{
	return detail::packer_serializer<Stream, T>::pack(o, v);
}

// deconvert operator
template <typename T, typename ForwardIterator>
inline void operator<< (typename object<ForwardIterator>::with_zone& o, const T& v)
{
	v.msgpack_object(static_cast<object<ForwardIterator>*>(&o), o.zone);
}

template <typename ForwardIterator>
inline bool operator==(const object<ForwardIterator>& x, const object<ForwardIterator>& y)
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

	case type::RAW:
		return boost::algorithm::equal(x.via.raw.begin, x.via.raw.end, y.via.raw.begin, y.via.raw.end);

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

template <typename T, typename ForwardIterator>
inline bool operator==(const object<ForwardIterator>& x, const T& y)
try {
	return x == object<ForwardIterator>(y);
} catch (msgpack::type_error&) {
	return false;
}

template <typename ForwardIterator>
inline bool operator!=(const object<ForwardIterator>& x, const object<ForwardIterator>& y)
{ return !(x == y); }

template <typename T, typeanme ForwardIterator>
inline bool operator==(const T& y, const object<ForwardIterator> x)
{ return x == y; }

template <typename T, typename ForwardIterator>
inline bool operator!=(const object<ForwardIterator>& x, const T& y)
{ return !(x == y); }

template <typename T, typename ForwardIterator>
inline bool operator!=(const T& y, const object<ForwardIterator>& x)
{ return x != y; }


template <typename ForwardIterator>
inline object<ForwardIterator>::implicit_type object<ForwardIterator>::convert() const
{
	return implicit_type(*this);
}

template <typename ForwardIterator>
template <typename T>
inline void object<ForwardIterator>::convert(T& v) const
{
	*this >> v;
}

template <typename ForwardIterator>
template <typename T>
inline void object<ForwardIterator>::convert(T* v) const
{
	convert(*v);
}

template <typename ForwardIterator>
template <typename T>
inline T object<ForwardIterator>::as() const
{
	T v;
	convert(v);
	return v;
}


template <typename ForwardIterator>
inline object<ForwardIterator>::object()
{
	type = type::NIL;
}

template <typename ForwardIterator>
template <typename T>
inline object<ForwardIterator>::object(const T& v)
{
	*this << v;
}

template <typename ForwardIterator>
template <typename T>
inline object<ForwardIterator>& object<ForwardIterator>::operator=(const T& v)
{
	*this = object<ForwardIterator>(v);
	return *this;
}

template <typename ForwardIterator>
template <typename T>
object<ForwardIterator>::object(const T& v, zone* z)
{
	with_zone oz(z);
	oz << v;
	type = oz.type;
	via = oz.via;
}


template <typename ForwardIterator>
inline object<ForwardIterator>::object(msgpack_object o)
{
	// FIXME beter way?
	::memcpy(this, &o, sizeof(o));
}

template <typename ForwardIterator>
inline void operator<< (object<ForwardIterator>& o, msgpack_object v)
{
	// FIXME beter way?
	::memcpy(&o, &v, sizeof(v));
}

template <typename ForwardIterator>
inline object<ForwardIterator>::operator msgpack_object() const
{
	// FIXME beter way?
	msgpack_object obj;
	::memcpy(&obj, this, sizeof(obj));
	return obj;
}


// obsolete
template <typename T, typename ForwardIterator>
inline void convert(T& v, object<ForwardIterator> const& o)
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


template <typename Stream, typename ForwardIterator>
packer<Stream>& operator<< (packer<Stream>& o, const object<ForwardIterator>& v)
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

	case type::RAW:
		o.pack_raw(std::distance(v.via.raw.begin, v.via.raw.end);
		o.pack_raw_body(v.via.raw.begin, v.via.raw.end);
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

template <typename ForwardIterator>
std::ostream& operator<< (std::ostream& s, const object<ForwardIterator>& o)
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

	case type::RAW:
		s << '"';
		std::copy(o.via.raw.begin, o.via.raw.end, std::ostream_iterator<char>(s));
		s << '"';
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
	return s;
}

}  // namespace msgpack

#include "msgpack/type.hpp"

#endif /* msgpack/object.hpp */

