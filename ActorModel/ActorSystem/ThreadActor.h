//
//  ThreadActor.hpp
//  ActorModel
//
//  Created by Matt on 7/2/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef ThreadActor_h
#define ThreadActor_h

#include <iostream>
#include "Actor.h"

// Internal actor used by ActorSystem
class ThreadActor : public Actor {
public:
    ThreadActor(const size_t threadId);
    
    const size_t threadId;
    
    void receive(const ActorRef& sender, ActorMessage& msg) override;
};

#endif /* ThreadActor_h */
