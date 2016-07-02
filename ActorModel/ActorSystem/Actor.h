//
//  Actor.h
//  ActorModel
//
//  Created by Matt on 7/1/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef Actor_h
#define Actor_h

#include <string>
#include <thread>
#include "SharedQueue.h"

#include "ActorSystem.h"
#include "ActorRef.h"
#include "ActorMessage.h"

class ThreadActor;

class Actor {
    private:
        friend class ActorSystem;
    
        // Reference to the actor system
        ActorSystem* actorSystem;
    
        // Actor ID
        std::string actorId;
    
        // Actor Thread Locking Mutex
        // Ensures only one thread ever runs the actor
        std::mutex threadMutex;
        std::atomic<ThreadActor*> threadActor;
    
        // Private mailbox for the actor
        typedef struct MailboxMessage_t {
            const std::string senderId;
            ActorMessage msg;
        } MailboxMessage_t;
    
        // Actor Message Queue
        Shared_Queue<MailboxMessage_t> mailbox;
    
        // Used to set the actor system this actor is managed by
        virtual void setActorSystem(ActorSystem* actorSystem) final;
    
        // Called by ActorSystem to execute messages
        bool dequeueAndExecute(ThreadActor* thread, std::atomic<size_t>& messageCount, size_t& remaining, const size_t limit);

    protected:
        // Actor receiver method
        virtual void receive(const ActorRef& sender, ActorMessage& msg) = 0;
    
    public:
        // Constructor
        Actor();
    
        virtual ~Actor();
    
        // Public actor send method
        virtual void send(const ActorRef& sender, const ActorMessage& msg) final;
    
        // Returns reference to self
        const virtual ActorRef self() final;
    
        // Gets the actor's id
        const std::string getActorId();
    
        // Gets the ActorSystem
        ActorSystem* getActorSystem();
};

#endif /* Actor_h */
