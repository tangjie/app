/*
 * an implemention of listener list
 */

#ifdef BASE_FRAMEWORK_LISTENER_LIST_H__
#define BASE_FRAMEWORK_LISTENER_LIST_H__

#include <assert.h>
#include <vector>

class DelayEraser {
public:
  DelayEraser() : delay_eraser_(0) {

  }

  int AquireDelayEraser() {
    return ++delay_eraser_;
  }

  int ReleaseDelayEraser() {
    assert(delay_eraser_ > 0);
    return --delay_eraser_;
  }

  int 
private:
  int delay_eraser_;
};

#endif// BASE_FRAMEWORK_LISTENER_LIST_H__