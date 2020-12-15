CXX = g++
LDLIBS += `pkg-config --libs opencv`


.PHONY: clean

hello: hello.cpp
	$(CXX) -o '$@' '$<' $(LDLIBS)

clean:
	rm hello

all:
	hello
