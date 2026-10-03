#include <msgpack.hpp>

#define BOOST_TEST_MODULE unpack_iterator
#include <boost/test/unit_test.hpp>

#include <cstring>
#include <iterator>
#include <list>
#include <sstream>
#include <string>
#include <vector>

// Tests for unpack(Iterator& begin, Iterator end, ...).
// Written in C++03 syntax so that it also runs in MSGPACK_USE_CPP03 mode.

namespace {

std::string pack_to_string(int v) {
    msgpack::sbuffer sb; msgpack::pack(sb, v); return std::string(sb.data(), sb.size());
}
std::string pack_to_string(std::string const& v) {
    msgpack::sbuffer sb; msgpack::pack(sb, v); return std::string(sb.data(), sb.size());
}
std::string pack_to_string(std::vector<int> const& v) {
    msgpack::sbuffer sb; msgpack::pack(sb, v); return std::string(sb.data(), sb.size());
}

// Three consecutive messages: 42, "hello", [1, 2, 3]
struct messages {
    std::string m1, m2, m3, all;
    messages() {
        std::vector<int> v; v.push_back(1); v.push_back(2); v.push_back(3);
        m1 = pack_to_string(42);
        m2 = pack_to_string(std::string("hello"));
        m3 = pack_to_string(v);
        all = m1 + m2 + m3;
    }
};

template <typename Iterator>
void check_three(Iterator& b, Iterator e, messages const& m) {
    msgpack::object_handle oh1 = msgpack::unpack(b, e);
    BOOST_CHECK_EQUAL(oh1.get().as<int>(), 42);

    msgpack::object_handle oh2 = msgpack::unpack(b, e);
    BOOST_CHECK_EQUAL(oh2.get().as<std::string>(), std::string("hello"));

    msgpack::object_handle oh3 = msgpack::unpack(b, e);
    std::vector<int> v = oh3.get().as<std::vector<int> >();
    BOOST_CHECK_EQUAL(v.size(), static_cast<std::size_t>(3));
    if (v.size() == 3) { BOOST_CHECK_EQUAL(v[0], 1); BOOST_CHECK_EQUAL(v[1], 2); BOOST_CHECK_EQUAL(v[2], 3); }

    // begin is advanced exactly to the end of the last object.
    BOOST_CHECK(b == e);
    (void)m;
}

bool ref_str_only(msgpack::type::object_type t, std::size_t, void*) {
    return t == msgpack::type::STR;
}

} // namespace

// --- contiguous: raw pointers (every C++ standard) -------------------------

BOOST_AUTO_TEST_CASE(pointer_range)
{
    messages m;
    const char* b = m.all.data();
    const char* e = m.all.data() + m.all.size();

    // begin advances by exactly the size of each object
    msgpack::object_handle oh1 = msgpack::unpack(b, e);
    BOOST_CHECK_EQUAL(static_cast<std::size_t>(b - m.all.data()), m.m1.size());
    msgpack::object_handle oh2 = msgpack::unpack(b, e);
    BOOST_CHECK_EQUAL(static_cast<std::size_t>(b - m.all.data()), m.m1.size() + m.m2.size());
    msgpack::object_handle oh3 = msgpack::unpack(b, e);
    BOOST_CHECK(b == e);
    BOOST_CHECK_EQUAL(oh1.get().as<int>(), 42);
    BOOST_CHECK_EQUAL(oh2.get().as<std::string>(), std::string("hello"));
    BOOST_CHECK_EQUAL(oh3.get().as<std::vector<int> >().size(), static_cast<std::size_t>(3));
}

BOOST_AUTO_TEST_CASE(unsigned_char_pointer_range)
{
    messages m;
    std::vector<unsigned char> buf(m.all.begin(), m.all.end());
    const unsigned char* b = &buf[0];
    const unsigned char* e = &buf[0] + buf.size();
    check_three(b, e, m);
}

// A contiguous range is unpacked in place: with a reference func the object
// refers to the caller's memory and `referenced` reports it.
BOOST_AUTO_TEST_CASE(pointer_range_referenced)
{
    messages m;
    const char* b = m.all.data();
    const char* e = m.all.data() + m.all.size();
    bool referenced = true;
    msgpack::object_handle oh1 = msgpack::unpack(b, e, referenced, ref_str_only);
    BOOST_CHECK(!referenced); // 42 is not a str
    msgpack::object_handle oh2 = msgpack::unpack(b, e, referenced, ref_str_only);
    BOOST_CHECK(referenced);
    const char* p = oh2.get().via.str.ptr;
    BOOST_CHECK(p >= m.all.data() && p < m.all.data() + m.all.size());
}

// --- forward / bidirectional: non-contiguous, re-readable -------------------

BOOST_AUTO_TEST_CASE(list_range)
{
    messages m;
    std::list<char> l(m.all.begin(), m.all.end());
    std::list<char>::iterator b = l.begin();
    std::list<char>::iterator e = l.end();
    check_three(b, e, m);
}

