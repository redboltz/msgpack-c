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

namespace detail {
	template <typename T>
	struct float_visitor : boost::static_visitor<T> {
		T operator()(double v) const {
			return static_cast<T>(v);
		}
		T operator()(int64_t v) const {
			return static_cast<T>(v);
		}
		T operator()(uint64_t v) const {
			return static_cast<T>(v);
		}
		template <typename U>
		T operator()(U const&) const {
			throw type_error();
			return 0;
		}
	};
}

inline float& operator>> (object const& o, float& v)
{
	v = boost::apply_visitor(detail::float_visitor<float>(), o.via);
	return v;
}

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, const float& v)
{
	o.pack_float(v);
	return o;
}


inline double& operator>> (object const& o, double& v)
{
	v = boost::apply_visitor(detail::float_visitor<double>(), o.via);
	return v;
}

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, const double& v)
{
	o.pack_double(v);
	return o;
}


inline void operator<< (object& o, float v)
{
	o.via = static_cast<double>(v);
}

inline void operator<< (object& o, double v)
{
	o.via = v;
}

}  // namespace msgpack

#endif /* msgpack/type/float.hpp */

