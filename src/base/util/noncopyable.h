/*
 * This File define class noncopyable, which can help you to build noncopyable and nonassignable class.
 * see http://www.boost.org/libs/utility for detail.
 *
 * As noncopyable in boost is very wholesome, so here we borrow its implementation.
 */

#ifndef BASE_UTIL_NONCOPYABLE
#define BASE_UTIL_NONCOPYABLE

#include "boost/noncopyable.hpp"

namespace base {
	typedef boost::noncopyable noncopyable;
}

#endif // BASE_UTIL_NONCOPYABLE