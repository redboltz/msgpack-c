//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2013 FURUHASHI Sadayuki and KONDO Takatoshi
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
#ifndef MSGPACK_TYPE_DEFINE_HPP
#define MSGPACK_TYPE_DEFINE_HPP

#define MSGPACK_DEFINE(...) \
	template <typename Packer> \
	void msgpack_pack(Packer& pk) const \
	{ \
		msgpack::type::make_define(__VA_ARGS__).msgpack_pack(pk); \
	} \
	void msgpack_unpack(msgpack::object const& o) \
	{ \
		msgpack::type::make_define(__VA_ARGS__).msgpack_unpack(o); \
	}\
	template <typename MSGPACK_OBJECT> \
	void msgpack_object(MSGPACK_OBJECT* o) const \
	{ \
		msgpack::type::make_define(__VA_ARGS__).msgpack_object(o); \
	}

// MSGPACK_ADD_ENUM must be used in the global namespace.
#define MSGPACK_ADD_ENUM(enum_type) \
  namespace msgpack { \
	template <> \
	inline enum_type& operator>> (object const& o, enum_type& v) \
	{ \
	  int tmp; \
	  o >> tmp; \
	  v = static_cast<enum_type>(tmp); \
	  return v; \
	} \
	template <> \
	inline void operator<< (object& o, const enum_type& v) \
	{ \
	  int tmp = static_cast<enum_type>(v); \
	  o << tmp; \
	} \
  }

namespace msgpack {
namespace type {

template <typename Tuple, std::size_t N>
struct define_imp {
	template <typename Packer>
	static void pack(Packer& pk, Tuple const& t) {
		define_imp<Tuple, N-1>::pack(pk, t);
		pk.pack(std::get<N-1>(t));
	}
	static void unpack(msgpack::object_array const& oa, Tuple& t) {
		define_imp<Tuple, N-1>::unpack(oa, t);
		const size_t size = oa.size();
		if(size <= N-1) { return; }
		oa[N-1].convert(std::get<N-1>(t));
	}
	static void object(msgpack::object_array& oa, Tuple const& t) {
		define_imp<Tuple, N-1>::object(oa, t);
		oa.push_back(msgpack::object(std::get<N-1>(t)));
	}
};

template <typename Tuple>
struct define_imp<Tuple, 1> {
	template <typename Packer>
	static void pack(Packer& pk, Tuple const& t) {
		pk.pack(std::get<0>(t));
	}
	static void unpack(msgpack::object_array const& oa, Tuple& t) {
		const size_t size = oa.size();
		if(size <= 0) { return; }
		oa[0].convert(std::get<0>(t));
	}
	static void object(msgpack::object_array& oa, Tuple const& t) {
		oa.push_back(msgpack::object(std::get<0>(t)));
	}
};

template <typename... Args>
struct define {
	typedef define<Args...> value_type;
	typedef tuple<Args...> tuple_type;
	define(Args&... args) :
		a(args...) {}
	template <typename Packer>
	void msgpack_pack(Packer& pk) const
	{
		pk.pack_array(sizeof...(Args));

		define_imp<tuple<Args&...>, sizeof...(Args)>::pack(pk, a);
	}
	void msgpack_unpack(msgpack::object const& o)
	{
		msgpack::object_array const* oa = boost::get<msgpack::object_array>(&o.via);
		if (!oa) { throw msgpack::type_error(); }

		define_imp<tuple<Args&...>, sizeof...(Args)>::unpack(*oa, a);
	}
	void msgpack_object(msgpack::object* o) const
	{
		msgpack::object_array oa;
		oa.reserve(sizeof...(Args));

		define_imp<tuple<Args&...>, sizeof...(Args)>::object(oa, a);
		o->via = boost::move(oa);
	}

	tuple<Args&...> a;
};

template <>
struct define<> {
	typedef define<> value_type;
	typedef tuple<> tuple_type;
	template <typename Packer>
	void msgpack_pack(Packer& pk) const
	{
		pk.pack_array(0);
	}
	void msgpack_unpack(msgpack::object const& o)
	{
		msgpack::object_array const* oa = boost::get<msgpack::object_array>(&o.via);
		if (!oa) { throw msgpack::type_error(); }
	}
	void msgpack_object(msgpack::object* o) const
	{
		o->via = msgpack::object_array();
	}
};

inline define<> make_define()
{
	return define<>();
}

template <typename... Args>
define<Args...> make_define(Args&... args)
{
	return define<Args...>(args...);
}

}  // namespace type
}  // namespace msgpack


#endif /* msgpack/type/define.hpp */

