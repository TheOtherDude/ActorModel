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
#include "ActorMessage.h"

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
                if (msg.msg->opts == ActorMessage::MessageOpts::DELETE_AFTER_USE) {
                    delete msg.msg;
                }
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