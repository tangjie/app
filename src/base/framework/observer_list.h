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

namespace base {
	template<typename ObserverType>
	class ObserverList : public noncopyable {
	public:
		typedef std::vector<ObserverType*> ListType;

		class Iterator {
		public:
			Iterator(std::shared_ptr<ObserverList<ObserverType>> &observers)
				: observers_(observers), index_(0) {
					observers_.lock()->notify_depth_++;
			}

			~Iterator() {
				if (observers_.lock() && --observers_.lock()->notify_depth_ == 0) {
					observers_.lock()->Compact();
				}
			}

			ObserverType* GetNext() {
				if (!observers_.lock()) {
					return nullptr;
				}
				size_t max_index = observers_.lock()->list_.size();
				while(index_ < max_index && observers_.lock()->list_[index_] == nullptr) {
					++index_;
				}
				return index_ < max_index ? observers_.lock()->list_[index_++] : nullptr;
			}

		private:
			size_t index_;
			// TODO: use weak_ptr take the place of reference.
			std::weak_ptr<ObserverList<ObserverType>> observers_;
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
					*iter = nullptr;
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
					*iter = nullptr;
				}else {
					list_.erase(iter);
				}
			}
		}
	private:
		// find items which assigned to nullptr and erase them;
		void Compact() {
			list_.erase(std::remove(list_.begin(), list_.end(), static_cast<ObserverType*>(nullptr)), list_.end());
		}

		ListType list_;
		int notify_depth_;
		friend class ObserverList::Iterator;
	};
}

// Note: observer_list parameter must be std::shared_ptr;
#define FOR_EACH_OBSERVER(ObserverType, observer_list, func)								\
	do {																														\
	base::ObserverList<ObserverType>::Iterator iter(observer_list);								\
	ObserverType *observer;																						\
	while((observer = iter.GetNext()) != nullptr) {															\
	observer->func;																									\
	}																															\
	}while(0)

#endif// BASE_FRAMEWORK_OBSERVER_LIST_H__