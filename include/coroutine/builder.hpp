/*
 * builder.hpp
 * Copyright © 2011 François-Xavier 'Bombela' Bourlet <bombela@gmail.com>
 *
*/

#pragma once
#ifndef BUILDER_H
#define BUILDER_H

#include <coroutine/coroutine.hpp>
#include <coroutine/best_context.hpp>

namespace coroutine {

	namespace details {

		template <template <typename> class PREDICATE,
			bool B, typename HEAD, typename... TAIL>
			struct find_if_iter;

		template <template <typename> class PREDICATE, typename... TAIL>
			struct find_if;

		template <template <typename> class PREDICATE,
				 typename HEAD, typename... TAIL>
			struct find_if<PREDICATE, HEAD, TAIL...>
			: find_if_iter<PREDICATE, PREDICATE<HEAD>::value, HEAD, TAIL...> {};

		template <template <typename> class PREDICATE,
			typename HEAD, typename... TAIL>
			struct find_if_iter<PREDICATE, true, HEAD, TAIL...> {
				typedef HEAD type;
			};

		template <template <typename> class PREDICATE,
			typename HEAD, typename... TAIL>
			struct find_if_iter<PREDICATE, false, HEAD, TAIL...>
				: find_if<PREDICATE, TAIL...> {};

		template <typename S>
			struct decay_sign_rm_ptr;

		template <typename R, typename... ARGS>
			struct decay_sign_rm_ptr<R (ARGS...)> {
				typedef R type (ARGS...);
			};

		template <typename R, typename... ARGS>
			struct decay_sign_rm_ptr<R (*)(ARGS...)> {
				typedef R type (ARGS...);
			};

		template <typename S>
			struct decay_sign_rm_yielder;

		template <typename R, typename A1, typename... ARGS>
			struct decay_sign_rm_yielder<R (yielder<R (A1)>, ARGS...)> {
				typedef R type (ARGS...);
			};

		template <typename R, typename... ARGS>
			struct decay_sign_rm_yielder<R (ARGS...)> {
				typedef R type (ARGS...);
			};

		template <typename S>
			struct decay_sign {
				typedef typename decay_sign_rm_ptr<S>::type sign;
				typedef typename decay_sign_rm_yielder<sign>::type type;
			};

	} // namespace details

	template <typename S, typename F, typename... CONFIGS>
		struct builder {
			typedef typename details::decay_sign<S>::type sign_t;
			typedef F func_t;

			typedef typename
				details::find_if<context::is, CONFIGS..., context::best>::type
				config_context_tag;

			typedef typename context::resolve_alias<config_context_tag>::type
				context_tag;

			typedef typename
				details::find_if<stack::is, CONFIGS...,
					typename context_tag::default_stack>::type
				stack_tag;

			typedef
				typename context_tag::template default_stack_size<stack_tag>
				default_stack_size_type;

			typedef typename
				details::find_if<stack::is_size, CONFIGS...,
					default_stack_size_type>::type
				stack_size_type;

			static const size_t stack_size = stack_size_type::value;
			typedef stack::stack<stack_tag, stack_size> stack_type;
			typedef context::context<context_tag, stack_type> context_type;

			typedef coroutine<sign_t, func_t, context_type> type;
		};

	template <typename S,
			 typename C0 = void,
			 typename C1 = void,
			 typename C2 = void,
			 typename C3 = void,
			 typename C4 = void,
			 typename C5 = void,
			 typename C6 = void,
			 typename C7 = void,
			 typename C8 = void,
			 typename C9 = void,
			 typename F>
		 auto coro(F f) -> typename
				 builder<S, F, C0, C1, C2, C3, C4, C5, C6, C7, C8, C9>::type
		 {
			 return typename
				 builder<S, F, C0, C1, C2, C3, C4, C5, C6, C7, C8, C9>::type(f);
		 }

	namespace details {
		template <typename S>
			struct extract_signature;

		template <typename Y, typename R>
			struct extract_signature<R (*)(Y)> {
				typedef R (type) ();
			};

		template <typename Y, typename R, typename FV>
			struct extract_signature<R (*)(Y, FV)> {
				typedef R (type) (FV);
			};

	} // namespace details

	template <
			 typename C0 = void,
			 typename C1 = void,
			 typename C2 = void,
			 typename C3 = void,
			 typename C4 = void,
			 typename C5 = void,
			 typename C6 = void,
			 typename C7 = void,
			 typename C8 = void,
			 typename C9 = void,
			 typename F>
		 auto corof(F f) -> typename
				 builder<typename details::extract_signature<F>::type, F,
		 C0, C1, C2, C3, C4, C5, C6, C7, C8, C9>::type
		 {
			 return typename
				 builder<typename details::extract_signature<F>::type, F,
			 C0, C1, C2, C3, C4, C5, C6, C7, C8, C9>::type(f);
		 }

} // namespace coroutine


#define COROL_DEF(...) \
	__VA_ARGS__ )

#define COROL(sign, ...) \
	::coroutine::coro<sign (__VA_ARGS__)>(\
			[=] (::coroutine::yielder<sign (__VA_ARGS__)> yield, ##__VA_ARGS__)\
			COROL_DEF


#endif /* BUILDER_H */
