#ifndef MACOS
#include <windows.h>
#pragma comment(lib, "user32.lib") 
#endif

#include "Agent.h"
#include "Environment.h"

#include <GL/glut.h>  // (or others, depending on the system in use)
bool agDebug = false; //true;//false;//true;
bool m_Cal3DDebug = false;
extern Environment* gEnv;
extern bool doSkeletalAnimation;

Agent::Agent() { 
  initialized = false;
  drawForce = true;
  drawVelocity = true;
  drawVR = false;
  qobj = NULL;
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
  ori = 0;
  radius = 5;
  
  drawForce = true;
  drawVelocity = true;
  drawVR = false;
  
  separationComponent = 10;
  cohesionComponent = 10;
  alignmentComponent = 10;
  pathComponent = 200;
  cal3dInitialized = false;
  qobj = NULL;
}

Agent::Agent(const Agent& other) {
  initialized = other.initialized;
  id = other.id;
  pos = other.pos;
  vel = other.vel;
  separationComponent = other.separationComponent;
  cohesionComponent = other.cohesionComponent;
  alignmentComponent = other.alignmentComponent;
  pathComponent = other.pathComponent;
  radius = other.radius;
  mass = other.mass;
  maxVel = other.maxVel;
  maxAccel = other.maxAccel;
  viewRadius = other.viewRadius;
  gridCell = other.gridCell;
  drawForce = other.drawForce;
  drawVelocity = other.drawVelocity;
  drawVR = other.drawVR;
  ori = other.ori;
  cal3dInitialized = false;
  qobj = NULL;
}

void setAgentValues(string& path, string& modelName) {
	vector<string> paths;
	vector<string> models;
	/*
	paths.push_back("cal3ddata/aliencal3d-anim2/");
	models.push_back("alien_corrected.cfg");
	paths.push_back("cal3ddata/armyman-cal3d2/");
	models.push_back("armyman.cfg");
	paths.push_back("cal3ddata/basicwoman2-cal3d2/");
	models.push_back("basicwoman.cfg");
	paths.push_back("cal3ddata/cartoonalien-cal3d2/");
	models.push_back("cartoonalien.cfg");
	paths.push_back("cal3ddata/coolkid-cal3d2/");
	models.push_back("coolkid.cfg");
	*/
	paths.push_back("cal3ddata/dino-cal3d2/");
	models.push_back("dino.cfg");
	paths.push_back("cal3ddata/fatboy-cal3d2/");
	models.push_back("fatboy.cfg");
	paths.push_back("cal3ddata/figure-cal3d2/");
	models.push_back("figure.cfg");
	paths.push_back("cal3ddata/masterchief-cal3d2/");
	models.push_back("masterchief.cfg");
	paths.push_back("cal3ddata/suitman-cal3d2/");
	models.push_back("suitman_corrected.cfg");
	paths.push_back("cal3ddata/terminator-cal3d2/");
	models.push_back("terminator.cfg");
	paths.push_back("cal3ddata/simpleman_costumes2/");
	models.push_back("simple-man_purple&light.cfg");
	paths.push_back("cal3ddata/simpleman_costumes2/");
	models.push_back("simple-man_black&medium.cfg");
	paths.push_back("cal3ddata/simpleman_costumes2/");
	models.push_back("simple-man_blue&medium.cfg");
	paths.push_back("cal3ddata/simpleman_costumes2/");
	models.push_back("simple-man_green&black.cfg");
	paths.push_back("cal3ddata/coolkid-cal3d2/");
	models.push_back("coolkid.cfg");
	/*
	*/
	int index = rand() % paths.size();
	path = paths[index];
	modelName = models[index];
	cout << "path='" << path << "' modeName='" << modelName << "'" << endl;
	//path = "cal3ddata/simpleman_costumes2/";
	//modelName = "simple-man_blue&medium.cfg";
}

