CFLAGS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv ` -lcurl

% : %.cpp
	        g++ -ggdb $(CFLAGS)  -o `basename $< .cpp`  $< $(LIBS)
