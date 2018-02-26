#include "gtask_queue.h"

using namespace gcanvas;
using namespace std;


int GTaskQueue::GetSize() {
    return size;
}

GTaskQueue::GTaskQueue() : size(0) {
}

GTaskQueue::~GTaskQueue() {
    Clear();
}

void GTaskQueue::Add(GTimerTask *task) {
    if (size + 1 == QUEUE_SIZE) {
        return;
    }
    queue[++size] = task;
    FixUp(size);
}

GTimerTask *GTaskQueue::Get(int index) {
    if (index > size) {
        return NULL;
    }
    return queue[index];
}

void GTaskQueue::FixUp(int k) {
    while (k > 1) {
        int j = k >> 1;
        if (queue[j]->NextExecutionTime <= queue[k]->NextExecutionTime) {
            break;
        }
        GTimerTask *tmp = queue[j];
        queue[j] = queue[k];
        queue[k] = tmp;
        k = j;
    }
}

void GTaskQueue::FixDown(int k) {
    int j;
    while ((j = k << 1) <= size && j > 0) {
        if (j < size &&
            queue[j]->NextExecutionTime > queue[j + 1]->NextExecutionTime) {
            j++;
        }
        if (queue[k]->NextExecutionTime <= queue[j]->NextExecutionTime) {
            break;
        }
        GTimerTask *tmp = queue[j];
        queue[j] = queue[k];
        queue[k] = tmp;
        k = j;
    }
}

void GTaskQueue::Heapify() {
    for (int i = size / 2; i >= 1; i--) {
        FixDown(i);
    }
}

void GTaskQueue::Clear() {
    for (int i = 1; i <= size; i++) {
        if (queue[i] != nullptr) {
            delete queue[i];
            queue[i] = nullptr;
        }
    }
    size = 0;
}

bool GTaskQueue::IsEmpty() {
    return size == 0;
}

void GTaskQueue::RescheduleMin(long newTime) {
    queue[1]->NextExecutionTime = newTime;
    FixDown(1);
}

void GTaskQueue::QuickRemove(int index) {
    if (index > size) {
        return;
    }

    queue[index] = queue[size];
    if (nullptr != queue[size]) {
        delete queue[size];
        queue[size--] = nullptr;
    }
}

void GTaskQueue::RemoveMin() {
    if (nullptr != queue[1]) {
        delete queue[1];
        queue[1] = queue[size];
        queue[size--] = nullptr;
        FixDown(1);
    }
}

GTimerTask *GTaskQueue::GetMin() {
    return queue[1];
}
