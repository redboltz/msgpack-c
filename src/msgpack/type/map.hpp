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
#ifndef MSGPACK_TYPE_MAP_HPP
#define MSGPACK_TYPE_MAP_HPP

#include "msgpack/object.hpp"
#include <map>
#include <vector>
#include <algorithm>

namespace msgpack {


namespace type {

template <typename K, typename V>
class assoc_vector : public std::vector< std::pair<K, V> > {};

namespace detail {
	template <typename K, typename V>
	struct pair_first_less {
		bool operator() (const std::pair<K, V>& x, const std::pair<K, V>& y) const
			{ return x.first < y.first; }
	};
}

}  //namespace type


template <typename K, typename V>
inline type::assoc_vector<K,V>& operator>> (object const& o, type::assoc_vector<K,V>& v)
{
	object_map const* om = boost::get<object_map>(&o.via);
	if (!om) { throw type_error(); }
	v.resize(om->size());

	std::vector<object_kv>::const_iterator b(om->begin());
	std::vector<object_kv>::const_iterator e(om->end());
	typename type::assoc_vector<K,V>::iterator it(v.begin());
	while (b != e) {
		b->key.convert(it->first);
		b->val.convert(it->second);
		++b;
		++it;
	}
	std::sort(v.begin(), v.end(), type::detail::pair_first_less<K,V>());
	return v;
}

template <typename Stream, typename K, typename V>
inline packer<Stream>& operator<< (packer<Stream>& o, const type::assoc_vector<K,V>& v)
{
	o.pack_map(v.size());
	for(typename type::assoc_vector<K,V>::const_iterator it(v.begin()), it_end(v.end());
			it != it_end; ++it) {
		o.pack(it->first);
		o.pack(it->second);
	}
	return o;
}

template <typename K, typename V>
inline void operator<< (object& o, const type::assoc_vector<K,V>& v)
{
	object_map om;
	om.reserve(v.size());
	std::for_each(v.begin(), v.end(), [&om](std::pair<K,V> const& e){
		om.emplace_back(e.first, e.second);
	});
	o.via = std::move(om);
}


template <typename K, typename V>
inline std::map<K, V> operator>> (object const& o, std::map<K, V>& v)
{
	object_map const* om = boost::get<object_map>(&o.via);
	if (!om) { throw type_error(); }

	std::vector<object_kv>::const_iterator b(om->begin());
	std::vector<object_kv>::const_iterator e(om->end());
	while (b != e) {
		K key;
		b->key.convert(key);
		typename std::map<K,V>::iterator it(v.find(key));
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
inline packer<Stream>& operator<< (packer<Stream>& o, const std::map<K,V>& v)
{
	o.pack_map(v.size());
	for(typename std::map<K,V>::const_iterator it(v.begin()), it_end(v.end());
			it != it_end; ++it) {
		o.pack(it->first);
		o.pack(it->second);
	}
	return o;
}

template <typename K, typename V>
inline void operator<< (object& o, const std::map<K,V>& v)
{
	object_map om;
	om.reserve(v.size());
	std::for_each(v.begin(), v.end(), [&om](std::pair<K,V> const& e){
		om.emplace_back(e.first, e.second);
	});
	o.via = std::move(om);
}


template <typename K, typename V>
inline std::multimap<K, V> operator>> (object const& o, std::multimap<K, V>& v)
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
inline packer<Stream>& operator<< (packer<Stream>& o, const std::multimap<K,V>& v)
{
	o.pack_map(v.size());
	for(typename std::multimap<K,V>::const_iterator it(v.begin()), it_end(v.end());
			it != it_end; ++it) {
		o.pack(it->first);
		o.pack(it->second);
	}
	return o;
}

template <typename K, typename V>
inline void operator<< (object& o, const std::multimap<K,V>& v)
{
	object_map om;
	om.reserve(v.size());
	std::for_each(v.begin(), v.end(), [&om](std::pair<K,V> const& e){
		om.emplace_back(e.first, e.second);
	});
	o.via = std::move(om);
}


}  // namespace msgpack

#endif /* msgpack/type/map.hpp */

