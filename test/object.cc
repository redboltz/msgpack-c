#include <msgpack.hpp>
#include <gtest/gtest.h>

struct myclass {
	myclass() : num(0), str("default") { }

	myclass(int num, const std::string& str) :
		num(0), str("default") { }

	~myclass() { }

	int num;
	std::string str;

	MSGPACK_DEFINE(num, str);

	bool operator==(const myclass& o) const
	{
		return num == o.num && str == o.str;
	}
};

std::ostream& operator<<(std::ostream& o, const myclass& m)
{
	return o << "myclass("<<m.num<<",\""<<m.str<<"\")";
}

namespace {
struct null_deleter {
	void operator()(char *) const {}
};
}


TEST(object, convert)
{
	myclass m1;

	msgpack::sbuffer sbuf;
	msgpack::pack(sbuf, m1);

	msgpack::object obj;

	boost::shared_ptr<char> sp(sbuf.data(), null_deleter());
	msgpack::unpack_return ret =
		msgpack::unpack(sp, sbuf.size(), NULL,  obj);
	EXPECT_EQ(ret, msgpack::UNPACK_SUCCESS);

	myclass m2;
	obj.convert(&m2);

	EXPECT_EQ(m1, m2);
}


TEST(object, as)
{
	myclass m1;

	msgpack::sbuffer sbuf;
	msgpack::pack(sbuf, m1);

	msgpack::object obj;

	boost::shared_ptr<char> sp(sbuf.data(), null_deleter());
	msgpack::unpack_return ret =
		msgpack::unpack(sp, sbuf.size(), NULL, obj);
	EXPECT_EQ(ret, msgpack::UNPACK_SUCCESS);

	EXPECT_EQ(m1, obj.as<myclass>());
}


TEST(object, print)
{
	msgpack::object obj;
	std::cout << obj << std::endl;
}


TEST(object, is_nil)
{
	msgpack::object obj;
	EXPECT_TRUE(obj.is_nil());
}


TEST(object, type_error)
{
	msgpack::object obj(1);
	EXPECT_THROW(obj.as<std::string>(), msgpack::type_error);
	EXPECT_THROW(obj.as<std::vector<int> >(), msgpack::type_error);
	EXPECT_EQ(1, obj.as<int>());
	EXPECT_EQ(1, obj.as<short>());
	EXPECT_EQ(1u, obj.as<unsigned int>());
	EXPECT_EQ(1u, obj.as<unsigned long>());
}


TEST(object, equal_primitive)
{
	msgpack::object obj_nil;
	EXPECT_EQ(obj_nil, msgpack::object());

	msgpack::object obj_int(1);
	EXPECT_EQ(obj_int, msgpack::object(1));
	EXPECT_EQ(obj_int, 1);

	msgpack::object obj_double(1.2);
	EXPECT_EQ(obj_double, msgpack::object(1.2));
	EXPECT_EQ(obj_double, 1.2);

	msgpack::object obj_bool(true);
	EXPECT_EQ(obj_bool, msgpack::object(true));
	EXPECT_EQ(obj_bool, true);
}


TEST(object, construct_primitive)
{
	msgpack::object obj_nil;
	EXPECT_TRUE(boost::get<msgpack::type::nil>(&obj_nil.via) != nullptr);

	msgpack::object obj_uint(1);
	uint64_t* ui64 = boost::get<uint64_t>(&obj_uint.via);
	EXPECT_TRUE(ui64 != nullptr);
	EXPECT_EQ(1u, *ui64);

	msgpack::object obj_int(-1);
	int64_t* i64 = boost::get<int64_t>(&obj_int.via);
	EXPECT_TRUE(i64 != nullptr);
	EXPECT_EQ(-1, *i64);

	msgpack::object obj_double(1.2);
	double* d = boost::get<double>(&obj_double.via);
	EXPECT_TRUE(d != nullptr);
	EXPECT_EQ(1.2, *d);

	msgpack::object obj_bool(true);
	bool* b = boost::get<bool>(&obj_bool.via);
	EXPECT_TRUE(b != nullptr);
	EXPECT_EQ(true, *b);
}

