/*
 * StringPiece, 意为一段字符串。它像std::string一样，满足STL容器要求，并且提供与其相同的绝大多数（注意下面的注释）API。
 * 不过此处需注意的是StringPiece并不拥有字符串的所有权，StringPiece当然也不会对其做生命周期控制。
 *
 * 之所以要有StringPiece，主要是为了方便控制C-String，同时尽量减少C-String到std::string转换所带来的无谓复制（虽然依靠
 * 右值引用可以避免无谓的std::string到std::string的复制）。
 *
 * 此处需要注意到是：
 * (1)由于不管理字符串的生命周期，因此在使用StringPiece时需自己控制其生命周期（比如一般来讲函数内部生成的StringPiece是不
 *    宜作为返回值的)。
 * (2)由于标准库要求std::string::c_str()返回的必须是null-terminated string,并且StringPiece无法确定内部字符串是否在len_后
 *    紧接着一个'\0'，考虑到使用者可能的误用，因此StringPiece不提供c_str()方法，而是提供了一组替代方法(见下)
 * (3)虽然StringPiece“像std::string一样”，但它毕竟不是std::string，因此可能会修改到原字符串的长度的方法是不提供的(诸如
 *    resize/reserve/push_back/pop_back/insert/erase/clear/append等)，但是可以使用iterator或者operator[]等直
 *    接修改字符串内容，也可以使用replace等修改字符串内容（只要不会修改原始字符串长度）
 * (4)由于assign在此处具有二义性（是深拷贝还是浅拷贝？），因此不予以提供。请使用operator =（浅拷贝）或者copy（深拷贝）代替。
 * (5)由于我们并不使用异常，因此在std::string中会抛异常的方法（诸如at/replace等）在此处均不会抛出异常。
 * (6)考虑到简单性及通用性，StringPiece暂时先不使用allocate，因此std::string中allocate相关的函数在StringPiece中都不予提供。
 *
 * ref:(1) http://en.cppreference.com/w/cpp/string/basic_string
 *     (2) http://src.chromium.org/svn/trunk/src/base/string_piece.h
 *     (3) c++ standard 2003 [lib.strings]
 */

#ifndef BASE_STRING_STRING_PIECE_H__
#define BASE_STRING_STRING_PIECE_H__

#include <algorithm>
#include <cassert>
#include <string>
#include "base/string/string_piece_inl.h"

namespace base {
	template<typename C>
	class StringPieceT {
	public:
		typedef C value_type;
		typedef C* pointer;
		typedef C& reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef C* iterator;
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef const C* const_pointer;
		typedef const C& const_reference;
		typedef const C* const_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

		static const size_t npos = -1;

		//constructor
		//TODO(oldman):remove the dangerous const_cast
		StringPieceT():ptr_(nullptr), size_(0){
		}
		StringPieceT(const C* str, size_t size):ptr_(const_cast<C*>(str)), size_(size){
		}
		explicit StringPieceT(const C* str): ptr_(const_cast<C*>(str)), size_(internal::StringPieceTrait<C>::strlen(str)){
		}
		explicit StringPieceT(const std::basic_string<C>& str): size_(str.size()){
			//在C++03中，并没有一个确切的方法获取std::string内的buffer，但是下面这个方法应该是对现在所有编译器都正确的
			//TODO(oldman): add reference to above comments.
			ptr_ = const_cast<C*>(str.empty() ? nullptr : &str.front());
		}

		//iterator
		iterator begin() {
			return ptr_;
		}
		const_iterator begin() const {
			return ptr_;
		}
		iterator end() {
			return ptr_ + size();
		}
		const_iterator end() const {
			return ptr_ + size();
		}
		reverse_iterator rbegin() {
			return reverse_iterator(ptr_ + size());
		}
		const_reverse_iterator rbegin() const {
			return const_reverse_iterator(ptr_ + size());
		}
		reverse_iterator rend() {
			return reverse_iterator(ptr_);
		}
		const_reverse_iterator rend() const {
			return const_reverse_iterator(ptr_);
		}

