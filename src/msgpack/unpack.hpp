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
#include "unpack_define.h"
#include "cpp_config.hpp"

#include <memory>
#include <stdexcept>




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

inline void unpack_uint8(uint8_t d, object& o)
{ o.via = static_cast<uint64_t>(d); }

inline void unpack_uint16(uint16_t d, object& o)
{ o.via = static_cast<uint64_t>(d); }

inline void unpack_uint32(uint32_t d, object& o)
{ o.via = static_cast<uint64_t>(d); }

inline void unpack_uint64(uint64_t d, object& o)
{ o.via = static_cast<uint64_t>(d); }

inline void unpack_int8(int8_t d, object& o)
{ if(d >= 0) { o.via = static_cast<uint64_t>(d); }
		else { o.via = static_cast<int64_t>(d);  } }

inline void unpack_int16(int16_t d, object& o)
{ if(d >= 0) { o.via = static_cast<uint64_t>(d); }
		else { o.via = static_cast<int64_t>(d);  } }

inline void unpack_int32(int32_t d, object& o)
{ if(d >= 0) { o.via = static_cast<uint64_t>(d); }
		else { o.via = static_cast<int64_t>(d);  } }

inline void unpack_int64(int64_t d, object& o)
{ if(d >= 0) { o.via = static_cast<uint64_t>(d); }
		else { o.via = static_cast<int64_t>(d);  } }

inline void unpack_float(float d, object& o)
{ o.via = static_cast<double>(d); }

inline void unpack_double(double d, object& o)
{ o.via = static_cast<double>(d); }

inline void unpack_nil(object& o)
{ o.via = type::nil(); }

inline void unpack_true(object& o)
{ o.via = true; }

inline void unpack_false(object& o)
{ o.via = false; }

struct unpack_array {
	void operator()(unsigned int n, object& o) const {
		object_array oa;
		oa.reserve(n);
		o.via = std::move(oa);
	}
};

inline void unpack_array_item(object_array& oa, object&& o)
{ oa.emplace_back(std::move(o)); }

struct unpack_map {
	void operator()(unsigned int n, object& o) const {
		object_map om;
		om.reserve(n);
		o.via = std::move(om);
	}
};

inline void unpack_map_item(object_map& om, object&& k, object&& v)
{
	om.emplace_back(std::move(k), std::move(v));
}

inline void unpack_str(std::shared_ptr<const char> const&  b, const char* p, unsigned int l, object& o)
{
	object_str os;
	os.ptr = std::shared_ptr<const char>(b, p);
	os.size = l;
	o.via = std::move(os);
}

inline void unpack_bin(std::shared_ptr<const char> const&  b, const char* p, unsigned int l, object& o)
{
	object_bin ob;
	ob.ptr = std::shared_ptr<const char>(b, p);
	ob.size = l;
	o.via = std::move(ob);
}


class unpack_stack {
public:
	object const& obj() const { return obj_; }
	object& obj() { return obj_; }
	void set_obj(object&& obj) { obj_ = std::move(obj); }
	size_t count() const { return count_; }
	void set_count(size_t count) { count_ = count; }
	size_t decl_count() { return --count_; }
	unsigned int ct() const { return ct_; }
	void set_ct(unsigned int ct) { ct_ = ct; }
	object const& map_key() const { return map_key_; }
	object& map_key() { return map_key_; }
	void set_map_key(object&& map_key) { map_key_ = std::move(map_key); }
private:
	object obj_;
	size_t count_;
	unsigned int ct_;
	object map_key_;
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

class context {
public:
	context():trail_(0), cs_(CS_HEADER), top_(0)
	{
		stack_[0].set_obj(object());
	}

	void init()
	{
		cs_ = CS_HEADER;
		trail_ = 0;
		top_ = 0;
		stack_[0].set_obj(object());
	}

	object data() const
	{
		return stack_[0].obj();
	}

