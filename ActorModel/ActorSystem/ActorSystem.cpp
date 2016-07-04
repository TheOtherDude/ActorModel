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

#include "ActorSystem.h"
#include "Actor.h"
#include "LoggingActor.h"
#include "ThreadActor.h"

ActorSystem::ActorSystem(const std::string name)
    : systemName(name), actorIdCounter(0), running(true), pendingJobs(std::thread::hardware_concurrency()),
    pendingJobCV(std::thread::hardware_concurrency()), pendingJobMutex(std::thread::hardware_concurrency()), loggingActor(nullptr), messageCount(0){
    
    initActorSystem(std::thread::hardware_concurrency(), ActorSystemStrategy::RESPONSIVE);
}

ActorSystem::ActorSystem(const std::string name, size_t numThreads)
    : systemName(name), actorIdCounter(0), running(true), pendingJobs(numThreads),
    pendingJobCV(numThreads), pendingJobMutex(numThreads), loggingActor(nullptr), messageCount(0) {
    
    initActorSystem(numThreads, ActorSystemStrategy::RESPONSIVE);
}

ActorSystem::ActorSystem(const std::string name, const ActorSystemStrategy strategy)
    : systemName(name), actorIdCounter(0), running(true), pendingJobs(std::thread::hardware_concurrency()),
    pendingJobCV(std::thread::hardware_concurrency()), pendingJobMutex(std::thread::hardware_concurrency()), loggingActor(nullptr), messageCount(0){
    
    initActorSystem(std::thread::hardware_concurrency(), strategy);
}

ActorSystem::~ActorSystem() {
    std::cout << "[ActorSystem::" + systemName + "] shutting down...\n";
    // TODO: Needs timeouts for stalled threads?
    running = false;
    
    // Notify each thread
    for (auto& cv : pendingJobCV) {
        cv.notify_all();
    }
    
    // Join all threads
    for (auto& thr : threadPool) {
        if (thr.joinable()) {
            thr.join();
        }
    }
    
    // Cleanup Actors
    for (auto& actr : actors) {
        if (actr.second != nullptr) {
            delete actr.second;
            actr.second = nullptr;
        }
    }
    std::cout << "[ActorSystem::" + systemName + "] shut down successfully.\nNumber of messages sent: " + std::to_string(messageCount) + "\n";
}

// Does some early actor system setup
void ActorSystem::initActorSystem(size_t numThreads, const ActorSystemStrategy strategy) {
    srand((unsigned int)time(NULL));
    initSystemProfile(strategy);
    initThreadPool(numThreads);
}

// Initializes specific strategies for the actor system
void ActorSystem::initSystemProfile(const ActorSystemStrategy strategy) {
    // Initiate specific strategy settings
    switch (strategy) {
        case ActorSystemStrategy::RESPONSIVE: {
            batchExecutionLimit = 1;
        }
        case ActorSystemStrategy::THROUGHPUT: {
            batchExecutionLimit = 100;
        }
    }
}

// Initializes the thread pool
void ActorSystem::initThreadPool(size_t numThreads) {
    if (numThreads == 0) {
        throw std::runtime_error("Got 0 threads as a parameter in ActorSystem::initThreadPool");
    }
    
    // Activate thread pool
    for (size_t i=0; i<numThreads; i++) {
        threadPool.push_back(std::thread(&ActorSystem::threadPoolExecutor, this, i));
    }
}

// Main loop for the thread pool
void ActorSystem::threadPoolExecutor(const size_t threadId) {
    // Create a ThreadActor which is used to store thread information
    // when acquiring actor objects as well as send messages
    Actor* threadBoxed = actors.find(createActor(new ThreadActor(threadId)).actorId)->second;
    ThreadActor* threadActor = (ThreadActor*)threadBoxed;
    
    while (running) {
        try {
            // Wait on condition variable
            std::unique_lock<std::mutex> lock(pendingJobMutex[threadId]);
            pendingJobCV[threadId].wait(lock);
            
            // If signaled to stop running then exit
            if (!running) {
                return;
            }
            
            // Execute pending jobs
            threadExecute(threadActor, threadId);
        }
        //catch (SharedQueue<Actor*>::QueueEmpty& e) {}
        catch (std::exception& e) {
            // TODO: Kill and restart thread etc or just crash.
            const std::string err = "Thread-" + std::to_string(threadActor->threadId) + " Execution Error in threadPoolExecutor: " + e.what();
            getLoggingActor().send(threadActor->self(), new StringMessage(err));
        }
    }
}

