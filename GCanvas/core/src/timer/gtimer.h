#ifndef G_TIMER_H
#define G_TIMER_H

#include <chrono>
#include <semaphore.h>
#include <pthread.h>
#include <exception>
#include <time.h>
#include <android/log.h>
#include <thread>
#include "gtask_queue.h"

#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "gtimer", __VA_ARGS__))

using namespace std;

namespace gcanvas {
    class GTimer {
    public:
        void Schedule(TimerAction task, millisec delay);

        void Schedule(TimerAction task, millisec delay, millisec period);

        void ScheduleAtFixedRate(TimerAction task, millisec delay, millisec period);

        void Destroy();

        void Start();

        void Stop();

        GTimer();

        ~GTimer();

    private:
        bool requestExit = false;

        bool isStarted = false;

        GTaskQueue *queue;

        sem_t time_control;

        pthread_t looper_thread;

        pthread_mutex_t main_lock;

        pthread_cond_t task_cond = PTHREAD_COND_INITIALIZER;

        void Sched(GTimerTask *task, millisec time, millisec period);

        static void *LoopThreadCallback(void *myself);

        void Loop();
    };
}
#endif