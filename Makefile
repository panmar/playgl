CXX=clang++
CXXFLAGS=-std=c++17 \
	-Iextern/ \
	-Wall -Wno-deprecated-declarations \
	-g
LDLIBS=-lGLU -lGL -lX11 -lpthread -lXrandr -lXi -ldl

SRCS=src/main.cc extern/glad.cc extern/libglfw3.a

playgl: $(SRCS)
	mkdir -p bin/
	$(CXX) $(CXXFLAGS) $(SRCS) -o bin/playgl $(LDLIBS)

clean:
	rm -rf bin
