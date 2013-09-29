//
// MessagePack for C++ deserializing routine
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
#ifndef MSGPACK_UNPACK_HPP
#define MSGPACK_UNPACK_HPP

#include "object.hpp"
#include "zone.hpp"
#include "unpack_define.h"
#include "cpp_config.hpp"

#include <memory>
#include <stdexcept>
#include <type_traits>



#define COUNTER_SIZE (sizeof(_msgpack_atomic_counter_t))

#ifndef MSGPACK_UNPACKER_INIT_BUFFER_SIZE
#define MSGPACK_UNPACKER_INIT_BUFFER_SIZE (64*1024)
#endif

#ifndef MSGPACK_UNPACKER_RESERVE_SIZE
#define MSGPACK_UNPACKER_RESERVE_SIZE (32*1024)
#endif


// backward compatibility
#ifndef MSGPACK_UNPACKER_DEFAULT_INITIAL_BUFFER_SIZE
#define MSGPACK_UNPACKER_DEFAULT_INITIAL_BUFFER_SIZE MSGPACK_UNPACKER_INIT_BUFFER_SIZE
#endif


namespace msgpack {

namespace detail {

class unpack_user {
public:
	zone const& z() const { return *z_; }
	zone& z() { return *z_; }
	void set_z(zone& z) { z_ = &z; }
	bool referenced() const { return referenced_; }
	void set_referenced(bool referenced) { referenced_ = referenced; }
private:
	zone* z_;
	bool referenced_;
};

template <typename ForwardIterator>
inline void unpack_uint8(unpack_user&, uint8_t d, object<ForwardIterator>& o)
{ o.type = type::POSITIVE_INTEGER; o.via.u64 = d; }

template <typename ForwardIterator>
inline void unpack_uint16(unpack_user&, uint16_t d, object<ForwardIterator>& o)
{ o.type = type::POSITIVE_INTEGER; o.via.u64 = d; }

template <typename ForwardIterator>
inline void unpack_uint32(unpack_user&, uint32_t d, object<ForwardIterator>& o)
{ o.type = type::POSITIVE_INTEGER; o.via.u64 = d; }

template <typename ForwardIterator>
inline void unpack_uint64(unpack_user&, uint64_t d, object<ForwardIterator>& o)
{ o.type = type::POSITIVE_INTEGER; o.via.u64 = d; }

template <typename ForwardIterator>
inline void unpack_int8(unpack_user&, int8_t d, object<ForwardIterator>& o)
{ if(d >= 0) { o.type = type::POSITIVE_INTEGER; o.via.u64 = d; }
		else { o.type = type::NEGATIVE_INTEGER; o.via.i64 = d; } }

template <typename ForwardIterator>
inline void unpack_int16(unpack_user&, int16_t d, object<ForwardIterator>& o)
{ if(d >= 0) { o.type = type::POSITIVE_INTEGER; o.via.u64 = d; }
		else { o.type = type::NEGATIVE_INTEGER; o.via.i64 = d; } }

template <typename ForwardIterator>
inline void unpack_int32(unpack_user&, int32_t d, object<ForwardIterator>& o)
{ if(d >= 0) { o.type = type::POSITIVE_INTEGER; o.via.u64 = d; }
		else { o.type = type::NEGATIVE_INTEGER; o.via.i64 = d; } }

template <typename ForwardIterator>
inline void unpack_int64(unpack_user&, int64_t d, object<ForwardIterator>& o)
{ if(d >= 0) { o.type = type::POSITIVE_INTEGER; o.via.u64 = d; }
		else { o.type = type::NEGATIVE_INTEGER; o.via.i64 = d; } }

template <typename ForwardIterator>
inline void unpack_float(unpack_user&, float d, object<ForwardIterator>& o)
{ o.type = type::DOUBLE; o.via.dec = d; }

template <typename ForwardIterator>
inline void unpack_double(unpack_user&, double d, object<ForwardIterator>& o)
{ o.type = type::DOUBLE; o.via.dec = d; }

template <typename ForwardIterator>
inline void unpack_nil(unpack_user&, object<ForwardIterator>& o)
{ o.type = type::NIL; }

template <typename ForwardIterator>
inline void unpack_true(unpack_user&, object<ForwardIterator>& o)
{ o.type = type::BOOLEAN; o.via.boolean = true; }

template <typename ForwardIterator>
inline void unpack_false(unpack_user&, object<ForwardIterator>& o)
{ o.type = type::BOOLEAN; o.via.boolean = false; }

struct unpack_array {
	template <typename ForwardIterator>
	bool operator()(unpack_user&u, unsigned int n, object<ForwardIterator>& o) const {
		o.type = type::ARRAY;
		o.via.array.size = 0;
		o.via.array.ptr = (object<ForwardIterator>*)u.z().malloc(n*sizeof(object<ForwardIterator>));
		if(o.via.array.ptr == nullptr) { return false; }
		return true;
	}
};

template <typename ForwardIterator>
inline void unpack_array_item(unpack_user&, object<ForwardIterator>& c, object<ForwardIterator> const& o)
{ c.via.array.ptr[c.via.array.size++] = o; }

struct unpack_map {
	template <typename ForwardIterator>
	bool operator()(unpack_user& u, unsigned int n, object<ForwardIterator>& o) const {
		o.type = type::MAP;
		o.via.map.size = 0;
		o.via.map.ptr = (object_kv<ForwardIterator>*)u.z().malloc(n*sizeof(object_kv<ForwardIterator>));
		if(o.via.map.ptr == nullptr) { return false; }
		return true;
	}
};

template <typename ForwardIterator>
inline void unpack_map_item(unpack_user&, object<ForwardIterator>& c, object<ForwardIterator> const& k, object<ForwardIterator> const& v)
{
	c.via.map.ptr[c.via.map.size].key = k;
	c.via.map.ptr[c.via.map.size].val = v;
	++c.via.map.size;
}

template <typename ForwardIterator>
inline void unpack_raw(unpack_user& u, ForwardIterator it, unsigned int l, object<ForwardIterator>& o)
{
	o.type = type::RAW;
	o.via.raw.begin = it;
	std::advance(it, l);
	o.via.raw.end = it;
	u.set_referenced(true);
}


template <typename ForwardIterator>
class template_unpack_stack {
public:
	object<ForwardIterator> const& obj() const { return obj_; }
	object<ForwardIterator>& obj() { return obj_; }
	void setObj(object<ForwardIterator> const& obj) { obj_ = obj; }
	size_t count() const { return count_; }
	void set_count(size_t count) { count_ = count; }
	size_t decl_count() { return --count_; }
	unsigned int ct() const { return ct_; }
	void set_ct(unsigned int ct) { ct_ = ct; }
	object<ForwardIterator> const& map_key() const { return map_key_; }
	void set_map_key(object<ForwardIterator> const& map_key) { map_key_ = map_key; }
private:
	object<ForwardIterator> obj_;
	size_t count_;
	unsigned int ct_;
	object<ForwardIterator> map_key_;
};

inline void init_count(void* buffer)
{
	*(volatile _msgpack_atomic_counter_t*)buffer = 1;
}

inline void decl_count(void* buffer)
{
	// atomic if(--*(_msgpack_atomic_counter_t*)buffer == 0) { free(buffer); }
	if(_msgpack_sync_decr_and_fetch((volatile _msgpack_atomic_counter_t*)buffer) == 0) {
		free(buffer);
	}
}

inline void incr_count(void* buffer)
{
	// atomic ++*(_msgpack_atomic_counter_t*)buffer;
	_msgpack_sync_incr_and_fetch((volatile _msgpack_atomic_counter_t*)buffer);
}

inline _msgpack_atomic_counter_t get_count(void* buffer)
{
	return *(volatile _msgpack_atomic_counter_t*)buffer;
}

struct fix_tag {
	char f1[65]; // FIXME unique size is required. or use is_same meta function.
};

template <typename T>
inline unsigned int load(const char* n, typename msgpack::enable_if<sizeof(T) == sizeof(fix_tag)>::type* = nullptr) {
	return static_cast<unsigned int>(*reinterpret_cast<const uint8_t*>(n)) & 0x0f;
}

template <typename T>
inline T load(const char* n, typename msgpack::enable_if<sizeof(T) == 1>::type* = nullptr) {
	return static_cast<T>(*reinterpret_cast<const uint8_t*>(n));
}

template <typename T>
inline T load(const char* n, typename msgpack::enable_if<sizeof(T) == 2>::type* = nullptr) {
	return static_cast<T>(
		(static_cast<uint16_t>(reinterpret_cast<const uint8_t*>(n)[0]) <<  8) |
		(static_cast<uint16_t>(reinterpret_cast<const uint8_t*>(n)[1])      ));
}

template <typename T>
inline T load(const char* n, typename msgpack::enable_if<sizeof(T) == 4>::type* = nullptr) {
	return static_cast<T>(
		(static_cast<uint32_t>(reinterpret_cast<const uint8_t*>(n)[0]) << 24) |
		(static_cast<uint32_t>(reinterpret_cast<const uint8_t*>(n)[1]) << 16) |
		(static_cast<uint32_t>(reinterpret_cast<const uint8_t*>(n)[2]) <<  8) |
		(static_cast<uint32_t>(reinterpret_cast<const uint8_t*>(n)[3])      ));
}

template <typename T>
inline T load(const char* n, typename msgpack::enable_if<sizeof(T) == 8>::type* = nullptr) {
	return static_cast<T>(
		(static_cast<uint64_t>(reinterpret_cast<const uint8_t*>(n)[0]) << 56) |
		(static_cast<uint64_t>(reinterpret_cast<const uint8_t*>(n)[1]) << 48) |
		(static_cast<uint64_t>(reinterpret_cast<const uint8_t*>(n)[2]) << 40) |
		(static_cast<uint64_t>(reinterpret_cast<const uint8_t*>(n)[3]) << 32) |
		(static_cast<uint64_t>(reinterpret_cast<const uint8_t*>(n)[4]) << 24) |
		(static_cast<uint64_t>(reinterpret_cast<const uint8_t*>(n)[5]) << 16) |
		(static_cast<uint64_t>(reinterpret_cast<const uint8_t*>(n)[6]) <<  8) |
		(static_cast<uint64_t>(reinterpret_cast<const uint8_t*>(n)[7])      ));
}

template <typename ForwardIterator>
class template_context {
public:
	template_context():trail_(0), cs_(CS_HEADER), top_(0)
	{
		stack_[0].setObj(object<ForwardIterator>());
	}

