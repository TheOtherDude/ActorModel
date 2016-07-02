//
//  main.cpp
//  ActorModel
//
//  Created by Matt on 7/1/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include <iostream>
#include <thread>
#include <atomic>
#include <cstdlib>
#include <ctime>
#include "ActorSystem.h"
#include "TestActor.h"
#include "TestActor2.h"

std::atomic<size_t> numMessagesSent;

// Randomly picks a source and dest actor and sends a message between them
// Does 2 per iteration (one for TestActor one for TestActor2 classes)
void generateMessages(const std::vector<ActorRef> actorRefs, size_t amount) {
    const std::string msg = "This is a test of the actor system";
    for (size_t i=0; i<amount/2; i++) {
        int target1 = rand() % actorRefs.size();
        int target2 = rand() % actorRefs.size();
        actorRefs[target1].send(actorRefs[target2], ActorMessage(msg));
        actorRefs[target2].send(actorRefs[target1], ActorMessage(msg));
        numMessagesSent += 2;
    }
}

int main(int argc, const char * argv[]) {
    srand(time(NULL));
    // Spawn an actor system
    ActorSystem actorSystem("testSystem", 4);
    
    // Heres some actors
    ActorRef test = actorSystem.createActor(new TestActor());
    ActorRef test2 = actorSystem.createActor(new TestActor2());
    
    // Heres a lot more actors
    size_t actorsAmount = 2000;
    std::vector<ActorRef> actorRefs;
    for (size_t i=0; i<actorsAmount/2; i++) {
        actorRefs.push_back(actorSystem.createActor(new TestActor()));
    }
    
    for (size_t i=0; i<actorsAmount/2; i++) {
        actorRefs.push_back(actorSystem.createActor(new TestActor2()));
    }
    
    // Heres some message generating threads
    size_t numThreads = 6;
    size_t messagesAmount = 2000000;
    numMessagesSent = 0;
    
    actorSystem.getLoggingActor().send(test, ActorMessage("Main: Sending " + std::to_string(messagesAmount*numThreads) + " messages..."));
    numMessagesSent++;
    std::vector<std::thread> threads;
    for (size_t i=0; i<numThreads; i++) {
        threads.push_back(std::thread(&generateMessages, actorRefs, messagesAmount));
    }
    
    actorSystem.getLoggingActor().send(test, ActorMessage("Main: Joining message spawning threads..."));
    numMessagesSent++;
    for (auto& thr : threads) {
        thr.join();
    }
    
    actorSystem.getLoggingActor().send(test, ActorMessage("Main: Finished sending all messages to the ActorSystem. Waiting 60 seconds before shutting down..."));
    numMessagesSent++;
    std::this_thread::sleep_for(std::chrono::seconds(60));
    
    std::cout << "Number of messages sent by main: " << numMessagesSent << std::endl;
    
    return 0;
}
