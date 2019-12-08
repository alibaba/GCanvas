//
// Created by mingzong on 2019-10-23.
//
#include "../../src/GCanvasManager.h"
#include "../../src/GCanvasWeex.hpp"

namespace gcanvas {
    void GCanvasManager::AddtoQueue(const std::string contextId, struct GCanvasCmd *p) {
        std::map<std::string, std::queue<struct GCanvasCmd *> *>::iterator it = mCmdQueue.find(
                contextId);
        if (it != mCmdQueue.end()) {
            std::queue<struct GCanvasCmd *> *queue = it->second;
            queue->push(p);
        } else {
            std::queue<struct GCanvasCmd *> *queue = new std::queue<struct GCanvasCmd *>;
            queue->push(p);
            mCmdQueue[contextId] = queue;
        }
    }

    std::queue<struct GCanvasCmd *> *
    GCanvasManager::getQueueByContextId(const std::string contextId) {
        std::map<std::string, std::queue<struct GCanvasCmd *> *>::iterator it = mCmdQueue.find(
                contextId);
        if (it != mCmdQueue.end()) {
            std::queue<struct GCanvasCmd *> *queue = it->second;
            it->second = 0;
            return queue;
        }

        return nullptr;
    }

    void GCanvasManager::clearQueue(std::queue<struct GCanvasCmd *> *queue) {
        if (queue != nullptr) {
            while (!queue->empty()) {
                struct GCanvasCmd *p = reinterpret_cast<struct GCanvasCmd *> (queue->front());
                queue->pop();
                delete p;
            }
        }
    }

    void GCanvasManager::clearQueueByContextId(const std::string contextId) {
        std::queue<struct GCanvasCmd *> *queue = this->getQueueByContextId(contextId);
        if (queue != nullptr) {
            clearQueue(queue);
        }
    }
}