	void init()
	{
		cs_ = CS_HEADER;
		trail_ = 0;
		top_ = 0;
		stack_[0].setObj(object<ForwardIterator>());
	}

	object<ForwardIterator> const& data() const
	{
		return stack_[0].obj();
	}

	unpack_user& user()
	{
		return user_;
	}

	unpack_user const& user() const
	{
		return user_;
	}

	int execute(ForwardIterator& it, ForwardIterator end)
	//int execute(const char* data, size_t len, size_t& off)
	{
		// Insert concept checking here

		// p == it
		// pe == end
		ForwardIterator n = it;

		// to support register optimization
		unsigned int trail = trail_;

		object<ForwardIterator> obj;
		template_unpack_stack<ForwardIterator>* c = nullptr;

		if(it == end) {
			trail_ = trail;
			return 0;
		}
		bool fixed_trail_again = false;
		do {
			if (cs_ == CS_HEADER) {
				fixed_trail_again = false;
				int selector = static_cast<int>(*it);
				if (0) {
				} else if(0x00 <= selector && selector <= 0x7f) { // Positive Fixnum
					unpack_uint8(user_, *it, obj);
					int ret = push_proc(c, obj, it);
					if (ret != 0) return ret;
				} else if(0xe0 <= selector && selector <= 0xff) { // Negative Fixnum
					unpack_int8(user_, *it, obj);
					int ret = push_proc(c, obj, it);
					if (ret != 0) return ret;
				} else if(0xc0 <= selector && selector <= 0xdf) { // Variable
					switch(selector) {
					case 0xc0: {	// nil
						unpack_nil(user_, obj);
						int ret = push_proc(c, obj, it);
						if (ret != 0) return ret;
					} break;
					//case 0xc1:  // string
					case 0xc2: {	// false
						unpack_false(user_, obj);
						int ret = push_proc(c, obj, it);
						if (ret != 0) return ret;
					} break;
					case 0xc3: {	// true
						unpack_true(user_, obj);
						int ret = push_proc(c, obj, it);
						if (ret != 0) return ret;
					} break;
					case 0xc4: // bin 8
					case 0xc5: // bin 16
					case 0xc6: // bin 32
						trail = 1 << (static_cast<unsigned int>(*it) & 0x03);
						cs_ = next_cs(it);
						fixed_trail_again = true;
						break;

					//case 0xc7:
					//case 0xc8:
					//case 0xc9:
					case 0xca:	// float
					case 0xcb:	// double
					case 0xcc:	// unsigned int	 8
					case 0xcd:	// unsigned int 16
					case 0xce:	// unsigned int 32
					case 0xcf:	// unsigned int 64
					case 0xd0:	// signed int  8
					case 0xd1:	// signed int 16
					case 0xd2:	// signed int 32
					case 0xd3:	// signed int 64
						trail = 1 << (static_cast<unsigned int>(*it) & 0x03);
						cs_ = next_cs(it);
						fixed_trail_again = true;
						break;
					//case 0xd4:
					//case 0xd5:
					//case 0xd6:  // big integer 16
					//case 0xd7:  // big integer 32
					//case 0xd8:  // big float 16
					case 0xd9:	// raw 8 (str 8)
					case 0xda:	// raw 16 (str 16)
					case 0xdb:	// raw 32 (str 32)
						trail = 1 << ((static_cast<unsigned int>(*it) & 0x03) - 1);
						cs_ = next_cs(it);
						fixed_trail_again = true;
						break;
					case 0xdc:	// array 16
					case 0xdd:	// array 32
					case 0xde:	// map 16
					case 0xdf:	// map 32
						trail = 2 << (static_cast<unsigned int>(*it) & 0x01);
						cs_ = next_cs(it);
						fixed_trail_again = true;
						break;
					default:
						trail_ = trail;
						return -1;
					}
				} else if(0xa0 <= selector && selector <= 0xbf) { // FixRaw
					trail = static_cast<unsigned int>(*it) & 0x1f;
					if(trail == 0) {
						unpack_raw(user_, n, trail, obj);
						int ret = push_proc(c, obj, it);
						if (ret != 0) return ret;
					}
					cs_ = ACS_RAW_VALUE;
					fixed_trail_again = true;

				} else if(0x90 <= selector && selector <= 0x9f) { // FixArray
					int ret = push_aggregate<fix_tag>(
						unpack_array(), CT_ARRAY_ITEM, c, obj, it, it, trail);
					if (ret != 0) return ret;
				} else if(0x80 <= selector && selector <= 0x8f) { // FixMap
					int ret = push_aggregate<fix_tag>(
						unpack_map(), CT_MAP_KEY, c, obj, it, it, trail);
					if (ret != 0) return ret;
				} else {
					trail_ = trail;
					return -1;
				}
				// end CS_HEADER
			}
			if (cs_ != CS_HEADER || fixed_trail_again) {
				if (fixed_trail_again) {
					++it;
					fixed_trail_again = false;
				}
				if(std::distance(it, end) < trail) {
					trail_ = trail;
					return 0;
				}
				n = it;
				std::advance(it, trail - 1);
				switch(cs_) {
				//case CS_
				//case CS_
				case CS_FLOAT: {
					union { uint32_t i; float f; } mem;
					mem.i = load<uint32_t>(n);
					unpack_float(user_, mem.f, obj);
					int ret = push_proc(c, obj, it);
					if (ret != 0) return ret;
				} break;
				case CS_DOUBLE: {
					union { uint64_t i; double f; } mem;
					mem.i = load<uint64_t>(n);
#if defined(__arm__) && !(__ARM_EABI__) // arm-oabi
					// https://github.com/msgpack/msgpack-perl/pull/1
					mem.i = (mem.i & 0xFFFFFFFFUL) << 32UL | (mem.i >> 32UL);
#endif
					unpack_double(user_, mem.f, obj);
					int ret = push_proc(c, obj, it);
					if (ret != 0) return ret;
				} break;
				case CS_UINT_8: {
					unpack_uint8(user_, load<uint8_t>(n), obj);
					int ret = push_proc(c, obj, it);
					if (ret != 0) return ret;
				} break;
				case CS_UINT_16: {
					unpack_uint16(user_, load<uint16_t>(n), obj);
					int ret = push_proc(c, obj, it);
					if (ret != 0) return ret;
				} break;
				case CS_UINT_32: {
					unpack_uint32(user_, load<uint32_t>(n), obj);
					int ret = push_proc(c, obj, it);
					if (ret != 0) return ret;
				} break;
				case CS_UINT_64: {
					unpack_uint64(user_, load<uint64_t>(n), obj);
					int ret = push_proc(c, obj, it);
					if (ret != 0) return ret;
				} break;
				case CS_INT_8: {
					unpack_int8(user_, load<uint8_t>(n), obj);
					int ret = push_proc(c, obj, it);
					if (ret != 0) return ret;
				} break;
				case CS_INT_16: {
					unpack_int16(user_, load<int16_t>(n), obj);
					int ret = push_proc(c, obj, it);
					if (ret != 0) return ret;
				} break;
				case CS_INT_32: {
					unpack_int32(user_, load<int32_t>(n), obj);
					int ret = push_proc(c, obj, it);
					if (ret != 0) return ret;
				} break;
				case CS_INT_64: {
					unpack_int64(user_, load<int64_t>(n), obj);
					int ret = push_proc(c, obj, it);
					if (ret != 0) return ret;
				} break;
				case CS_BIN_8:
				case CS_RAW_8:
					trail = load<uint8_t>(n);
					if(trail == 0) {
						unpack_raw(user_, data, n, trail, obj);
						int ret = push_proc(c, obj, it);
						if (ret != 0) return ret;
					}
					else {
						cs_ = ACS_RAW_VALUE;
						fixed_trail_again = true;
					}
					break;
				case CS_BIN_16:
				case CS_RAW_16:
					trail = load<uint16_t>(n);
					if(trail == 0) {
						unpack_raw(user_, data, n, trail, obj);
						int ret = push_proc(c, obj, it);
						if (ret != 0) return ret;
					}
					else {
						cs_ = ACS_RAW_VALUE;
						fixed_trail_again = true;
					}
					break;
				case CS_BIN_32:
				case CS_RAW_32:
					trail = load<uint32_t>(n);
					if(trail == 0) {
						unpack_raw(user_, data, n, trail, obj);
						int ret = push_proc(c, obj, it);
						if (ret != 0) return ret;
					}
					else {
						cs_ = ACS_RAW_VALUE;
						fixed_trail_again = true;
					}
					break;
				case ACS_RAW_VALUE: {
					unpack_raw(user_, data, n, trail, obj);
					int ret = push_proc(c, obj, it);
					if (ret != 0) return ret;
				} break;
				case CS_ARRAY_16: {
					int ret = push_aggregate<uint16_t>(
						unpack_array(), CT_ARRAY_ITEM, c, obj, it, n, trail);
					if (ret != 0) return ret;
				} break;
				case CS_ARRAY_32: {
					/* FIXME security guard */
					int ret = push_aggregate<uint32_t>(
						unpack_array(), CT_ARRAY_ITEM, c, obj, it, n, trail);
					if (ret != 0) return ret;
				} break;
				case CS_MAP_16: {
					int ret = push_aggregate<uint16_t>(
						unpack_map(), CT_MAP_KEY, c, obj, it, n, trail);
					if (ret != 0) return ret;
				} break;
				case CS_MAP_32: {
					/* FIXME security guard */
					int ret = push_aggregate<uint32_t>(
						unpack_map(), CT_MAP_KEY, c, obj, it, n, trail);
					if (ret != 0) return ret;
				} break;
				default:
					trail_ = trail;
					return -1;
				}
			}
		} while(it != end);

		trail_ = trail;
		return 0;
	}

private:
	static unsigned int next_cs(ForwardIterator const& it)
	{
		return static_cast<unsigned int>(*it) & 0x1f;
	}