// A non-contiguous range is copied first; the object never references the
// caller's range, whatever the reference func says.
BOOST_AUTO_TEST_CASE(list_range_not_referenced)
{
    messages m;
    std::list<char> l(m.all.begin(), m.all.end());
    std::list<char>::iterator b = l.begin();
    std::list<char>::iterator e = l.end();
    bool referenced = true;
    msgpack::object_handle oh1 = msgpack::unpack(b, e, referenced, ref_str_only);
    BOOST_CHECK(!referenced);
    msgpack::object_handle oh2 = msgpack::unpack(b, e, referenced, ref_str_only);
    BOOST_CHECK(!referenced);
    BOOST_CHECK_EQUAL(oh2.get().as<std::string>(), std::string("hello"));
}

// Objects larger than the internal chunk are assembled across chunks and
// `begin` still lands exactly after the object.
BOOST_AUTO_TEST_CASE(list_range_multi_chunk)
{
    std::string big(100000, 'x');
    std::string packed = pack_to_string(big) + pack_to_string(7);
    std::list<char> l(packed.begin(), packed.end());
    std::list<char>::iterator b = l.begin();
    std::list<char>::iterator e = l.end();
    msgpack::object_handle oh1 = msgpack::unpack(b, e);
    BOOST_CHECK_EQUAL(oh1.get().as<std::string>().size(), static_cast<std::size_t>(100000));
    BOOST_CHECK(oh1.get().as<std::string>() == big);
    msgpack::object_handle oh2 = msgpack::unpack(b, e);
    BOOST_CHECK_EQUAL(oh2.get().as<int>(), 7);
    BOOST_CHECK(b == e);
}

// --- std::vector<char>::iterator: contiguous under C++20, forward otherwise --

BOOST_AUTO_TEST_CASE(vector_iterator_range)
{
    messages m;
    std::vector<char> v(m.all.begin(), m.all.end());
    std::vector<char>::iterator b = v.begin();
    std::vector<char>::iterator e = v.end();
    check_three(b, e, m);
}

BOOST_AUTO_TEST_CASE(vector_iterator_range_referenced)
{
    messages m;
    std::vector<char> v(m.all.begin(), m.all.end());
    std::vector<char>::iterator b = v.begin();
    std::vector<char>::iterator e = v.end();
    bool referenced = true;
    msgpack::object_handle oh1 = msgpack::unpack(b, e, referenced, ref_str_only);
    BOOST_CHECK(!referenced);
    msgpack::object_handle oh2 = msgpack::unpack(b, e, referenced, ref_str_only);
    BOOST_CHECK_EQUAL(oh2.get().as<std::string>(), std::string("hello"));
#if defined(MSGPACK_HAS_CONTIGUOUS_ITERATOR)
    // std::contiguous_iterator available: unpacked in place, zero copy.
    BOOST_CHECK(referenced);
    const char* p = oh2.get().via.str.ptr;
    BOOST_CHECK(p >= &v[0] && p < &v[0] + v.size());
#else
    // Treated as a forward iterator: copied, so never referenced.
    BOOST_CHECK(!referenced);
#endif
}

// --- input (single-pass): never reads past the end of an object -----------

BOOST_AUTO_TEST_CASE(istreambuf_iterator_range)
{
    messages m;
    std::istringstream is(m.all);
    std::istreambuf_iterator<char> b(is);
    std::istreambuf_iterator<char> e;

    msgpack::object_handle oh1 = msgpack::unpack(b, e);
    BOOST_CHECK_EQUAL(oh1.get().as<int>(), 42);
    // the stream has been consumed exactly up to the end of the first object
    BOOST_CHECK_EQUAL(static_cast<std::size_t>(is.tellg()), m.m1.size());

    msgpack::object_handle oh2 = msgpack::unpack(b, e);
    BOOST_CHECK_EQUAL(oh2.get().as<std::string>(), std::string("hello"));
    BOOST_CHECK_EQUAL(static_cast<std::size_t>(is.tellg()), m.m1.size() + m.m2.size());

    msgpack::object_handle oh3 = msgpack::unpack(b, e);
    BOOST_CHECK_EQUAL(oh3.get().as<std::vector<int> >().size(), static_cast<std::size_t>(3));
    BOOST_CHECK(b == e);
}

BOOST_AUTO_TEST_CASE(istreambuf_iterator_large)
{
    std::string big(100000, 'y');
    std::istringstream is(pack_to_string(big));
    std::istreambuf_iterator<char> b(is);
    std::istreambuf_iterator<char> e;
    msgpack::object_handle oh = msgpack::unpack(b, e);
    BOOST_CHECK(oh.get().as<std::string>() == big);
    BOOST_CHECK(b == e);
}

// --- errors -----------------------------------------------------------------

