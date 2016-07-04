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

#include "Actor.h"
#include "ThreadActor.h"
#include "StringMessage.h"

Actor::Actor() : actorSystem(nullptr), threadActor(nullptr) {
    
}

Actor::~Actor() {
    
}

// Called ActorSystem thread to acquire the actor's lock
bool Actor::acquireLock(ThreadActor* thread) {
    if (threadMutex.try_lock()) {
        // Set this actor's thread
        threadActor = thread;
        return true;
    }
    else {
        return false;
    }
}

// Called ActorSystem thread to release the actor's lock
void Actor::releaseLock() {
    threadMutex.unlock();
}

// Called by ActorSystem to execute messages
bool Actor::execute(ThreadActor* thread, std::atomic<size_t>& messageCount, size_t& remaining, const size_t limit) {
    // Execute messages
    // Some top scope variables for error resolution
    bool cleanupMessage = false;
    const ActorMessage* lastMsg = nullptr;
    
    try {
        cleanupMessage = false;
        lastMsg = nullptr;
        for (size_t i=0; (!mailbox.isEmpty() && (i<limit)); i++) {
            // Dequeue messages
            MailboxMessage_t msg = mailbox.pop();
            // Save this message in case of an exception so we don't leak
            lastMsg = msg.msg;
            if (msg.msg->opts == ActorMessage::MessageOpts::DELETE_AFTER_USE) {
                cleanupMessage = true;
            }
            
            // Execute actor instance
            receive(ActorRef(msg.senderId, actorSystem), msg.msg);
            
            // This is only here for science
            messageCount++;
            
            // If the message generator indicated cleanup now is the right time
            if (cleanupMessage) {
                delete msg.msg;
            }
        }
    }
    catch (SharedQueue<Actor*>::QueueEmpty& e) {}
    // Exception catch for the actor
    catch (std::exception& e) {
        const std::string err = "Thread-" + std::to_string(thread->threadId) + " Execution Error in " + actorId + ": " + e.what();
        getActorSystem()->getLoggingActor().send(thread->self(), new StringMessage(err));
        threadActor = nullptr;
        if (cleanupMessage && (lastMsg != nullptr)) {
            delete lastMsg;
        }
        return false;
    }
    
    threadActor = nullptr;
    remaining = mailbox.size();
    return true;
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

// Internal use actor send method. Use an ActorRef to properly send messages
void Actor::send(const ActorRef& sender, const ActorMessage* msg) {
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