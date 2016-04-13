//
// MessagePack for C++ deserializing routine
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGPACK_V2_UNPACK_HPP
#define MSGPACK_V2_UNPACK_HPP

#include "msgpack/unpack_decl.hpp"

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v2) {
/// @endcond

namespace detail {

class create_object_visitor {
    class unpack_stack {
    public:
        msgpack::object const& obj() const { return m_obj; }
        msgpack::object& obj() { return m_obj; }
        void set_obj(msgpack::object const& obj) { m_obj = obj; }
        std::size_t count() const { return m_count; }
        void set_count(std::size_t count) { m_count = count; }
        std::size_t decr_count() { return --m_count; }
        uint32_t container_type() const { return m_container_type; }
        void set_container_type(uint32_t container_type) { m_container_type = container_type; }
        msgpack::object const& map_key() const { return m_map_key; }
        void set_map_key(msgpack::object const& map_key) { m_map_key = map_key; }
    private:
        msgpack::object m_obj;
        std::size_t m_count;
        uint32_t m_container_type;
        msgpack::object m_map_key;
    };

public:
    create_object_visitor(unpack_reference_func f, void* user_data, unpack_limit const& limit)
        :m_func(f), m_user_data(user_data), m_limit(limit) {
        m_stack.reserve(MSGPACK_EMBED_STACK_SIZE);
        m_stack.push_back(unpack_stack());
        m_obj = &m_stack[0].obj();
    }
    void init() {
        m_stack.resize(1);
        m_stack[0].set_obj(msgpack::object());
        m_obj = &m_stack[0].obj();
    }
    msgpack::object const& data() const
    {
        return m_stack[0].obj();
    }
    msgpack::zone const& zone() const { return *m_zone; }
    msgpack::zone& zone() { return *m_zone; }
    void set_zone(msgpack::zone& zone) { m_zone = &zone; }
    bool referenced() const { return m_referenced; }
    void set_referenced(bool referenced) { m_referenced = referenced; }
    bool visit_nil();
    bool visit_boolean(bool);
    bool visit_positive_integer(uint64_t v) {
        m_obj->type = msgpack::type::POSITIVE_INTEGER;
        m_obj->via.u64 = v;
        if(m_stack.size() == 1) {
            return true;
        }
        return false;
    }
    bool visit_negative_integer(int64_t v) {
        if(v >= 0) {
            m_obj->type = msgpack::type::POSITIVE_INTEGER;
            m_obj->via.u64 = v;
        }
        else {
            m_obj->type = msgpack::type::NEGATIVE_INTEGER;
            m_obj->via.i64 = v;
        }
        return true;
    }
    bool visit_float(double v) {
        m_obj->type = msgpack::type::FLOAT;
        m_obj->via.f64 = v;
        return true;
    }
    bool visit_str(const char* v, uint32_t size) {
        m_obj->type = msgpack::type::STR;
        if (m_func(m_obj->type, size, m_user_data)) {
            m_obj->via.str.ptr = v;
            set_referenced(true);
        }
        else {
            if (size > m_limit.str()) throw msgpack::str_size_overflow("str size overflow");
            char* tmp = static_cast<char*>(zone().allocate_align(size));
            std::memcpy(tmp, v, size);
            m_obj->via.str.ptr = tmp;
        }
        m_obj->via.str.size = size;
        return true;
    }
    bool visit_bin(const char*, uint32_t size);
    bool visit_ext(int8_t type, const char* data, uint32_t size);
    bool start_array(uint32_t num_elements);
    bool end_array();
    bool start_map(uint32_t num_kv_pairs);
    bool start_map_key();
    bool end_map_key();
    bool start_map_value();
    bool end_map_value();
    bool end_map();
    void parse_error(size_t parsed_offset, size_t error_offset);
    void insufficient_bytes(size_t parsed_offset, size_t error_offset);
private:
    unpack_reference_func m_func;
    void* m_user_data;
    unpack_limit m_limit;
    std::vector<unpack_stack> m_stack;
    msgpack::object* m_obj;
    msgpack::zone* m_zone;
    bool m_referenced;
};

template <typename unpack_visitor>
class context {
public:
    context(unpack_visitor& v)
        :m_trail(0), m_visitor(v), m_cs(MSGPACK_CS_HEADER)
    {
    }

    void init()
    {
        m_cs = MSGPACK_CS_HEADER;
        m_trail = 0;
        m_visitor.init();
    }

    unpack_visitor& visitor()
    {
        return m_visitor;
    }

    unpack_visitor const& visitor() const
    {
        return m_visitor;
    }

    int execute(const char* data, std::size_t len, std::size_t& off);

private:
    template <typename T>
    static uint32_t next_cs(T p)
    {
        return static_cast<uint32_t>(*p) & 0x1f;
    }
#if 0
    template <typename T, typename Func>
    int push_aggregate(
        Func const& f,
        uint32_t container_type,
        msgpack::object& obj,
        const char* load_pos,
        std::size_t& off) {
        typename value<T>::type tmp;
        load<T>(tmp, load_pos);
        f(m_user, tmp, m_stack.back().obj());
        if(tmp == 0) {
            obj = m_stack.back().obj();
            int ret = push_proc(obj, off);
            if (ret != 0) return ret;
        }
        else {
            m_stack.back().set_container_type(container_type);
            m_stack.back().set_count(tmp);
            if (m_stack.size() <= m_user.limit().depth()) {
                m_stack.push_back(unpack_stack());
            }
            else {
                throw msgpack::depth_size_overflow("depth size overflow");
            }
            m_cs = MSGPACK_CS_HEADER;
            ++m_current;
        }
        return 0;
    }

    int push_item(msgpack::object& obj) {
        bool finish = false;
        while (!finish) {
            if(m_stack.size() == 1) {
                return 1;
            }
            unpack_stack& sp = *(m_stack.end() - 2);
            switch(sp.container_type()) {
            case MSGPACK_CT_ARRAY_ITEM:
                unpack_array_item(sp.obj(), obj);
                if(sp.decr_count() == 0) {
                    obj = sp.obj();
                    m_stack.pop_back();
                }
                else {
                    finish = true;
                }
                break;
            case MSGPACK_CT_MAP_KEY:
                sp.set_map_key(obj);
                sp.set_container_type(MSGPACK_CT_MAP_VALUE);
                finish = true;
                break;
            case MSGPACK_CT_MAP_VALUE:
                unpack_map_item(sp.obj(), sp.map_key(), obj);
                if(sp.decr_count() == 0) {
                    obj = sp.obj();
                    m_stack.pop_back();
                }
                else {
                    sp.set_container_type(MSGPACK_CT_MAP_KEY);
                    finish = true;
                }
                break;
            default:
                return -1;
            }
        }
        return 0;
    }

    int push_proc(msgpack::object& obj, std::size_t& off) {
        int ret = push_item(obj);
        if (ret > 0) {
            m_stack[0].set_obj(obj);
            ++m_current;
            /*printf("-- finish --\n"); */
            off = m_current - m_start;
        }
        else if (ret < 0) {
            off = m_current - m_start;
        }
        else {
            m_cs = MSGPACK_CS_HEADER;
            ++m_current;
        }
        return ret;
    }
#endif

private:
    char const* m_start;
    char const* m_current;

    std::size_t m_trail;
    unpack_visitor& m_visitor;
    uint32_t m_cs;
};

template <std::size_t N>
inline void check_ext_size(std::size_t /*size*/) {
}

template <>
inline void check_ext_size<4>(std::size_t size) {
    if (size == 0xffffffff) throw msgpack::ext_size_overflow("ext size overflow");
}

template <typename unpack_visitor>
inline int context<unpack_visitor>::execute(const char* data, std::size_t len, std::size_t& off)
{
    assert(len >= off);

    m_start = data;
    m_current = data + off;
    const char* const pe = data + len;
    const char* n = nullptr;

    msgpack::object obj;

    if(m_current == pe) {
        off = m_current - m_start;
        return 0;
    }
    bool fixed_trail_again = false;
    do {
        if (m_cs == MSGPACK_CS_HEADER) {
            fixed_trail_again = false;
            int selector = *reinterpret_cast<const unsigned char*>(m_current);
            if (0x00 <= selector && selector <= 0x7f) { // Positive Fixnum
                bool ret = m_visitor.visit_positive_integer(*reinterpret_cast<const uint8_t*>(m_current));
                ++m_current;
                if (ret) {
                    off = m_current - m_start;
                    return 1;
                }
                m_cs = MSGPACK_CS_HEADER;
            } else if(0xe0 <= selector && selector <= 0xff) { // Negative Fixnum
                bool ret = m_visitor.visit_negative_integer(*reinterpret_cast<const int8_t*>(m_current));
                ++m_current;
                if (ret) {
                    off = m_current - m_start;
                    return 1;
                }
                m_cs = MSGPACK_CS_HEADER;
            } else if (0xc4 <= selector && selector <= 0xdf) {
                const uint32_t trail[] = {
                    1, // bin     8  0xc4
                    2, // bin    16  0xc5
                    4, // bin    32  0xc6
                    1, // ext     8  0xc7
                    2, // ext    16  0xc8
                    4, // ext    32  0xc9
                    4, // float  32  0xca
                    8, // float  64  0xcb
                    1, // uint    8  0xcc
                    2, // uint   16  0xcd
                    4, // uint   32  0xce
                    8, // uint   64  0xcf
                    1, // int     8  0xd0
                    2, // int    16  0xd1
                    4, // int    32  0xd2
                    8, // int    64  0xd3
                    2, // fixext  1  0xd4
                    3, // fixext  2  0xd5
                    5, // fixext  4  0xd6
                    9, // fixext  8  0xd7
                    17,// fixext 16  0xd8
                    1, // str     8  0xd9
                    2, // str    16  0xda
                    4, // str    32  0xdb
                    2, // array  16  0xdc
                    4, // array  32  0xdd
                    2, // map    16  0xde
                    4, // map    32  0xdf
                };
                m_trail = trail[selector - 0xc4];
                m_cs = next_cs(m_current);
                fixed_trail_again = true;
            } else if(0xa0 <= selector && selector <= 0xbf) { // FixStr
                m_trail = static_cast<uint32_t>(*m_current) & 0x1f;
                if(m_trail == 0) {
                    unpack_str(m_user, n, static_cast<uint32_t>(m_trail), obj);
                    int ret = push_proc(obj, off);
                    if (ret != 0) return ret;
                }
                else {
                    m_cs = MSGPACK_ACS_STR_VALUE;
                    fixed_trail_again = true;
                }

            } else if(0x90 <= selector && selector <= 0x9f) { // FixArray
                int ret = push_aggregate<fix_tag>(
                    unpack_array(), MSGPACK_CT_ARRAY_ITEM, obj, m_current, off);
                if (ret != 0) return ret;
            } else if(0x80 <= selector && selector <= 0x8f) { // FixMap
                int ret = push_aggregate<fix_tag>(
                    unpack_map(), MSGPACK_CT_MAP_KEY, obj, m_current, off);
                if (ret != 0) return ret;
            } else if(selector == 0xc2) { // false
                unpack_false(obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } else if(selector == 0xc3) { // true
                unpack_true(obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } else if(selector == 0xc0) { // nil
                unpack_nil(obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } else {
                off = m_current - m_start;
                return -1;
            }
            // end MSGPACK_CS_HEADER
        }
        if (m_cs != MSGPACK_CS_HEADER || fixed_trail_again) {
            if (fixed_trail_again) {
                ++m_current;
                fixed_trail_again = false;
            }
            if(static_cast<std::size_t>(pe - m_current) < m_trail) {
                off = m_current - m_start;
                return 0;
            }
            n = m_current;
            m_current += m_trail - 1;
            switch(m_cs) {
                //case MSGPACK_CS_
                //case MSGPACK_CS_
            case MSGPACK_CS_FLOAT: {
                union { uint32_t i; float f; } mem;
                load<uint32_t>(mem.i, n);
                unpack_float(mem.f, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_DOUBLE: {
                union { uint64_t i; double f; } mem;
                load<uint64_t>(mem.i, n);
#if defined(TARGET_OS_IPHONE)
                // ok
#elif defined(__arm__) && !(__ARM_EABI__) // arm-oabi
                // https://github.com/msgpack/msgpack-perl/pull/1
                mem.i = (mem.i & 0xFFFFFFFFUL) << 32UL | (mem.i >> 32UL);
#endif
                unpack_double(mem.f, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_UINT_8: {
                uint8_t tmp;
                load<uint8_t>(tmp, n);
                unpack_uint8(tmp, obj);
                m_visitor.
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_UINT_16: {
                uint16_t tmp;
                load<uint16_t>(tmp, n);
                unpack_uint16(tmp, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_UINT_32: {
                uint32_t tmp;
                load<uint32_t>(tmp, n);
                unpack_uint32(tmp, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_UINT_64: {
                uint64_t tmp;
                load<uint64_t>(tmp, n);
                unpack_uint64(tmp, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_INT_8: {
                int8_t tmp;
                load<int8_t>(tmp, n);
                unpack_int8(tmp, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_INT_16: {
                int16_t tmp;
                load<int16_t>(tmp, n);
                unpack_int16(tmp, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_INT_32: {
                int32_t tmp;
                load<int32_t>(tmp, n);
                unpack_int32(tmp, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_INT_64: {
                int64_t tmp;
                load<int64_t>(tmp, n);
                unpack_int64(tmp, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_FIXEXT_1: {
                unpack_ext(m_user, n, 1+1, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_FIXEXT_2: {
                unpack_ext(m_user, n, 2+1, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_FIXEXT_4: {
                unpack_ext(m_user, n, 4+1, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_FIXEXT_8: {
                unpack_ext(m_user, n, 8+1, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_FIXEXT_16: {
                unpack_ext(m_user, n, 16+1, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_STR_8: {
                uint8_t tmp;
                load<uint8_t>(tmp, n);
                m_trail = tmp;
                if(m_trail == 0) {
                    unpack_str(m_user, n, static_cast<uint32_t>(m_trail), obj);
                    int ret = push_proc(obj, off);
                    if (ret != 0) return ret;
                }
                else {
                    m_cs = MSGPACK_ACS_STR_VALUE;
                    fixed_trail_again = true;
                }
            } break;
            case MSGPACK_CS_BIN_8: {
                uint8_t tmp;
                load<uint8_t>(tmp, n);
                m_trail = tmp;
                if(m_trail == 0) {
                    unpack_bin(m_user, n, static_cast<uint32_t>(m_trail), obj);
                    int ret = push_proc(obj, off);
                    if (ret != 0) return ret;
                }
                else {
                    m_cs = MSGPACK_ACS_BIN_VALUE;
                    fixed_trail_again = true;
                }
            } break;
            case MSGPACK_CS_EXT_8: {
                uint8_t tmp;
                load<uint8_t>(tmp, n);
                m_trail = tmp + 1;
                if(m_trail == 0) {
                    unpack_ext(m_user, n, m_trail, obj);
                    int ret = push_proc(obj, off);
                    if (ret != 0) return ret;
                }
                else {
                    m_cs = MSGPACK_ACS_EXT_VALUE;
                    fixed_trail_again = true;
                }
            } break;
            case MSGPACK_CS_STR_16: {
                uint16_t tmp;
                load<uint16_t>(tmp, n);
                m_trail = tmp;
                if(m_trail == 0) {
                    unpack_str(m_user, n, static_cast<uint32_t>(m_trail), obj);
                    int ret = push_proc(obj, off);
                    if (ret != 0) return ret;
                }
                else {
                    m_cs = MSGPACK_ACS_STR_VALUE;
                    fixed_trail_again = true;
                }
            } break;
            case MSGPACK_CS_BIN_16: {
                uint16_t tmp;
                load<uint16_t>(tmp, n);
                m_trail = tmp;
                if(m_trail == 0) {
                    unpack_bin(m_user, n, static_cast<uint32_t>(m_trail), obj);
                    int ret = push_proc(obj, off);
                    if (ret != 0) return ret;
                }
                else {
                    m_cs = MSGPACK_ACS_BIN_VALUE;
                    fixed_trail_again = true;
                }
            } break;
            case MSGPACK_CS_EXT_16: {
                uint16_t tmp;
                load<uint16_t>(tmp, n);
                m_trail = tmp + 1;
                if(m_trail == 0) {
                    unpack_ext(m_user, n, m_trail, obj);
                    int ret = push_proc(obj, off);
                    if (ret != 0) return ret;
                }
                else {
                    m_cs = MSGPACK_ACS_EXT_VALUE;
                    fixed_trail_again = true;
                }
            } break;
            case MSGPACK_CS_STR_32: {
                uint32_t tmp;
                load<uint32_t>(tmp, n);
                m_trail = tmp;
                if(m_trail == 0) {
                    unpack_str(m_user, n, static_cast<uint32_t>(m_trail), obj);
                    int ret = push_proc(obj, off);
                    if (ret != 0) return ret;
                }
                else {
                    m_cs = MSGPACK_ACS_STR_VALUE;
                    fixed_trail_again = true;
                }
            } break;
            case MSGPACK_CS_BIN_32: {
                uint32_t tmp;
                load<uint32_t>(tmp, n);
                m_trail = tmp;
                if(m_trail == 0) {
                    unpack_bin(m_user, n, static_cast<uint32_t>(m_trail), obj);
                    int ret = push_proc(obj, off);
                    if (ret != 0) return ret;
                }
                else {
                    m_cs = MSGPACK_ACS_BIN_VALUE;
                    fixed_trail_again = true;
                }
            } break;
            case MSGPACK_CS_EXT_32: {
                uint32_t tmp;
                load<uint32_t>(tmp, n);
                check_ext_size<sizeof(std::size_t)>(tmp);
                m_trail = tmp;
                ++m_trail;
                if(m_trail == 0) {
                    unpack_ext(m_user, n, m_trail, obj);
                    int ret = push_proc(obj, off);
                    if (ret != 0) return ret;
                }
                else {
                    m_cs = MSGPACK_ACS_EXT_VALUE;
                    fixed_trail_again = true;
                }
            } break;
            case MSGPACK_ACS_STR_VALUE: {
                unpack_str(m_user, n, static_cast<uint32_t>(m_trail), obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_ACS_BIN_VALUE: {
                unpack_bin(m_user, n, static_cast<uint32_t>(m_trail), obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_ACS_EXT_VALUE: {
                unpack_ext(m_user, n, m_trail, obj);
                int ret = push_proc(obj, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_ARRAY_16: {
                int ret = push_aggregate<uint16_t>(
                    unpack_array(), MSGPACK_CT_ARRAY_ITEM, obj, n, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_ARRAY_32: {
                /* FIXME security guard */
                int ret = push_aggregate<uint32_t>(
                    unpack_array(), MSGPACK_CT_ARRAY_ITEM, obj, n, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_MAP_16: {
                int ret = push_aggregate<uint16_t>(
                    unpack_map(), MSGPACK_CT_MAP_KEY, obj, n, off);
                if (ret != 0) return ret;
            } break;
            case MSGPACK_CS_MAP_32: {
                /* FIXME security guard */
                int ret = push_aggregate<uint32_t>(
                    unpack_map(), MSGPACK_CT_MAP_KEY, obj, n, off);
                if (ret != 0) return ret;
            } break;
            default:
                off = m_current - m_start;
                return -1;
            }
        }
    } while(m_current != pe);

    off = m_current - m_start;
    return 0;
}

} // detail


/// Unpacking class for a stream deserialization.
class unpacker : public  basic_unpacker<create_object_visitor> {
public:
    unpacker(unpack_reference_func f = &unpacker::default_reference_func,
             void* user_data = nullptr,
             std::size_t initial_buffer_size = MSGPACK_UNPACKER_INIT_BUFFER_SIZE,
             unpack_limit const& limit = unpack_limit())
        :basic_unpacker<create_object_visitor>(m_visitor, initial_buffer_size),
        m_visitor(f, user_data, limit)
    {
    }
private:
    create_object_visitor m_visitor;
};

template <unpack_visitor>
class basic_unpacker {
public:
    /// Constructor
    /**
     * @param referenced If the unpacked object contains reference of the buffer, then set as true, otherwise false.
     * @param f A judging function that msgpack::object refer to the buffer.
     * @param user_data This parameter is passed to f.
     * @param initial_buffer_size The memory size to allocate when unpacker is constructed.
     * @param limit The size limit information of msgpack::object.
     *
     */
    basic_unpacker(unpack_visitor& vunpack_reference_func f = &unpacker::default_reference_func,
                   void* user_data = nullptr,
                   std::size_t initial_buffer_size = MSGPACK_UNPACKER_INIT_BUFFER_SIZE,
                   unpack_limit const& limit = unpack_limit());

#if !defined(MSGPACK_USE_CPP03)
    basic_unpacker(unpacker&& other);
    basic_unpacker<unpack_visitor>& operator=(basic_unpacker<unpack_visitor>&& other);
#endif // !defined(MSGPACK_USE_CPP03)

    ~unpacker();

public:
    /// Reserve a buffer memory.
    /**
     * @param size The size of allocating memory.
     *
     * After returning this function, buffer_capacity() returns at least 'size'.
     * See:
     * https://github.com/msgpack/msgpack-c/wiki/v1_1_cpp_unpacker#msgpack-controls-a-buffer
     */
    void reserve_buffer(std::size_t size = MSGPACK_UNPACKER_RESERVE_SIZE);

    /// Get buffer pointer.
    /**
     * You need to care about the memory is enable between buffer() and buffer() + buffer_capacity()
     * See:
     * https://github.com/msgpack/msgpack-c/wiki/v1_1_cpp_unpacker#msgpack-controls-a-buffer
     */
    char* buffer();

    /// Get buffer capacity.
    /**
     * @return The memory size that you can write.
     *
     * See:
     * https://github.com/msgpack/msgpack-c/wiki/v1_1_cpp_unpacker#msgpack-controls-a-buffer
     */
    std::size_t buffer_capacity() const;

    /// Notify a buffer consumed information to msgpack::unpacker.
    /**
     * @param size The size of memory that you consumed.
     *
     * After copying the data to the memory that is pointed by buffer(), you need to call the
     * function to notify how many bytes are consumed. Then you can call next() functions.
     *
     * See:
     * https://github.com/msgpack/msgpack-c/wiki/v1_1_cpp_unpacker#msgpack-controls-a-buffer
     */
    void buffer_consumed(std::size_t size);

    /// Unpack one msgpack::object. [obsolete]
    /**
     *
     * @param result The object that contains unpacked data.
     *
     * @return If one msgpack::object is unpacked, then return true, if msgpack::object is incomplete
     *         and additional data is required, then return false. If data format is invalid, throw
     *         msgpack::parse_error.
     *
     * See:
     * https://github.com/msgpack/msgpack-c/wiki/v1_1_cpp_unpacker#msgpack-controls-a-buffer
     * This function is obsolete. Use the reference inteface version of next() function instead of
     * the pointer interface version.
     */
    bool next(msgpack::object_handle* result);

    /// Unpack one msgpack::object.
    /**
     *
     * @param result The object that contains unpacked data.
     * @param referenced If the unpacked object contains reference of the buffer,
     *                   then set as true, otherwise false.
     *
     * @return If one msgpack::object is unpacked, then return true, if msgpack::object is incomplete
     *         and additional data is required, then return false. If data format is invalid, throw
     *         msgpack::parse_error.
     *
     * See:
     * https://github.com/msgpack/msgpack-c/wiki/v1_1_cpp_unpacker#msgpack-controls-a-buffer
     */
    bool next(msgpack::object_handle& result, bool& referenced);

    /// Unpack one msgpack::object.
    /**
     *
     * @param result The object that contains unpacked data.
     *
     * @return If one msgpack::object is unpacked, then return true, if msgpack::object is incomplete
     *         and additional data is required, then return false. If data format is invalid, throw
     *         msgpack::parse_error.
     *
     * See:
     * https://github.com/msgpack/msgpack-c/wiki/v1_1_cpp_unpacker#msgpack-controls-a-buffer
     */
    bool next(msgpack::object_handle& result);

    /// Get message size.
    /**
     * @return Returns parsed_size() + nonparsed_size()
     */
    std::size_t message_size() const;

    /*! for backward compatibility */
    bool execute();

    /*! for backward compatibility */
    msgpack::object const& data();

    /*! for backward compatibility */
    msgpack::zone* release_zone();

    /*! for backward compatibility */
    void reset_zone();

    /*! for backward compatibility */
    void reset();

public:
    /// Get parsed message size.
    /**
     * @return Parsed message size.
     *
     * This function is usable when non-MessagePack message follows after
     * MessagePack message.
     */
    std::size_t parsed_size() const;

    /// Get the address that is not parsed in the buffer.
    /**
     * @return Address of the buffer that is not parsed
     *
     * This function is usable when non-MessagePack message follows after
     * MessagePack message.
     */
    char* nonparsed_buffer();

    /// Get the size of the buffer that is not parsed.
    /**
     * @return Size of the buffer that is not parsed
     *
     * This function is usable when non-MessagePack message follows after
     * MessagePack message.
     */
    std::size_t nonparsed_size() const;

    /// Skip the specified size of non-parsed buffer.
    /**
     * @param size to skip
     *
     * Note that the `size' argument must be smaller than nonparsed_size().
     * This function is usable when non-MessagePack message follows after
     * MessagePack message.
     */
    void skip_nonparsed_buffer(std::size_t size);

    /// Remove nonparsed buffer and reset the current position as a new start point.
    /**
     * This function is usable when non-MessagePack message follows after
     * MessagePack message.
     */
    void remove_nonparsed_buffer();

private:
    void expand_buffer(std::size_t size);
    int execute_imp();
    bool flush_zone();
    static bool default_reference_func(msgpack::type::object_type type, std::size_t len, void*);

private:
    char* m_buffer;
    std::size_t m_used;
    std::size_t m_free;
    std::size_t m_off;
    std::size_t m_parsed;
    msgpack::unique_ptr<msgpack::zone> m_z;
    std::size_t m_initial_buffer_size;
    create_object_visitor m_cov;
    detail::context<> m_ctx;

#if defined(MSGPACK_USE_CPP03)
private:
    unpacker(const unpacker&);
    unpacker& operator=(const unpacker&);
#else  // defined(MSGPACK_USE_CPP03)
    unpacker(const unpacker&) = delete;
    unpacker& operator=(const unpacker&) = delete;
#endif // defined(MSGPACK_USE_CPP03)
};

inline unpacker::unpacker(unpack_reference_func f,
                          void* user_data,
                          std::size_t initial_buffer_size,
                          unpack_limit const& limit)
    :m_z(new msgpack::zone), m_ctx(f, user_data, limit)
{
    if(initial_buffer_size < COUNTER_SIZE) {
        initial_buffer_size = COUNTER_SIZE;
    }

    char* buffer = static_cast<char*>(::malloc(initial_buffer_size));
    if(!buffer) {
        throw std::bad_alloc();
    }

    m_buffer = buffer;
    m_used = COUNTER_SIZE;
    m_free = initial_buffer_size - m_used;
    m_off = COUNTER_SIZE;
    m_parsed = 0;
    m_initial_buffer_size = initial_buffer_size;

    detail::init_count(m_buffer);

    m_ctx.init();
    m_ctx.user().set_zone(*m_z);
    m_ctx.user().set_referenced(false);
}

#if !defined(MSGPACK_USE_CPP03)
// Move constructor and move assignment operator

inline unpacker::unpacker(unpacker&& other)
    :m_buffer(other.m_buffer),
     m_used(other.m_used),
     m_free(other.m_free),
     m_off(other.m_off),
     m_parsed(other.m_parsed),
     m_z(std::move(other.m_z)),
     m_initial_buffer_size(other.m_initial_buffer_size),
     m_ctx(other.m_ctx) {
    other.m_buffer = nullptr;
}

inline unpacker& unpacker::operator=(unpacker&& other) {
    this->~unpacker();
    new (this) unpacker(std::move(other));
    return *this;
}

#endif // !defined(MSGPACK_USE_CPP03)


inline unpacker::~unpacker()
{
    // These checks are required for move operations.
    if (m_buffer) detail::decr_count(m_buffer);
}


inline void unpacker::reserve_buffer(std::size_t size)
{
    if(m_free >= size) return;
    expand_buffer(size);
}

inline void unpacker::expand_buffer(std::size_t size)
{
    if(m_used == m_off && detail::get_count(m_buffer) == 1
        && !m_ctx.user().referenced()) {
        // rewind buffer
        m_free += m_used - COUNTER_SIZE;
        m_used = COUNTER_SIZE;
        m_off  = COUNTER_SIZE;

        if(m_free >= size) return;
    }

    if(m_off == COUNTER_SIZE) {
        std::size_t next_size = (m_used + m_free) * 2;    // include COUNTER_SIZE
        while(next_size < size + m_used) {
            std::size_t tmp_next_size = next_size * 2;
            if (tmp_next_size <= next_size) {
                next_size = size + m_used;
                break;
            }
            next_size = tmp_next_size;
        }

        char* tmp = static_cast<char*>(::realloc(m_buffer, next_size));
        if(!tmp) {
            throw std::bad_alloc();
        }

        m_buffer = tmp;
        m_free = next_size - m_used;

    } else {
        std::size_t next_size = m_initial_buffer_size;  // include COUNTER_SIZE
        std::size_t not_parsed = m_used - m_off;
        while(next_size < size + not_parsed + COUNTER_SIZE) {
            std::size_t tmp_next_size = next_size * 2;
            if (tmp_next_size <= next_size) {
                next_size = size + not_parsed + COUNTER_SIZE;
                break;
            }
            next_size = tmp_next_size;
        }

        char* tmp = static_cast<char*>(::malloc(next_size));
        if(!tmp) {
            throw std::bad_alloc();
        }

        detail::init_count(tmp);

        std::memcpy(tmp+COUNTER_SIZE, m_buffer + m_off, not_parsed);

        if(m_ctx.user().referenced()) {
            try {
                m_z->push_finalizer(&detail::decr_count, m_buffer);
            }
            catch (...) {
                ::free(tmp);
                throw;
            }
            m_ctx.user().set_referenced(false);
        } else {
            detail::decr_count(m_buffer);
        }

        m_buffer = tmp;
        m_used  = not_parsed + COUNTER_SIZE;
        m_free  = next_size - m_used;
        m_off   = COUNTER_SIZE;
    }
}

inline char* unpacker::buffer()
{
    return m_buffer + m_used;
}

inline std::size_t unpacker::buffer_capacity() const
{
    return m_free;
}

inline void unpacker::buffer_consumed(std::size_t size)
{
    m_used += size;
    m_free -= size;
}

inline bool unpacker::next(msgpack::object_handle& result, bool& referenced)
{
    referenced = false;
    int ret = execute_imp();
    if(ret < 0) {
        throw msgpack::parse_error("parse error");
    }

    if(ret == 0) {
        result.zone().reset();
        result.set(msgpack::object());
        return false;

    } else {
        referenced = m_ctx.user().referenced();
        result.zone().reset( release_zone() );
        result.set(data());
        reset();
        return true;
    }
}

inline bool unpacker::next(msgpack::object_handle& result)
{
    bool referenced;
    return next(result, referenced);
}

inline bool unpacker::next(msgpack::object_handle* result)
{
    return next(*result);
}


inline bool unpacker::execute()
{
    int ret = execute_imp();
    if(ret < 0) {
        throw msgpack::parse_error("parse error");
    } else if(ret == 0) {
        return false;
    } else {
        return true;
    }
}

inline int unpacker::execute_imp()
{
    std::size_t off = m_off;
    int ret = m_ctx.execute(m_buffer, m_used, m_off);
    if(m_off > off) {
        m_parsed += m_off - off;
    }
    return ret;
}

inline msgpack::object const& unpacker::data()
{
    return m_ctx.data();
}

inline msgpack::zone* unpacker::release_zone()
{
    if(!flush_zone()) {
        return nullptr;
    }

    msgpack::zone* r =  new msgpack::zone;
    msgpack::zone* old = m_z.release();
    m_z.reset(r);
    m_ctx.user().set_zone(*m_z);

    return old;
}

inline void unpacker::reset_zone()
{
    m_z->clear();
}

inline bool unpacker::flush_zone()
{
    if(m_ctx.user().referenced()) {
        try {
            m_z->push_finalizer(&detail::decr_count, m_buffer);
        } catch (...) {
            return false;
        }
        m_ctx.user().set_referenced(false);

        detail::incr_count(m_buffer);
    }

    return true;
}

inline void unpacker::reset()
{
    m_ctx.init();
    // don't reset referenced flag
    m_parsed = 0;
}

inline std::size_t unpacker::message_size() const
{
    return m_parsed - m_off + m_used;
}

inline std::size_t unpacker::parsed_size() const
{
    return m_parsed;
}

inline char* unpacker::nonparsed_buffer()
{
    return m_buffer + m_off;
}

inline std::size_t unpacker::nonparsed_size() const
{
    return m_used - m_off;
}

inline void unpacker::skip_nonparsed_buffer(std::size_t size)
{
    m_off += size;
}

inline void unpacker::remove_nonparsed_buffer()
{
    m_used = m_off;
}


inline msgpack::object_handle unpack(
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit
)
{
    msgpack::object obj;
    msgpack::unique_ptr<msgpack::zone> z(new msgpack::zone);
    referenced = false;
    std::size_t noff = off;
    unpack_return ret = detail::unpack_imp(
        data, len, noff, *z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case UNPACK_SUCCESS:
        off = noff;
        return msgpack::object_handle(obj, msgpack::move(z));
    case UNPACK_EXTRA_BYTES:
        off = noff;
        return msgpack::object_handle(obj, msgpack::move(z));
    case UNPACK_CONTINUE:
        throw msgpack::insufficient_bytes("insufficient bytes");
    case UNPACK_PARSE_ERROR:
    default:
        throw msgpack::parse_error("parse error");
    }
    return msgpack::object_handle();
}

inline msgpack::object_handle unpack(
    const char* data, std::size_t len, std::size_t& off,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    bool referenced;
    return unpack(data, len, off, referenced, f, user_data, limit);
}

inline msgpack::object_handle unpack(
    const char* data, std::size_t len, bool& referenced,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    std::size_t off = 0;
    return unpack(data, len, off, referenced, f, user_data, limit);
}

inline msgpack::object_handle unpack(
    const char* data, std::size_t len,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    return unpack(data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    msgpack::object_handle& result,
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    msgpack::object obj;
    msgpack::unique_ptr<msgpack::zone> z(new msgpack::zone);
    referenced = false;
    std::size_t noff = off;
    unpack_return ret = detail::unpack_imp(
        data, len, noff, *z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case UNPACK_SUCCESS:
        off = noff;
        result.set(obj);
        result.zone() = msgpack::move(z);
        return;
    case UNPACK_EXTRA_BYTES:
        off = noff;
        result.set(obj);
        result.zone() = msgpack::move(z);
        return;
    case UNPACK_CONTINUE:
        throw msgpack::insufficient_bytes("insufficient bytes");
    case UNPACK_PARSE_ERROR:
    default:
        throw msgpack::parse_error("parse error");
    }
}

inline void unpack(
    msgpack::object_handle& result,
    const char* data, std::size_t len, std::size_t& off,
    unpack_reference_func f, void* user_data,
            unpack_limit const& limit)
{
    bool referenced;
    unpack(result, data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    msgpack::object_handle& result,
    const char* data, std::size_t len, bool& referenced,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    std::size_t off = 0;
    unpack(result, data, len, off, referenced, f, user_data, limit);
}

inline void unpack(
    msgpack::object_handle& result,
    const char* data, std::size_t len,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    unpack(result, data, len, off, referenced, f, user_data, limit);
}


inline msgpack::object unpack(
    msgpack::zone& z,
    const char* data, std::size_t len, std::size_t& off, bool& referenced,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    msgpack::object obj;
    std::size_t noff = off;
    referenced = false;
    unpack_return ret = detail::unpack_imp(
        data, len, noff, z, obj, referenced, f, user_data, limit);

    switch(ret) {
    case UNPACK_SUCCESS:
        off = noff;
        return obj;
    case UNPACK_EXTRA_BYTES:
        off = noff;
        return obj;
    case UNPACK_CONTINUE:
        throw msgpack::insufficient_bytes("insufficient bytes");
    case UNPACK_PARSE_ERROR:
    default:
        throw msgpack::parse_error("parse error");
    }
    return obj;
}

inline msgpack::object unpack(
    msgpack::zone& z,
    const char* data, std::size_t len, std::size_t& off,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    bool referenced;
    return unpack(z, data, len, off, referenced, f, user_data, limit);
}

inline msgpack::object unpack(
    msgpack::zone& z,
    const char* data, std::size_t len, bool& referenced,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    std::size_t off = 0;
    return unpack(z, data, len, off, referenced, f, user_data, limit);
}

inline msgpack::object unpack(
    msgpack::zone& z,
    const char* data, std::size_t len,
    unpack_reference_func f, void* user_data,
    unpack_limit const& limit)
{
    bool referenced;
    std::size_t off = 0;
    return unpack(z, data, len, off, referenced, f, user_data, limit);
}

template <typename UnpackVisitor>
inline bool unpack(const char* data, size_t len, size_t& off, UnpackVisitor&) {
}

namespace detail {

inline unpack_return
unpack_imp(const char* data, std::size_t len, std::size_t& off,
           msgpack::zone& result_zone, msgpack::object& result, bool& referenced,
           unpack_reference_func f = nullptr, void* user_data = nullptr,
           unpack_limit const& limit = unpack_limit())
{
    std::size_t noff = off;

    if(len <= noff) {
        // FIXME
        return UNPACK_CONTINUE;
    }

    create_object_visitor v(f, user_data, limit);
    detail::context ctx(f, user_data, limit);
    ctx.init();

    ctx.user().set_zone(result_zone);
    ctx.user().set_referenced(false);
    referenced = false;

    unpack(data, len, noff, v);
    int e = ctx.execute(data, len, noff);
    if(e < 0) {
        return UNPACK_PARSE_ERROR;
    }

    referenced = ctx.user().referenced();
    off = noff;

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


/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v2)
/// @endcond

}  // namespace msgpack


#endif // MSGPACK_V2_UNPACK_HPP