void Agent::Update(vector<Agent>& agents, double dt) {
  if( !initialized ) {
    cout << "Agent::Update id: " << id << " HAS NOT BEEN INITIALIZED> " << endl;
    exit(-1);
  }
  if (!cal3dInitialized && doSkeletalAnimation) {
	  cout << "Agent::cal3d Not Initialized! id: " << id << "...initializing." << endl;
	  //vector<string> models = getTagsFromString(m_cal3dmodelAnims, ",");
	  //int rindex = lrand48() % models.size();
	  string m_cal3dmodelPath = "";// "cal3ddata/simpleman_costumes2/";
	  //string selected_model = "simple-man.cfg";//models[rindex];
	  string selected_model = "";// "simple-man_blue&medium.cfg";
	  setAgentValues(m_cal3dmodelPath, selected_model);
	  string full_model = m_cal3dmodelPath + selected_model;
	  m_Cal3dRunSpeed = make_pair(6.0, 1.0);
	  m_Cal3dWalkSpeed = make_pair(4.0, 1.0);
	  m_Cal3dWaitSpeed = make_pair(0.2, 0.8);
	  m_cal3dAnimationSpeed = 0.05;
	  m_cal3dScaleFactor = 0.15;
	  //m_cal3dScaleFactor = 0.45;
	  InitCal3dModel(m_cal3dmodelPath, full_model, m_Cal3dRunSpeed, m_Cal3dWalkSpeed, m_Cal3dWaitSpeed, m_cal3dAnimationSpeed, m_cal3dScaleFactor);
	  cal3dInitialized = true;
  }

  //////////////////////////////////////////////////////////////////
  //find simple path
  Vector3d pathForce(0,0,0);
  
  if( pathCells.empty() ) {
    if(agDebug) cout << "Calling FindPathToRandomFreeCell!" << endl;
    gEnv->FindPathToRandomFreeCell(pos, pathCells);
  }
  else {
    //grab the last cell's center, generate a force in that direction
    int pathIndex = pathCells.size()-1;
    Vector3d& goalLoc = gEnv->cells[pathCells[pathIndex].first][pathCells[pathIndex].second].center;
    //create a force in that direction
    pathForce = goalLoc - pos;
    double mag = pathForce.norm();
    if( mag > pathComponent ) {
      pathForce.selfScale(pathComponent);
    }
    //if distance is small pop it off
    if( mag < 5 ) { //pop off
      pathCells.pop_back();
    }
  }
  //////////////////////////////////////////////////////////////////

  //double separationComponent = 240;
  Vector3d separation(0,0,0);
  Vector3d cohesion(0,0,0);
  Vector3d alignment(0,0,0);
  Vector3d center(0,0,0);
  Vector3d force(0,0,0);
  separationForce.reset();
  cohesionForce.reset();
  alignmentForce.reset();
  int numSeen = 0;
  if( agDebug ) cout << "Update for agent: " << id << endl;
  //generate the forces
  for(int i=0; i<(int)agents.size(); i++) {
    if( id == i ) continue; //skip self
    //see if within view radius
    double dist = (agents[i].GetPos()-pos).norm();
    if( dist < viewRadius ) {
      if( agDebug ) cout << "Agent: " << id << " is within range of agent: " << i << " dist= " << dist << " viewRadius= " << viewRadius << endl;
      if( agDebug ) cout << "SEPDBG agent[" << i << "].GetPos() " << agents[i].GetPos() << " pos["<<id<< "]" << pos << endl;
      if( agDebug ) cout << "SEPDBG agent["<<i<<"]: separation component: " << agents[i].GetPos()-pos << endl;
      //separation += (agents[i].GetPos()-pos).normalize()*(1.0-dist/viewRadius);
      separation += (pos-agents[i].GetPos()).normalize()*(1.0-dist/viewRadius);
      center += agents[i].GetPos();
      alignment += agents[i].GetVel();
      numSeen++;
    }
  }//endfor i

  if( numSeen>0 ) {
    center /= 1.0*numSeen;
    double distToCenter = (center-pos).norm();
    cohesion = (center-pos).normalize()*cohesionComponent*(distToCenter/viewRadius);
    //separation *= separationComponent;
    //alignment *= alignmentComponent;
    separation = separation.normalize() * separationComponent;
    alignment = alignment.normalize() * alignmentComponent;

    if( agDebug ) cout << "component vectors for : " << id << endl;
    if( agDebug ) cout << "sep " << separation << endl;
    if( agDebug ) cout << "coh " << cohesion << endl;
    if( agDebug ) cout << "ali " << alignment << endl;
    //force = separation + cohesion + alignment + pathForce;
    force = separation + pathForce;
    if( force.norm()>maxAccel ) force.selfScale(maxAccel);
    separationForce = separation;
    cohesionForce = cohesion;
    alignmentForce = alignment;
    //force = separation;// + cohesion + alignment;
    if( agDebug ) cout << "totalf " << force << endl;
    force /= mass;
  }
  else {
    if( agDebug ) cout << "No agents visible for agent:  " << id << endl;
    force = pathForce;
  }
  if( agDebug ) cout << "origPos " << pos << endl;
  if( agDebug ) cout << "origVel " << vel << endl;
  oldPos = pos;
  pos = pos + vel*dt;
  vel = vel + force*dt;
  if( agDebug ) cout << "newPos " << pos << endl;
  if (agDebug);
  double velMag = vel.norm();
  if( velMag > maxVel ) {
    vel.selfScale(maxVel);
    velMag = maxVel;
  }
  if (0&&id<10) cout << id << " newVel " << vel << " mag: " << vel.norm() << " maxVel: " << maxVel << endl;
  if( velMag > 0.01 ) {
    ori = std::atan2(vel.GetX(),vel.GetY());//saved as radians
  }
  if( agDebug ) cout << "newVel(trunc) " << vel << endl;

  if (doSkeletalAnimation) UpdateCal3d(velMag, "");

  bool wrapWorld=false;
  if( wrapWorld ) {
    bool updated=false;
    Vector3d pNew = gEnv->GetWrappedPosition(pos, updated);
    if( updated )
      pos = pNew;
  }

  bool cdWithAgents=true;
  if( cdWithAgents ) {
    ResolveCollisionWithOtherAgents(agents);
  }

  bool cdWorld=true;
  if( cdWorld ) {
    bool updated=false;
    //Vector3d pNew = gEnv->GetValidPosition(pos, oldPos, radius, vel, updated);
    Vector3d pNew = gEnv->GetValidPosition2(pos, oldPos, radius, vel, updated);
    if( updated ) {
      if( agDebug ) cout << "found a collision at pos: " << pos << ". resetting position to: " << pNew << endl;
      pos = pNew;
    }
  }

}


