/*
 * This file use to implement UI message pump
 */

#ifndef BASE_FRAMEWORK_UI_MESSAGE_PUMP_H__
#define BASE_FRAMEWORK_UI_MESSAGE_PUMP_H__


#include <Windows.h>
#include "base/framework/message_pump.h"
#include "base/time/time.h"

namespace base {
  class UIMessagePump : public MessagePump {
  public:
    class Dispatcher {
    public:
      virtual ~Dispatcher() {
      }

      virtual bool Dispatch(const MSG &msg) = 0;
    };

    class UIObserver {
    public:
      virtual void PreProcessMessage(const MSG &msg) = 0;
      virtual void PostProcessMessage(const MSG &msg) = 0;
    };

    void AddUIObserver(UIObserver *listener);
    void RemoveUIObserver(UIObserver *listener);
	// Start UI message punp.
    void RunWithDispatcher(Delegate *delegate, Dispatcher *dispatcher);
    virtual void Run(Delegate *delegate);
    virtual void Quit();
    virtual void DispatchWork();
    virtual void DispatchDelayWork(const TimeTicks &delayed_work_time);
    UIMessagePump() : have_work_(0), state_(nullptr) {
    }

    virtual ~UIMessagePump() {
    }

  private:
    struct RunState {
      int run_depth_;
      bool should_quit_;
      Delegate *delegate_;
      Dispatcher *dispatcher_;
    };

    static LRESULT WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
    int GetCurrentDelay() const;
    void DoRunLoop();
    void InitMessageWnd();
    void HandleWorkMessage();
    void HandleTimerMessage();

    RunState* state_;
    long have_work_;
    TimeTicks delayed_work_time_;
    HWND hwnd_;
    ObserverList<UIObserver> ui_observers_;
  };
}

#endif// BASE_FRAMEWORK_UI_MESSAGE_PUMP_H__