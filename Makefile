TARGET := slapi_rest 
SRC_FILES := $(wildcard *.cpp)
OBJ_FILES := $(patsubst %.cpp,%.o,$(SRC_FILES))
LDFLAGS := -L/usr/local/lib -I/usr/local/include -rdynamic ./libs/cpprestsdk/build.release/Binaries/libcpprest.a -lssl -lcrypto -lboost_system -lboost_thread -lboost_log -lboost_program_options -lboost_chrono -lpthread -lfolly -lgrpc++_unsecure -lgrpc -lprotobuf -lpthread -ldl -liosxrsl -lglog 
CPPFLAGS := -g -std=c++14 -I./libs/cpprestsdk/Release/include -I/usr/local/include -pthread

.PHONY: all
all: $(TARGET)

%.o: %.cc
	g++ $(CPPFLAGS) -c -o $@ $<

$(TARGET): $(OBJ_FILES)
	g++ $^ $(LDFLAGS) -o $@


.PHONY: clean
clean:
	-${RM} ${TARGET} ${OBJ_FILES}
