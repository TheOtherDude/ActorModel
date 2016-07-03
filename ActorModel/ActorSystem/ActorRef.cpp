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

#include "ActorRef.h"
#include "ActorSystem.h"
#include "ActorMessage.h"

ActorRef::ActorRef(const std::string id, ActorSystem* actorSystem) : actorId(id), actorSystem(actorSystem) {
    
}

ActorRef::ActorRef(const ActorRef* ref) : actorId(ref->actorId), actorSystem(ref->actorSystem) {
    if (ref == nullptr) {
        throw std::runtime_error("Got null pointer in ActorRef::ActorRef");
    }
}

// Public actor send method
void ActorRef::send(const ActorRef& sender, const ActorMessage* msg) const {
    actorSystem->send(sender, this, msg);
}

// Convenience Method (Send to self)
void ActorRef::send(const ActorMessage* msg) const {
    ActorRef self(this);
    actorSystem->send(self, this, msg);
}