
COMMON	=	src/common
CLIENT	=	src/client
SERVER	=	src/server

CC		= g++

SRC_CLIENT = $(wildcard $(CLIENT)/*.cpp)
SRC_SERVER = $(wildcard $(SERVER)/*.cpp)
SRC_COMMON = $(wildcard $(COMMON)/*.cpp)


CPPFLAGS	+=	-Iinclude

LDFLAGS	+=	-lboost_program_options -lpthread

all: sikradio-receiver sikradio-sender


sikradio-receiver: 
		$(CC)  -o sikradio-receiver  $(CPPFLAGS) $(LDFLAGS) $(SRC_CLIENT) $(SRC_COMMON) 
sikradio-sender: 
		$(CC)  -o sikradio-sender $(CPPFLAGS) $(CPPFLAGS) $(LDFLAGS) $(SRC_SERVER) $(SRC_COMMON) 

.PHONY : clean
clean :
		rm sikradio-receiver sikradio-sender 
