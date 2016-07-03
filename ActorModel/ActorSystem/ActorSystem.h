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

#ifndef ActorSystem_h
#define ActorSystem_h

#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <future>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "ActorRef.h"
#include "SharedQueue.h"
class Actor;
class LoggingActor;
class ThreadActor;

/*
 * ActorSystem
 * The ActorSystem manages the entire lifecycle of the actors
 * from creation and message routing to job scheduling.
 */
class ActorSystem {
    public:
        enum class ActorSystemStrategy {
            THROUGHPUT, // Prefers high throughput versus overall actor latency
            RESPONSIVE, // Prefers actor responsiveness over throughput
        };
    
        // Actor system name
        const std::string systemName;
    
    private:
        friend class ActorRef;
    
        // Actors mapping
        std::unordered_map<std::string, Actor*> actors;
    
        // Actor Id Counter
        std::atomic<size_t> actorIdCounter;
    
        // TODO: This is all pretty crude
        // Signal for the thread pool
        std::atomic<bool> running;
    
        // ActorSystem thread pool
        std::vector<std::thread> threadPool;
    
        // Job queues for the thread pool
        std::vector<SharedQueue<Actor*>> pendingJobs;
    
        // Condition variables indicating job ready
        std::vector<std::condition_variable> pendingJobCV;
    
        // Mutex for the condition variable
        std::vector<std::mutex> pendingJobMutex;
    
        // Mutex for creating an actor
        std::mutex createActorMutex;
    
        // Logging Actor
        Actor* loggingActor;
    
        // Limit on how many messages to execute per actor scheduling
        size_t batchExecutionLimit;
    
        std::atomic<size_t> messageCount;
    
        // Does some early actor system setup
        void initActorSystem(size_t numThreads, const ActorSystemStrategy strategy);
    
        // Initializes specific strategies for the actor system
        void initSystemProfile(const ActorSystemStrategy strategy);
    
        // Initializes the thread pool
        void initThreadPool(size_t numThreads);
    
        // Main loop for the thread pool
        void threadPoolExecutor(const size_t threadId);
    
        // Executes pending jobs a given thread
        void threadExecute(ThreadActor* threadActor, const size_t threadId);
    
        // Randomly schedules an actor to a thread
        void scheduleRandom(Actor* target);
    
        // Schedules an actor to a specific thread id
        void scheduleThread(size_t threadId, Actor* target);
    
        // Called by an ActorRef to send a message
        void routeMessage(const ActorRef& sender, const ActorRef* receiver, const ActorMessage* msg);
    
    public:
        // Creates the actor system and internal thread pool
        ActorSystem(const std::string name);
        ActorSystem(const std::string name, size_t numThreads);
        ActorSystem(const std::string name, const ActorSystemStrategy strategy);
    
        ~ActorSystem();
    
        // Generates an actor id
        std::string generateActorId();
    
        // Creates an actor
        const ActorRef createActor(Actor* actor);
    
        // Returns a reference to the logging actor
        const ActorRef getLoggingActor();
};

#endif /* ActorSystem_h */
