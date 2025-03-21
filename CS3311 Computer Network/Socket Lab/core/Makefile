COMPILER   = g++
BUILDTYPE ?= debug
INCLUDES   = -Iinclude

ifeq ($(OS),Windows_NT)
	LIBRARY  = -lws2_32
endif

ifeq ($(BUILDTYPE),debug)
	CPPFLAGS = -fdiagnostics-color=always -g -Wall -Wextra -Werror -std=c++20
else ifeq ($(BUILDTYPE),release)
	CPPFLAGS = -fdiagnostics-color=always -O3 -Wall -Wextra -Werror -std=c++20 -static -static-libgcc -static-libstdc++
else
	$(error Invalid BUILDTYPE: $(BUILDTYPE))
endif

all: core.exe

core.exe: Core.cpp socket.o exceptions.o protocol.o client.o server.o interface.o
	$(COMPILER) $(CPPFLAGS) $(INCLUDES) Core.cpp socket.o exceptions.o protocol.o client.o server.o interface.o -o core.exe $(LIBRARY)

socket.o: source/Socket.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDES) -c source/Socket.cpp -o socket.o

exceptions.o: source/Exceptions.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDES) -c source/Exceptions.cpp -o exceptions.o

protocol.o: source/Protocol.cpp
	$(COMPILER) $(CPPFLAGS) $(INCLUDES) -c source/Protocol.cpp -o protocol.o

client.o:
	$(COMPILER) $(CPPFLAGS) $(INCLUDES) -c source/Client.cpp -o client.o

server.o:
	$(COMPILER) $(CPPFLAGS) $(INCLUDES) -c source/Server.cpp -o server.o

interface.o:
	$(COMPILER) $(CPPFLAGS) $(INCLUDES) -c source/Interface.cpp -o interface.o

clean:
ifeq ($(OS),Windows_NT)
	powershell Remove-Item -Path *.o, *.exe -Force -Recurse
else
	rm -f *.o *.exe
endif