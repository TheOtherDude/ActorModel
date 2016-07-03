/*
 * Copyright (C) 2016  Matt Smith
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef SharedQueue_h
#define SharedQueue_h

#include <queue>
#include <mutex>

/*
 * SharedQueue
 * This is a lousy thread-safe queue.
 * Needs to be replaced with a lockfree one.
 */
template <typename T>
class SharedQueue {
private:
    std::queue<T> sharedQueue;
    std::mutex queueMutex;
    
public:
    SharedQueue() {
        
    }
    
    T pop() {
        try {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (!sharedQueue.empty()) {
                T result = sharedQueue.front();
                sharedQueue.pop();
                return result;
            }
            else {
                throw std::runtime_error("Shared Queue is Empty");
            }
        }
        catch (std::runtime_error e) {
            throw e;
        }
    }
    
    void push(T& input) {
        std::lock_guard<std::mutex> lock(queueMutex);
        sharedQueue.push(input);
    }
    
    bool isEmpty() {
        std::lock_guard<std::mutex> lock(queueMutex);
        return sharedQueue.empty();
    }
    
    size_t size() {
        return sharedQueue.size();
    }
};


#endif /* SharedQueue_h */