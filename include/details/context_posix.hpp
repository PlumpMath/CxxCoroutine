/*
 * context_posix.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef CONTEXT_POSIX_H
#define CONTEXT_POSIX_H

#include <error.hpp>
#include <details/stack.hpp>

namespace coroutine {
namespace details {
namespace posix {

#include <ucontext.h>
#include <string.h>
#include <errno.h>

template <typename F>
inline void trampoline(F* f)
{
	(*f)();
}

template <template <size_t> class StackImpl = stack::Static,
		 size_t STACK_SIZE = stack::DEFAULT_SIZE>
class Context
{
	typedef Stack<StackImpl, STACK_SIZE> stack_t;

	public:
		template <typename F>
		Context(F& cb)
		{
			if (getcontext(&_coroContext) == -1)
				throw createError("coroutine::details::ContextBase(), "
						"getcontext failed");
			_coroContext.uc_link = &_mainContext;
			_coroContext.uc_stack.ss_sp = _stack.getStack();
			_coroContext.uc_stack.ss_size = stack_t::SIZE;
			void (*cb_ptr)(F*) = &trampoline<F>;
			makecontext(&_coroContext, (void (*)())cb_ptr, 1, &cb);
		}

		void run()
		{
			if (swapcontext(&_mainContext, &_coroContext) == -1)
				throw createError("coroutine::details::ContextBase::run(), "
						"swapcontext failed");
		}

		void yield()
		{
			if (swapcontext(&_coroContext, &_mainContext) == -1)
				throw createError("coroutine::details::ContextBase::yield(), "
						"swapcontext failed");
		}
		
	private:
		ucontext _mainContext;
		ucontext _coroContext;
		stack_t  _stack;

		error::System createError(const char* msg)
		{
			char buf[256];
			if (strerror_r(errno, buf, sizeof buf) == 0)
				return error::System(std::string(msg) + ", " + buf);
			return error::System(std::string(msg) + ", unknown error");
		}
};

} // namespace posix
} // namespace details
} // namespace coroutine

#endif /* CONTEXT_POSIX_H */
