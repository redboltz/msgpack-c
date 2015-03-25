#include <msgpack.hpp>
#include <gtest/gtest.h>
#include <sstream>

TEST(int_head, positive_fixint)
{
    {
        std::stringstream ss;
        msgpack::pack(ss, 0);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0x00u));
    }
    {
        std::stringstream ss;
        msgpack::pack(ss, 0x7fu);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0x7fu));
    }
}

TEST(int_head, negative_fixint)
{
    {
        std::stringstream ss;
        msgpack::pack(ss, -32);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xe0u));
    }
    {
        std::stringstream ss;
        msgpack::pack(ss, -1);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xffu));
    }
}

TEST(int_head, uint8)
{
    {
        std::stringstream ss;
        uint8_t v = 0x7fu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0x7fu));
    }
    {
        std::stringstream ss;
        uint8_t v = 0x80u;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xccu));
    }
    {
        std::stringstream ss;
        uint8_t v = 0xffu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xccu));
    }
}

TEST(int_head, uint16)
{
    {
        std::stringstream ss;
        uint16_t v = 0x7fu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0x7fu));
    }
    {
        std::stringstream ss;
        uint16_t v = 0x80u;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xccu));
    }
    {
        std::stringstream ss;
        uint16_t v = 0xffu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xccu));
    }
    {
        std::stringstream ss;
        uint16_t v = 0x100u;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xcdu));
    }
    {
        std::stringstream ss;
        uint16_t v = 0xffffu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xcdu));
    }
}

TEST(int_head, uint32)
{
    {
        std::stringstream ss;
        uint32_t v = 0x7fu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0x7fu));
    }
    {
        std::stringstream ss;
        uint32_t v = 0x80u;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xccu));
    }
    {
        std::stringstream ss;
        uint32_t v = 0xffu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xccu));
    }
    {
        std::stringstream ss;
        uint32_t v = 0x100u;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xcdu));
    }
    {
        std::stringstream ss;
        uint32_t v = 0xffffu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xcdu));
    }
    {
        std::stringstream ss;
        uint32_t v = 0x10000u;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xceu));
    }
    {
        std::stringstream ss;
        uint32_t v = 0xffffffffu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xceu));
    }
}

TEST(int_head, uint64)
{
    {
        std::stringstream ss;
        uint64_t v = 0x7fu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0x7fu));
    }
    {
        std::stringstream ss;
        uint64_t v = 0x80u;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xccu));
    }
    {
        std::stringstream ss;
        uint64_t v = 0xffu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xccu));
    }
    {
        std::stringstream ss;
        uint64_t v = 0x100u;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xcdu));
    }
    {
        std::stringstream ss;
        uint64_t v = 0xffffu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xcdu));
    }
    {
        std::stringstream ss;
        uint64_t v = 0x10000u;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xceu));
    }
    {
        std::stringstream ss;
        uint64_t v = 0xffffffffu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xceu));
    }
    {
        std::stringstream ss;
        uint64_t v = 0x100000000LLu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xcfu));
    }
    {
        std::stringstream ss;
        uint64_t v = 0xffffffffffffffffLLu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xcfu));
    }
}

TEST(int_head, int8)
{
    {
        std::stringstream ss;
        int8_t v = 0x7fu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0x7fu));
    }
    {
        std::stringstream ss;
        int8_t v = -32;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xe0u));
    }
    {
        std::stringstream ss;
        int8_t v = -128;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd0u));
    }
}

TEST(int_head, int16)
{
    {
        std::stringstream ss;
        int16_t v = 0x7fu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0x7fu));
    }
    {
        std::stringstream ss;
        int16_t v = -32;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xe0u));
    }
    {
        std::stringstream ss;
        int16_t v = -128;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd0u));
    }
    {
        std::stringstream ss;
        int16_t v = 128;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd1u));
    }
    {
        std::stringstream ss;
        int16_t v = -129;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd1u));
    }
    {
        std::stringstream ss;
        int16_t v = 32767;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd1u));
    }
    {
        std::stringstream ss;
        int16_t v = -32768;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd1u));
    }
}

TEST(int_head, int32)
{
    {
        std::stringstream ss;
        int32_t v = 0x7fu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0x7fu));
    }
    {
        std::stringstream ss;
        int32_t v = -32;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xe0u));
    }
    {
        std::stringstream ss;
        int32_t v = -128;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd0u));
    }
    {
        std::stringstream ss;
        int16_t v = 128;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd1u));
    }
    {
        std::stringstream ss;
        int16_t v = -129;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd1u));
    }
    {
        std::stringstream ss;
        int32_t v = 32767;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd1u));
    }
    {
        std::stringstream ss;
        int32_t v = -32768;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd1u));
    }
    {
        std::stringstream ss;
        int32_t v = 32768;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd2u));
    }
    {
        std::stringstream ss;
        int32_t v = -32769;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd2u));
    }
    {
        std::stringstream ss;
        int32_t v = 2147483647;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd2u));
    }
    {
        std::stringstream ss;
        int32_t v = -2147483648;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd2u));
    }
}

TEST(int_head, int64)
{
    {
        std::stringstream ss;
        int64_t v = 0x7fu;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0x7fu));
    }
    {
        std::stringstream ss;
        int64_t v = -32;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xe0u));
    }
    {
        std::stringstream ss;
        int64_t v = -128;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd0u));
    }
    {
        std::stringstream ss;
        int16_t v = 128;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd1u));
    }
    {
        std::stringstream ss;
        int16_t v = -129;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd1u));
    }
    {
        std::stringstream ss;
        int64_t v = 32767;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd1u));
    }
    {
        std::stringstream ss;
        int64_t v = -32768;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd1u));
    }
    {
        std::stringstream ss;
        int32_t v = 32768;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd2u));
    }
    {
        std::stringstream ss;
        int32_t v = -32769;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd2u));
    }
    {
        std::stringstream ss;
        int64_t v = 2147483647;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd2u));
    }
    {
        std::stringstream ss;
        int64_t v = -2147483648;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd2u));
    }
    {
        std::stringstream ss;
        int64_t v = 2147483648LL;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd3u));
    }
    {
        std::stringstream ss;
        int64_t v = -2147483649LL;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd3u));
    }
    {
        std::stringstream ss;
        int64_t v = 0x7fffffffffffffffLL;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd3u));
    }
    {
        std::stringstream ss;
        int64_t v = 0x8000000000000000LL;
        msgpack::pack(ss, v);
        EXPECT_EQ(ss.str()[0], static_cast<char>(0xd3u));
    }
}
