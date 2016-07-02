//
//  Actor.cpp
//  ActorModel
//
//  Created by Matt on 7/1/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "Actor.h"
#include "ThreadActor.h"

Actor::Actor() : actorSystem(nullptr), threadActor(nullptr) {
    
}

Actor::~Actor() {
    
}

// Called by ActorSystem to execute messages
bool Actor::dequeueAndExecute(ThreadActor* thread, std::atomic<size_t>& messageCount, size_t& remaining, const size_t limit) {
    // Acquire the actor's lock
    std::unique_lock<std::mutex> lock(threadMutex, std::defer_lock);
    if (lock.try_lock()) {
        // Set this actor's thread
        threadActor = thread;
        
        // Execute messages
        try {
            for (size_t i=0; i<limit; i++) {
                MailboxMessage_t msg = mailbox.pop();
                receive(ActorRef(msg.senderId, actorSystem), msg.msg);
                messageCount++;
            }
        }
        catch (std::runtime_error& e) {
            // Queue empty
        }
        
        threadActor = nullptr;
        remaining = mailbox.size();
        return true;
    }
    else {
        // Lock acquire failed
        return false;
    }
}

// Should only be called by ActorSystem on object creation
void Actor::setActorSystem(ActorSystem* actorSystem) {
    // Ignore attempts to change this actors system
    if (this->actorSystem != nullptr) {
        throw std::runtime_error("Ignored attempt to change actorSystem in Actor::setActorSystem");
    }
    else {
        this->actorSystem = actorSystem;
        this->actorId = actorSystem->generateActorId();
    }
}

// Public actor send method
void Actor::send(const ActorRef& sender, const ActorMessage& msg) {
    if (sender.actorSystem->systemName.compare(this->actorSystem->systemName)) {
        const std::string err = "Got message from a different actor system in: " + this->actorId;
        throw std::runtime_error(err);
    }
    MailboxMessage_t packedMsg = { sender.actorId, msg };
    mailbox.push(packedMsg);
}

// Returns reference to self
const ActorRef Actor::self() {
    return ActorRef(actorId, actorSystem);
}

// Gets the Actor's ID
const std::string Actor::getActorId() {
    return this->actorId;
}

// Gets the ActorSystem
ActorSystem* Actor::getActorSystem() {
    return this->actorSystem;
}