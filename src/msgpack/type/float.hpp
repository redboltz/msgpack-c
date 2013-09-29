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
#ifndef MSGPACK_TYPE_FLOAT_HPP
#define MSGPACK_TYPE_FLOAT_HPP

#include "msgpack/object.hpp"
#include <vector>

namespace msgpack {


// FIXME check overflow, underflow

template <typename ForwardIterator>
inline float& operator>> (object<ForwardIterator> const& o, float& v)
{
	if(o.type != type::DOUBLE) { throw type_error(); }
	v = (float)o.via.dec;
	return v;
}

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, const float& v)
{
	o.pack_float(v);
	return o;
}


template <typename ForwardIterator>
inline double& operator>> (object<ForwardIterator> const& o, double& v)
{
	if(o.type != type::DOUBLE) { throw type_error(); }
	v = o.via.dec;
	return v;
}

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, const double& v)
{
	o.pack_double(v);
	return o;
}


template <typename ForwardIterator>
inline void operator<< (object<ForwardIterator>& o, float v)
{
	o.type = type::DOUBLE;
	o.via.dec = (double)v;
}

template <typename ForwardIterator>
inline void operator<< (object<ForwardIterator>& o, double v)
{
	o.type = type::DOUBLE;
	o.via.dec = v;
}

template <typename ForwardIterator>
inline void operator<< (typename object<ForwardIterator>::with_zone& o, float v)
	{ static_cast<object<ForwardIterator>&>(o) << v; }

template <typename ForwardIterator>
inline void operator<< (typename object<ForwardIterator>::with_zone& o, double v)
	{ static_cast<object<ForwardIterator>&>(o) << v; }


}  // namespace msgpack

#endif /* msgpack/type/float.hpp */

