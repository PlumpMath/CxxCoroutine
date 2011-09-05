/*
 * coroutine.hpp
 * Copyright © 2010 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef COROUTINE_H
#define COROUTINE_H

#include <context.hpp>
#include <yielder.hpp>


#include<iostream>

namespace coroutine {

	// RV f(FV feedValue)
	template <typename RV, typename FV, typename IMPL>
		class coroutine_base
		{
			public:
				RV operator ()(FV fval)
				{
					_fv = &fval;
					IMPL* impl = static_cast<IMPL*>(this);
					impl->_context.enter();
					return *_rv;
				}

			protected:
				static void bootstrap_trampoline(void* self) {
					reinterpret_cast<coroutine_base*>(self)->bootstrap();
				}

			private:
				RV* _rv;
				FV* _fv;

				void bootstrap()
				{
					Yielder<RV, FV> yielder(&yield_trampoline, this);
					yield(static_cast<IMPL*>(this)->_func(yielder, *_fv));
					abort();
				}

				static FV yield_trampoline(void* self, RV value) {
					return reinterpret_cast<coroutine_base*>(self)->yield(value);
				}

				FV yield(RV value)
				{
					_rv = &value;
					IMPL* impl = static_cast<IMPL*>(this);
					impl->_context.leave();
					return *_fv;
				}
		};

	// RV f()
	template <typename RV, typename IMPL>
		class coroutine_base<RV, void, IMPL>
		{
			public:
				RV operator ()()
				{
					static_cast<IMPL*>(this)->_context.enter();
					return *_rv;
				}

			protected:
				static void bootstrap_trampoline(void* self) {
					reinterpret_cast<coroutine_base*>(self)->bootstrap();
				}

			private:
				RV* _rv;

				void bootstrap()
				{
					Yielder<RV, void> yielder(&yield_trampoline, this);
					yield(static_cast<IMPL*>(this)->_func(yielder));
					abort();
				}

				static void yield_trampoline(void* self, RV value) {
					reinterpret_cast<coroutine_base*>(self)->yield(value);
				}

				void yield(RV value)
				{
					_rv = &value;
					static_cast<IMPL*>(this)->_context.leave();
				}
		};

	// void f(FV feedValue)
	template <typename FV, typename IMPL>
		class coroutine_base<void, FV, IMPL>
		{
			public:
				void operator ()(FV fval)
				{
					_fv = &fval;
					static_cast<IMPL*>(this)->_context.enter();
				}

			protected:
				static void bootstrap_trampoline(void* self) {
					reinterpret_cast<coroutine_base*>(self)->bootstrap();
				}

			private:
				FV* _fv;

				void bootstrap()
				{
					Yielder<void, FV> yielder(&yield_trampoline, this);
					static_cast<IMPL*>(this)->_func(yielder, *_fv);
					yield();
					abort();
				}

				static FV yield_trampoline(void* self) {
					return reinterpret_cast<coroutine_base*>(self)->yield();
				}

				FV yield()
				{
					static_cast<IMPL*>(this)->_context.leave();
					return *_fv;
				}
		};

	// void f()
	template <typename IMPL>
		class coroutine_base<void, void, IMPL>
		{
			public:
				void operator ()()
				{
					static_cast<IMPL*>(this)->_context.enter();
				}

			protected:
				static void bootstrap_trampoline(void* self) {
					reinterpret_cast<coroutine_base*>(self)->bootstrap();
				}

			private:

				void bootstrap()
				{
					Yielder<void, void> yielder(&yield_trampoline, this);
					static_cast<IMPL*>(this)->_func(yielder);
					yield();
					abort();
				}

				static void yield_trampoline(void* self) {
					reinterpret_cast<coroutine_base*>(self)->yield();
				}

				void yield()
				{
					static_cast<IMPL*>(this)->_context.leave();
				}
		};

	namespace details {

		template <typename RV, typename FV>
			struct func_type { typedef RV (*type)(Yielder<RV, FV>, FV); };

		template <typename RV>
			struct func_type<RV, void> { typedef RV (*type)(Yielder<RV, void>); };

	} // namespace details

	// use some signature rather
	// than RV/FV
	// compute automatically F with the signature.
	// else, use the fonctor.

	template<
		typename                RV    = void,
								typename                FV    = void,
								typename                F     = typename details::func_type<RV, FV>::type,
								size_t                  SSIZE = stack::default_size,
								template <size_t> class S     = stack::Default,
								template <class>  class C     = Context
									>
									class coroutine: public coroutine_base<RV, FV, coroutine<RV, FV, F, SSIZE, S, C> >
									{
										friend class coroutine_base<RV, FV, coroutine<RV, FV, F, SSIZE, S, C> >;

										public:
										typedef RV          return_t;
										typedef FV          feedval_t;
										typedef F           func_t;
										static const size_t ssize = SSIZE;
										typedef S<ssize>    stack_t;
										typedef C<stack_t>  context_t;
										typedef coroutine_base<RV, FV,
												coroutine<RV, FV, F, ssize, S, C> > parent_t;

										coroutine(func_t f):
											_context(&parent_t::bootstrap_trampoline, this),
											_func(f)
										{}

										private:
										context_t _context;
										func_t    _func;
									};

	template<
		typename RV = void,
				 typename FV = void,
				 typename F  = typename details::func_type<RV, FV>::type,
				 typename C  = Context<>
					 >
					 class coroutine2: public coroutine_base<RV, FV, coroutine2<RV, FV, F, C> >
					 {
						 friend class coroutine_base<RV, FV, coroutine2<RV, FV, F, C> >;

						 public:
						 typedef RV return_t;
						 typedef FV feedval_t;
						 typedef F  func_t;
						 typedef C  context_t;
						 typedef coroutine_base<RV, FV,
								 coroutine2<RV, FV, F, C> > parent_t;

						 coroutine2(func_t f):
							 _context(&parent_t::bootstrap_trampoline, this),
							 _func(f)
						 {}

						 private:
						 context_t _context;
						 func_t    _func;
					 };

} // namespace coroutine

#endif /* COROUTINE_H */
