//
//  ActorRef.cpp
//  ActorModel
//
//  Created by Matt on 7/1/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "ActorRef.h"
#include "ActorSystem.h"
#include "ActorMessage.h"

ActorRef::ActorRef(const std::string id, ActorSystem* actorSystem) : actorId(id), actorSystem(actorSystem) {
    
}

// Public actor send method
void ActorRef::send(const ActorRef& sender, const ActorMessage& msg) const {
    actorSystem->send(sender, this, msg);
}