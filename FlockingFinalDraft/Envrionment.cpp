//#include <windows.h>
#pragma comment(lib, "user32.lib") 

#include "Environment.h"
#include <fstream>

#include <GLUT/GLUT.h>  


Cell::Cell() {  
  blocked = false;
}
void Cell::Init(int _i, int _j, Point _center, double _dx, double _dy, bool _blocked) {
  xi = _i;
  xj = _j;
  dx = _dx;
  dy = _dy;
  center = _center;
  minPt.x = center.x-dx/2.0;
  minPt.y = center.y-dy/2.0;
  maxPt.x = center.x+dx/2.0;
  maxPt.y = center.y+dy/2.0;
  blocked = _blocked;
}

void Cell::Draw() {

  if( blocked ) {
    glColor3f(1,0,0);
    glBegin(GL_POLYGON);
  }
  else {
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
  }
  glVertex2f(minPt.x, minPt.y);
  glVertex2f(maxPt.x, minPt.y);
  glVertex2f(maxPt.x, maxPt.y);
  glVertex2f(minPt.x, maxPt.y);
  glEnd();
}

Environment::Environment() {
}
Environment::Environment(string _inputFile, double _xmin, double _xmax, double _ymin, double _ymax ) {
  inputFile = _inputFile;

  xmin = _xmin;
  xmax = _xmax;
  ymin = _ymin;
  ymax = _ymax;
  cout << " Environment boundaries: [" << xmin << ", " << xmax << " | " << ymin << ", " << ymax << "]"<< endl;

  cout << "Attempting to load environment file: " << inputFile << endl;
  ifstream infile;
  infile.open(inputFile.c_str());
  if( !infile ) {
    cout << "Could NOT open infile: " << inputFile << endl;
    //exit(-1);
  }
  if(infile >> rows) {
    cout << "There will be " << rows << " rows ";
  }
  if(infile >> columns) {
    cout << "There will be " << columns << " rows ";
  }
  cells = new Cell*[rows];
  for(int i=0; i<rows; i++) {
    cells[i] = new Cell[columns];
  }
  double dx = (_xmax-_xmin)/columns;
  double dy = (_ymax-_ymin)/rows;
  string colData;
  string tmpStr;
  getline(infile, tmpStr); //just to get rid of carriage return
  int rowIndex=0;
  while( getline(infile, colData) ) {
    //cout << rowIndex << " going to process '"<<colData<<"'"<<endl;
    for(int i=0; i<colData.length(); i++) {
      Cell& cell = cells[rowIndex][i];
      bool isBlocked = (colData.at(i)=='x'||colData.at(i)=='X')? true: false;
      Point center(xmin+ dx/2+dx*i,ymin+ dy/2+dy*rowIndex);
      cell.Init(rowIndex,i,center,dx,dy,isBlocked);
    }
    rowIndex++;
  }
  infile.close();
}


void Environment::Draw() {
  for(int i=0; i<rows; i++) 
    for(int j=0; j<columns; j++) {
      cells[i][j].Draw();
    }
}

Vector3d Environment::GetWrappedPosition(Vector3d p, bool& updated) {
  Vector3d pNew(p);
  if( pNew.GetX()>xmax ) {
    pNew.SetX( pNew.GetX() - (xmax-xmin) );
    updated=true;
  }
  else if( pNew.GetX()<xmin ) {
    pNew.SetX(pNew.GetX() + (xmax - xmin));
    updated=true;
  }

  if( pNew.GetY()>ymax ) {
   
    pNew.SetY(pNew.GetY() - (ymax - ymin));
    updated=true;
  }
  else if( pNew.GetY()<ymin ) {
    pNew.SetY(pNew.GetY() + (ymax - ymin));
    updated=true;
  }
  return pNew;
}



Vector3d Environment::GetForceFromWall(Vector3d p, double maxWallForce){
    double closestDistance = fabs(p.GetX() - xmin);
    Vector3d force(1, 0, 0);
    double distance = fabs(p.GetX() - xmax);
    if (distance < closestDistance) {
        closestDistance = distance;
        force.SetX(-1);
        force.SetY(0);
    }
    distance = fabs(p.GetY() - ymax);
    if (distance < closestDistance) {
        closestDistance = distance;
        force.SetX(0);
        force.SetY(-1);
    }
    distance = fabs(p.GetY() - ymin);
    if (distance < closestDistance) {
        closestDistance = distance;
        force.SetX(0);
        force.SetY(1);
    }
    double range = xmax - xmin;
    force *= maxWallForce * (1 - closestDistance / range);
    return force;
}


Vector3d Environment::GetResolvedPosition(Vector3d p, double r, bool& updated) {
     Vector3d pNew(p);
     if (pNew.GetX()+r>xmax) {
          pNew.SetX(xmax-r);
          updated = true;
     }
     else if (pNew.GetX()-r<xmin) {
          pNew.SetX(xmin-r);
          updated = true;
     }

     if (pNew.GetY()+r>ymax) {

          pNew.SetY(ymax-r);
          updated = true;
     }
     else if (pNew.GetY()-r<ymin) {
          pNew.SetY(ymin-r);
          updated = true;
     }
     return pNew;
}

