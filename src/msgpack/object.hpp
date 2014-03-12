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
#include <boost/variant.hpp>
#include <boost/move/move.hpp>
#include <boost/container/vector.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>

namespace msgpack {

namespace type {

struct nil { };
inline bool operator==(nil const&, nil const&) { return true; }

}  // namespace type

class type_error : public std::bad_cast { };


struct object;
struct object_kv;

typedef boost::container::vector<object> object_array;
typedef boost::container::vector<object_kv> object_map;

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
	typedef boost::variant<
		type::nil,
		bool,
		uint64_t,
		int64_t,
		double,
		object_array,
		object_map,
		object_str,
		object_bin
	> union_type;

	union_type via;

	template <typename T>
	T as() const;

	bool is_nil() const {
		return boost::get<type::nil>(&via);
	}

	template <typename T>
	void convert(T& v) const;
	template <typename T>
	void convert(T* v) const;

	object(msgpack_object o);

	object() {};

	object(object const& other):via(other.via) {}
	object(BOOST_RV_REF(object) other):via(boost::move(other.via)) {}
	template <typename T>
	explicit object(const T& v, typename boost::disable_if<boost::is_same<T, object> >::type* = nullptr) {
		*this << v;
	}

	object& operator=(BOOST_COPY_ASSIGN_REF(object) other) {
		this->via = other.via;
		return *this;
	}
	object& operator=(BOOST_RV_REF(object) other) {
		this->via = boost::move(other.via);
		return *this;
	}
	template <typename T>
	typename boost::disable_if<boost::is_same<T, object>, object>::type& operator=(const T& v) {
		*this = object(v);
		return *this;
	}

	operator msgpack_object() const;

private:
	struct implicit_type;

public:
	implicit_type convert() const;
};

inline bool operator==(object const& lhs, object const& rhs) {
	return lhs.via == rhs.via;
}

struct object_kv {
	BOOST_COPYABLE_AND_MOVABLE(object_kv);
public:
	object_kv(BOOST_RV_REF(object) k, BOOST_RV_REF(object) v):key(boost::move(k)), val(boost::move(v)) {}

	object_kv(object_kv const& other):key(other.key), val(other.val) {}
	object_kv(BOOST_RV_REF(object_kv) other):key(boost::move(other.key)), val(boost::move(other.val)) {}
	object_kv& operator=(BOOST_RV_REF(object_kv) other) {
		key = boost::move(other.key);
		val = boost::move(other.val);
		return *this;
	}
	object_kv& operator=(BOOST_COPY_ASSIGN_REF(object_kv) other) {
		key = other.key;
		val = other.val;
		return *this;
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


template <typename Stream>
template <typename T>
inline packer<Stream>& packer<Stream>::pack(const T& v)
{
	*this << v;
	return *this;
}

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
}

// serialize operator
template <typename Stream, typename T>
inline packer<Stream>& operator<< (packer<Stream>& o, const T& v)
{
	return detail::packer_serializer<Stream, T>::pack(o, v);
}


template <typename T>
inline bool operator==(const object& x, const T& y)
try {
	return x == object(y);
} catch (msgpack::type_error&) {
	return false;
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

inline object::object(msgpack_object o)
{
	// FIXME beter way?
	::memcpy(this, &o, sizeof(o));
}

template <typename T>
void operator<< (object& o, const T& v)
{
	v.msgpack_object(static_cast<object*>(&o));
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
class pack_visitor :public boost::static_visitor<void>
{
public:
	pack_visitor(packer<Stream>& packer):packer_(packer) {}
	void operator()(type::nil) const {
		packer_.pack_nil();
	}
	void operator()(bool v) const {
		v ? packer_.pack_true() : packer_.pack_false();
	}
	void operator()(uint64_t v) const {
		packer_.pack_uint64(v);
	}
	void operator()(int64_t v) const {
		packer_.pack_int64(v);
	}
	void operator()(double v) const {
		packer_.pack_double(v);
	}
	void operator()(object_array const& v) const {
		packer_.pack_array(v.size());
		std::for_each(v.begin(), v.end(), boost::bind(&pack_visitor::pack_array, this, _1));
	}
	void operator()(object_map const& v) const {
		packer_.pack_map(v.size());
		std::for_each(v.begin(), v.end(), boost::bind(&pack_visitor::pack_map, this, _1));
	}
	void operator()(object_str const& v) const {
		packer_.pack_str(v.size);
		packer_.pack_str_body(v.ptr.get(), v.size);
	}
	void operator()(object_bin const& v) const {
		packer_.pack_bin(v.size);
		packer_.pack_bin_body(v.ptr.get(), v.size);
	}
	void operator()(...) const {
		throw type_error();
	}

private:
	void pack_array(object const& o) const {
		packer_ << o;
	}
	void pack_map(object_kv const& o) const {
		packer_ << o.key;
		packer_ << o.val;
	}

	packer<Stream>& packer_;
};

template <typename Stream>
packer<Stream>& operator<< (packer<Stream>& s, const object& o)
{
	boost::apply_visitor(pack_visitor<Stream>(s), o.via);
	return s;
}

std::ostream& operator<< (std::ostream& s, const object& o);

class ostream_visitor :public boost::static_visitor<void>
{
public:
	ostream_visitor(std::ostream& stream):stream_(stream) {}
	void operator()(type::nil) const {
		stream_ << "nil";
	}
	void operator()(bool v) const {
		stream_ << std::boolalpha << v;
	}
	void operator()(uint64_t v) const {
		stream_ << v;
	}
	void operator()(int64_t v) const {
		stream_ << v;
	}
	void operator()(double v) const {
		stream_ << v;
	}
	void operator()(object_array const& v) const {
		boost::container::vector<object>::const_iterator b(v.begin());
		boost::container::vector<object>::const_iterator e(v.end());
		stream_ << "[";
		if (b != e) {
			stream_ << *b++;
			while (b != e) {
				stream_ << ", " << *b++;
			}
		}
		stream_ << "]";
	}
	void operator()(object_map const& v) const {
		boost::container::vector<object_kv>::const_iterator b(v.begin());
		boost::container::vector<object_kv>::const_iterator e(v.end());
		stream_ << "{";
		if (b != e) {
			stream_ << b->key << "=>" << b->val;
			++b;
			while (b != e) {
				stream_ << ", " << b->key << "=>" << b->val;
				++b;
			}
		}
		stream_ << "}";
	}
	void operator()(object_str const& v) const {
		(stream_ << '"').write(v.ptr.get(), v.size) << '"';
	}
	void operator()(object_bin const& v) const {
		(stream_ << '"').write(v.ptr.get(), v.size) << '"';
	}
	void operator()(...) const {
		stream_ << "#<UNKNOWN>";
	}

private:
	std::ostream& stream_;
};

std::ostream& operator<< (std::ostream& s, const object& o)
{
	boost::apply_visitor(ostream_visitor(s), o.via);
	return s;
}

}  // namespace msgpack

#include "msgpack/type.hpp"

#endif /* msgpack/object.hpp */