		//size
		size_t size() const {
			return size_;
		}
		size_t length() const {
			return size_;
		}
		size_t max_size() const {
			return size_;
		}
		size_t capacity() const {
			return size_;
		}
		bool empty() const {
			return size_ == 0;
		}

		//element
		const C& operator[](size_t pos) const {
			assert(pos < size());
			return ptr_[pos];
		}
		C& operator[](size_t pos) {
			assert(pos < size());
			return ptr_[pos];
		}
		const C& at(size_t pos) const {
			return operator[](pos);
		}
		C& at(size_t pos) {
			return operator[](pos);
		}

		//modifier:replace
		//注意由于replace不能修改字符串长度，因此其原型与std::string略有不同
		//将从pos开始的内容替换成[str, str+min(len, size() - pos))
		StringPieceT& replace(size_t pos, const C* src, size_t len) {
			assert(pos < size());
			memcpy(ptr_ + pos, src, std::min(len, size() - pos));
			return *this;
		}
		//将[ptr_ + pos, ptr_ + min(pos+len, size()))全部替换成c
		StringPieceT& replace(size_t pos, size_t len, C c) {
			assert(pos < size());
			std::fill(ptr_ + pos, ptr_ + std::min(pos + len, size()), c);
			return *this;
		}
		//迭代器版本，含义同上
		StringPieceT& replace(const_iterator begin, const_iterator last, const C* src) {
			assert(begin < last && last <= end());
			memcpy(begin, src, last - begin);
			return *this;
		}
		StringPieceT& replace(const_iterator begin, const_iterator last, C c) {
			asseret(begin < last && last <= end());
			std::fill(begin, last, c);
			return *this;
		}

		//modifier:copy
		//把[ptr_ + pos, ptr_ + min(size(), pos+count))复制到dest中，返回实际复制字符数
		size_t copy(C* dest, size_t count, size_t pos = 0) {
			assert(pos < size());
			size_t n = std::min(size() - pos, count);
			memcpy(dest, ptr_ + pos, n);
			return n;
		}

		//c_str
		//正如文件头部所描述的那样，StringPiece不提供c_str()，而是提供下面一组函数
		std::basic_string<C> to_string() const {
			return std::basic_string<C>(ptr_, size());
		}
		void to_string(std::basic_string<C>& dest) const {
			dest.assign(ptr_, size());
		}
		const C* as_array() const {
			return ptr_;
		}
		C* as_array() {
			return ptr_;
		}
		//data
		const C* data() const{
			return ptr_;
		}
		//substr
		StringPieceT substr(size_t pos = 0, size_t count = npos) {
			assert(pos < size());
			size_t n = (count == npos) ? (size() - pos) : std::min(size() - pos, count);
			return StringPieceT(ptr_ + pos, n);
		}
		//swap
		void swap(StringPieceT& other) {
			StringPieceT r(other);
			other = *this;
			*this = r;
		}
		void swap(StringPieceT&& other) {
			*this = other;
			other = StringPieceT();
		}

		//string opertion: find
		size_t find(C c, size_t pos = 0) const {
			return find(&c, pos, 1);
		}
		size_t find(const C* s, size_t pos, size_t count) const {
			return find(StringPieceT(s, count), pos);
		}
		size_t find(const C* s, size_t pos = 0) const {
			//TODO(oldman):用一遍循环完成比较
			return find(StringPieceT(s), pos);
		}
		size_t find(const std::basic_string<C>& str, size_t pos = 0) const {
			return find(StringPieceT(str), pos);
		}
		template<typename To, typename From>
		inline static To implicit_cast(From const &f) {
			return f;
		}
		size_t find(const StringPieceT& str, size_t pos = 0) const{
			if (pos >= size()) {
				return npos;
			}
			const C* r = std::search(begin() + pos, end(), str.begin(), str.end());
			return r == end() ? npos : implicit_cast<size_t>(r - ptr_);
		}

