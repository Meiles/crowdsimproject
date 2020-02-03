#include "Simulator.h"
#include <stdlib.h>

#include <GLUT/glut.h>  // (or others, depending on the system in use)
#include "Basic.h"

Simulator::Simulator() {
  id = 0;
  ifstream infile;
  infile.open("input.txt");
  string name;
  int MaxVel;
  int size;
  while (!infile.eof()) {
    infile >> name;
    infile >> MaxVel;
    infile >> size;
    double x = 50 + 300 * (myRand() - 0.5);
    double y = 50 + 300 * (myRand() - 0.5);
    double z = 0;
    Vector3d pos(x, y, z);
    //vel
    double vx = 20 * (myRand() - 0.5);
    double vy = 20 * (myRand() - 0.5);
    double vz = 0;
    Vector3d vel(vx, vy, vz);
    double mass = 4 + myRand() * 10; //random between 4 and 14
    //random between 1 and 6
    //double maxAccel = 20+drand48()*10; //random between 20 and 30
    double maxVel = getMaxVel(MaxVel);
    double maxAccel = 20 + myRand() * 10; //random between 20 and 30
    double viewRadius = 40 + myRand() * 10; //random between 40 and 50
    Agent a_i;
    a_i.Init(id,pos,vel,mass,maxVel,maxAccel,viewRadius);
    agents.push_back( a_i );
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
  double dt = 0.125;
  for(int i=0; i<(int)agents.size(); i++) {
    agents[i].Update(agents, dt);
    if (i == 3){agents[i].SetIsAdversary(true);}
    if (agents[i].GetAgentDeath())agents.erase(agents.begin() + i);
  }
}


double Simulator::getMaxVel(int inputMaxVel) {
  double maxVel = inputMaxVel + myRand() * 5;
  return maxVel;
}
void Simulator::getPath(Agent& a, string model) {
  string modeldatapath;
  string fullpath;
  string modelpath;
  if (model == "dino") {
    modeldatapath = "cal3ddata/dino-cal3d2/";
    modelpath = "dino.cfg";
  }
  if (model == "fatboy") {
    modeldatapath = "cal3ddata/fatboy-cal3d2/";
    modelpath = "fatboy.cfg";

  }
  if (model == "figure") {
    modeldatapath = "cal3ddata/figure-cal3d2/";
    modelpath = "figure.cfg";

  }
  if (model == "masterchief") {
    modeldatapath = "cal3ddata/masterchief-cal3d2/";
    modelpath = "masterchief.cfg";

  }
  if (model == "suitman") {
    modeldatapath = "cal3ddata/suitman-cal3d2/";
    modelpath = "suitman.cfg";

  }
  if (model == "terminator") {
    modeldatapath = "cal3ddata/terminator-cal3d2/";
    modelpath = "terminator.cfg";

  }
  if (model == "simpleman") {
    modeldatapath = "cal3ddata/simpleman_costumes2/";
    modelpath = "simple-man_purple&light.cfg";

  }
  if (model == "coolkid") {
    modeldatapath = "cal3ddata/coolkid-cal3d2/";
    modelpath = "coolkid.cfg";

  }

  fullpath = modeldatapath + modelpath;
  cout << " fullpath is " << fullpath << "\n and modelpath is " << modelpath << " and modeldatapath is " << modeldatapath << endl;
  a.setFullInputModel(fullpath);
  a.setModelInputPath(modeldatapath);
}
