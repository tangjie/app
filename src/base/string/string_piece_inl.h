/*
 * StringPiece实现细节，主要是StringPieceTrait，目前该Trait只提供计算字符串长度的方法。
 */

#ifndef BASE_STRING_STRING_PIECE_INL_H__
#define BASE_STRING_STRING_PIECE_INL_H__

#include <cstring>

namespace base {
	namespace internal{
		template <typename C>
		struct StringPieceTrait {
			static size_t strlen(const C* str);
		};
		template <>
		struct StringPieceTrait<char> {
			static size_t strlen(const char* str) {
				return str == nullptr ? 0 : ::strlen(str);
			}
		};
		template<>
		struct StringPieceTrait<wchar_t> {
			static size_t strlen(const wchar_t* str) {
				return str == nullptr ? 0 : ::wcslen(str);
			}
		};
	}
}


#endif //#ifndef BASE_STRING_STRING_PIECE_INL_H__