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
#ifndef MSGPACK_TYPE_TR1_UNORDERED_MAP_HPP
#define MSGPACK_TYPE_TR1_UNORDERED_MAP_HPP

#include "msgpack/object.hpp"

#if defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)

#define MSGPACK_HAS_STD_UNOURDERED_MAP
#include <unordered_map>
#define MSGPACK_STD_TR1 std

#else   // defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)

#if __GNUC__ >= 4

#define MSGPACK_HAS_STD_TR1_UNOURDERED_MAP

#include <tr1/unordered_map>
#define MSGPACK_STD_TR1 std::tr1

#endif // __GNUC__ >= 4

#endif  // defined(_LIBCPP_VERSION) || (_MSC_VER >= 1700)


namespace msgpack {


template <typename K, typename V>
inline MSGPACK_STD_TR1::unordered_map<K, V> operator>> (object o, MSGPACK_STD_TR1::unordered_map<K, V>& v)
{
	object_map const* om = boost::get<object_map>(&o.via);
	if (!om) { throw type_error(); }

	std::vector<object_kv>::const_iterator b(om->begin());
	std::vector<object_kv>::const_iterator e(om->end());
	while (b != e) {
		K key;
		b->key.convert(key);
		typename MSGPACK_STD_TR1::unordered_map<K,V>::iterator it(v.find(key));
		if (it == v.end()) {
			V val;
			b->val.convert(val);
			v.insert(it, std::pair<K,V>(key, val));
		}
		else {
			b->val.convert(it->second);
		}
		++b;
	}
	return v;
}

template <typename Stream, typename K, typename V>
inline packer<Stream>& operator<< (packer<Stream>& o, const MSGPACK_STD_TR1::unordered_map<K,V>& v)
{
	o.pack_map(v.size());
	for(typename MSGPACK_STD_TR1::unordered_map<K,V>::const_iterator it(v.begin()), it_end(v.end());
		it != it_end; ++it) {
		o.pack(it->first);
		o.pack(it->second);
	}
	return o;
}

template <typename K, typename V>
inline void operator<< (object& o, const MSGPACK_STD_TR1::unordered_map<K,V>& v)
{
	object_map om;
	om.reserve(v.size());
	std::for_each(v.begin(), v.end(), [&om](std::pair<K,V> const& e){
		om.emplace_back(e.first, e.second);
	});
	o.via = std::move(om);
}


template <typename K, typename V>
inline MSGPACK_STD_TR1::unordered_multimap<K, V> operator>> (object o, MSGPACK_STD_TR1::unordered_multimap<K, V>& v)
{
	object_map const* om = boost::get<object_map>(&o.via);
	if (!om) { throw type_error(); }

	std::vector<object_kv>::const_iterator b(om->begin());
	std::vector<object_kv>::const_iterator e(om->end());
	while (b != e) {
		std::pair<K,V> pair;
		b->key.convert(pair.first);
		b->val.convert(pair.second);
		v.insert(pair);
		++b;
	}
	return v;
}

template <typename Stream, typename K, typename V>
inline packer<Stream>& operator<< (packer<Stream>& o, const MSGPACK_STD_TR1::unordered_multimap<K,V>& v)
{
	o.pack_map(v.size());
	for(typename MSGPACK_STD_TR1::unordered_multimap<K,V>::const_iterator it(v.begin()), it_end(v.end());
		it != it_end; ++it) {
		o.pack(it->first);
		o.pack(it->second);
	}
	return o;
}

template <typename K, typename V>
inline void operator<< (object& o, const MSGPACK_STD_TR1::unordered_multimap<K,V>& v)
{
	object_map om;
	om.reserve(v.size());
	std::for_each(v.begin(), v.end(), [&om](std::pair<K,V> const& e){
		om.emplace_back(e.first, e.second);
	});
	o.via = std::move(om);
}


}  // namespace msgpack

#undef MSGPACK_STD_TR1

#endif /* msgpack/type/map.hpp */