	int execute(std::shared_ptr<const char> const& data, size_t len, size_t& off)
	{
		assert(len >= off);

		const char* current = data.get();
		const char* p = current + off;
		const char* const pe = current + len;
		const char* n = nullptr;

		// to support register optimization
		unsigned int trail = trail_;

		object obj;
		unpack_stack* c = nullptr;

		if(p == pe) {
			off = update_attributes(p, current, trail);
			return 0;
		}
		bool fixed_trail_again = false;
		do {
			if (cs_ == CS_HEADER) {
				fixed_trail_again = false;
				int selector = *reinterpret_cast<const unsigned char*>(p);
				if (0) {
				} else if(0x00 <= selector && selector <= 0x7f) { // Positive Fixnum
					unpack_uint8(*reinterpret_cast<const uint8_t*>(p), obj);
					int ret = push_proc(c, obj, p, current, off, trail);
					if (ret != 0) return ret;
				} else if(0xe0 <= selector && selector <= 0xff) { // Negative Fixnum
					unpack_int8(*reinterpret_cast<const int8_t*>(p), obj);
					int ret = push_proc(c, obj, p, current, off, trail);
					if (ret != 0) return ret;
				} else if(0xc0 <= selector && selector <= 0xdf) { // Variable
					switch(selector) {
					case 0xc0: {	// nil
						unpack_nil(obj);
						int ret = push_proc(c, obj, p, current, off, trail);
						if (ret != 0) return ret;
					} break;
					//case 0xc1:  // string
					case 0xc2: {	// false
						unpack_false(obj);
						int ret = push_proc(c, obj, p, current, off, trail);
						if (ret != 0) return ret;
					} break;
					case 0xc3: {	// true
						unpack_true(obj);
						int ret = push_proc(c, obj, p, current, off, trail);
						if (ret != 0) return ret;
					} break;
					case 0xc4: // bin 8
					case 0xc5: // bin 16
					case 0xc6: // bin 32
						trail = 1 << (static_cast<unsigned int>(*p) & 0x03);
						cs_ = next_cs(p);
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
						trail = 1 << (static_cast<unsigned int>(*p) & 0x03);
						cs_ = next_cs(p);
						fixed_trail_again = true;
						break;
					//case 0xd4:
					//case 0xd5:
					//case 0xd6:  // big integer 16
					//case 0xd7:  // big integer 32
					//case 0xd8:  // big float 16
					case 0xd9:	// str 8
					case 0xda:	// str 16
					case 0xdb:	// str 32
						trail = 1 << ((static_cast<unsigned int>(*p) & 0x03) - 1);
						cs_ = next_cs(p);
						fixed_trail_again = true;
						break;
					case 0xdc:	// array 16
					case 0xdd:	// array 32
					case 0xde:	// map 16
					case 0xdf:	// map 32
						trail = 2 << (static_cast<unsigned int>(*p) & 0x01);
						cs_ = next_cs(p);
						fixed_trail_again = true;
						break;
					default:
						off = update_attributes(p, current, trail);
						return -1;
					}
				} else if(0xa0 <= selector && selector <= 0xbf) { // FixStr
					trail = static_cast<unsigned int>(*p) & 0x1f;
					if(trail == 0) {
						unpack_str(data, n, trail, obj);
						int ret = push_proc(c, obj, p, current, off, trail);
						if (ret != 0) return ret;
					}
					cs_ = ACS_STR_VALUE;
					fixed_trail_again = true;

				} else if(0x90 <= selector && selector <= 0x9f) { // FixArray
					int ret = push_aggregate<fix_tag>(
						unpack_array(), CT_ARRAY_ITEM, c, obj, p, p, current, off, trail);
					if (ret != 0) return ret;
				} else if(0x80 <= selector && selector <= 0x8f) { // FixMap
					int ret = push_aggregate<fix_tag>(
						unpack_map(), CT_MAP_KEY, c, obj, p, p, current, off, trail);
					if (ret != 0) return ret;
				} else {
					off = update_attributes(p, current, trail);
					return -1;
				}
				// end CS_HEADER
			}
			if (cs_ != CS_HEADER || fixed_trail_again) {
				if (fixed_trail_again) {
					++p;
					fixed_trail_again = false;
				}
				if((size_t)(pe - p) < trail) {
					off = update_attributes(p, current, trail);
					return 0;
				}
				n = p;
				p += trail - 1;
				switch(cs_) {
				//case CS_
				//case CS_
				case CS_FLOAT: {
					union { uint32_t i; float f; } mem;
					mem.i = load<uint32_t>(n);
					unpack_float(mem.f, obj);
					int ret = push_proc(c, obj, p, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case CS_DOUBLE: {
					union { uint64_t i; double f; } mem;
					mem.i = load<uint64_t>(n);
#if defined(__arm__) && !(__ARM_EABI__) // arm-oabi
					// https://github.com/msgpack/msgpack-perl/pull/1
					mem.i = (mem.i & 0xFFFFFFFFUL) << 32UL | (mem.i >> 32UL);
#endif
					unpack_double(mem.f, obj);
					int ret = push_proc(c, obj, p, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case CS_UINT_8: {
					unpack_uint8(load<uint8_t>(n), obj);
					int ret = push_proc(c, obj, p, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case CS_UINT_16: {
					unpack_uint16(load<uint16_t>(n), obj);
					int ret = push_proc(c, obj, p, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case CS_UINT_32: {
					unpack_uint32(load<uint32_t>(n), obj);
					int ret = push_proc(c, obj, p, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case CS_UINT_64: {
					unpack_uint64(load<uint64_t>(n), obj);
					int ret = push_proc(c, obj, p, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case CS_INT_8: {
					unpack_int8(load<uint8_t>(n), obj);
					int ret = push_proc(c, obj, p, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case CS_INT_16: {
					unpack_int16(load<int16_t>(n), obj);
					int ret = push_proc(c, obj, p, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case CS_INT_32: {
					unpack_int32(load<int32_t>(n), obj);
					int ret = push_proc(c, obj, p, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case CS_INT_64: {
					unpack_int64(load<int64_t>(n), obj);
					int ret = push_proc(c, obj, p, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case CS_STR_8:
					trail = load<uint8_t>(n);
					if(trail == 0) {
						unpack_str(data, n, trail, obj);
						int ret = push_proc(c, obj, p, current, off, trail);
						if (ret != 0) return ret;
					}
					else {
						cs_ = ACS_STR_VALUE;
						fixed_trail_again = true;
					}
					break;
				case CS_BIN_8:
					trail = load<uint8_t>(n);
					if(trail == 0) {
						unpack_bin(data, n, trail, obj);
						int ret = push_proc(c, obj, p, current, off, trail);
						if (ret != 0) return ret;
					}
					else {
						cs_ = ACS_BIN_VALUE;
						fixed_trail_again = true;
					}
					break;
				case CS_STR_16:
					trail = load<uint16_t>(n);
					if(trail == 0) {
						unpack_str(data, n, trail, obj);
						int ret = push_proc(c, obj, p, current, off, trail);
						if (ret != 0) return ret;
					}
					else {
						cs_ = ACS_STR_VALUE;
						fixed_trail_again = true;
					}
					break;
				case CS_BIN_16:
					trail = load<uint16_t>(n);
					if(trail == 0) {
						unpack_bin(data, n, trail, obj);
						int ret = push_proc(c, obj, p, current, off, trail);
						if (ret != 0) return ret;
					}
					else {
						cs_ = ACS_BIN_VALUE;
						fixed_trail_again = true;
					}
					break;
				case CS_STR_32:
					trail = load<uint32_t>(n);
					if(trail == 0) {
						unpack_str(data, n, trail, obj);
						int ret = push_proc(c, obj, p, current, off, trail);
						if (ret != 0) return ret;
					}
					else {
						cs_ = ACS_STR_VALUE;
						fixed_trail_again = true;
					}
					break;
				case CS_BIN_32:
					trail = load<uint32_t>(n);
					if(trail == 0) {
						unpack_bin(data, n, trail, obj);
						int ret = push_proc(c, obj, p, current, off, trail);
						if (ret != 0) return ret;
					}
					else {
						cs_ = ACS_BIN_VALUE;
						fixed_trail_again = true;
					}
					break;
				case ACS_STR_VALUE: {
					unpack_str(data, n, trail, obj);
					int ret = push_proc(c, obj, p, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case ACS_BIN_VALUE: {
					unpack_bin(data, n, trail, obj);
					int ret = push_proc(c, obj, p, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case CS_ARRAY_16: {
					int ret = push_aggregate<uint16_t>(
						unpack_array(), CT_ARRAY_ITEM, c, obj, p, n, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case CS_ARRAY_32: {
					/* FIXME security guard */
					int ret = push_aggregate<uint32_t>(
						unpack_array(), CT_ARRAY_ITEM, c, obj, p, n, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case CS_MAP_16: {
					int ret = push_aggregate<uint16_t>(
						unpack_map(), CT_MAP_KEY, c, obj, p, n, current, off, trail);
					if (ret != 0) return ret;
				} break;
				case CS_MAP_32: {
					/* FIXME security guard */
					int ret = push_aggregate<uint32_t>(
						unpack_map(), CT_MAP_KEY, c, obj, p, n, current, off, trail);
					if (ret != 0) return ret;
				} break;
				default:
					off = update_attributes(p, current, trail);
					return -1;
				}
			}
		} while(p != pe);

		off = update_attributes(p, current, trail);
		return 0;
	}

private:
	template <typename T>
	static unsigned int next_cs(T p)
	{
		return static_cast<unsigned int>(*p) & 0x1f;
	}

	template <typename T, typename Func>
	int push_aggregate(
		Func const& f,
		unsigned int ct,
		unpack_stack*& c,
		object& obj,
		const char*& current,
		const char* load_pos,
		const char* origin,
		size_t& off,
		unsigned int trail) {
		if(top_ < MSGPACK_EMBED_STACK_SIZE) { /* FIXME */
			f(load<T>(load_pos), stack_[top_].obj());
			if(load<T>(load_pos) == 0) {
				obj = stack_[top_].obj();
				int ret = push_proc(c, obj, current, origin, off, trail);
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
			off = update_attributes(current, origin, trail);
			return -1;
		}
		return 0;
	}

	size_t update_attributes(
		const char* current,
		const char* origin,
		unsigned int trail) {
		trail_ = trail;
		return current - origin;
	}

	void header_again(const char*& current) {
		cs_ = CS_HEADER;
		++current;
	}
	int push_item(unpack_stack*& c, object& obj) {
		bool finish = false;
		while (!finish) {
			if(top_ == 0) {
				return 1;
			}
			c = &stack_[top_ - 1];
			switch(c->ct()) {
			case CT_ARRAY_ITEM: {
				object_array& oa(boost::get<object_array>(c->obj().via));
				unpack_array_item(oa, std::move(obj));
				if(c->decl_count() == 0) {
					obj = std::move(c->obj());
					--top_;
					/*printf("stack pop %d\n", top_);*/
				}
				else {
					finish = true;
				}
			} break;
			case CT_MAP_KEY:
				c->set_map_key(std::move(obj));
				c->set_ct(CT_MAP_VALUE);
				finish = true;
				break;
			case CT_MAP_VALUE: {
				object_map& om(boost::get<object_map>(c->obj().via));
				unpack_map_item(om, std::move(c->map_key()), std::move(obj));
				if(c->decl_count() == 0) {
					obj = std::move(c->obj());
					--top_;
					/*printf("stack pop %d\n", top_);*/
				}
				else {
					c->set_ct(CT_MAP_KEY);
					finish = true;
				}
			} break;
			default:
				return -1;
			}
		}
		return 0;
	}

	int push_proc(
		unpack_stack*& c,
		object& obj,
		const char*& current,
		const char* origin,
		size_t& off,
		unsigned int trail) {
		int ret = push_item(c, obj);
		if (ret > 0) {
			stack_[0].set_obj(std::move(obj));
			++current;
			/*printf("-- finish --\n"); */
			off = update_attributes(current, origin, trail);
		}
		else if (ret < 0) {
			off = update_attributes(current, origin, trail);
		}
		else {
			header_again(current);
		}
		return ret;
	}

private:
	unsigned int trail_;
	unsigned int cs_;
	unsigned int top_;
	unpack_stack stack_[MSGPACK_EMBED_STACK_SIZE];
};

} // detail


struct unpack_error : public std::runtime_error {
	unpack_error(const std::string& msg) :
		std::runtime_error(msg) { }
};


class unpacked {
public:
	unpacked() { }

	unpacked(object&& obj) :
		m_obj(std::move(obj)) { }

	void set(object&& obj)
		{ m_obj = std::move(obj); }

	object get()
		{ return m_obj; }

private:
	object m_obj;
};


class unpacker {
	class deleter {
	public:
		deleter():delete_(true) {}
		void operator()(char *p) {
			if (delete_) free(p);
		}
		void no_delete() {
			delete_ = false;
		}
	private:
		bool delete_;
	};
public:
	unpacker(size_t init_buffer_size = MSGPACK_UNPACKER_INIT_BUFFER_SIZE);

public:
	/*! 1. reserve buffer. at least `size' bytes of capacity will be ready */
	void reserve_buffer(size_t size = MSGPACK_UNPACKER_RESERVE_SIZE);

	/*! 2. read data to the buffer() up to buffer_capacity() bytes */
	char* buffer();
	size_t buffer_capacity() const;

	/*! 3. specify the number of bytes actually copied */
	void buffer_consumed(size_t size);

	/*! 4. repeat next() until it retunrs false */
	bool next(unpacked* result);

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
	object data() const;

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

private:
	std::shared_ptr<char> buffer_;
	size_t used_;
	size_t free_;
	size_t off_;
	size_t parsed_;
	size_t initial_buffer_size_;
	detail::context ctx_;

private:
	unpacker(const unpacker&);
};


inline void unpack(unpacked& result,
				   std::shared_ptr<const char> const& data,
				   size_t len,
				   size_t* offset = nullptr);
inline void unpack(unpacked* result,
				   std::shared_ptr<const char> const& data,
				   size_t len,
				   size_t* offset = nullptr);

// obsolete
typedef enum {
	UNPACK_SUCCESS				=  2,
	UNPACK_EXTRA_BYTES			=  1,
	UNPACK_CONTINUE			=  0,
	UNPACK_PARSE_ERROR			= -1
} unpack_return;

// obsolete
static unpack_return unpack(std::shared_ptr<const char> const& data,
							size_t len,
							size_t* off,
							object& result);
static unpack_return unpack(std::shared_ptr<const char> const& data,
							size_t len,
							size_t* off,
							object* result);


// obsolete
static object unpack(std::shared_ptr<const char> const& data,
					 size_t len,
					 size_t* off = nullptr);


inline unpacker::unpacker(size_t initial_buffer_size)
{
	char* buffer = reinterpret_cast<char*>(::malloc(initial_buffer_size));
	if(!buffer) {
		throw std::bad_alloc();
	}

	buffer_.reset(buffer, deleter());

	used_ = 0;
	free_ = initial_buffer_size - used_;
	off_ = 0;
	parsed_ = 0;
	initial_buffer_size_ = initial_buffer_size;

	ctx_.init();
}

inline void unpacker::reserve_buffer(size_t size)
{
	if(free_ >= size) return;
	expand_buffer(size);
}

inline void unpacker::expand_buffer(size_t size)
{
	if (used_ == off_ && buffer_.use_count() == 1) {
		// rewind buffer
		free_ += used_;
		used_ = 0;
		off_  = 0;

		if(free_ >= size) return;
	}

	if(off_ == 0) {
		size_t next_size = (used_ + free_) * 2;
		while(next_size < size + used_) {
			next_size *= 2;
		}

		char* current = buffer_.get();
		char* tmp = reinterpret_cast<char*>(::realloc(current, next_size));
		if(!tmp) {
			throw std::bad_alloc();
		}
		if (current != tmp) {
			std::get_deleter<deleter>(buffer_)->no_delete();
			buffer_.reset(tmp, deleter());
		}

		free_ = next_size - used_;

	} else {
		size_t next_size = initial_buffer_size_;
		size_t not_parsed = used_ - off_;
		while(next_size < size + not_parsed) {
			next_size *= 2;
		}

		char* tmp = reinterpret_cast<char*>(::malloc(next_size));
		if(!tmp) {
			throw std::bad_alloc();
		}
		::memcpy(tmp, buffer_.get() + off_, not_parsed);
		buffer_.reset(tmp, deleter());

		used_	= not_parsed;
		free_	= next_size - used_;
		off_	= 0;
	}
}

inline char* unpacker::buffer()
{
	return buffer_.get() + used_;
}

inline size_t unpacker::buffer_capacity() const
{
	return free_;
}

inline void unpacker::buffer_consumed(size_t size)
{
	used_ += size;
	free_ -= size;
}

inline bool unpacker::next(unpacked* result)
{
	int ret = execute_imp();

	if(ret < 0) {
		throw unpack_error("parse error");
	}

	if(ret == 0) {
		result->set(object());
		return false;

	} else {
		result->set(data());
		reset();
		return true;
	}
}


inline bool unpacker::execute()
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

inline int unpacker::execute_imp()
{
	size_t off = off_;
	int ret = ctx_.execute(buffer_, used_, off_);
	if(off_ > off) {
		parsed_ += off_ - off;
	}
	return ret;
}

inline object unpacker::data() const
{
	return ctx_.data();
}

inline void unpacker::reset()
{
	ctx_.init();
	// don't reset referenced flag
	parsed_ = 0;
}

inline size_t unpacker::message_size() const
{
	return parsed_ - off_ + used_;
}

inline size_t unpacker::parsed_size() const
{
	return parsed_;
}

inline char* unpacker::nonparsed_buffer()
{
	return buffer_.get() + off_;
}

inline size_t unpacker::nonparsed_size() const
{
	return used_ - off_;
}

inline void unpacker::skip_nonparsed_buffer(size_t size)
{
	off_ += size;
}

inline void unpacker::remove_nonparsed_buffer()
{
	used_ = off_;
}

namespace detail {

inline unpack_return
unpack_imp(std::shared_ptr<const char> const& data, size_t len, size_t* off,
	object& result)
{
	size_t noff = 0;
	if(off != nullptr) { noff = *off; }

	if(len <= noff) {
		// FIXME
		return UNPACK_CONTINUE;
	}

	detail::context ctx;
	ctx.init();

	int e = ctx.execute(data, len, noff);
	if(e < 0) {
		return UNPACK_PARSE_ERROR;
	}

	if(off != nullptr) { *off = noff; }

	if(e == 0) {
		return UNPACK_CONTINUE;
	}

	result = ctx.data();

	if(noff < len) {
		return UNPACK_EXTRA_BYTES;
	}

	return UNPACK_SUCCESS;
}

} // detail

// reference version
inline void unpack(unpacked& result,
				   std::shared_ptr<const char> const& data, size_t len, size_t* offset)
{
	object obj;
	unpack_return ret = detail::unpack_imp(
			data, len, offset, obj);


	switch(ret) {
	case UNPACK_SUCCESS:
		result.set(std::move(obj));
		return;

	case UNPACK_EXTRA_BYTES:
		result.set(std::move(obj));
		return;

	case UNPACK_CONTINUE:
		throw unpack_error("insufficient bytes");

	case UNPACK_PARSE_ERROR:
	default:
		throw unpack_error("parse error");
	}
}
// pointer version
inline void unpack(unpacked* result,
				   std::shared_ptr<const char> const& data, size_t len, size_t* offset) {
	unpack(*result, data, len, offset);
}


// obsolete
// reference version
inline unpack_return unpack(std::shared_ptr<const char> const& data, size_t len, size_t* off,
							object& result)
{
	return detail::unpack_imp(data, len, off,
			result);
}
// pointer version
inline unpack_return unpack(std::shared_ptr<const char> const& data, size_t len, size_t* off,
							object* result)
{
	return unpack(data, len, off, *result);
}

// obsolete
inline object unpack(std::shared_ptr<const char> const& data, size_t len, size_t* off)
{
	object result;

	switch( unpack(data, len, off, result) ) {
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

}  // namespace msgpack

#endif /* msgpack/unpack.hpp */