	template <typename T, typename Func>
	int push_aggregate(
		Func const& f,
		unsigned int ct,
		template_unpack_stack<ForwardIterator>*& c,
		object<ForwardIterator>& obj,
		ForwardIterator*& current,
		ForwardIterator* load_pos,
		unsigned int trail) {
		if(top_ < MSGPACK_EMBED_STACK_SIZE /* FIXME */
		   && f(user_, load<T>(load_pos), stack_[top_].obj())) {
			if(load<T>(load_pos) == 0) {
				obj = stack_[top_].obj();
				int ret = push_proc(c, obj, current);
				if (ret != 0) return ret;
			}
			else {
				stack_[top_].set_ct(ct);
				stack_[top_].set_count(load<T>(load_pos));
				++top_;
				header_again(current);
			}
		}
		else {
			trail_ = trail;
			return -1;
		}
		return 0;
	}

	void header_again(ForwardIterator& current) {
		cs_ = CS_HEADER;
		++current;
	}

	int push_item(template_unpack_stack<ForwardIterator>*& c, object<ForwardIterator>& obj) {
		bool finish = false;
		while (!finish) {
			if(top_ == 0) {
				return 1;
			}
			c = &stack_[top_ - 1];
			switch(c->ct()) {
			case CT_ARRAY_ITEM:
				unpack_array_item(user_, c->obj(), obj);
				if(c->decl_count() == 0) {
					obj = c->obj();
					--top_;
					/*printf("stack pop %d\n", top_);*/
				}
				else {
					finish = true;
				}
				break;
			case CT_MAP_KEY:
				c->set_map_key(obj);
				c->set_ct(CT_MAP_VALUE);
				finish = true;
				break;
			case CT_MAP_VALUE:
				unpack_map_item(user_, c->obj(), c->map_key(), obj);
				if(c->decl_count() == 0) {
					obj = c->obj();
					--top_;
					/*printf("stack pop %d\n", top_);*/
				}
				else {
					c->set_ct(CT_MAP_KEY);
					finish = true;
				}
				break;
			default:
				return -1;
			}
		}
		return 0;
	}

