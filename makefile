BOOST = /home/jack/docs/boost/
CLIENT_FILE = littlebox_client
SERVER_FILE = littlebox_server

all: clean littlebox_client littlebox_server

littlebox_client:
	g++ -o littlebox_client \
	./src/client/main.cpp \
	./src/client/client.cpp \
	-I $(BOOST) \
	-L $(BOOST)/libs \
	-l boost_system \
	-l pthread

littlebox_server:
	g++ -o littlebox_server \
	./src/server/main.cpp \
	./src/server/server.cpp \
	-I $(BOOST) \
	-L $(BOOST)/libs \
	-l boost_system \
	-l pthread

clean:
ifneq ("$(wildcard $(CLIENT_FILE))","")
	rm $(CLIENT_FILE)
endif
ifneq ("$(wildcard $(SERVER_FILE))","")
	rm $(SERVER_FILE)
endif
