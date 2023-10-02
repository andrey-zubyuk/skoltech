CXXFLAGS = -std=gnu++1z
LDFLAGS = -lpthread -lcnpy -lmatplot
SOURCES = solver.cpp
HEADERS = solver.h

all: main

%: $(HEADERS) $(SOURCES) %.cpp
	g++ $(SOURCES) $@.cpp $(CXXFLAGS) $(LDFLAGS) -o $@

clean:
	$(RM) main