	int push_proc(
		template_unpack_stack<ForwardIterator>*& c,
		object<ForwardIterator>& obj,
		ForwardIterator& current) {
		int ret = push_item(c, obj);
		if (ret > 0) {
			stack_[0].setObj(obj);
			++current;
			/*printf("-- finish --\n"); */
		}
		else if (ret < 0) {
		}
		else {
			header_again(current);
		}
		return ret;
	}

private:
	unsigned int trail_;
	unpack_user user_;
	unsigned int cs_;
	unsigned int top_;
	template_unpack_stack<ForwardIterator> stack_[MSGPACK_EMBED_STACK_SIZE];
};

} // detail


struct unpack_error : public std::runtime_error {
	unpack_error(const std::string& msg) :
		std::runtime_error(msg) { }
};

template <typename ForwardIterator = char*>
class unpacked {
public:
	unpacked() { }

	unpacked(object<ForwardIterator> const& obj, msgpack::unique_ptr<msgpack::zone> z) :
		m_obj(obj), m_zone(msgpack::move(z)) { }

	void set(object<ForwardIterator> const& obj)
		{ m_obj = obj; }

	const object<ForwardIterator>& get() const
		{ return m_obj; }

	msgpack::unique_ptr<msgpack::zone>& zone()
		{ return m_zone; }

