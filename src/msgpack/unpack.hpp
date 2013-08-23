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

#include <memory>
#include <stdexcept>


#define CTX_CAST(m) ((detail::template_context*)(m))
#define CTX_REFERENCED(mpac) CTX_CAST((mpac)->ctx_)->user.referenced

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


#define msgpack_unpack_callback(name) \
	template_callback ## name

#define msgpack_unpack_user unpack_user

namespace msgpack {

namespace detail {

struct unpack_user {
	zone* z;
	bool referenced;
};


static inline ::msgpack::object template_callback_root(unpack_user* u)
{ ::msgpack::object o = {}; return o; }

static inline int template_callback_uint8(unpack_user* u, uint8_t d, ::msgpack::object* o)
{ o->type = ::msgpack::type::POSITIVE_INTEGER; o->via.u64 = d; return 0; }

static inline int template_callback_uint16(unpack_user* u, uint16_t d, ::msgpack::object* o)
{ o->type = ::msgpack::type::POSITIVE_INTEGER; o->via.u64 = d; return 0; }

static inline int template_callback_uint32(unpack_user* u, uint32_t d, ::msgpack::object* o)
{ o->type = ::msgpack::type::POSITIVE_INTEGER; o->via.u64 = d; return 0; }

static inline int template_callback_uint64(unpack_user* u, uint64_t d, ::msgpack::object* o)
{ o->type = ::msgpack::type::POSITIVE_INTEGER; o->via.u64 = d; return 0; }

static inline int template_callback_int8(unpack_user* u, int8_t d, ::msgpack::object* o)
{ if(d >= 0) { o->type = ::msgpack::type::POSITIVE_INTEGER; o->via.u64 = d; return 0; }
		else { o->type = ::msgpack::type::NEGATIVE_INTEGER; o->via.i64 = d; return 0; } }

static inline int template_callback_int16(unpack_user* u, int16_t d, ::msgpack::object* o)
{ if(d >= 0) { o->type = ::msgpack::type::POSITIVE_INTEGER; o->via.u64 = d; return 0; }
		else { o->type = ::msgpack::type::NEGATIVE_INTEGER; o->via.i64 = d; return 0; } }

static inline int template_callback_int32(unpack_user* u, int32_t d, ::msgpack::object* o)
{ if(d >= 0) { o->type = ::msgpack::type::POSITIVE_INTEGER; o->via.u64 = d; return 0; }
		else { o->type = ::msgpack::type::NEGATIVE_INTEGER; o->via.i64 = d; return 0; } }

static inline int template_callback_int64(unpack_user* u, int64_t d, ::msgpack::object* o)
{ if(d >= 0) { o->type = ::msgpack::type::POSITIVE_INTEGER; o->via.u64 = d; return 0; }
		else { o->type = ::msgpack::type::NEGATIVE_INTEGER; o->via.i64 = d; return 0; } }

static inline int template_callback_float(unpack_user* u, float d, ::msgpack::object* o)
{ o->type = ::msgpack::type::DOUBLE; o->via.dec = d; return 0; }

static inline int template_callback_double(unpack_user* u, double d, ::msgpack::object* o)
{ o->type = ::msgpack::type::DOUBLE; o->via.dec = d; return 0; }

static inline int template_callback_nil(unpack_user* u, ::msgpack::object* o)
{ o->type = ::msgpack::type::NIL; return 0; }

static inline int template_callback_true(unpack_user* u, ::msgpack::object* o)
{ o->type = ::msgpack::type::BOOLEAN; o->via.boolean = true; return 0; }

static inline int template_callback_false(unpack_user* u, ::msgpack::object* o)
{ o->type = ::msgpack::type::BOOLEAN; o->via.boolean = false; return 0; }

static inline int template_callback_array(unpack_user* u, unsigned int n, ::msgpack::object* o)
{
	o->type = ::msgpack::type::ARRAY;
	o->via.array.size = 0;
	o->via.array.ptr = (::msgpack::object*)u->z->malloc(n*sizeof(::msgpack::object));
	if(o->via.array.ptr == NULL) { return -1; }
	return 0;
}

static inline int template_callback_array_item(unpack_user* u, ::msgpack::object* c, ::msgpack::object o)
{ c->via.array.ptr[c->via.array.size++] = o; return 0; }

static inline int template_callback_map(unpack_user* u, unsigned int n, ::msgpack::object* o)
{
	o->type = ::msgpack::type::MAP;
	o->via.map.size = 0;
	o->via.map.ptr = (::msgpack::object_kv*)u->z->malloc(n*sizeof(::msgpack::object_kv));
	if(o->via.map.ptr == NULL) { return -1; }
	return 0;
}

static inline int template_callback_map_item(unpack_user* u, ::msgpack::object* c, ::msgpack::object k, ::msgpack::object v)
{
	c->via.map.ptr[c->via.map.size].key = k;
	c->via.map.ptr[c->via.map.size].val = v;
	++c->via.map.size;
	return 0;
}

static inline int template_callback_raw(unpack_user* u, const char* b, const char* p, unsigned int l, ::msgpack::object* o)
{
	o->type = ::msgpack::type::RAW;
	o->via.raw.ptr = p;
	o->via.raw.size = l;
	u->referenced = true;
	return 0;
}


struct template_unpack_stack {
	::msgpack::object obj;
	size_t count;
	unsigned int ct;
	::msgpack::object map_key;
};

struct template_context {
	msgpack_unpack_user user;
	unsigned int cs;
	unsigned int trail;
	unsigned int top;
	/*
	msgpack_unpack_struct(_stack)* stack;
	unsigned int stack_size;
	msgpack_unpack_struct(_stack) embed_stack[MSGPACK_EMBED_STACK_SIZE];
	*/
	template_unpack_stack stack[MSGPACK_EMBED_STACK_SIZE];
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

inline void template_init(template_context* ctx)
{
	ctx->cs = CS_HEADER;
	ctx->trail = 0;
	ctx->top = 0;
	/*
	ctx->stack = ctx->embed_stack;
	ctx->stack_size = MSGPACK_EMBED_STACK_SIZE;
	*/
	ctx->stack[0].obj = msgpack_unpack_callback(_root)(&ctx->user);
}

::msgpack::object template_data(template_context* ctx)
{
	return (ctx)->stack[0].obj;
}


int template_execute(template_context* ctx, const char* data, size_t len, size_t* off)
{
	assert(len >= *off);

	const unsigned char* p = (unsigned char*)data + *off;
	const unsigned char* const pe = (unsigned char*)data + len;
	const void* n = nullptr;

	unsigned int trail = ctx->trail;
	unsigned int cs = ctx->cs;
	unsigned int top = ctx->top;
	detail::template_unpack_stack* stack = ctx->stack;
	/*
	unsigned int stack_size = ctx->stack_size;
	*/
	msgpack_unpack_user* user = &ctx->user;

	::msgpack::object obj;
	detail::template_unpack_stack* c = nullptr;

	int ret;

#define push_simple_value(func) \
	if(msgpack_unpack_callback(func)(user, &obj) < 0) { goto _failed; } \
	goto _push
#define push_fixed_value(func, arg) \
	if(msgpack_unpack_callback(func)(user, arg, &obj) < 0) { goto _failed; } \
	goto _push
#define push_variable_value(func, base, pos, len) \
	if(msgpack_unpack_callback(func)(user, \
		(const char*)base, (const char*)pos, len, &obj) < 0) { goto _failed; } \
	goto _push

#define again_fixed_trail(_cs, trail_len) \
	trail = trail_len; \
	cs = _cs; \
	goto _fixed_trail_again
#define again_fixed_trail_if_zero(_cs, trail_len, ifzero) \
	trail = trail_len; \
	if(trail == 0) { goto ifzero; } \
	cs = _cs; \
	goto _fixed_trail_again

#define start_container(func, count_, ct_) \
	if(top >= MSGPACK_EMBED_STACK_SIZE) { goto _failed; } /* FIXME */ \
	if(msgpack_unpack_callback(func)(user, count_, &stack[top].obj) < 0) { goto _failed; } \
	if((count_) == 0) { obj = stack[top].obj; goto _push; } \
	stack[top].ct = ct_; \
	stack[top].count = count_; \
	++top; \
	/*printf("container %d count %d stack %d\n",stack[top].obj,count_,top);*/ \
	/*printf("stack push %d\n", top);*/ \
	/* FIXME \
	if(top >= stack_size) { \
		if(stack_size == MSGPACK_EMBED_STACK_SIZE) { \
			size_t csize = sizeof(msgpack_unpack_struct(_stack)) * MSGPACK_EMBED_STACK_SIZE; \
			size_t nsize = csize * 2; \
			msgpack_unpack_struct(_stack)* tmp = (msgpack_unpack_struct(_stack)*)malloc(nsize); \
			if(tmp == NULL) { goto _failed; } \
			memcpy(tmp, ctx->stack, csize); \
			ctx->stack = stack = tmp; \
			ctx->stack_size = stack_size = MSGPACK_EMBED_STACK_SIZE * 2; \
		} else { \
			size_t nsize = sizeof(msgpack_unpack_struct(_stack)) * ctx->stack_size * 2; \
			msgpack_unpack_struct(_stack)* tmp = (msgpack_unpack_struct(_stack)*)realloc(ctx->stack, nsize); \
			if(tmp == NULL) { goto _failed; } \
			ctx->stack = stack = tmp; \
			ctx->stack_size = stack_size = stack_size * 2; \
		} \
	} \
	*/ \
	goto _header_again

#define NEXT_CS(p) \
	((unsigned int)*p & 0x1f)

#ifdef USE_CASE_RANGE
#define SWITCH_RANGE_BEGIN	   switch(*p) {
#define SWITCH_RANGE(FROM, TO) case FROM ... TO:
#define SWITCH_RANGE_DEFAULT   default:
#define SWITCH_RANGE_END	   }
#else
#define SWITCH_RANGE_BEGIN	   { if(0) {
#define SWITCH_RANGE(FROM, TO) } else if(FROM <= *p && *p <= TO) {
#define SWITCH_RANGE_DEFAULT   } else {
#define SWITCH_RANGE_END	   } }
#endif

	if(p == pe) { goto _out; }
	do {
		switch(cs) {
		case CS_HEADER:
			SWITCH_RANGE_BEGIN
			SWITCH_RANGE(0x00, 0x7f)  // Positive Fixnum
				push_fixed_value(_uint8, *(uint8_t*)p);
			SWITCH_RANGE(0xe0, 0xff)  // Negative Fixnum
				push_fixed_value(_int8, *(int8_t*)p);
			SWITCH_RANGE(0xc0, 0xdf)  // Variable
				switch(*p) {
				case 0xc0:	// nil
					push_simple_value(_nil);
				//case 0xc1:  // string
				//	again_terminal_trail(NEXT_CS(p), p+1);
				case 0xc2:	// false
					push_simple_value(_false);
				case 0xc3:	// true
					push_simple_value(_true);
				case 0xc4: // bin 8
				case 0xc5: // bin 16
				case 0xc6: // bin 32
					again_fixed_trail(NEXT_CS(p), 1 << (((unsigned int)*p) & 0x03));
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
					again_fixed_trail(NEXT_CS(p), 1 << (((unsigned int)*p) & 0x03));
				//case 0xd4:
				//case 0xd5:
				//case 0xd6:  // big integer 16
				//case 0xd7:  // big integer 32
				//case 0xd8:  // big float 16
				case 0xd9:  // raw 8 (str 8)
				case 0xda:  // raw 16 (str 16)
				case 0xdb:  // raw 32 (str 32)
					again_fixed_trail(NEXT_CS(p), 1 << ((((unsigned int)*p) & 0x03) - 1));
				case 0xdc:	// array 16
				case 0xdd:	// array 32
				case 0xde:	// map 16
				case 0xdf:	// map 32
					again_fixed_trail(NEXT_CS(p), 2 << (((unsigned int)*p) & 0x01));
				default:
					goto _failed;
				}
			SWITCH_RANGE(0xa0, 0xbf)  // FixRaw
				again_fixed_trail_if_zero(ACS_RAW_VALUE, ((unsigned int)*p & 0x1f), _raw_zero);
			SWITCH_RANGE(0x90, 0x9f)  // FixArray
				start_container(_array, ((unsigned int)*p) & 0x0f, CT_ARRAY_ITEM);
			SWITCH_RANGE(0x80, 0x8f)  // FixMap
				start_container(_map, ((unsigned int)*p) & 0x0f, CT_MAP_KEY);

			SWITCH_RANGE_DEFAULT
				goto _failed;
			SWITCH_RANGE_END
			// end CS_HEADER


		_fixed_trail_again:
			++p;

		default:
			if((size_t)(pe - p) < trail) { goto _out; }
			n = p;	p += trail - 1;
			switch(cs) {
			//case CS_
			//case CS_
			case CS_FLOAT: {
					union { uint32_t i; float f; } mem;
					mem.i = _msgpack_load32(uint32_t,n);
					push_fixed_value(_float, mem.f); }
			case CS_DOUBLE: {
					union { uint64_t i; double f; } mem;
					mem.i = _msgpack_load64(uint64_t,n);
#if defined(__arm__) && !(__ARM_EABI__) // arm-oabi
					// https://github.com/msgpack/msgpack-perl/pull/1
					mem.i = (mem.i & 0xFFFFFFFFUL) << 32UL | (mem.i >> 32UL);
#endif
					push_fixed_value(_double, mem.f); }
			case CS_UINT_8:
				push_fixed_value(_uint8, *(uint8_t*)n);
			case CS_UINT_16:
				push_fixed_value(_uint16, _msgpack_load16(uint16_t,n));
			case CS_UINT_32:
				push_fixed_value(_uint32, _msgpack_load32(uint32_t,n));
			case CS_UINT_64:
				push_fixed_value(_uint64, _msgpack_load64(uint64_t,n));

			case CS_INT_8:
				push_fixed_value(_int8, *(int8_t*)n);
			case CS_INT_16:
				push_fixed_value(_int16, _msgpack_load16(int16_t,n));
			case CS_INT_32:
				push_fixed_value(_int32, _msgpack_load32(int32_t,n));
			case CS_INT_64:
				push_fixed_value(_int64, _msgpack_load64(int64_t,n));

			//case CS_
			//case CS_
			//case CS_BIG_INT_16:
			//	again_fixed_trail_if_zero(ACS_BIG_INT_VALUE, _msgpack_load16(uint16_t,n), _big_int_zero);
			//case CS_BIG_INT_32:
			//	again_fixed_trail_if_zero(ACS_BIG_INT_VALUE, _msgpack_load32(uint32_t,n), _big_int_zero);
			//case ACS_BIG_INT_VALUE:
			//_big_int_zero:
			//	// FIXME
			//	push_variable_value(_big_int, data, n, trail);

			//case CS_BIG_FLOAT_16:
			//	again_fixed_trail_if_zero(ACS_BIG_FLOAT_VALUE, _msgpack_load16(uint16_t,n), _big_float_zero);
			//case CS_BIG_FLOAT_32:
			//	again_fixed_trail_if_zero(ACS_BIG_FLOAT_VALUE, _msgpack_load32(uint32_t,n), _big_float_zero);
			//case ACS_BIG_FLOAT_VALUE:
			//_big_float_zero:
			//	// FIXME
			//	push_variable_value(_big_float, data, n, trail);

			case CS_BIN_8:
			case CS_RAW_8:
				again_fixed_trail_if_zero(ACS_RAW_VALUE, *(uint8_t*)n, _raw_zero);
			case CS_BIN_16:
			case CS_RAW_16:
				again_fixed_trail_if_zero(ACS_RAW_VALUE, _msgpack_load16(uint16_t,n), _raw_zero);
			case CS_BIN_32:
			case CS_RAW_32:
				again_fixed_trail_if_zero(ACS_RAW_VALUE, _msgpack_load32(uint32_t,n), _raw_zero);
			case ACS_RAW_VALUE:
			_raw_zero:
				push_variable_value(_raw, data, n, trail);

			case CS_ARRAY_16:
				start_container(_array, _msgpack_load16(uint16_t,n), CT_ARRAY_ITEM);
			case CS_ARRAY_32:
				/* FIXME security guard */
				start_container(_array, _msgpack_load32(uint32_t,n), CT_ARRAY_ITEM);

			case CS_MAP_16:
				start_container(_map, _msgpack_load16(uint16_t,n), CT_MAP_KEY);
			case CS_MAP_32:
				/* FIXME security guard */
				start_container(_map, _msgpack_load32(uint32_t,n), CT_MAP_KEY);

			default:
				goto _failed;
			}
		}

_push:
	if(top == 0) { goto _finish; }
	c = &stack[top-1];
	switch(c->ct) {
	case CT_ARRAY_ITEM:
		if(msgpack_unpack_callback(_array_item)(user, &c->obj, obj) < 0) { goto _failed; }
		if(--c->count == 0) {
			obj = c->obj;
			--top;
			/*printf("stack pop %d\n", top);*/
			goto _push;
		}
		goto _header_again;
	case CT_MAP_KEY:
		c->map_key = obj;
		c->ct = CT_MAP_VALUE;
		goto _header_again;
	case CT_MAP_VALUE:
		if(msgpack_unpack_callback(_map_item)(user, &c->obj, c->map_key, obj) < 0) { goto _failed; }
		if(--c->count == 0) {
			obj = c->obj;
			--top;
			/*printf("stack pop %d\n", top);*/
			goto _push;
		}
		c->ct = CT_MAP_KEY;
		goto _header_again;

	default:
		goto _failed;
	}

_header_again:
		cs = CS_HEADER;
		++p;
	} while(p != pe);
	goto _out;


_finish:
	stack[0].obj = obj;
	++p;
	ret = 1;
	/*printf("-- finish --\n"); */
	goto _end;

_failed:
	/*printf("** FAILED **\n"); */
	ret = -1;
	goto _end;

_out:
	ret = 0;
	goto _end;

_end:
	ctx->cs = cs;
	ctx->trail = trail;
	ctx->top = top;
	*off = p - (const unsigned char*)data;

	return ret;
}


#undef msgpack_unpack_func
#undef msgpack_unpack_callback
#undef msgpack_unpack_struct
#undef msgpack_unpack_user

#undef push_simple_value
#undef push_fixed_value
#undef push_variable_value
#undef again_fixed_trail
#undef again_fixed_trail_if_zero
#undef start_container

#undef NEXT_CS


} // detail


struct unpack_error : public std::runtime_error {
	unpack_error(const std::string& msg) :
		std::runtime_error(msg) { }
};


class unpacked {
public:
	unpacked() { }

