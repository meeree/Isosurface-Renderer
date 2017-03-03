CFLAGS=-ggdb -std=c++11  
OPENGL=-L/usr/local/lib -lGLEW -lGLU -lm -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXi -lXcursor -lXrender -lGL -lm -lpthread -ldl -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

run : main.o 
	g++ main.o -o run $(CFLAGS) $(OPENGL)

main.o : main.cpp
	g++ -c main.cpp $(CFLAGS)

clean : 
	rm *.o run
