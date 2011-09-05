/*
 * builder.cpp
 * Copyright © 2011 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <test.hpp>
#include <coroutine/builder.hpp>
#include <coroutine/impl/context_posix.hpp>

#include<iostream>

using namespace coroutine;

int f(yielder<int ()>) { return 42; }
void f2(yielder<>) { }
struct F { int operator()(yielder<int ()>) const { return 42; } };

BOOST_AUTO_TEST_CASE(try_to_compile)
{
	auto c1 = coro<int ()>(F());
	auto c2 = coro<int ()
		 ,context::posix
		 ,stack::static_
		 >(F());
	auto c3 = coro<int (),
		 stack::size_in_kb<42>
			 >(&f);

	typedef builder<int (), F, stack::size_in_b<42> > coro_info_t;

	BOOST_CHECK(coro_info_t::stack_size == 42);
}

#if 0
int f_ret_feed(yielder<int, float> yield, float v)
{
	BOOST_CHECK(v == 2.2f);
	std::cout << "v=" << v << std::endl;
	float a = yield(42);
	BOOST_CHECK(a == 99.f);
	std::cout << "a=" << a << std::endl;
	return 84;
}

BOOST_AUTO_TEST_CASE(ret_feed)
{
	typedef Coroutine<int, float, int (*)(yielder<int, float>, float),
		1024*8, stack::Default, Context> coro_t;

	coro_t c(&f_ret_feed);

	int r1 = c(2.2f);
	BOOST_CHECK(r1 == 42);
	std::cout << "r1=" << r1 << std::endl;
	int r2 = c(99.f);
	BOOST_CHECK(r2 == 84);
	std::cout << "r2=" << r2 << std::endl;
}

int f_ret(yielder<int> yield)
{
	yield(43);
	return 85;
}

BOOST_AUTO_TEST_CASE(ret)
{
	typedef Coroutine<int, void, int (*)(yielder<int>),
		1024*8, stack::Default, Context> coro_t;

	coro_t c(&f_ret);

	int r1 = c();
	BOOST_CHECK(r1 == 43);
	std::cout << "r1=" << r1 << std::endl;
	int r2 = c();
	BOOST_CHECK(r2 == 85);
	std::cout << "r2=" << r2 << std::endl;
}

void f_feed(yielder<void, float> yield, float v)
{
	BOOST_CHECK(v == 2.1f);
	std::cout << "v=" << v << std::endl;
	float a = yield();
	BOOST_CHECK(a == 98.f);
	std::cout << "a=" << a << std::endl;
}

BOOST_AUTO_TEST_CASE(feed)
{
	typedef Coroutine<void, float, void (*)(yielder<void, float>, float),
		1024*8, stack::Default, Context> coro_t;

	coro_t c(&f_feed);

	c(2.1f);
	c(98.f);
}

#endif
