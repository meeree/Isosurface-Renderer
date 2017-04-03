CUFLAGS=-g -std=c++11 -Wno-deprecated-gpu-targets -Xcudafe "--diag_suppress=declared_but_not_referenced" -arch=compute_35 
CFLAGS=-ggdb -std=c++11 
OPENGL=-L/usr/local/lib -lGLEW -lGLU -lm -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXi -lXcursor -lXrender -lGL -lm -lpthread -ldl -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

run : marching_cubes.o graphics.o main.o 
	nvcc marching_cubes.o graphics.o main.o -o run $(CUFLAGS) $(OPENGL)

main.o : ../src/main.cpp ../src/graphics.cpp
	g++ -c ../src/main.cpp $(CFLAGS)

graphics.o : ../src/graphics.cpp ../src/marching_cubes.cu
	g++ -c ../src/graphics.cpp $(CFLAGS)

marching_cubes.o : ../src/marching_cubes.cu
	nvcc -c ../src/marching_cubes.cu $(CUFLAGS)

clean : 
	rm *.o run
