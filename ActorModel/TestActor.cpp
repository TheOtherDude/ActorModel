//
//  TestActor.cpp
//  ActorModel
//
//  Created by Matt on 7/1/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "TestActor.h"

TestActor::TestActor() {
}

void TestActor::receive(const ActorRef& sender, ActorMessage& msg) {
    std::string logmsg = this->getActorId() + " got a message from " + sender.actorId + "!\n"
    + "Msg: " + msg.msg + "\n";
    //this->getActorSystem()->log(logmsg);
    
    /*
    if (count < limit) {
        sender.send(self(), ActorMessage("Message acknowledged from " + this->getActorId()));
        count++;
    }
    getActorSystem()->getLoggingActor().send(self(), logmsg);
    */
}