	const msgpack::unique_ptr<msgpack::zone>& zone() const
		{ return m_zone; }

private:
	object<ForwardIterator> m_obj;
	msgpack::unique_ptr<msgpack::zone> m_zone;
};


template <typename ForwardIterator>
class unpacker {
public:
	unpacker(size_t init_buffer_size = MSGPACK_UNPACKER_INIT_BUFFER_SIZE);
	~unpacker();

public:
	/*! 1. reserve buffer. at least `size' bytes of capacity will be ready */
	void reserve_buffer(size_t size = MSGPACK_UNPACKER_RESERVE_SIZE);

	/*! 2. read data to the buffer() up to buffer_capacity() bytes */
	char* buffer();
	size_t buffer_capacity() const;

	/*! 3. specify the number of bytes actually copied */
	void buffer_consumed(size_t size);

	/*! 4. repeat next() until it retunrs false */
	bool next(unpacked<ForwardIterator>* result);

	/*! 5. check if the size of message doesn't exceed assumption. */
	size_t message_size() const;

	// Basic usage of the unpacker is as following:
	//
	// unpacker pac;
	// while( /* input is readable */ ) {
	//
	//     // 1.
	//     pac.reserve_buffer(32*1024);
	//
	//     // 2.
	//     size_t bytes = input.readsome(pac.buffer(), pac.buffer_capacity());
	//
	//     // error handling ...
	//
	//     // 3.
	//     pac.buffer_consumed(bytes);
	//
	//     // 4.
	//     unpacked result;
	//     while(pac.next(&result)) {
	//         // do some with the object with the zone.
	//         object obj = result.get();
	//         std::auto_ptr<msgpack:zone> z = result.zone();
	//         on_message(obj, z);
	//
	//         //// boost::shared_ptr is also usable:
	//         // boost::shared_ptr<zone> life(z.release());
	//         // on_message(result.get(), life);
	//     }
	//
	//     // 5.
	//     if(pac.message_size() > 10*1024*1024) {
	//         throw std::runtime_error("message is too large");
	//     }
	// }
	//

