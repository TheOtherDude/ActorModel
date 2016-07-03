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

#ifndef Actor_h
#define Actor_h

#include <string>
#include <thread>
#include "SharedQueue.h"

#include "ActorSystem.h"
#include "ActorRef.h"
#include "ActorMessage.h"

class ThreadActor;

/*
 * Actor
 * This is the base class for all actor objects in the system.
 * Every actor must inherit from this and override the receive() method.
 */
class Actor {
public:
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
            const ActorMessage* msg;
        } MailboxMessage_t;
    
        // Actor Message Queue
        SharedQueue<MailboxMessage_t> mailbox;
    
        // Used to set the actor system this actor is managed by
        virtual void setActorSystem(ActorSystem* actorSystem) final;
    
        // Called by ActorSystem to execute messages
        bool dequeueAndExecute(ThreadActor* thread, std::atomic<size_t>& messageCount, size_t& remaining, const size_t limit);
    
        // Internal actor send method
        virtual void send(const ActorRef& sender, const ActorMessage* msg) final;

    protected:
        // Actor receiver method
        virtual void receive(const ActorRef& sender, const ActorMessage* msg) = 0;
    
    public:
        // Constructor
        Actor();
    
        // Destructor
        virtual ~Actor();
    
        // Returns reference to self
        const virtual ActorRef self() final;
    
        // Gets the actor's id
        const std::string getActorId();
    
        // Gets the ActorSystem
        ActorSystem* getActorSystem();
};

#endif /* Actor_h */
