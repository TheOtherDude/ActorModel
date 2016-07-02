//
//  TestActor2.h
//  ActorModel
//
//  Created by Matt on 7/1/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef TestActor2_h
#define TestActor2_h

#include <iostream>
#include "Actor.h"

class TestActor2 : public Actor {
public:
    TestActor2();
    size_t count = 0;
    size_t limit = 5;
    
    void receive(const ActorRef& sender, ActorMessage& msg) override;
};

#endif /* TestActor2_h */
