//
//  TestActor.h
//  ActorModel
//
//  Created by Matt on 7/1/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef TestActor_h
#define TestActor_h

#include <iostream>
#include "Actor.h"

class TestActor : public Actor {
public:
    TestActor();
    size_t count = 0;
    size_t limit = 5;
    
    void receive(const ActorRef& sender, ActorMessage& msg) override;
};

#endif /* TestActor_h */
