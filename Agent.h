#ifndef AGENT
#define AGENT
#include "Vector.h"
using namespace mathtool;
#include <vector>
using namespace std;

#include "cal3dmodel.h"

class Agent {
public:
  Agent();
  Agent(const Agent& other);

  void Init(int _id, Vector3d _pos, Vector3d _vel, double _mass, double _maxVel, double _maxAccel, double _viewRadius);
  Vector3d& GetPos() { return pos; }
  Vector3d& GetVel() { return vel; }  
  double GetRadius() { return radius; }
  int GetID() { return id; }


  void Update(vector<Agent>& agents, double dt);
  void ResolveCollisionWithOtherAgents(vector<Agent>& agents);
  void Draw();

  //flocking additions
    bool GetIsAdversary(){return IsAdversary;}

  void SetIsAdversary(bool p){IsAdversary = p;}

  bool GetAgentDeath(){return agentDeath;}


  //////////////////////////////////////////////////////////////////////
  // cal3d model for drawing moving models
  bool InitCal3dModel(string _cal3dDataPath, string _cal3dModel,
	  pair<double, double> cal3dRunS,
	  pair<double, double> cal3dWalkS,
	  pair<double, double> cal3dWaitS,
	  double animation_speed, double scale_factor);
  void UpdateCal3d(double vel, string tag);
  Cal3dModel* m_Cal3dModel;
  string m_strDatapath;
  std::string m_strCal3D_Datapath;
  pair<double, double> m_Cal3dRunSpeed;
  pair<double, double> m_Cal3dWalkSpeed;
  pair<double, double> m_Cal3dWaitSpeed;
  double m_Cal3dScaleFactor;
  double m_Cal3dAnimationSpeed;
  bool m_DoCal3d;
  float motionBlend[3];
  bool cal3dInitialized;
  void setFullInputModel(string a) { fullInputModel = a; }
  string getFullInputModel() { return fullInputModel; }
  void setModelInputPath(string a) { modeInputPath = a; }
  string getModelInputPath() { return modeInputPath; }
  void setMaxVel(double a) { maxVel = a; }
  double getMaxVel() { return maxVel; }
  void setSize(int s) { size = s; }
  int getSize() { return size; }

  ///////-------------------------
  //pair<double, double> m_cal3dRunSpeed;   
  //pair<double, double> m_cal3dWalkSpeed;  
  //pair<double, double> m_cal3dWaitSpeed;  
  double m_cal3dAnimationSpeed;         
  double m_cal3dScaleFactor;           
  /////////////////////////////////////////////////////////////////////
  GLUquadric* qobj; //for drawing simple sphere
  /////////////////////////////////////////////////////////////////////

private:
  int id;
  int size;
  Vector3d pos, oldPos;
  Vector3d vel;
  double ori; 
  //Vector3d accel;
  Vector3d separationForce;
  Vector3d cohesionForce;
  Vector3d alignmentForce;
  
  double separationComponent;
  double cohesionComponent;
  double alignmentComponent;
  double pathComponent;

  double radius;
  double mass;
  double maxVel;
  double maxAccel;
  double viewRadius;

  bool initialized;
  string cal3dDataPath;
  string fullInputModel;
  string modeInputPath;
  
  pair<int,int> gridCell;

  bool drawForce;
  bool drawVelocity;
  bool drawVR;
  bool IsAdversary;
  bool agentDeath;

  //path
  vector< pair<int,int> > pathCells;
};

#endif
