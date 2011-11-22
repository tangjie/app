#include <assert.h>
#include <Windows.h>
#include "thread_local.h"

namespace base {
	namespace internal {
		void ThreadLocalImpl::AllocSlot(SlotType &slot) {
			slot = TlsAlloc();
			assert(slot != TLS_OUT_OF_INDEXES);
		}

		void ThreadLocalImpl::FreeSlot(SlotType &slot) {
			if (!TlsFree(slot)) {
				assert(false);
			}
		}

		void ThreadLocalImpl::SetValueInSlot(SlotType &slot, void *value) {
			if (!TlsSetValue(slot, value)) {
				assert(false);
			}
		}

		void* ThreadLocalImpl::GetValueFromSlot(SlotType &slot) {
			return TlsGetValue(slot);
		}
	}
}