//const double TWOPI = 6.28318;
void drawCircle(double radius, int divisions) {
  double deltaAng = TWOPI / divisions;
  double curAng = 0;
  glBegin(GL_LINE_LOOP);
  for(int i=0; i<divisions; i++) {
    double x = radius * cos(curAng);
    double y = radius * sin(curAng);
    if( 0&&agDebug ) cout << "Circle " << i << " x= " << x << " y= " << y << endl;
    //glVertex2f( x, y );
	glVertex3f(x, 0, y);
	curAng += deltaAng;
    if( 0&&agDebug ) cout << "curAng= " << curAng << " deltaAng= " << deltaAng << endl;
  }
  glEnd();
}

void Agent::Draw() {
  glColor3f(0.2, 0.2, 0.8);
  glPushMatrix();
  glTranslatef(pos.GetX(), 0, pos.GetY());
  drawCircle(radius, 10);
  glPopMatrix();

  if( drawForce ) {
    glColor3f(1.0, 0.48, 0.0);
    glBegin(GL_LINES);
    glVertex3f( pos.GetX(),0,pos.GetY() );
    glVertex3f( pos.GetX()+separationForce.GetX(),0, pos.GetY()+separationForce.GetY() );
    glEnd();

    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
	glVertex3f(pos.GetX(), 0, pos.GetY());
	glVertex3f(pos.GetX() + cohesionForce.GetX(), 0, pos.GetY() + cohesionForce.GetY());
    glEnd();

    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
	glVertex3f(pos.GetX(), 0, pos.GetY());
	glVertex3f(pos.GetX() + alignmentForce.GetX(), 0, pos.GetY() + alignmentForce.GetY());
    glEnd();
  }

  if( drawVelocity ) {
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_LINES);
	//glVertex2f(pos.GetX(), pos.GetY());
	//glVertex2f(pos.GetX() + vel.GetX(), pos.GetY() + vel.GetY());
	glVertex3f(pos.GetX(), 0, pos.GetY());
	glVertex3f(pos.GetX() + vel.GetX(), 0, pos.GetY() + vel.GetY());
    glEnd();
  }
  else { /*cout << "NO drawVelocity. " << endl;*/ }

  //draw path
  if( true && gEnv != NULL ) {
    glColor3f(0,0,1);
    glBegin(GL_LINE_STRIP);
    for(int i=0; i<(int)pathCells.size(); i++) {
      Vector3d& p = gEnv->cells[pathCells[i].first][pathCells[i].second].center;
      glVertex3f(p[0],0,p[1]);
    }
    glEnd();
  }

  if (doSkeletalAnimation && m_Cal3dModel && cal3dInitialized) {
	  
	  glPushMatrix();
	  glTranslatef(pos.GetX(), 0, pos.GetY());
	  double r[16] = { cos(ori), 0, -sin(ori), 0,
		           0, 1, 0, 0,
			   sin(ori), 0, cos(ori), 0, 
			   0, 0, 0, 1 };
	  glMultMatrixd(r);
	  glRotated(-90, 1, 0, 0);

	  //draw cal3d model
	  glEnable(GL_LIGHTING);
	  glEnable(GL_LIGHT0);
	  glEnable(GL_LIGHT1);

          glEnable(GL_DEPTH_TEST);
	  //Vector3d color(0.9, 0.9, 0.9);
	  Vector3d color(0.9, 0.09, 0.09);
	  glColor3f(color.GetX(), color.GetY(), color.GetZ());
	  glEnable(GL_NORMALIZE);
	  glScaled(m_Cal3dScaleFactor, m_Cal3dScaleFactor, m_Cal3dScaleFactor);

	  m_Cal3dModel->OnRender();


	  glPopMatrix();
  }
  else {
	  //draw cylinder
	  if (qobj == NULL) {
		  cout << id << " Creating cylinder" << endl;
		  qobj = gluNewQuadric();
		  gluQuadricNormals(qobj, GLU_SMOOTH);
	  }
	  /*void gluCylinder(GLUquadric*  	quad,
	  GLdouble  	base,
	  GLdouble  	top,
	  GLdouble  	height,
	  GLint  	slices,
	  GLint  	stacks);*/
	  //cout << id << " Drawing cylinder" << endl;
	  glEnable(GL_LIGHTING);
	  glEnable(GL_LIGHT0);
	  glEnable(GL_LIGHT1);
	  Vector3d color(0.9, 0.19, 0.39);
	  glColor3f(color.GetX(), color.GetY(), color.GetZ());
	  glColor3f(1,0,0);
	  glEnable(GL_NORMALIZE);
	  glPushMatrix();	  
	  glTranslatef(pos.GetX(), 0, pos.GetY());
	  /*
	  double r[16] = { cos(ori), 0, -sin(ori), 0,
		  0, 1, 0, 0,
		  sin(ori), 0, cos(ori), 0,
		  0, 0, 0, 1 };
	  glMultMatrixd(r);
	  */
	  glRotated(-90, 1, 0, 0);
	  
	  gluCylinder(qobj, radius, radius, 4, 8, 16);
	  glPopMatrix();
  }
}

