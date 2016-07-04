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

#include "ScatterActor.h"

ScatterMsg::ScatterMsg(const Type type, uint64_t sum)
    : type(type), sum(sum) {
    
}

ScatterMsg::ScatterMsg(const Type type, const std::vector<uint64_t>& values)
    : type(type), values(values), sum(0) {
    
}

ScatterMsg::ScatterMsg(const Type type)
    : type(type), sum(0) {
    
}

ScatterActor::ScatterActor() : sum(0), recvCount(0) {

}

void ScatterActor::receive(const ActorRef& sender, const ActorMessage* msg) {
    // Cast message to ScatterMsg
    if (const ScatterMsg* scmsg = dynamic_cast<const ScatterMsg*>(msg)) {
        handleMessage(sender, scmsg);
    }
    else {
        handleError(sender, scmsg);
    }
}

void ScatterActor::handleMessage(const ActorRef& sender, const ScatterMsg* msg) {
    switch (msg->type) {
        case ScatterMsg::Type::INIT: {
            // Create worker actors
            for (size_t i=0; i<1000; i++) {
                actorRefs.push_back(getActorSystem()->createActor(new ScatterActor()));
            }
            
            // Generate some work
            realSum = 0;    // To check result
            std::vector<uint64_t> values;
            for (uint64_t i=0; i<1000000; i++) {
                values.push_back(i);
                realSum += i;
            }
            realSum *= actorRefs.size();
            
            // Send them work to do
            for (auto& ref : actorRefs) {
                ref.send(self(), new ScatterMsg(ScatterMsg::Type::SUM, values));
            }
            break;
        }
        // Worker actors do their own sum here
        case ScatterMsg::Type::SUM: {
            sum = 0;
            for (auto& i : msg->values) {
                sum += i;
            }
            sender.send(self(), new ScatterMsg(ScatterMsg::Type::GATHER, sum));
            break;
        }
        // Master actor gathers the result here
        case ScatterMsg::Type::GATHER: {
            recvCount++;
            sum += msg->sum;
            if (recvCount == actorRefs.size()) {
                getActorSystem()->getLoggingActor().send(self(), new StringMessage("Gather completed. Expected Result: " + std::to_string(realSum) + ", Result: " + std::to_string(sum)));
            }
            break;
        }
    }
}

void ScatterActor::handleError(const ActorRef& sender, const ActorMessage* msg) {
    getActorSystem()->getLoggingActor().send(self(), new StringMessage("Got invalid message from " + sender.actorId));
}