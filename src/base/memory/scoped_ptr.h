/*
 * scoped_ptr, means a pointer used in some scope. it's like std::auto_ptr, and safer than the later
 * since the former should not be copied.
 * see http://www.boost.org/doc/libs/1_47_0/libs/smart_ptr/scoped_ptr.htm for detail.
 *
 * Since scoped pointer is a handy tool, so we don't put it into base namespace.
 *
 * NOTE: scoped_ptr is different form std::unique_ptr since the former is only designed for scoped usability.
 *       if you want to put your pointer into STL container, please consider std::unique_ptr or std::shared_ptr.
 */

/*
Synopsis (modify from boost document)

  template<class T> class scoped_ptr : noncopyable {

   public:
     typedef T element_type;

     explicit scoped_ptr(T * p = 0); // never throws
     ~scoped_ptr(); // never throws

     void reset(T * p = 0); // never throws
     T* release(); // never throws

     T & operator*() const; // never throws
     T * operator->() const; // never throws
     T * get() const; // never throws

     void swap(scoped_ptr & b); // never throws

     bool operator==(T* p) const; // never throws
     bool operator!=(T* p) const; // never throws
	 bool operator==(const scoped_ptr&) const = delete;
	 bool operator!=(const scoped_ptr&) const = delete;
  };

  template<class T> void swap(scoped_ptr<T> & a, scoped_ptr<T> & b); // never throws
  template<class T> bool operator==(T* p1, const scoped_ptr<T>& p2); // never throws
  template<class T> bool operator!=(T* p1, const scoped_ptr<T>& p2); // never throws
*/


//Although delete a incomplete class pointer is undefined as c++ standard, vc complier can report it as error,
//so here we don't check whether the class is complete or not.
//Ref: http://en.wikibooks.org/w/index.php?title=More_C%2B%2B_Idioms/Checked_delete
//     http://www.boost.org/doc/libs/1_47_0/libs/utility/checked_delete.html

#ifndef BASE_MEMORY_SCOPED_PTR
#define BASE_MEMORY_SCOPED_PTR

#include "base/util/noncopyable.h"

template <typename T>
class scoped_ptr : public base::noncopyable {
public:
	//element type, it isn't omitted. see Synopsis for detail.
	typedef T element_type;

	//Constructor. Note the explicit key word
	explicit scoped_ptr(T *p = nullptr) : ptr_(p) {}

	//Destructor. Don't need to test ptr_ == nullptr, because C++ does it for us.
	~scoped_ptr() {
		delete ptr_;
	}

	//Reset. Delete the current pointer and take ownership of the new pointer.
	void reset(T *p = nullptr) {
		if (p != ptr_) {
			delete ptr_;
			ptr_ = p;
		}
	}

	//Get. Return current pointer.
	T* get() const {
		return ptr_;
	}

	//Release. release the ownership of current pointer, it equals get() plus reset(nullptr)
	T* release() {
		T* p = ptr_;
		ptr_ = nullptr;
		return p;
	}

	//operator* and operator-> make you use scoped_ptr<T> like T*
	T& operator*() const {
		return *ptr_;
	}
	T * operator->() const {
		return ptr_;
	}

	// Comparison operators.
	bool operator==(T* p) const {
		return ptr_ == p;
	}
	bool operator!=(T* p) const {
		return ptr_ != p;
	}

	// Swap.
	void swap(scoped_ptr & b) {
		T* p = ptr_;
		ptr_ = b.ptr_;
		b.ptr_ = p;
	}

private:
	//Pointer.
	T* ptr_;

	// Comparison operators. it doesn't make sense to compare two scoped pointer no matter T2 == T or not.
	template <class T2> bool operator==(const scoped_ptr<T2>&) const;
	template <class T2> bool operator!=(const scoped_ptr<T2>&) const;
};

template<class T>
void swap(scoped_ptr<T>& a, scoped_ptr<T>& b) {
	a.swap(b);
}

template<class T>
bool operator==(T* p1, const scoped_ptr<T>& p2) {
	return p1 == p2.get();
}

template<class T>
bool operator!=(T* p1, const scoped_ptr<T>& p2) {
	return p1 != p2.get();
}

#endif // #ifndef BASE_MEMORY_SCOPED_PTR