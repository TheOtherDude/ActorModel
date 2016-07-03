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

#ifndef LoggingActor_h
#define LoggingActor_h

#include <iostream>
#include <ctime>
#include <cstring>
#include "Actor.h"
#include "StringMessage.h"

/*
 * LoggingActor
 * This is the built-in logging actor in the actor system.
 * Send it a StringMessage which will then be timestamped and printed.
 */
class LoggingActor : public Actor {
    private:
        // Message handlers
        void handleMessage(const ActorRef& sender, const StringMessage* msg);
        void handleError(const ActorRef& sender, const ActorMessage* msg);
    
    public:
        LoggingActor();
    
        void receive(const ActorRef& sender, const ActorMessage* msg) override;
};

#endif /* LoggingActor_h */
