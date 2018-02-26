#ifndef G_TIMER_TASK_H
#define G_TIMER_TASK_H

#include <pthread.h>
#include <chrono>
#include <android/log.h>
#include "gtimer_types.h"

#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "gtimer", __VA_ARGS__))

namespace gcanvas {
    class GTimerTask {
        typedef short State;
    public:
        static const State VIRGIN = 0;
        static const State SCHEDULED = 1;
        static const State EXECUTED = 2;
        static const State CANCELLED = 3;

        GTimerTask(TimerAction* act);

        ~GTimerTask();

        millisec NextExecutionTime;

        void DoTask(void *timer);

        bool Cancel();

        millisec ScheduledExecutionTime();

        millisec Period;

        void Lock();

        void Unlock();

        State state;
    private:
        TimerAction* action;
        pthread_mutex_t mutex;
    };
}
#endif