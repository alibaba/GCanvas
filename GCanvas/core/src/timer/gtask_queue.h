#ifndef G_TASK_QUEUE_H
#define G_TASK_QUEUE_H

#include <vector>
#include <android/log.h>
#include "gtimer_task.h"

#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "gtimer", __VA_ARGS__))

using namespace std;

namespace gcanvas {
    class GTaskQueue {
    public:
        GTaskQueue();

        ~GTaskQueue();

        int GetSize();

        void Add(GTimerTask *task);

        GTimerTask *Get(int index);

        GTimerTask *GetMin();

        void RemoveMin();

        void QuickRemove(int index);

        void RescheduleMin(long newTime);

        bool IsEmpty();

        void Clear();

        void Heapify();

    private:
        static const int QUEUE_SIZE = 64;
        GTimerTask* queue[QUEUE_SIZE];
        int size;

        void FixUp(int k);

        void FixDown(int k);
    };
}
#endif