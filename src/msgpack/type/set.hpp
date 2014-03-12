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
#ifndef MSGPACK_TYPE_SET_HPP
#define MSGPACK_TYPE_SET_HPP

#include "msgpack/object.hpp"
#include <set>

namespace msgpack {


template <typename T>
inline std::set<T>& operator>> (object const& o, std::set<T>& v)
{
	object_array const* oa = boost::get<object_array>(&o.via);
	if (!oa) { throw type_error(); }

	boost::container::vector<object>::const_iterator b(oa->begin());
	boost::container::vector<object>::const_iterator e(oa->end());
	while (b != e) {
		v.insert(b->as<T>());
		++b;
	}
	return v;
}

template <typename Stream, typename T>
inline packer<Stream>& operator<< (packer<Stream>& o, const std::set<T>& v)
{
	o.pack_array(v.size());
	for(typename std::set<T>::const_iterator it(v.begin()), it_end(v.end());
			it != it_end; ++it) {
		o.pack(*it);
	}
	return o;
}

template <typename T>
inline void operator<< (object& o, const std::set<T>& v)
{
	object_array oa;
	oa.reserve(v.size());
	std::for_each(v.begin(), v.end(), std::back_inserter(oa));
	o.via = boost::move(oa);
}


template <typename T>
inline std::multiset<T>& operator>> (object const& o, std::multiset<T>& v)
{
	object_array const* oa = boost::get<object_array>(&o.via);
	if (!oa) { throw type_error(); }

	boost::container::vector<object>::const_iterator b(oa->begin());
	boost::container::vector<object>::const_iterator e(oa->end());
	while (b != e) {
		v.insert(b->as<T>());
		++b;
	}
	return v;
}

template <typename Stream, typename T>
inline packer<Stream>& operator<< (packer<Stream>& o, const std::multiset<T>& v)
{
	o.pack_array(v.size());
	for(typename std::multiset<T>::const_iterator it(v.begin()), it_end(v.end());
			it != it_end; ++it) {
		o.pack(*it);
	}
	return o;
}

template <typename T>
inline void operator<< (object& o, const std::multiset<T>& v)
{
	object_array oa;
	oa.reserve(v.size());
	std::for_each(v.begin(), v.end(), std::back_inserter(oa));
	o.via = boost::move(oa);
}


}  // namespace msgpack

#endif /* msgpack/type/set.hpp */

