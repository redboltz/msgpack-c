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
#ifndef MSGPACK_TYPE_STRING_HPP
#define MSGPACK_TYPE_STRING_HPP

#include "msgpack/object.hpp"
#include <string>

namespace msgpack {

namespace detail {
	class string_visitor : public boost::static_visitor<void> {
	public:
		string_visitor(std::string& s):s_(s) {}
		void operator()(object_bin const& v) const {
			s_.assign(v.ptr.get(), v.size);
		}
		void operator()(object_str const& v) const {
			s_.assign(v.ptr.get(), v.size);
		}
		template <typename U>
		void operator()(U const&) const {
			throw type_error();
		}
	private:
		std::string& s_;
	};
}

inline std::string& operator>> (object const& o, std::string& v)
{
	boost::apply_visitor(detail::string_visitor(v), o.via);
	return v;
}

template <typename Stream>
inline packer<Stream>& operator<< (packer<Stream>& o, const std::string& v)
{
	o.pack_bin(v.size());
	o.pack_bin_body(v.data(), v.size());
	return o;
}

inline void operator<< (object& o, const std::string& v)
{
	object_bin ob;
	ob.size = v.size();
	char* p = reinterpret_cast<char*>(malloc(ob.size));
	if (!p) throw std::bad_alloc();
	ob.ptr.reset(p, free);
	memcpy(p, v.data(), ob.size);
	o.via = boost::move(ob);
}

// Are reference version and move version required?


}  // namespace msgpack

#endif /* msgpack/type/string.hpp */

