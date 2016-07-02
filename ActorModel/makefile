CC=g++
CFLAGS=-std=c++11 -Ofast -march=native -mtune=native -I./ActorSystem -c -Wall
LDFLAGS=-lpthread
SOURCES=main.cpp TestActor.cpp TestActor2.cpp ActorSystem/ActorSystem.cpp ActorSystem/Actor.cpp ActorSystem/ActorRef.cpp ActorSystem/LoggingActor.cpp ActorSystem/ThreadActor.cpp
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
