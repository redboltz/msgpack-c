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
#ifndef MSGPACK_TYPE_VECTOR_HPP
#define MSGPACK_TYPE_VECTOR_HPP

#include "msgpack/object.hpp"
#include <vector>

namespace msgpack {


template <typename T>
inline std::vector<T>& operator>> (object const& o, std::vector<T>& v)
{
	object_array const* oa = boost::get<object_array>(&o.via);
	if(!oa) { throw type_error(); }
	v.resize(oa->size());

	boost::container::vector<object>::const_iterator b(oa->begin());
	boost::container::vector<object>::const_iterator e(oa->end());
	typename std::vector<T>::iterator it(v.begin());
	while (b != e) {
		b->convert(*it++);
		++b;
	}

	return v;
}

template <typename Stream, typename T>
inline packer<Stream>& operator<< (packer<Stream>& o, const std::vector<T>& v)
{
	o.pack_array(v.size());
	for(typename std::vector<T>::const_iterator it(v.begin()), it_end(v.end());
			it != it_end; ++it) {
		o.pack(*it);
	}
	return o;
}

template <typename T>
inline void operator<< (object& o, const std::vector<T>& v)
{
	object_array oa;
	oa.reserve(v.size());
	// std::copy(v.begin(), v.end(), std::back_inserter(oa));
	for (typename std::vector<T>::const_iterator
		 i = v.begin(),
		 e = v.end();
		 i != e;
		 ++i) {
		oa.emplace_back(*i);
	}
	o.via = boost::move(oa);
}


}  // namespace msgpack

#endif /* msgpack/type/vector.hpp */

