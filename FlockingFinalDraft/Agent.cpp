//#include <windows.h>
#pragma comment(lib, "user32.lib") 

#include "Agent.h"
#include "Environment.h"

#include <GLUT/GLUT.h>  // (or others, depending on the system in use)
bool agDebug = false;

extern Environment* gEnv;

Agent::Agent() { 
  initialized = false;
  drawForce = true;
  drawVelocity = true;
  drawVR = false;
  IsAdversary = false;
  DistanceRemaining = 50;
  agentDeath = false;
} //empty constructor  

void Agent::Init(int _id, Vector3d _pos, Vector3d _vel, double _mass, 
                 double _maxVel, double _maxAccel, double _viewRadius){
  initialized = true;
  id = _id;
  pos = _pos;
  vel = _vel;
  mass = _mass;
  maxVel = _maxVel;
  maxAccel = _maxAccel;
  viewRadius = _viewRadius;
  radius = 8;
  
  drawForce = true;
  drawVelocity = true;
  drawVR = false;
  
  separationComponent = 40;
  cohesionComponent = 10;
  alignmentComponent = 20;
}

Agent::Agent(const Agent& other) {
  initialized = other.initialized;
  id = other.id;
  pos = other.pos;
  vel = other.vel;
  separationComponent = other.separationComponent;
  cohesionComponent = other.cohesionComponent;
  alignmentComponent = other.alignmentComponent;
  radius = other.radius;
  mass = other.mass;
  maxVel = other.maxVel;
  maxAccel = other.maxAccel;
  viewRadius = other.viewRadius;
  gridCell = other.gridCell;
  drawForce = other.drawForce;
  drawVelocity = other.drawVelocity;
  drawVR = other.drawVR;
  IsAdversary = other.IsAdversary;
  DistanceRemaining = other.DistanceRemaining;
  agentDeath = other.agentDeath;
}

void Agent::Update(vector<Agent>& agents, double dt) {
  if( !initialized ) {
    cout << "Agent::Update id: " << id << " HAS NOT BEEN INITIALIZED> " << endl;
    exit(-1);
  }
  Vector3d separation(0,0,0);
  Vector3d cohesion(0,0,0);
  Vector3d alignment(0,0,0);
  Vector3d center(0,0,0);
  Vector3d force(0,0,0);
  Vector3d boundaryForce(0, 0, 0);
  Vector3d adversarialForce(0, 0, 0);
  double maxAdvForce = 50;
  separationForce.reset();
  cohesionForce.reset();
  alignmentForce.reset();
  boundaryForce.reset();
  int numSeen = 0;
  if( agDebug ) cout << "Update for agent: " << id << endl;
  //generate the forces
  for(int i=0; i<(int)agents.size(); i++) {
    if( id == i ) continue; //skip self
    //see if within view radius
    double distance = (agents[i].GetPos()-pos).norm();
    if (DistanceRemaining < 0) agentDeath = true;
    if (distance < viewRadius) {
         if (agDebug) cout << "Agent: " << id << " is within range of agent: " << i << " distance= " << distance << " viewRadius= " << viewRadius << endl;
         if (agDebug) cout << "SEPDBG agent[" << i << "].GetPos() " << agents[i].GetPos() << " pos[" << id << "]" << pos << endl;
         if (agDebug) cout << "SEPDBG agent[" << i << "]: separation component: " << agents[i].GetPos() - pos << endl;

         separation += (pos - agents[i].GetPos()).normalize()*(1.0 - distance / viewRadius);
         center += agents[i].GetPos();
         alignment += agents[i].GetVel();
         numSeen++;
         if (agents[i].GetIsAdversary()){
              adversarialForce += maxAdvForce*(pos - agents[i].GetPos()).normalize();
         }
    }
  }

  if( numSeen>0 ) {
    center /= 1.0*numSeen;
    double distToCenter = (center-pos).norm();
    cohesion = (center-pos).normalize()*cohesionComponent*(distToCenter/viewRadius);
    separation = separation * separationComponent;
    alignment = alignment.normalize() * alignmentComponent;

    if( agDebug ) cout << "component vectors for : " << id << endl;
    if( agDebug ) cout << "sep " << separation << endl;
    if( agDebug ) cout << "coh " << cohesion << endl;
    if( agDebug ) cout << "ali " << alignment << endl;
    boundaryForce = gEnv->GetForceFromWall(pos, 65);
    force = separation + cohesion + alignment + boundaryForce + adversarialForce;
    if( force.norm()>maxAccel ) force.selfScale(maxAccel);
    separationForce = separation;
    cohesionForce = cohesion;
    alignmentForce = alignment;
    
    if( agDebug ) cout << "totalf " << force << endl;
    force /= mass;
  }
  else {
    if( agDebug ) cout << "No agents visible for agent:  " << id << endl;
    boundaryForce = gEnv -> GetForceFromWall(pos, 30);
    force = boundaryForce;
  }
  if( agDebug ) cout << "origPos " << pos << endl;
  if( agDebug ) cout << "origVel " << vel << endl;
  if (oldPos.size() != 0) DistanceRemaining -= .01 * (oldPos.back() - pos).norm();
  if (agDebug) cout << "Distance remaining is: " << DistanceRemaining << endl;
  oldPos.push_back(pos);
  pos = pos + vel*dt;
  vel = vel + force*dt;
  if( agDebug ) cout << "newPos " << pos << endl;
  if( agDebug ) cout << "newVel " << vel << endl;
  if( vel.norm() > maxVel ) {
    vel.selfScale(maxVel);
  }
  if( agDebug ) cout << "newVel(trunc) " << vel << endl;

  bool wrapWorld=false;
  if( wrapWorld ) {
    bool updated=false;
    Vector3d pNew = gEnv->GetResolvedPosition(pos, radius, updated);
    if( updated )
      pos = pNew;
  }

}

