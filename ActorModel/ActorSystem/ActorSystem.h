//
//  ActorSystem.hpp
//  ActorModel
//
//  Created by Matt on 7/1/16.
//  Copyright © 2016 Matt. All rights reserved.
//

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

class ActorSystem {
    public:
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
        std::vector<Shared_Queue<Actor*>> pendingJobs;
    
        // Condition variables indicating job ready
        std::vector<std::condition_variable> pendingJobCV;
    
        // Mutex for the condition variable
        std::vector<std::mutex> pendingJobMutex;
    
        // Mutex for creating an actor
        std::mutex createActorMutex;
    
        // Logging Actor
        Actor* logger;
    
        std::atomic<size_t> messageCount;
    
        // Main loop for the thread pool
        void threadPoolExecutor(const size_t threadId);
    
        // Called by an ActorRef to send a message
        void send(const ActorRef& sender, const ActorRef* receiver, const ActorMessage& msg);
    
    public:
        // Creates the actor system
        ActorSystem(const std::string name, size_t numThreads);
    
        ~ActorSystem();
    
        // Generates an actor id
        std::string generateActorId();
    
        // Creates an actor
        const ActorRef createActor(Actor* actor);
    
        // Returns a reference to the logging actor
        const ActorRef getLoggingActor();
};

#endif /* ActorSystem_h */