BOOST_AUTO_TEST_CASE(truncated_range_throws)
{
    messages m;
    std::string cut = m.all.substr(0, m.all.size() - 2); // last object incomplete
    {
        std::list<char> l(cut.begin(), cut.end());
        std::list<char>::iterator b = l.begin();
        std::list<char>::iterator e = l.end();
        msgpack::object_handle oh1 = msgpack::unpack(b, e);
        msgpack::object_handle oh2 = msgpack::unpack(b, e);
        BOOST_CHECK_THROW(msgpack::unpack(b, e), msgpack::insufficient_bytes);
    }
    {
        const char* b = cut.data();
        const char* e = cut.data() + cut.size();
        msgpack::object_handle oh1 = msgpack::unpack(b, e);
        msgpack::object_handle oh2 = msgpack::unpack(b, e);
        BOOST_CHECK_THROW(msgpack::unpack(b, e), msgpack::insufficient_bytes);
    }
    {
        std::istringstream is(cut);
        std::istreambuf_iterator<char> b(is);
        std::istreambuf_iterator<char> e;
        msgpack::object_handle oh1 = msgpack::unpack(b, e);
        msgpack::object_handle oh2 = msgpack::unpack(b, e);
        BOOST_CHECK_THROW(msgpack::unpack(b, e), msgpack::insufficient_bytes);
    }
}

BOOST_AUTO_TEST_CASE(empty_range_throws)
{
    {
        std::list<char> l;
        std::list<char>::iterator b = l.begin();
        std::list<char>::iterator e = l.end();
        BOOST_CHECK_THROW(msgpack::unpack(b, e), msgpack::insufficient_bytes);
    }
    {
        const char* b = "";
        const char* e = b;
        BOOST_CHECK_THROW(msgpack::unpack(b, e), msgpack::insufficient_bytes);
    }
}

BOOST_AUTO_TEST_CASE(limit_is_honored)
{
    std::vector<int> v; v.push_back(1); v.push_back(2); v.push_back(3);
    std::string packed = pack_to_string(v);
    msgpack::unpack_limit limit(2); // array size limit 2, data has 3 elements
    {
        std::list<char> l(packed.begin(), packed.end());
        std::list<char>::iterator b = l.begin();
        std::list<char>::iterator e = l.end();
        BOOST_CHECK_THROW(msgpack::unpack(b, e, MSGPACK_NULLPTR, MSGPACK_NULLPTR, limit),
                          msgpack::array_size_overflow);
    }
    {
        const char* b = packed.data();
        const char* e = packed.data() + packed.size();
        BOOST_CHECK_THROW(msgpack::unpack(b, e, MSGPACK_NULLPTR, MSGPACK_NULLPTR, limit),
                          msgpack::array_size_overflow);
    }
}

// --- `begin` on error ---------------------------------------------------------

namespace {
// one valid object (42) followed by the reserved byte 0xc1 and some junk
std::string valid_then_invalid() {
    std::string s = pack_to_string(42);
    s.push_back(static_cast<char>(0xc1));
    s += std::string(1000, 'Z');
    return s;
}
} // namespace

// contiguous: begin is unchanged on error, like `off` in unpack(data, len, off)
BOOST_AUTO_TEST_CASE(error_leaves_begin_unchanged_pointer)
{
    std::string s = valid_then_invalid();
    const char* b = s.data();
    const char* e = s.data() + s.size();
    msgpack::object_handle oh = msgpack::unpack(b, e);
    const char* before = b;
    BOOST_CHECK_THROW(msgpack::unpack(b, e), msgpack::parse_error);
    BOOST_CHECK(b == before);

    std::string cut = pack_to_string(std::string("hello")).substr(0, 3); // truncated
    const char* cb = cut.data();
    const char* ce = cut.data() + cut.size();
    BOOST_CHECK_THROW(msgpack::unpack(cb, ce), msgpack::insufficient_bytes);
    BOOST_CHECK(cb == cut.data());
}

// forward (multi-pass): chunks are read ahead but begin is restored on error
BOOST_AUTO_TEST_CASE(error_leaves_begin_unchanged_list)
{
    std::string s = valid_then_invalid();
    std::list<char> l(s.begin(), s.end());
    std::list<char>::iterator b = l.begin();
    std::list<char>::iterator e = l.end();
    msgpack::object_handle oh = msgpack::unpack(b, e);
    std::list<char>::iterator before = b;
    BOOST_CHECK_THROW(msgpack::unpack(b, e), msgpack::parse_error);
    BOOST_CHECK(b == before);

    // insufficient_bytes: begin restored, so the caller can retry later
    std::string cut = pack_to_string(std::string("hello")).substr(0, 3);
    std::list<char> cl(cut.begin(), cut.end());
    std::list<char>::iterator cb = cl.begin();
    std::list<char>::iterator ce = cl.end();
    BOOST_CHECK_THROW(msgpack::unpack(cb, ce), msgpack::insufficient_bytes);
    BOOST_CHECK(cb == cl.begin());
}

// single-pass: the offending byte had to be read, nothing more is consumed
BOOST_AUTO_TEST_CASE(error_consumes_minimum_istream)
{
    std::string s = valid_then_invalid();
    std::istringstream is(s);
    std::istreambuf_iterator<char> b(is);
    std::istreambuf_iterator<char> e;
    msgpack::object_handle oh = msgpack::unpack(b, e);
    std::size_t before = static_cast<std::size_t>(is.tellg());
    BOOST_CHECK_THROW(msgpack::unpack(b, e), msgpack::parse_error);
    BOOST_CHECK_EQUAL(static_cast<std::size_t>(is.tellg()) - before, static_cast<std::size_t>(1));
}