	unpacked(object obj, std::unique_ptr<msgpack::zone> z) :
		m_obj(obj), m_zone(std::move(z)) { }

	object& get()
		{ return m_obj; }

	const object& get() const
		{ return m_obj; }

	std::unique_ptr<msgpack::zone>& zone()
		{ return m_zone; }

	const std::unique_ptr<msgpack::zone>& zone() const
		{ return m_zone; }

private:
	object m_obj;
	std::unique_ptr<msgpack::zone> m_zone;
};


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
	bool next(unpacked* result);

	/*! 5. check if the size of message doesn't exceed assumption. */
	size_t message_size() const;

	// Basic usage of the unpacker is as following:
	//
	// msgpack::unpacker pac;
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
	//     msgpack::unpacked result;
	//     while(pac.next(&result)) {
	//         // do some with the object with the zone.
	//         msgpack::object obj = result.get();
	//         std::auto_ptr<msgpack:zone> z = result.zone();
	//         on_message(obj, z);
	//
	//         //// boost::shared_ptr is also usable:
	//         // boost::shared_ptr<msgpack::zone> life(z.release());
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
	object data();

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
	int	 execute_imp();
	bool flush_zone();

private:
	char* buffer_;
	size_t used_;
	size_t free_;
	size_t off_;
	size_t parsed_;
	zone* z_;
	size_t initial_buffer_size_;
	void* ctx_;

private:
	unpacker(const unpacker&);
};


static void unpack(unpacked* result,
		const char* data, size_t len, size_t* offset = NULL);


// obsolete
typedef enum {
	UNPACK_SUCCESS				=  2,
	UNPACK_EXTRA_BYTES			=  1,
	UNPACK_CONTINUE				=  0,
	UNPACK_PARSE_ERROR			= -1,
} unpack_return;

// obsolete
static unpack_return unpack(const char* data, size_t len, size_t* off,
		zone* z, object* result);


// obsolete
static object unpack(const char* data, size_t len, zone& z, size_t* off = NULL);


inline unpacker::unpacker(size_t initial_buffer_size)
{
	if(initial_buffer_size < COUNTER_SIZE) {
		initial_buffer_size = COUNTER_SIZE;
	}

	char* buffer = (char*)::malloc(initial_buffer_size);
	if(!buffer) {
		throw std::bad_alloc();
	}

	void* ctx = ::malloc(sizeof(detail::template_context));
	if(!ctx) {
		::free(buffer);
		throw std::bad_alloc();
	}

	zone* z = zone::create(MSGPACK_ZONE_CHUNK_SIZE);
	if(!z) {
		::free(ctx);
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
	ctx_ = ctx;

	detail::init_count(buffer_);

	detail::template_init(CTX_CAST(ctx_));
	CTX_CAST(ctx_)->user.z = z_;
	CTX_CAST(ctx_)->user.referenced = false;
}

inline unpacker::~unpacker()
{
	zone::destroy(z_);
	::free(ctx_);
	detail::decl_count(buffer_);
}


inline void unpacker::reserve_buffer(size_t size)
{
	if(free_ >= size) return;
	expand_buffer(size);
}

inline void unpacker::expand_buffer(size_t size)
{
	if(used_ == off_ && detail::get_count(buffer_) == 1
			&& !CTX_REFERENCED(this)) {
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

		char* tmp = (char*)::realloc(buffer_, next_size);
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

		char* tmp = (char*)::malloc(next_size);
		if(!tmp) {
			throw std::bad_alloc();
		}

		detail::init_count(tmp);

		::memcpy(tmp+COUNTER_SIZE, buffer_ + off_, not_parsed);

		if(CTX_REFERENCED(this)) {
			try {
				z_->push_finalizer(&detail::decl_count, buffer_);
			}
			catch (...) {
				::free(tmp);
				throw;
			}
			CTX_REFERENCED(this) = false;
		} else {
			detail::decl_count(buffer_);
		}

		buffer_ = tmp;
		used_	= not_parsed + COUNTER_SIZE;
		free_	= next_size - used_;
		off_	= COUNTER_SIZE;
	}
}

inline char* unpacker::buffer()
{
	return buffer_ + used_;
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
		if (result->zone().get() != NULL) result->zone().reset();
		result->get() = object();
		return false;

	} else {
		if (result->zone().get() != NULL) result->zone().reset( release_zone() );
		result->get() = data();
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
	int ret = detail::template_execute(CTX_CAST(ctx_),
			buffer_, used_, &off_);
	if(off_ > off) {
		parsed_ += off_ - off;
	}
	return ret;
}

inline object unpacker::data()
{
	return template_data(CTX_CAST(ctx_));
}

inline zone* unpacker::release_zone()
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
	CTX_CAST(ctx_)->user.z = z_;

	return old;
}

inline void unpacker::reset_zone()
{
	z_->clear();
}

inline bool unpacker::flush_zone()
{
	if(CTX_REFERENCED(this)) {
		try {
			z_->push_finalizer(&detail::decl_count, buffer_);
		} catch (...) {
			return false;
		}
		CTX_REFERENCED(this) = false;

		detail::incr_count(buffer_);
	}

	return true;
}

inline void unpacker::reset()
{
	detail::template_init(CTX_CAST(ctx_));
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
	return buffer_ + off_;
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
unpack_imp(const char* data, size_t len, size_t* off,
		zone* result_zone, ::msgpack::object* result)
{
	size_t noff = 0;
	if(off != NULL) { noff = *off; }

	if(len <= noff) {
		// FIXME
		return UNPACK_CONTINUE;
	}

	detail::template_context ctx;
	detail::template_init(&ctx);

	ctx.user.z = result_zone;
	ctx.user.referenced = false;

	int e = detail::template_execute(&ctx, data, len, &noff);
	if(e < 0) {
		return UNPACK_PARSE_ERROR;
	}

	if(off != NULL) { *off = noff; }

	if(e == 0) {
		return UNPACK_CONTINUE;
	}

	*result = detail::template_data(&ctx);

	if(noff < len) {
		return UNPACK_EXTRA_BYTES;
	}

	return UNPACK_SUCCESS;
}

} // detail

inline void unpack(unpacked* result,
		const char* data, size_t len, size_t* offset)
{
	msgpack::object obj;
	std::unique_ptr<msgpack::zone> z(new zone());

	unpack_return ret = detail::unpack_imp(
			data, len, offset, z.get(), &obj);


	switch(ret) {
	case UNPACK_SUCCESS:
		result->get() = obj;
		result->zone() = std::move(z);
		return;

	case UNPACK_EXTRA_BYTES:
		result->get() = obj;
		result->zone() = std::move(z);
		return;

	case UNPACK_CONTINUE:
		throw unpack_error("insufficient bytes");

	case UNPACK_PARSE_ERROR:
	default:
		throw unpack_error("parse error");
	}
}


// obsolete
inline unpack_return unpack(const char* data, size_t len, size_t* off,
		zone* z, object* result)
{
	return detail::unpack_imp(data, len, off,
			z, result);
}

// obsolete
inline object unpack(const char* data, size_t len, zone& z, size_t* off)
{
	object result;

	switch( msgpack::unpack(data, len, off, &z, &result) ) {
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

