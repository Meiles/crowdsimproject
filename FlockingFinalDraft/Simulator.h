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
      void Update();
   private:
      vector<Agent> agents;
};

#endif