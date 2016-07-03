#
# Copyright (C) 2016  Matt Smith
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

CC=g++
CFLAGS=-std=c++11 -Ofast -march=native -mtune=native -I./ActorSystem -c -Wall
LDFLAGS=-lpthread
SOURCES=main.cpp TestActor.cpp TestActor2.cpp TestActor3.cpp ActorSystem/ActorSystem.cpp ActorSystem/Actor.cpp ActorSystem/ActorRef.cpp ActorSystem/ActorMessage.cpp ActorSystem/LoggingActor.cpp ActorSystem/ThreadActor.cpp ActorSystem/StringMessage.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=actor.x

all: clean $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS)
	rm -rf $(EXECUTABLE)
	rm -rf $(TEST_EXEC)
