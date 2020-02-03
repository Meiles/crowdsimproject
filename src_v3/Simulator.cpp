#include "Simulator.h"
#include <stdlib.h>

#include <GL/glut.h>  // (or others, depending on the system in use)
#include "Basic.h"

Simulator::Simulator() {
  for(int i=0; i<20; i++) {
    //pos
    double x = 50+300*(myRand()-0.5);
    double y = 50+300*(myRand()-0.5);
    double z = 0;
    Vector3d pos(x,y,z);
    //vel
    double vx = 20*(myRand()-0.5);
	double vy = 20 * (myRand() - 0.5);
    double vz = 0;
    Vector3d vel(vx,vy,vz);
	double mass = 4 + myRand() * 10; //random between 4 and 14
	double maxVel = 1 + myRand() * 5; //random between 18 and 28
    //double maxAccel = 20+drand48()*10; //random between 20 and 30
	double maxAccel = 20 + myRand() * 10; //random between 20 and 30
	double viewRadius = 40 + myRand() * 10; //random between 40 and 50
    //agents.push_back( Vector3d(x,y) );
    Agent a_i;
    a_i.Init(i,pos,vel,mass,maxVel,maxAccel,viewRadius);
    agents.push_back( a_i );
  }
}

void Simulator::Draw() {
  for(int i=0; i<(int)agents.size(); i++) {
    agents[i].Draw();
  }
}

void Simulator::Update() {
  double dt = 0.25;
  for(int i=0; i<(int)agents.size(); i++) {
    agents[i].Update(agents, dt);
  }


}
