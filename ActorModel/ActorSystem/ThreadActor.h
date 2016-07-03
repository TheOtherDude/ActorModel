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

#ifndef ThreadActor_h
#define ThreadActor_h

#include <iostream>
#include "Actor.h"

/*
 * ThreadActor
 * This is an internal use fake actor type.
 * Don't use it. Inherit from Actor instead.
 */
class ThreadActor : public Actor {
public:
    ThreadActor(const size_t threadId);
    
    const size_t threadId;
    
    void receive(const ActorRef& sender, const ActorMessage* msg) override;
};

#endif /* ThreadActor_h */
