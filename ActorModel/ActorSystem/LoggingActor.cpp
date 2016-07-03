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

#include "LoggingActor.h"

LoggingActor::LoggingActor() {
    
}

// Actor receive method
void LoggingActor::receive(const ActorRef& sender, const ActorMessage* msg) {
    // In a well designed system this would be the only actor calling std::cout
    // thus everything is thread safe.
    
    // Cast message to StringMessage
    if (const StringMessage* strmsg = dynamic_cast<const StringMessage*>(msg)) {
        handleMessage(sender, strmsg);
    }
    else {
        handleError(sender, msg);
    }
}

// Handles a valid string message
void LoggingActor::handleMessage(const ActorRef& sender, const StringMessage* msg) {
    // Get time info
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    // Get rid of trailing newline
    char* timestr = asctime(timeinfo);
    size_t len = strlen(timestr);
    timestr[len-1] = 0;
    
    std::cout << "[" << timestr << "] [ActorSystem::" << getActorSystem()->systemName << "] [" << sender.actorId << "] " << msg->msg << std::endl;
}

// Handles an unexpected message
void LoggingActor::handleError(const ActorRef& sender, const ActorMessage* msg) {
    // Send itself an error message for fun
    self().send(new StringMessage("Logger: Received invalid message"));
}