//
//  LoggingActor.cpp
//  ActorModel
//
//  Created by Matt on 7/2/16.
//  Copyright © 2016 Matt. All rights reserved.
//

#include "LoggingActor.h"

LoggingActor::LoggingActor() {
    
}

void LoggingActor::receive(const ActorRef& sender, ActorMessage& msg) {
    // In a well designed system this would be the only actor calling std::cout
    // thus everything is thread safe.
    
    // Get time info
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    // Get rid of trailing newline
    char* timestr = asctime(timeinfo);
    size_t len = strlen(timestr);
    timestr[len-1] = 0;
    
    std::cout << "[" << timestr << "] [ActorSystem::" << getActorSystem()->systemName << "] " << msg.msg << std::endl;
}