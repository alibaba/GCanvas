#include "gtimer_task.h"

using namespace gcanvas;

GTimerTask::GTimerTask(TimerAction* act) : NextExecutionTime(0), Period(0), state(VIRGIN) {
    pthread_mutex_init(&mutex, 0);
    action = act;
}

GTimerTask::~GTimerTask() {
    pthread_mutex_destroy(&mutex);
    action = nullptr;
}

bool GTimerTask::Cancel() {
    pthread_mutex_lock(&mutex);
    bool result = (state == SCHEDULED);
    state = CANCELLED;
    pthread_mutex_unlock(&mutex);
    return result;
}

millisec GTimerTask::ScheduledExecutionTime() {
    pthread_mutex_lock(&mutex);
    millisec result = (Period < 0 ? NextExecutionTime + Period
                              : NextExecutionTime - Period);
    pthread_mutex_unlock(&mutex);
    return result;
}

void GTimerTask::DoTask(void *timer) {
    action(timer);
}

void GTimerTask::Lock() {
    pthread_mutex_lock(&mutex);
}

void GTimerTask::Unlock() {
    pthread_mutex_unlock(&mutex);
}
