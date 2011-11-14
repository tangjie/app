/*
 * An implemention of observer list,which can be modified during iteration without invalidating the iterator.
 * Modify from the observer_list.h in chrome source.
 */

#ifndef BASE_FRAMEWORK_OBSERVER_LIST_H__
#define BASE_FRAMEWORK_OBSERVER_LIST_H__

#include <algorithm>
#include <limits>
#include <vector>
#include "base/base_types.h"
#include "base/util/noncopyable.h"

template<typename ObserverType>
class ObserverList : public noncopyable {
public:
	typedef std::vector<ObserverType*> ListType;

	class Iterator {
	public:
		Iterator(ObserverList<ObserverType> &observers)
			: observers_(observers), index_(0) {
				observers_.notify_depth_++;
		}

		~Iterator() {
			if (--observers_.notify_depth_ == 0) {
				observers_.Compact();
			}
		}

		ObserverType* GetNext() {
			size_t max_index = observers_.list_.size();
			while(index_ < max_index && observers_.list_[index_] == NULL) {
				++index_;
			}
			return index_ < max_index ? observers_.list_[index_++] : NULL;
		}

	private:
		size_t index_;
		// TODO: use weak_ptr take the place of reference.
		ObserverList<ObserverType>& observers_;
	};

	ObserverList() : notify_depth_(0) {
	}

	~ObserverList() {
	}

	void AddObserver(ObserverType *observer) {
		if (std::find(list_.begin(), list_.end(), observer) == list_.end()) {
			list_.push_back(observer);
		}
	}

	void Clear() {
		if (notify_depth_ != 0) {
			for (ListType::iterator iter = list_.begin(); iter != list_.end(); iter++) {
				*iter = NULL;
			}
		}else {
			list_.clear();
		}
	}

	bool HasObserver(ObserverType *observer) {
		return std::find(list_.begin(), list_.end(), observer) != list_.end();
	}

	void RemoveObserver(ObserverType *observer) {
		ListType::iterator iter = std::find(list_.begin(), list_.end(), observer);
		if (iter != list_.end()) {
			if (notify_depth_ != 0) {
				*iter = NULL;
			}else {
				list_.erase(iter);
			}
		}
	}
private:
	// find items which assigned to NULL and erase them;
	void Compact() {
		list_.erase(std::remove(list_.begin(), list_.end(), static_cast<ObserverType*>(NULL)), list_.end());
	}

	ListType list_;
	int notify_depth_;
	friend class ObserverList::Iterator;
};

#define FOR_EACH_OBSERVER(ObserverType, observer_list, func) \
	do {																							\
		ObserverList<ObserverType>::Iterator iter(observer_list);		\
		ObserverType *observer;														\
		while((observer = iter.GetNext()) != NULL) {							\
			observer->func;																\
		}																							\
	}while(0)

#endif// BASE_FRAMEWORK_OBSERVER_LIST_H__