// Executes pending jobs for a given thread
void ActorSystem::threadExecute(ThreadActor* threadActor, const size_t threadId) {
    // Execute as many pending jobs as possible consecutively between cv waits
    while (!pendingJobs[threadId].isEmpty()) {
        Actor* target = pendingJobs[threadId].pop();
        if (target == nullptr) {
            // Assume the actor was destroyed
            continue;
        }
        
        // Get the actor's lock
        if (target->acquireLock(threadActor)) {
            // Execute messages up to the batch count
            size_t messagesRemaining = 0;
            if (!target->execute(threadActor, messageCount, messagesRemaining, batchExecutionLimit)) {
                // Execution error occurred
                target->releaseLock();
                // TODO: Delete the actor etc. It's in a bad state
                continue;
            }
            
            // If there are any messages remaining, reschedule the actor to run again
            if (messagesRemaining > 0) {
                scheduleThread(threadId, target);
            }
            target->releaseLock();
        }
        else {
            // Lock acquire failed so reschedule the actor
            ThreadActor* other = target->threadActor;
            if (other == nullptr) {
                // Random reschedule
                scheduleRandom(target);
            }
            else {
                // Another thread is operating on this actor so give them the job
                scheduleThread(other->threadId, target);
            }
        }
    }
}

// Randomly schedules an actor to a thread
void ActorSystem::scheduleRandom(Actor* target) {
    const size_t otherId = rand() % threadPool.size();
    pendingJobs[otherId].push(target);
    pendingJobCV[otherId].notify_one();
}

// Schedules an actor to a specific thread id
void ActorSystem::scheduleThread(size_t threadId, Actor* target) {
    pendingJobs[threadId].push(target);
    pendingJobCV[threadId].notify_one();
}

// Creates an actor in the actor system
const ActorRef ActorSystem::createActor(Actor* actor) {
    std::lock_guard<std::mutex> lock(createActorMutex);
    if (actor == nullptr) {
        throw std::runtime_error("Failed to create actor, null pointer received in ActorSystem::createActor");
    }
    actor->setActorSystem(this);
    actors.emplace(actor->actorId, actor);
    return actor->self();
}

// ActorSystem message receiver which then routes to actor mailboxes
void ActorSystem::routeMessage(const ActorRef& sender, const ActorRef* receiver, const ActorMessage* msg) {
    if (receiver == nullptr) {
        throw std::runtime_error("Got null receiver reference in ActorSystem::routeMessage");
    }
    
    // Check if this is a valid actorid
    auto itr = actors.find(receiver->actorId);
    if (itr == actors.end()) {
        const std::string err = "Got a message for an invalid actor id in ActorSystem::routeMessage" + receiver->actorId;
        throw std::runtime_error(err);
    }
    
    // Put message in actor's mailbox
    itr->second->send(sender, msg);
    
    // Queue actor for execution
    // Doing random dispatch scheduling here.
    scheduleRandom(itr->second);
}

// Generates a unique actor id
std::string ActorSystem::generateActorId() {
    size_t id = actorIdCounter++;
    std::string strid = systemName + "-actor-";
    strid += std::to_string(id);
    return strid;
}

// Returns an ActorRef to the logging actor
const ActorRef ActorSystem::getLoggingActor() {
    // Creation deferred because GCC complained heavily about
    // calling new in constructor initializer lists
    if (loggingActor == nullptr) {
        loggingActor = new LoggingActor();
        createActor(loggingActor);
    }
    return loggingActor->self();
}