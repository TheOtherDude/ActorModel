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

#ifndef ScatterActor_h
#define ScatterActor_h

#include "Actor.h"
#include "StringMessage.h"

class ScatterMsg : public ActorMessage {
public:
    enum class Type {
        INIT,
        SUM,
        GATHER,
    };
    const Type type;
    const std::vector<uint64_t> values;
    uint64_t sum;
    
    ScatterMsg(const Type type, uint64_t sum);
    ScatterMsg(const Type type, const std::vector<uint64_t>& values);
    ScatterMsg(const Type type);
};

class ScatterActor : public Actor {
    private:
        uint64_t sum;
        uint64_t realSum;
        size_t recvCount;
        std::vector<ActorRef> actorRefs;
    
        void handleMessage(const ActorRef& sender, const ScatterMsg* msg);
        void handleError(const ActorRef& sender, const ActorMessage* msg);
    
    public:
        ScatterActor();
    
        void receive(const ActorRef& sender, const ActorMessage* msg) override;
};

#endif /* ScatterActor_h */
