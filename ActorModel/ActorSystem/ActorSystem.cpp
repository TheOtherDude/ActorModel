//
//  ActorSystem.cpp
//  ActorModel
//
//  Created by Matt on 7/1/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "ActorSystem.h"
#include "Actor.h"
#include "LoggingActor.h"
#include "ThreadActor.h"

ActorSystem::ActorSystem(const std::string name, size_t numThreads) : systemName(name), actorIdCounter(0), running(true),
    pendingJobs(numThreads), pendingJobCV(numThreads), pendingJobMutex(numThreads), logger(nullptr), messageCount(0) {
    for (size_t i=0; i<numThreads; i++) {
        threadPool.push_back(std::thread(&ActorSystem::threadPoolExecutor, this, i));
    }
    srand(time(NULL));
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

// Main loop for the thread pool
void ActorSystem::threadPoolExecutor(const size_t threadId) {
    // Create a ThreadActor which is used to store thread information
    // when acquiring actor objects
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
            else {
                // Execute as many pending jobs as possible consecutively between cv waits
                while (!pendingJobs[threadId].isEmpty()) {
                    Actor* target = pendingJobs[threadId].pop();
                    
                    size_t remaining = 0;
                    if (target->dequeueAndExecute(threadActor, messageCount, remaining, 1)) {
                        // If any remaining, reschedule to run again
                        if (remaining > 0) {
                            pendingJobs[threadId].push(target);
                            pendingJobCV[threadId].notify_one();
                        }
                    }
                    else {
                        // Actor lock acquire failed so reschedule the job
                        ThreadActor* other = target->threadActor;
                        if (other == nullptr) {
                            // Random reschedule
                            const size_t otherId = rand() % threadPool.size();
                            pendingJobs[otherId].push(target);
                            pendingJobCV[otherId].notify_one();
                        }
                        else {
                            // Another thread is operating on this actor so give them the job
                            pendingJobs[other->threadId].push(target);
                            pendingJobCV[other->threadId].notify_one();
                        }
                    }
                }
            }
        }
        catch (std::runtime_error& e) {
            // TODO: Queue is empty, should be its own explicit exception
        }
    }
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
void ActorSystem::send(const ActorRef& sender, const ActorRef* receiver, const ActorMessage& msg) {
    if (receiver == nullptr) {
        throw std::runtime_error("Got null receiver reference in ActorSystem::send");
    }
    
    // Check if this is a valid actorid
    auto itr = actors.find(receiver->actorId);
    if (itr == actors.end()) {
        const std::string err = "Got a message for an invalid actor id in ActorSystem::send" + receiver->actorId;
        throw std::runtime_error(err);
    }
    
    // Put message in actor's mailbox
    itr->second->send(sender, msg);
    
    // Queue actor for execution
    // Doing random dispatch scheduling here.
    const size_t threadid = rand() % threadPool.size();
    pendingJobs[threadid].push(itr->second);
    pendingJobCV[threadid].notify_one();
}

// Generates a unique actor id
std::string ActorSystem::generateActorId() {
    size_t id = actorIdCounter++;
    std::string strid = systemName;
    strid += std::to_string(id);
    return strid;
}

// Returns an ActorRef to the logging actor
const ActorRef ActorSystem::getLoggingActor() {
    if (logger == nullptr) {
        logger = new LoggingActor();
        createActor(logger);
    }
    return logger->self();
}