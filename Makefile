LDFLAGS = -lpthread

all: main

%: %.cpp
	g++ $< $(LDFLAGS) -o $@

clean:
	$(RM) main
