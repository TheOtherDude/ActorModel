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

#include <iostream>
#include <thread>
#include <atomic>
#include <cstdlib>
#include <ctime>
#include "ActorSystem.h"
#include "StringMessage.h"
#include "TestActor.h"
#include "TestActor2.h"
#include "TestActor3.h"
#include "ScatterActor.h"

std::atomic<size_t> numMessagesSent;

// Randomly picks a source and dest actor and sends a message between them
// Does 2 per iteration (one for TestActor one for TestActor2 classes)
void generateMessages(const std::vector<ActorRef> actorRefs, size_t amount) {
    const std::string msg = "This is a test of the actor system";
    for (size_t i=0; i<amount/2; i++) {
        int target1 = rand() % actorRefs.size();
        int target2 = rand() % actorRefs.size();
        actorRefs[target1].send(actorRefs[target2], new StringMessage(msg));
        actorRefs[target2].send(actorRefs[target1], new StringMessage(msg));
        numMessagesSent += 2;
    }
}

// Scatter-Gather Demo
int main(int argc, const char * argv[]) {
    srand((unsigned int)time(NULL));
    ActorSystem actorSystem("Scatter-Gather tester");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    // Create a scatter actor
    ActorRef scatterMain = actorSystem.createActor(new ScatterActor());
    
    // Send an init message to itself which will then spawn other actors
    scatterMain.send(scatterMain, new ScatterMsg(ScatterMsg::Type::INIT));
    std::this_thread::sleep_for(std::chrono::seconds(60));
}

/*
int main(int argc, const char * argv[]) {
    srand((unsigned int)time(NULL));
    // Spawn an actor system
    ActorSystem actorSystem("testSystem");
    
    // Heres some actors
    ActorRef test = actorSystem.createActor(new TestActor());
    ActorRef test2 = actorSystem.createActor(new TestActor2());
    ActorRef test3 = actorSystem.createActor(new TestActor3());
    
    // Heres a lot more actors
    size_t actorsAmount = 2000;
    std::vector<ActorRef> actorRefs;
    for (size_t i=0; i<actorsAmount/2; i++) {
        actorRefs.push_back(actorSystem.createActor(new TestActor()));
    }
    
    for (size_t i=0; i<actorsAmount/2; i++) {
        actorRefs.push_back(actorSystem.createActor(new TestActor2()));
    }
    
    // Test the logging and dynamic casting mechanisms
    actorSystem.getLoggingActor().send(test, new StringMessage("Main: Sending logger an invalid message..."));
    actorSystem.getLoggingActor().send(test, new ActorMessage());
    numMessagesSent += 2;
    
    // Test thread executor error handling
    test3.send(test3, new ActorMessage());
    
    // Heres some message generating threads
    size_t numThreads = 6;
    size_t messagesAmount = 2000000;
    numMessagesSent = 0;
    
    actorSystem.getLoggingActor().send(test, new StringMessage("Main: Sending " + std::to_string(messagesAmount*numThreads) + " messages..."));
    numMessagesSent++;
    std::vector<std::thread> threads;
    for (size_t i=0; i<numThreads; i++) {
        threads.push_back(std::thread(&generateMessages, actorRefs, messagesAmount));
    }
    
    actorSystem.getLoggingActor().send(test, new StringMessage("Main: Joining message spawning threads..."));
    numMessagesSent++;
    for (auto& thr : threads) {
        thr.join();
    }
    
    actorSystem.getLoggingActor().send(test, new StringMessage("Main: Finished sending all messages to the ActorSystem. Waiting 60 seconds before shutting down..."));
    numMessagesSent++;
    std::this_thread::sleep_for(std::chrono::seconds(60));
    
    std::cout << "Number of messages sent by main: " << numMessagesSent << std::endl;
    
    return 0;
}
*/