CXXFLAGS += -g -std=c++11 -D_FILE_OFFSET_BITS=64 -I.

MACHINE = $(shell uname -s)

ifeq ($(MACHINE),Darwin)
	LDFLAGS +=-L. -lmongoclient -lfuse_ino64 -lboost_thread-mt -lboost_filesystem-mt -lboost_system-mt
else
	LDFLAGS +=-L. -lmongoclient -lfuse -lboost_thread -lboost_filesystem -lboost_system -lpthread -lssl -lcrypto
endif

OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

mount_gridfs : $(OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

debian : mount_gridfs

install: mount_gridfs
	install -t /usr/local/bin mount_gridfs

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.o: main.cpp operations.h options.h utils.h

operations.o : operations.cpp operations.h options.h utils.h local_gridfile.h

options.o: options.cpp options.h

local_gridfile.o: local_gridfile.cpp local_gridfile.h

clean:
	rm -f $(OBJS)