void Agent::ResolveCollisionWithOtherAgents(vector<Agent>& agents) {
  for(int i=0; i<agents.size(); i++) {
    if( id == agents[i].GetID() ) continue; //ignore self
    else{
      double dist = (pos - agents[i].GetPos()).norm();
      if( dist < (radius + agents[i].GetRadius()) ) {
	double overlap = fabs( dist-(radius +agents[i].GetRadius()) );
	Vector3d resolveDir = (pos-agents[i].GetPos()).normalize();
	pos += (overlap/2)*resolveDir;
	Vector3d& pos_i = agents[i].GetPos();
	pos_i += (-overlap/2)*resolveDir;
      }//end if dist
    }
  }//endfor i<agents
}


//////////////////////////////////////////////////////////////////////////////////
////cal3d functions
bool Agent::InitCal3dModel(string _cal3dDataPath, string _cal3dModel,
	pair<double, double> cal3dRunS,
	pair<double, double> cal3dWalkS,
	pair<double, double> cal3dWaitS,
	double animation_speed, double scale_factor) {
#ifdef CAL3D
	m_Cal3dModel = new Cal3dModel();
	//m_strDatapath="run-env/cal3ddata/";
	m_strCal3D_Datapath = _cal3dDataPath;
	//string model = "simpleman";//"skeleton";//"cally"; //"skeleton";
	//stringfull_mdoel(m_strCal3D_Datapath + model + "/simple-man_corrected.cfg"))
	string full_model = _cal3dModel;
	//m_Cal3dModel->setPath( m_strCal3D_Datapath + "skeleton/" );
	m_Cal3dModel->SetPath(m_strCal3D_Datapath);

	//if(!m_Cal3dModel->onInit(m_strCal3D_Datapath + "skeleton.cfg"))
	if (!m_Cal3dModel->OnInit(full_model)) {
		delete m_Cal3dModel;
		std::cerr << "cal3dModel initialization failed! (cally)" << std::endl;
		exit(0);
		//return false;
	}

	m_Cal3dRunSpeed = cal3dRunS;
	m_Cal3dWalkSpeed = cal3dWalkS;
	m_Cal3dWaitSpeed = cal3dWaitS;
	m_Cal3dAnimationSpeed = animation_speed;
	m_Cal3dScaleFactor = scale_factor;

	int max_num_steps_offset = 20;
	int offset_steps = 0;// lrand48() % max_num_steps_offset;
	for (int bb = 0; bb<offset_steps; bb++)
		m_Cal3dModel->OnUpdate(m_Cal3dAnimationSpeed);
#endif

	m_DoCal3d = true;
	return true;
}

