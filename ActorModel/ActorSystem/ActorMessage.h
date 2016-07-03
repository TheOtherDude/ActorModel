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

#ifndef ActorMessage_h
#define ActorMessage_h

/*
 * ActorMessage
 * This is the base class for all messages in the actor system.
 * Define a new message type and inherit from this.
 */
class ActorMessage {
    public:
        // Policy for this message during use
        enum class MessageOpts {
            DELETE_AFTER_USE,   // Thread engine will delete the message
            STAY_RESIDENT       // Message object will remain untouched
        };
    
        // Actor Message Options
        const MessageOpts opts;
    
        // Constructor defaults to delete.
        ActorMessage(const MessageOpts opts = MessageOpts::DELETE_AFTER_USE);
    
        virtual ~ActorMessage();
};

#endif /* ActorMessage_h */
