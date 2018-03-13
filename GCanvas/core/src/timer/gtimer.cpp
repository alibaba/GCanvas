#include "gtimer.h"

using namespace gcanvas;
using namespace std;
using namespace chrono;

void GTimer::Schedule(TimerAction task, millisec delay) {
    if (delay < 0) {
        LOGE("Non-positive delay: %d", delay);
        return;
    };
    Sched(new GTimerTask(task),
          duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() +
          delay, 0);
}

void GTimer::Schedule(TimerAction task, millisec delay, millisec period) {
    if (delay < 0) {
        LOGE("Non-positive delay: %d", delay);
        return;
    };
    if (period <= 0) {
        LOGE("Non-positive period: %d", period);
        return;
    }

    Sched(new GTimerTask(task),
          duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() +
          delay,
          -period);
}

void GTimer::ScheduleAtFixedRate(TimerAction task, millisec delay, millisec period) {
    if (delay < 0) {
        LOGE("Non-positive delay: %d", delay);
        return;
    };
    if (period <= 0) {
        LOGE("Non-positive period: %d", period);
        return;
    }
    Sched(new GTimerTask(task),
          duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() +
          delay,
          period);
}

GTimer::GTimer() {
    queue = new GTaskQueue();
    sem_init(&time_control, 0, 0);
    pthread_mutex_init(&main_lock, 0);
}

GTimer::~GTimer() {
    requestExit = true;
    pthread_cond_signal(&task_cond);
    if (nullptr != queue) {
        delete queue;
        queue = nullptr;
    }
    pthread_cond_destroy(&task_cond);
    pthread_mutex_destroy(&main_lock);
    sem_destroy(&time_control);
    pthread_join(looper_thread, 0);
}

void GTimer::Sched(GTimerTask *task, millisec time, millisec period) {
    if (time < 0 || task == 0) {
        LOGE("Illegal execution time: %d", time);
        return;
    }

    if (requestExit) {
        return;
    }

    pthread_mutex_lock(&main_lock);

    task->Lock();
    if (task->state != GTimerTask::VIRGIN) {
        pthread_mutex_unlock(&main_lock);
        return;
    }
    task->NextExecutionTime = time;
    task->Period = period;
    task->state = GTimerTask::SCHEDULED;
    task->Unlock();

    queue->Add(task);
    if (queue->GetMin() == task) {
        pthread_cond_signal(&task_cond);
    }

    if (!isStarted) {
        Start();
    }
    pthread_mutex_unlock(&main_lock);
}


void *GTimer::LoopThreadCallback(void *myself) {
    GTimer *timer = (GTimer *) myself;
    timer->Loop();
    pthread_exit(0);
}

void GTimer::Loop() {
    while (!requestExit) {
        GTimerTask *task;
        bool taskFired;

        pthread_mutex_lock(&main_lock);
        while (!requestExit && queue->IsEmpty()) {
            pthread_cond_wait(&task_cond, &main_lock);
        }

        if (requestExit) {
            pthread_mutex_unlock(&main_lock);
            break;
        }

        long currentTime, executionTime;
        task = queue->GetMin();
        task->Lock();
        if (task->state == GTimerTask::CANCELLED) {
            queue->RemoveMin();
            task->Unlock();
            pthread_mutex_unlock(&main_lock);
            continue;
        }
        currentTime = duration_cast<milliseconds>(
                high_resolution_clock::now().time_since_epoch()).count();
        executionTime = task->NextExecutionTime;
        taskFired = (executionTime <= currentTime);
        if (taskFired) {
            if (task->Period == 0) {
                queue->RemoveMin();
                task->state = GTimerTask::EXECUTED;
            } else {
                queue->RescheduleMin(task->Period < 0
                                     ? currentTime - task->Period
                                     : executionTime + task->Period);
            }
        }
        task->Unlock();
        pthread_mutex_unlock(&main_lock);

        if (requestExit) {
            break;
        }

        if (!taskFired) {
            std::this_thread::sleep_for(milliseconds(executionTime - currentTime));
        } else {
            task->DoTask(this);
        }
    }
}

void GTimer::Destroy() {
    pthread_mutex_lock(&main_lock);
    queue->Clear();
    requestExit = true;
    pthread_cond_signal(&task_cond);
    pthread_mutex_unlock(&main_lock);
}

void GTimer::Start() {
    pthread_mutex_lock(&main_lock);
    requestExit = false;
    isStarted = true;
    pthread_create(&looper_thread, 0, LoopThreadCallback, this);
    pthread_mutex_unlock(&main_lock);
}

void GTimer::Stop() {
    pthread_mutex_lock(&main_lock);
    requestExit = true;
    isStarted = false;
    pthread_cond_signal(&task_cond);
    pthread_mutex_unlock(&main_lock);
    pthread_join(looper_thread, 0);
}
