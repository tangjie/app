/*
 * This file use to implement UI message pump
 */

#ifdef BASE_FRAMEWORK_UI_MESSAGE_PUMP_H__
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

    class UIListener {
    public:
      virtual void PreProcessMessage(const MSG &msg) = 0;
      virtual void PostProcessMessage(const MSG &msg) = 0;
    };

    void AddUIListener(UIListener *listener);
    void RemoveListener(UIListener *listener);
    void RunWithDispatcher(Delegate *delegate, Dispatcher *dispatcher);
    virtual void Run(Delegate *delegate);
    virtual void Quit();
    virtual void DispatchWork();
    virtual void DispatchDelayWork(const TimeTicks &next_time);
    UIMessagePump() : have_work_(0), state_(NULL) {
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
    int GetCurrentDelay();
    void DoRunLoop();
    void InitMessageWnd();
    void HandleWorkMessage();
    void HandleTimerMessage();

    RunState* state_;
    long have_work_;
    TimeTicks next_time_;
    HWND hwnd_;
    ListenerList<UIListener> ui_listeners_;
  };
}

#endif// BASE_FRAMEWORK_UI_MESSAGE_PUMP_H__