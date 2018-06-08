
INC=-I./include

COM=./src/common
CLIENT=./src/client
SERVER=./src/server

sikradio-receiver: $(COM) $(CLIENT) -lboost_filesystem -lpthread
sikradio-sender: $(COM) $(SERVER) -lboost_filesystem -lpthread

.PHONY : clean
clean :
        -rm sikradio-receiver sikradio-sender *.gch *.o
