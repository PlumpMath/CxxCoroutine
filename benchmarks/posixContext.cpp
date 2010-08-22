/*
 * posixContext.cpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#include <benchmark.hpp>
#include <details/context_posix.hpp>

using namespace coroutine::details;
using namespace coroutine::details::posix;

#define BENCH_ARCH posix
#include "context.cpp.inc"

BENCH_MAIN(context)
