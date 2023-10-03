CXXFLAGS = -std=gnu++1z
LDFLAGS = -lpthread -lcnpy -lmatplot
SOURCES = solver.cpp
HEADERS = solver.h

all: main tests

main: $(HEADERS) $(SOURCES) main.cpp
	g++ $(SOURCES) $@.cpp $(CXXFLAGS) $(LDFLAGS) -o $@

tests: $(HEADERS) $(SOURCES) tests.h tests.cpp
	g++ -D__TEST_HOOKS__ $(SOURCES) $@.cpp $(CXXFLAGS) $(LDFLAGS) -o $@

clean:
	$(RM) main tests
