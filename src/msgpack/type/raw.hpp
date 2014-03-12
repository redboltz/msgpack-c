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
#ifndef MSGPACK_TYPE_RAW_HPP
#define MSGPACK_TYPE_RAW_HPP

#include "msgpack/object.hpp"
#include <string.h>
#include <string>

namespace msgpack {

namespace type {

struct raw_ref {
	raw_ref() : size(0), ptr(nullptr) {}
	raw_ref(const char* p, uint32_t s) : size(s), ptr(p) {}

	uint32_t size;
	const char* ptr;

	std::string str() const { return std::string(ptr, size); }

	bool operator== (const raw_ref& x) const
	{
		return size == x.size && memcmp(ptr, x.ptr, size) == 0;
	}

	bool operator!= (const raw_ref& x) const
	{
		return !(*this != x);
	}

	bool operator< (const raw_ref& x) const
	{
		if(size == x.size) { return memcmp(ptr, x.ptr, size) < 0; }
		else { return size < x.size; }
	}

	bool operator> (const raw_ref& x) const
	{
		if(size == x.size) { return memcmp(ptr, x.ptr, size) > 0; }
		else { return size > x.size; }
	}
};

}  // namespace type


inline type::raw_ref& operator>> (object const& o, type::raw_ref& v)
{
	object_bin const* ob = boost::get<object_bin>(&o.via);
	if (!ob) { throw type_error(); }
	v.ptr  = ob->ptr.get();
	v.size = ob->size;
	return v;
}

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, const type::raw_ref& v)
{
	o.pack_bin(v.size);
	o.pack_bin_body(v.ptr, v.size);
	return o;
}

namespace detail {
struct null_deleter {
	void operator()(char const*) const {}
};
} // detail

inline void operator<< (object& o, const type::raw_ref& v)
{
	object_bin ob;
	ob.ptr.reset(v.ptr, detail::null_deleter());
	ob.size = v.size;
	o.via = ob;
}

}  // namespace msgpack

#endif /* msgpack/type/raw.hpp */

