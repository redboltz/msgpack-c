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
#ifndef MSGPACK_TYPE_NIL_HPP
#define MSGPACK_TYPE_NIL_HPP

#include "msgpack/object.hpp"

namespace msgpack {

namespace type {

struct nil { };

}  // namespace type

template <typename ForwardIterator>
inline type::nil& operator>> (object<ForwardIterator> const& o, type::nil& v)
{
	if(o.type != type::NIL) { throw type_error(); }
	return v;
}

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, const type::nil& v)
{
	o.pack_nil();
	return o;
}

template <typename ForwardIterator>
inline void operator<< (object<ForwardIterator>& o, type::nil v)
{
	o.type = type::NIL;
}

template <typename ForwardIterator>
inline void operator<< (typename object<ForwardIterator>::with_zone& o, type::nil v)
	{ static_cast<object<ForwardIterator>&>(o) << v; }


#if 0
template <typename ForwardIterator>
template <>
inline void object<ForwardIterator>::as<void>() const
{
	msgpack::type::nil v;
	convert(v);
}
#endif

}  // namespace msgpack

#endif /* msgpack/type/nil.hpp */

