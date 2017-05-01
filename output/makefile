CFLAGS=-ggdb -std=c++11  
ARMA=-I /home/jhazelden/Downloads/armadillo-7.500.2/include -DARMA_DONT_USE_WRAPPER
OPENGL=-L/usr/local/lib -lGLEW -lGLU -lm -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXi -lXcursor -lXrender -lGL -lm -lpthread -ldl -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

run : polygoniser.o graphics.o main.o 
	g++ polygoniser.o graphics.o main.o -o run -lblas -llapack $(CFLAGS) $(OPENGL)

main.o : ../src/main.cpp ../src/graphics.cpp
	g++ -c ../src/main.cpp $(CFLAGS)

graphics.o : ../src/graphics.cpp ../src/polygoniser.cpp
	g++ -c ../src/graphics.cpp $(CFLAGS)

polygoniser.o : ../src/polygoniser.cpp
	g++ -c ../src/polygoniser.cpp -lblas -llapack $(CFLAGS) $(ARMA)

clean : 
	rm *.o run