	/*! for backward compatibility */
	bool execute();

	/*! for backward compatibility */
	object<ForwardIterator> const& data();

	/*! for backward compatibility */
	zone* release_zone();

	/*! for backward compatibility */
	void reset_zone();

	/*! for backward compatibility */
	void reset();

public:
	// These functions are usable when non-MessagePack message follows after
	// MessagePack message.
	size_t parsed_size() const;

	/*! get address of the buffer that is not parsed */
	char* nonparsed_buffer();
	size_t nonparsed_size() const;

	/*! skip specified size of non-parsed buffer, leaving the buffer */
	// Note that the `size' argument must be smaller than nonparsed_size()
	void skip_nonparsed_buffer(size_t size);

	/*! remove unparsed buffer from unpacker */
	// Note that reset() leaves non-parsed buffer.
	void remove_nonparsed_buffer();

private:
	void expand_buffer(size_t size);
	int execute_imp();
	bool flush_zone();

private:
	char* buffer_;
	size_t used_;
	size_t free_;
	size_t off_;
	size_t parsed_;
	zone* z_;
	size_t initial_buffer_size_;
	detail::template_context<ForwardIterator> ctx_;

private:
	unpacker(const unpacker&);
};


template <typename ForwardIterator>
inline void unpack(unpacked<ForwardIterator>& result,
		ForwardIterator data, size_t len, size_t* offset = nullptr);
template <typename ForwardIterator>
inline void unpack(unpacked<ForwardIterator>* result,
		ForwardIterator data, size_t len, size_t* offset = nullptr);

// obsolete
typedef enum {
	UNPACK_SUCCESS				=  2,
	UNPACK_EXTRA_BYTES			=  1,
	UNPACK_CONTINUE			=  0,
	UNPACK_PARSE_ERROR			= -1
} unpack_return;

// obsolete
template <typename ForwardIterator>
static unpack_return unpack(ForwardIterator data, size_t len, size_t* off,
	zone& z, object<ForwardIterator>& result);

template <typename ForwardIterator>
static unpack_return unpack(ForwardIterator data, size_t len, size_t* off,
	zone* z, object<ForwardIterator>* result);


// obsolete
template <typename ForwardIterator>
static object<ForwardIterator>
unpack(ForwardIterator data, size_t len, zone& z, size_t* off = nullptr);

template <typename ForwardIterator>
static object<ForwardIterator>
unpack(ForwardIterator data, size_t len, zone* z, size_t* off = nullptr);


template <typename ForwardIterator>
inline unpacker<ForwardIterator>::unpacker(size_t initial_buffer_size)
{
	if(initial_buffer_size < COUNTER_SIZE) {
		initial_buffer_size = COUNTER_SIZE;
	}

	char* buffer = reinterpret_cast<char*>(::malloc(initial_buffer_size));
	if(!buffer) {
		throw std::bad_alloc();
	}

	zone* z = zone::create(MSGPACK_ZONE_CHUNK_SIZE);
	if(!z) {
		::free(buffer);
		throw std::bad_alloc();
	}

	buffer_ = buffer;
	used_ = COUNTER_SIZE;
	free_ = initial_buffer_size - used_;
	off_ = COUNTER_SIZE;
	parsed_ = 0;
	initial_buffer_size_ = initial_buffer_size;
	z_ = z;

	detail::init_count(buffer_);

	ctx_.init();
	ctx_.user().set_z(*z_);
	ctx_.user().set_referenced(false);
}

template <typename ForwardIterator>
inline unpacker<ForwardIterator>::~unpacker()
{
	zone::destroy(z_);
	detail::decl_count(buffer_);
}


template <typename ForwardIterator>
inline void unpacker<ForwardIterator>::reserve_buffer(size_t size)
{
	if(free_ >= size) return;
	expand_buffer(size);
}

template <typename ForwardIterator>
inline void unpacker<ForwardIterator>::expand_buffer(size_t size)
{
	if(used_ == off_ && detail::get_count(buffer_) == 1
		&& !ctx_.user().referenced()) {
		// rewind buffer
		free_ += used_ - COUNTER_SIZE;
		used_ = COUNTER_SIZE;
		off_  = COUNTER_SIZE;

		if(free_ >= size) return;
	}

	if(off_ == COUNTER_SIZE) {
		size_t next_size = (used_ + free_) * 2;	 // include COUNTER_SIZE
		while(next_size < size + used_) {
			next_size *= 2;
		}

		char* tmp = reinterpret_cast<char*>(::realloc(buffer_, next_size));
		if(!tmp) {
			throw std::bad_alloc();
		}

		buffer_ = tmp;
		free_ = next_size - used_;

	} else {
		size_t next_size = initial_buffer_size_;  // include COUNTER_SIZE
		size_t not_parsed = used_ - off_;
		while(next_size < size + not_parsed + COUNTER_SIZE) {
			next_size *= 2;
		}

		char* tmp = reinterpret_cast<char*>(::malloc(next_size));
		if(!tmp) {
			throw std::bad_alloc();
		}

		detail::init_count(tmp);

		::memcpy(tmp+COUNTER_SIZE, buffer_ + off_, not_parsed);

		if(ctx_.user().referenced()) {
			try {
				z_->push_finalizer(&detail::decl_count, buffer_);
			}
			catch (...) {
				::free(tmp);
				throw;
			}
			ctx_.user().set_referenced(false);
		} else {
			detail::decl_count(buffer_);
		}

		buffer_ = tmp;
		used_	= not_parsed + COUNTER_SIZE;
		free_	= next_size - used_;
		off_	= COUNTER_SIZE;
	}
}

template <typename ForwardIterator>
inline char* unpacker<ForwardIterator>::buffer()
{
	return buffer_ + used_;
}

template <typename ForwardIterator>
inline size_t unpacker<ForwardIterator>::buffer_capacity() const
{
	return free_;
}

template <typename ForwardIterator>
inline void unpacker<ForwardIterator>::buffer_consumed(size_t size)
{
	used_ += size;
	free_ -= size;
}

template <typename ForwardIterator>
inline bool unpacker<ForwardIterator>::next(unpacked<ForwardIterator>* result)
{
	int ret = execute_imp();

	if(ret < 0) {
		throw unpack_error("parse error");
	}

	if(ret == 0) {
		if (result->zone().get() != nullptr) result->zone().reset();
		result->set(object<ForwardIterator>());
		return false;

	} else {
		if (result->zone().get() != nullptr) result->zone().reset( release_zone() );
		result->set(data());
		reset();
		return true;
	}
}


template <typename ForwardIterator>
inline bool unpacker<ForwardIterator>::execute()
{
	int ret = execute_imp();
	if(ret < 0) {
		throw unpack_error("parse error");
	} else if(ret == 0) {
		return false;
	} else {
		return true;
	}
}

template <typename ForwardIterator>
inline int unpacker<ForwardIterator>::execute_imp()
{
	const char *it = buffer_ + off_;
	const char *it_org = it;
	int ret = ctx_.execute(it, it + used_);
	if(it > it_org) {
		parsed_ += it - it_org;
	}
	return ret;
}

template <typename ForwardIterator>
inline object<ForwardIterator> const& unpacker<ForwardIterator>::data()
{
	return ctx_.data();
}

template <typename ForwardIterator>
inline zone* unpacker<ForwardIterator>::release_zone()
{
	if(!flush_zone()) {
		return nullptr;
	}

	zone* r =  zone::create(MSGPACK_ZONE_CHUNK_SIZE);
	if(!r) {
		return nullptr;
	}

	zone* old = z_;
	z_ = r;
	ctx_.user().set_z(*z_);

	return old;
}

template <typename ForwardIterator>
inline void unpacker<ForwardIterator>::reset_zone()
{
	z_->clear();
}

template <typename ForwardIterator>
inline bool unpacker<ForwardIterator>::flush_zone()
{
	if(ctx_.user().referenced()) {
		try {
			z_->push_finalizer(&detail::decl_count, buffer_);
		} catch (...) {
			return false;
		}
		ctx_.user().set_referenced(false);

		detail::incr_count(buffer_);
	}

	return true;
}

template <typename ForwardIterator>
inline void unpacker<ForwardIterator>::reset()
{
	ctx_.init();
	// don't reset referenced flag
	parsed_ = 0;
}

template <typename ForwardIterator>
inline size_t unpacker<ForwardIterator>::message_size() const
{
	return parsed_ - off_ + used_;
}

template <typename ForwardIterator>
inline size_t unpacker<ForwardIterator>::parsed_size() const
{
	return parsed_;
}

template <typename ForwardIterator>
inline char* unpacker<ForwardIterator>::nonparsed_buffer()
{
	return buffer_ + off_;
}

template <typename ForwardIterator>
inline size_t unpacker<ForwardIterator>::nonparsed_size() const
{
	return used_ - off_;
}

template <typename ForwardIterator>
inline void unpacker<ForwardIterator>::skip_nonparsed_buffer(size_t size)
{
	off_ += size;
}

template <typename ForwardIterator>
inline void unpacker<ForwardIterator>::remove_nonparsed_buffer()
{
	used_ = off_;
}

namespace detail {

template <typename ForwardIterator>
inline unpack_return
unpack_imp(ForwardIterator& it, ForwardIterator end,
   zone& result_zone, object<ForwardIterator>& result)
{
	if(it == end) {
		// FIXME
		return UNPACK_CONTINUE;
	}

	detail::template_context<ForwardIterator> ctx;
	ctx.init();

	ctx.user().set_z(result_zone);
	ctx.user().set_referenced(false);

	int e = ctx.execute(it, end);
	if(e < 0) {
		return UNPACK_PARSE_ERROR;
	}

	if(e == 0) {
		return UNPACK_CONTINUE;
	}

	result = ctx.data();

	if(it != end) {
		return UNPACK_EXTRA_BYTES;
	}

	return UNPACK_SUCCESS;
}

} // detail

// reference version
template <typename ForwardIterator>
inline void unpack(unpacked<ForwardIterator>& result,
   ForwardIterator data, size_t len, size_t* offset)
{
	object<ForwardIterator> obj;
	msgpack::unique_ptr<zone> z(new zone());
	ForwardIterator org(data);

	ForwardIterator end = data;
	std::advance(end, len);

	unpack_return ret = detail::unpack_imp(
			data, end, *z, obj);

	*offset = std::distance(org, data);

	switch(ret) {
	case UNPACK_SUCCESS:
		result.set(obj);
		result.zone() = msgpack::move(z);
		return;

	case UNPACK_EXTRA_BYTES:
		result.set(obj);
		result.zone() = msgpack::move(z);
		return;

	case UNPACK_CONTINUE:
		throw unpack_error("insufficient bytes");

	case UNPACK_PARSE_ERROR:
	default:
		throw unpack_error("parse error");
	}
}
// pointer version
template <typename ForwardIterator>
inline void unpack(unpacked<ForwardIterator>* result,
	ForwardIterator data, size_t len, size_t* offset) {
	unpack(*result, data, len, offset);
}


// obsolete
// reference version
template <typename ForwardIterator>
inline unpack_return unpack(ForwardIterator data, size_t len, size_t* off,
	zone& z, object<ForwardIterator>& result)
{
	ForwardIterator org = data;
	ForwardIterator end = data;
	std::advance(end, len);

	unpack_return ret = detail::unpack_imp(data, end, z, result);
	*off = std::distance(org, data);

	return ret;
}
// pointer version
template <typename ForwardIterator>
inline unpack_return unpack(ForwardIterator data, size_t len, size_t* off,
	zone* z, object<ForwardIterator>* result)
{
	return unpack(data, len, off, *z, *result);
}

// obsolete
// reference version
template <typename ForwardIterator>
inline object<ForwardIterator> unpack(ForwardIterator data, size_t len, zone& z, size_t* off)
{
	object<ForwardIterator> result;

	switch( unpack(data, len, off, z, result) ) {
	case UNPACK_SUCCESS:
		return result;

	case UNPACK_EXTRA_BYTES:
		if(off) {
			return result;
		} else {
			throw unpack_error("extra bytes");
		}

	case UNPACK_CONTINUE:
		throw unpack_error("insufficient bytes");

	case UNPACK_PARSE_ERROR:
	default:
		throw unpack_error("parse error");
	}
}
// pointer version
template <typename ForwardIterator>
inline object<ForwardIterator> unpack(ForwardIterator data, size_t len, zone* z, size_t* off)
{
	return unpack(data, len, *z, off);
}

}  // namespace msgpack

#endif /* msgpack/unpack.hpp */

