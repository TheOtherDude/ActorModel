//
//  LoggingActor.h
//  ActorModel
//
//  Created by Matt on 7/2/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef LoggingActor_h
#define LoggingActor_h

#include <iostream>
#include <ctime>
#include <cstring>
#include "Actor.h"

class LoggingActor : public Actor {
    public:
        LoggingActor();
    
        void receive(const ActorRef& sender, ActorMessage& msg) override;
};

#endif /* LoggingActor_h */
