//
//  ActorRef.h
//  ActorModel
//
//  Created by Matt on 7/1/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#ifndef ActorRef_h
#define ActorRef_h

#include <string>

class ActorSystem;
class ActorMessage;

// Dummy actor reference paired by actor id
class ActorRef {
    public:
        // Actor ID
        const std::string actorId;
    
    private:
        friend class Actor;
    
        // Actor System Reference
        ActorSystem* actorSystem;
    
    public:
        ActorRef(const std::string id, ActorSystem* actorSystem);
    
        // Public actor send method
        void send(const ActorRef& sender, const ActorMessage& msg) const;
};

#endif /* ActorRef_h */
