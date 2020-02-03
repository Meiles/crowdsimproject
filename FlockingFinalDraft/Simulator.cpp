#include "Simulator.h"
#include <stdlib.h>

#include <GLUT/GLUT.h>  // (or others, depending on the system in use)
#include "Basic.h"

Simulator::Simulator() {
     for (int i = 0; i < 10; i++) {
          if (i == 0){

          }
          //pos
          double x = 50 + 200 * (myRand() - 0.5);
          double y = 50 + 200 * (myRand() - 0.5);
          double z = 0;
          Vector3d pos(x, y, z);
          //vel
          double vx = 20 * (myRand() - 0.5);
          double vy = 20 * (myRand() - 0.5);
          double vz = 0;
          Vector3d vel(vx, vy, vz);
          double mass = 4 + myRand() * 10; //random between 4 and 14
          double maxVel = 8 + myRand() * 5; //random between 8 and 13
          double maxAccel = 20 + myRand() * 10; //random between 20 and 30
          double viewRadius = 40 + myRand() * 10; //random between 40 and 50
          Agent a_i;

          if (i == 0) a_i.SetIsAdversary(true);
          a_i.Init(i, pos, vel, mass, maxVel, maxAccel, viewRadius);
          agents.push_back(a_i);
     }
}

  void Simulator::addAgent(int n, double x, double y) {
     for (int i = 0; i<n; i++) {
          //pos
          double x = 50 + 200 * (myRand() - 0.5);
          double y = 50 + 200 * (myRand() - 0.5);
          double z = 0;
          Vector3d pos(x, y, z);
          //vel
          double vx = 20 * (myRand() - 0.5);
          double vy = 20 * (myRand() - 0.5);
          double vz = 0;
          Vector3d vel(vx, vy, vz);
          double mass = 4 + myRand() * 10; //random between 4 and 14
          double maxVel = 8 + myRand() * 5; //random between 8 and 13
          double maxAccel = 20 + myRand() * 10; //random between 20 and 30
          double viewRadius = 40 + myRand() * 10; //random between 40 and 50
          Agent a_i;

          a_i.Init(i, pos, vel, mass, maxVel, maxAccel, viewRadius);
          agents.push_back(a_i);
          

     }

}

void Simulator::Draw() {
  for(int i=0; i<(int)agents.size(); i++) {
    agents[i].Draw();
  }
}

void Simulator::Update() {
  double dt = 0.05;
  for (int i = 0; i < (int)agents.size(); i++) {

    agents[i].Update(agents, dt);
    if (i == 3){
         agents[i].SetIsAdversary(true);
    }
    if (agents[i].GetAgentDeath())agents.erase(agents.begin() + i);

  }


}