void drawCircle(double radius, int divisions) {
  double deltaAng = TWOPI / divisions;
  double curAng = 0;
  glBegin(GL_POLYGON);
  for(int i=0; i<divisions; i++) {
    double x = radius * cos(curAng);
    double y = radius * sin(curAng);
    if( agDebug ) cout << "Circle " << i << " x= " << x << " y= " << y << endl;
    glVertex2f( x, y );
    curAng += deltaAng;
    if( agDebug ) cout << "curAng= " << curAng << " deltaAng= " << deltaAng << endl;
  }
  glEnd();
}
/*
void drawTriangle(double radius, int divisions) {
    double deltaAng = TWOPI / divisions;
    double curAng = 0;
    //glBegin(GL_LINE_LOOP);
    
     glBegin(GL_TRIANGLES);
    for(int i=0; i<divisions; i++){
     double x = radius * cos(curAng);
     double y = radius * sin(curAng);
     double z = radius * tan(curAng);
        if( agDebug ) cout << "Triangle " << i << " x= " << x << " y= " << y << " z= " << z << endl;
        glVertex3f( x, y, z );
        curAng += deltaAng;
        if( agDebug ) cout << "curAng= " << curAng << " deltaAng= " << deltaAng << endl;
     glColor3f(0.1, 0.2, 0.3);
        glVertex3f(x,y,z);
    }
     glEnd();
*/

void Agent::Draw() {

     glColor3f(0.2, 0.2, 0.8);
     if (IsAdversary)
          glColor3f(1.0, 0.0, 0.0);

     glPushMatrix();
     glTranslatef(pos.GetX(), pos.GetY(), 0);
     glRasterPos2d(.3, .3);
     string Distance = to_string(DistanceRemaining);
     for (int i = 0; i < 3; i++){
          glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, Distance[i]);
     }
     drawCircle(radius, 30);
     //drawTriangle(radius, 20)
     glPopMatrix();

     if (drawForce) {
          glColor3f(1.0, 0.48, 0.0);
          glBegin(GL_LINES);
          glVertex2f(pos.GetX(), pos.GetY());
          glVertex2f(pos.GetX() + separationForce.GetX(), pos.GetY() + separationForce.GetY());
          glEnd();

          glColor3f(0.0, 0.0, 1.0);
          glBegin(GL_LINES);
          glVertex2f(pos.GetX(), pos.GetY());
          glVertex2f(pos.GetX() + cohesionForce.GetX(), pos.GetY() + cohesionForce.GetY());
          glEnd();

          glColor3f(0.0, 1.0, 0.0);
          glBegin(GL_LINES);
          glVertex2f(pos.GetX(), pos.GetY());
          glVertex2f(pos.GetX() + alignmentForce.GetX(), pos.GetY() + alignmentForce.GetY());
          glEnd();
     }

     if (drawVelocity) {
          glColor3f(1.0, 1.0, 0.0);
          glBegin(GL_LINES);
          glVertex2f(pos.GetX(), pos.GetY());
          glVertex2f(pos.GetX() + vel.GetX(), pos.GetY() + vel.GetY());
          glEnd();
     }
     else {
         cout << "Do nothing" << endl;
     }
}