		//string opertion: rfind
		size_t rfind(C c, size_t pos = npos) const {
			return rfind(&c, pos, 1);
		}
		size_t rfind(const C* s, size_t pos, size_t count) const {
			return rfind(StringPieceT(s, count), pos);
		}
		size_t rfind(const C* s, size_t pos = npos) const {
			//TODO(oldman):用一遍循环完成比较
			return rfind(StringPieceT(s), pos);
		}
		size_t rfind(const std::basic_string<C>& str, size_t pos = npos) const {
			return rfind(StringPieceT(str), pos);
		}
		size_t rfind(const StringPieceT& str, size_t pos = npos) const {
			size_t n = pos >= size() ? 0 : size() - pos;
			const_iterator it = std::find_end(begin(), end() - n, str.begin(), str.end());
			return it == end() - n ? npos : implicit_cast<size_t>(it - begin());
		}

		//string opertion: find_first_of
		size_t find_first_of(C c, size_t pos = 0) const {
			return find_first_of(&c, pos, 1);
		}
		size_t find_first_of(const C* s, size_t pos, size_t count) const {
			return find_first_of(StringPieceT(s, count), pos);
		}
		size_t find_first_of(const C* s, size_t pos = 0) const {
			//TODO(oldman):用一遍循环完成查找
			return find_first_of(StringPieceT(s), pos);
		}
		size_t find_first_of(const std::basic_string<C>& str, size_t pos = 0) const {
			return find_first_of(StringPieceT(str), pos);
		}
		size_t find_first_of(const StringPieceT& str, size_t pos = 0) const {
			for (const_iterator it = begin() + pos; it < end(); ++it) {
				if (str.find(*it) != npos) {
					return implicit_cast<size_t>(it - begin());
				}
			}
			return npos;
		}

		//string opertion: find_first_not_of
		size_t find_first_not_of(C c, size_t pos = 0) const {
			return find_first_not_of(&c, pos, 1);
		}
		size_t find_first_not_of(const C* s, size_t pos, size_t count) const {
			return find_first_not_of(StringPieceT(s, count), pos);
		}
		size_t find_first_not_of(const C* s, size_t pos = 0) const {
			//TODO(oldman):用一遍循环完成查找
			return find_first_not_of(StringPieceT(s), pos);
		}
		size_t find_first_not_of(const std::basic_string<C>& str, size_t pos = 0) const {
			return find_first_not_of(StringPieceT(str), pos);
		}
		size_t find_first_not_of(const StringPieceT& str, size_t pos = 0) const {
			for (const_iterator it = begin() + pos; it < end(); ++it) {
				if (str.find(*it) == npos) {
					return implicit_cast<size_t>(it - begin());
				}
			}
			return npos;
		}

		//string opertion: find_last_of
		size_t find_last_of(C c, size_t pos = 0) const {
			return find_last_of(&c, pos, 1);
		}
		size_t find_last_of(const C* s, size_t pos, size_t count) const {
			return find_last_of(StringPieceT(s, count), pos);
		}
		size_t find_last_of(const C* s, size_t pos = 0) const {
			//TODO(oldman):用一遍循环完成查找
			return find_last_of(StringPieceT(s), pos);
		}
		size_t find_last_of(const std::basic_string<C>& str, size_t pos = 0) const {
			return find_last_of(StringPieceT(str), pos);
		}
		size_t find_last_of(const StringPieceT& str, size_t pos = npos) const {
			for (const_reverse_iterator it = rbegin() + pos; it < rend(); ++it) {
				if (str.find(*it) != npos) {
					//since (&(*rbegin()) = (end()) - 1, we should correct this offset
					return implicit_cast<size_t>(it.base() -1 - begin());
				}
			}
			return npos;
		}

		//string opertion: find_last_not_of
		size_t find_last_not_of(C c, size_t pos = 0) const {
			return find_last_not_of(&c, pos, 1);
		}
		size_t find_last_not_of(const C* s, size_t pos, size_t count) const {
			return find_last_not_of(StringPieceT(s, count), pos);
		}
		size_t find_last_not_of(const C* s, size_t pos = 0) const {
			//TODO(oldman):用一遍循环完成查找
			return find_last_not_of(StringPieceT(s), pos);
		}
		size_t find_last_not_of(const std::basic_string<C>& str, size_t pos = 0) const {
			return find_last_not_of(StringPieceT(str), pos);
		}
		size_t find_last_not_of(const StringPieceT& str, size_t pos = npos) const {
			for (const_reverse_iterator it = rbegin() + pos; it < rend(); ++it) {
				if (str.find(*it) == npos) {
					return implicit_cast<size_t>(it.base() - begin());
				}
			}
			return npos;
		}

