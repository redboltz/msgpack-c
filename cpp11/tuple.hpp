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
#ifndef MSGPACK_TYPE_TUPLE_HPP
#define MSGPACK_TYPE_TUPLE_HPP

#include <tuple>

#include "msgpack/object.hpp"
#include "msgpack/cpp_config.hpp"

namespace msgpack {

namespace type {
	// tuple
	using std::get;
	using std::tuple_size;
	using std::tuple_element;
	using std::uses_allocator;
	using std::ignore;
	using std::make_tuple;
	using std::tie;
	using std::forward_as_tuple;
	using std::swap;

	template< class... Types >
	class tuple : public std::tuple<Types...> {
	public:
		using base = std::tuple<Types...>;

		using base::tuple;

		tuple() = default;
		tuple(tuple const&) = default;
		tuple(tuple&&) = default;

		template<typename... OtherTypes>
		tuple(tuple<OtherTypes...> const& other):base(static_cast<std::tuple<OtherTypes...> const&>(other)) {}
		template<typename... OtherTypes>
		tuple(tuple<OtherTypes...> && other):base(static_cast<std::tuple<OtherTypes...> &&>(other)) {}

		tuple& operator=(tuple const&) = default;
		tuple& operator=(tuple&&) = default;

		template<typename... OtherTypes>
		tuple& operator=(tuple<OtherTypes...> const& other) {
			*static_cast<base*>(this) = static_cast<std::tuple<OtherTypes...> const&>(other);
			return *this;
		}
		template<typename... OtherTypes>
		tuple& operator=(tuple<OtherTypes...> && other) {
			*static_cast<base*>(this) = static_cast<std::tuple<OtherTypes...> &&>(other);
			return *this;
		}

		template< std::size_t I>
		typename tuple_element<I, base >::type&
		get() { return std::get<I>(*this); }

		template< std::size_t I>
		typename tuple_element<I, base >::type const&
		get() const { return std::get<I>(*this); }

		template< std::size_t I>
		typename tuple_element<I, base >::type&&
		get() && { return std::get<I>(*this); }
	};

	template< class... Tuples >
	auto tuple_cat(Tuples&&... args) ->
		decltype(
			std::tuple_cat(std::forward<typename std::remove_reference<Tuples>::type::base>(args)...)
		) {
		return std::tuple_cat(std::forward<typename std::remove_reference<Tuples>::type::base>(args)...);
	}
} // namespace type

// --- Pack ( from tuple to packer stream ---
template <typename Stream, typename Tuple, std::size_t N>
struct Packer {
	static void pack(
		packer<Stream>& o,
		const Tuple& v) {
		Packer<Stream, Tuple, N-1>::pack(o, v);
		o.pack(type::get<N-1>(v));
	}
};

template <typename Stream, typename Tuple>
struct Packer<Stream, Tuple, 1> {
	static void pack (
		packer<Stream>& o,
		const Tuple& v) {
		o.pack(type::get<0>(v));
	}
};

template <typename Stream, typename... Args>
const packer<Stream>& operator<< (
	packer<Stream>& o,
	const type::tuple<Args...>& v) {
	o.pack_array(sizeof...(Args));
	Packer<Stream, decltype(v), sizeof...(Args)>::pack(o, v);
	return o;
}

// --- Convert from tuple to object ---

template <typename Tuple, std::size_t N>
struct Converter {
	static void convert(
		object const& o,
		Tuple& v) {
		Converter<Tuple, N-1>::convert(o, v);
		o.via.array.ptr[N-1].convert<typename std::remove_reference<decltype(type::get<N-1>(v))>::type>(type::get<N-1>(v));
	}
};

template <typename Tuple>
struct Converter<Tuple, 1> {
	static void convert (
		object const& o,
		Tuple& v) {
		o.via.array.ptr[0].convert<typename std::remove_reference<decltype(type::get<0>(v))>::type>(type::get<0>(v));
	}
};

template <typename... Args>
type::tuple<Args...>& operator>> (
	object const& o,
	type::tuple<Args...>& v) {
	if(o.type != type::ARRAY) { throw type_error(); }
	if(o.via.array.size < sizeof...(Args)) { throw type_error(); }
	Converter<decltype(v), sizeof...(Args)>::convert(o, v);
	return v;
}

// --- Convert from tuple to object with zone ---
template <typename Tuple, std::size_t N>
struct TupleToObject {
	static void convert(
		object& o,
		const Tuple& v) {
		TupleToObject<Tuple, N-1>::convert(o, v);
		o.via.array.ptr[N-1] = object(type::get<N-1>(v));
	}
};

template <typename Tuple>
struct TupleToObject<Tuple, 1> {
	static void convert (
		object& o,
		const Tuple& v) {
		o.via.array.ptr[0] = object(type::get<0>(v));
	}
};

template <typename... Args>
inline void operator<< (
		object& o,
		type::tuple<Args...>& v) {
	o.type = type::ARRAY;
	object* p = static_cast<object*>(::malloc(sizeof(object)*v.size()));
	if (!p) throw std::bad_alloc();
	o.via.array.ptr = p;
	o.via.array.size = sizeof...(Args);
	TupleToObject<decltype(v), sizeof...(Args)>::convert(o, v);
}

} // msgpack

#endif /* msgpack/type/tuple.hpp */