void Agent::UpdateCal3d(double vel, string tag) {
	//find which place in animation squence I'm in
	int WalkIndex = 0;
	int WaitIndex = 1;
	int RunIndex = 2;
	if (tag == "DEAD") {
		motionBlend[WaitIndex] = 1;
		motionBlend[WalkIndex] = 0;
		motionBlend[RunIndex] = 0;
	}
	else if (vel < m_Cal3dWaitSpeed.first) {
		//use wait animation plus a little walk animation
		double f1 = m_Cal3dWaitSpeed.second;
		double f2 = 0;
		if (f1 < 1)
			f2 = 1 - f1;
		motionBlend[WaitIndex] = f1;
		motionBlend[WalkIndex] = f2;
		motionBlend[RunIndex] = 0;
	}
	else if (vel < m_Cal3dWalkSpeed.first) {
		//use wait and walk animation
		//this assumes v2 is set higher than v1
		double v1 = m_Cal3dWaitSpeed.first;
		double v2 = m_Cal3dWalkSpeed.first;
		double f1 = m_Cal3dWaitSpeed.second;
		double f2 = m_Cal3dWalkSpeed.second;
		double range = v2 - v1;
		double diff = vel - v1;
		f2 = f2* (diff / range); //base it off walk
		if (f2 < 1)
			f1 = 1 - f2;
		motionBlend[WaitIndex] = f1;
		motionBlend[WalkIndex] = f2;
		motionBlend[RunIndex] = 0;
	}
	else if (vel < m_Cal3dRunSpeed.first) {
		//use walk and run animation
		//this assumes v2 is set higher than v1
		double v1 = m_Cal3dWalkSpeed.first;
		double v2 = m_Cal3dRunSpeed.first;
		double f1 = m_Cal3dWalkSpeed.second;
		double f2 = m_Cal3dRunSpeed.second;
		double range = v2 - v1;
		double diff = vel - v1;
		f2 = f2* (diff / range); //base it off run
		if (f2 < 1)
			f1 = 1 - f2;
		motionBlend[WaitIndex] = 0;
		motionBlend[WalkIndex] = f1;
		motionBlend[RunIndex] = f2;
	}
	else {
		motionBlend[WaitIndex] = 0;
		motionBlend[WalkIndex] = 0;
		motionBlend[RunIndex] = 1;
	}
	//These next few lines can be used to test which motions match
	//motionBlend[0]=0;
	//motionBlend[1]=0;
	//motionBlend[2]=1;
	if (m_Cal3DDebug)
		cout << "velocity: " << vel << " motion blend: [" << motionBlend[0] << "," << motionBlend[1] << "," << motionBlend[2] << "]" << endl;
	m_Cal3dModel->SetMotionBlend(motionBlend, 0.1f);
	//call update
	m_Cal3dModel->OnUpdate(m_Cal3dAnimationSpeed);
}