		//compare
		int compare(const C* s) const {
			//TODO(oldman):用一遍循环完成比较
			return compare(StringPieceT(s));
		}
		int compare(size_t pos1, size_t count1, const C* s) const {
			//TODO(oldman):用一遍循环完成比较
			return compare(pos1, count1, StringPieceT(s));
		}
		int compare(size_t pos1, size_t count1, const C* s, size_t count2) const {
			return compare(pos1, count1, StringPieceT(s, count2));
		}
		int compare(const std::basic_string<C>& str) const {
			return compare(StringPieceT(str.data(), str.size()));
		}
		int compare(size_t pos1, size_t count1, const std::basic_string<C>& str) const {
			return compare(pos1, count1, StringPieceT(str.data(), str.size()));
		}
		int compare(size_t pos1, size_t count1, const std::basic_string<C>& str, size_t pos2, size_t count2) const {
				return compare(pos1, count1, StringPieceT(str.data(), str.size()), pos2, count2);
		}
		int compare(const StringPieceT& str) const {
			return compare(0, str.size(), str);
		}
		int compare(size_t pos1, size_t count1, const StringPieceT& str) const {
			return compare(pos1, count1, str, 0, str.size());
		}
		int compare(size_t pos1, size_t count1, const StringPieceT& str, size_t pos2, size_t count2) const {
			assert (pos1 < size() && pos2 < str.size());
			size_t len1 = std::min(count1, size() - pos1);
			size_t len2 = std::min(count2, str.size() - pos2);
			int r = memcmp(data(), str.data(), std::min(len1, len2) * sizeof(C));
			return r != 0 ? r : len1 - len2;
		}

		//bool operator
		bool operator<(const StringPieceT& other) const {
			return compare(other) < 0;
		}
		bool operator>(const StringPieceT& other) const {
			return compare(other) > 0;
		}
		bool operator==(const StringPieceT& other) const {
			return compare(other) == 0;
		}
		bool operator<=(const StringPieceT& other) const {
			return !operator>(other);
		}
		bool operator>=(const StringPieceT& other) const {
			return !operator<(other);
		}
		bool operator!=(const StringPieceT& other) const {
			return !operator==(other);
		}
		bool operator<(const std::basic_string<C>& other) const {
			return compare(other) < 0;
		}
		bool operator>(const std::basic_string<C>& other) const {
			return compare(other) > 0;
		}
		bool operator==(const std::basic_string<C>& other) const {
			return compare(other) == 0;
		}
		bool operator<=(const std::basic_string<C>& other) const {
			return !operator>(other);
		}
		bool operator>=(const std::basic_string<C>& other) const {
			return !operator<(other);
		}
		bool operator!=(const std::basic_string<C>& other) const {
			return !operator==(other);
		}

	private:
		C* ptr_;
		size_t size_;
	};

	//StringPiece StringPiece16
	typedef StringPieceT<char> StringPiece;
	typedef StringPieceT<wchar_t> StringPiece16;

	//bool operator
	template <typename C>
	inline bool operator < (const std::basic_string<C>& s1, const StringPieceT<C>& s2) {
		return s2 > s1;
	}
	template <typename C>
	inline bool operator > (const std::basic_string<C>& s1, const StringPieceT<C>& s2) {
		return s2 < s1;
	}
	template <typename C>
	inline bool operator == (const std::basic_string<C>& s1, const StringPieceT<C>& s2) {
		return s2 == s1;
	}
	template <typename C>
	inline bool operator <= (const std::basic_string<C>& s1, const StringPieceT<C>& s2) {
		return s2 >= s1;
	}
	template <typename C>
	inline bool operator >= (const std::basic_string<C>& s1, const StringPieceT<C>& s2) {
		return s2 <= s1;
	}
	template <typename C>
	inline bool operator != (const std::basic_string<C>& s1, const StringPieceT<C>& s2) {
		return s2 != s1;
	}
}
#endif //#ifndef BASE_STRING_STRING_PIECE_H__