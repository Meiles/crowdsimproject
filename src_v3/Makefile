all: crowdSim

crowdSim.o: crowdSim.cpp
	g++ -O2 -DMACOS -c crowdSim.cpp -I/usr/X11/include -I./cal3d-0.11.0/src

Basic.o: Basic.cpp
	g++ -O2 -DMACOS -c Basic.cpp -I/usr/X11/include -I./cal3d-0.11.0/src

Simulator.o: Simulator.cpp
	g++ -O2 -DMACOS -c Simulator.cpp -I/usr/X11/include -I./cal3d-0.11.0/src

Agent.o: Agent.cpp
	g++ -O2 -DMACOS -c Agent.cpp -I/usr/X11/include -I./cal3d-0.11.0/src

Environment.o: Environment.cpp
	g++ -O2 -DMACOS -c Environment.cpp -I/usr/X11/include -I./cal3d-0.11.0/src

cal3dmodel.o: cal3dmodel.cpp
	g++ -O2 -DMACOS -c cal3dmodel.cpp -I/usr/X11/include -I./cal3d-0.11.0/src

crowdSim: crowdSim.o Simulator.o Agent.o Environment.o cal3dmodel.o Basic.o
	g++ crowdSim.o Simulator.o Agent.o Environment.o cal3dmodel.o  Basic.o -o crowdSim -framework OpenGL -framework GLUT -L./cal3d-0.11.0/src/cal3d/.libs -lcal3d
 

clean:
	rm *.o crowdSim
