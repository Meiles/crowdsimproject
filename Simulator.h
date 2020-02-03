#ifndef SIMULATOR
#define SIMULATOR

#include "Vector.h"
using namespace mathtool;

#include "Agent.h"

#include <vector>
using namespace std;

class Simulator {
   public:
      Simulator(); 

      void Draw();
      void addAgent(int n, double x, double y);
      void getPath(Agent &, string);
      double getMaxVel(int);
      void Update();

      int GetNumAgents() { return agents.size(); }
      Agent& GetAgent(int i) { return agents[i]; }
   private:
      vector<Agent> agents;
      int id;
};

#endif
