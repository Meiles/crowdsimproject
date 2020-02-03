#ifndef MACOS
#include <windows.h>
#pragma comment(lib, "user32.lib") 
#endif

#include "Environment.h"
#include <fstream>

#include <GL/glut.h>  // (or others, depending on the system in use)

bool envDebug = false; //true;//false;
///////////////////////////////////////////////////////////
Cell::Cell() {  
  blocked = false;
  isCol = false;
}
void Cell::Init(int _i, int _j, Vector3d _center, double _dx, double _dy, bool _blocked) {
  xi = _i;
  xj = _j;
  dx = _dx;
  dy = _dy;
  center = _center;
  minPt[0] = center[0]-dx/2.0;
  minPt[1] = center[1]-dy/2.0;
  maxPt[0] = center[0]+dx/2.0;
  maxPt[1] = center[1]+dy/2.0;
  blocked = _blocked;
  isCol = false;
}

void Cell::Draw() {

  glLineWidth(1);
  if( blocked ) {
    if( isCol )
      glColor3f(0.6,0.9,0.6);
    else
      glColor3f(0.6,0.6,0.6);
    glBegin(GL_POLYGON);
  }
  else {
    glColor3f(0.6, 0.6, 0.6);
    glBegin(GL_LINE_LOOP);
  }
  //glVertex2f(minPt[0], minPt[1]);
  //glVertex2f(maxPt[0], minPt[1]);
  //glVertex2f(maxPt.x, maxPt[1]);
  //glVertex2f(minPt.x, maxPt[1]);
  glVertex3f(minPt[0], 0, minPt[1]);
  glVertex3f(maxPt[0], 0, minPt[1]);
  glVertex3f(maxPt[0], 0, maxPt[1]);
  glVertex3f(minPt[0], 0, maxPt[1]);
  glEnd();
}

bool Cell::IsCollision(Vector3d pNew, double radius) {
  if( (pNew[0]<(center.GetX()+dx/2+radius)) &&
      (pNew[0]>(center.GetX()-dx/2-radius)) &&
      (pNew[1]<(center.GetY()+dy/2+radius)) &&
      (pNew[1]>(center.GetY()-dy/2-radius)) ) {
    //cout << "pNew=" << pNew << " radius=" << radius << " dx= " << dx << " dy=" << dy << endl;
    //cout << "condition1=" << (center.GetX()+dx+radius) << endl;
    //cout << "condition2=" << (center.GetX()-dx-radius) << endl;
    //cout << "condition3=" << (center.GetY()+dy+radius) << endl;
    //cout << "condition4=" << (center.GetY()-dy-radius) << endl;
    isCol = true;
    return true;
  }
  return false;
}


//this function will set maximums for the cell and return values
//like whether or not the x/y is in collision, the
//min/max values, and which condition is violated
void Cell::EdgeCheck(Vector3d p, double radius, bool& flipX, bool& flipY, double& txmin, double& txmax, double& tymin, double& tymax, int& xcondition, int& ycondition) {
  txmin = center.GetX()-dx/2-radius;
  txmax = center.GetX()+dx/2+radius;
  tymax = center.GetY()+dy/2+radius;
  tymin = center.GetY()-dy/2-radius;
  double perc = 0.2;
  double percInX = perc*(txmax - txmin);  
  double percInY = perc*(tymax - tymin);
  xcondition = -1;
  ycondition = -1;
  flipX = false;
  flipY = false;
  if( p[0]>txmin && p[0]<txmax ) {
    //flipX = true;
	if ((fabs(p[0] - txmax) < fabs(p[0] - txmin)) && (percInX > (fabs(p[0] - txmax))) ) {
		flipX = true;
		xcondition = 1;
	}
	else if ((fabs(p[0] - txmax) > fabs(p[0] - txmin)) && (percInX > (fabs(p[0] - txmin))) ) {
		flipX = true;
		xcondition = 0;
	}
  } 
  if( (p[1]>tymin && p[1]<tymax)  ) {
    //flipY = true;
	if (fabs(p[1] - tymax) < fabs(p[1] - tymin) && (percInY > (fabs(p[1] - tymax)))) {
		  flipY = true;
		  ycondition = 1;
	  }
	else if (fabs(p[1] - tymax) > fabs(p[1] - tymin) && (percInY > (fabs(p[1] - tymax)))) {
		flipY = true;
		ycondition = 0;
	}
  }
}

///////////////////////////////////////////////////////////
Environment::Environment() {
	initialized = false;
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
  dx = (_xmax-_xmin)/columns;
  dy = (_ymax-_ymin)/rows;
  string colData;
  string tmpStr;
  getline(infile, tmpStr); //just to get rid of carriage return
  int rowIndex=0;
  while( getline(infile, colData) ) {
    //cout << rowIndex << " going to process '"<<colData<<"'"<<endl;
    for(int j=0; j<(int)colData.length(); j++) {
      Cell& cell = cells[rowIndex][j];
      bool isBlocked = (colData.at(j)=='x'||colData.at(j)=='X')? true: false;
      Vector3d center(xmin+ dx/2+dx*j,ymin+ dy/2+dy*rowIndex,0);
      //cout << "Initializing cell: " << rowIndex << ", " << i << " center: " << center.GetX() << " " << center.GetY() << " dx " << dx << " dy " << dy << " isBlocked " << isBlocked << endl;
      cell.Init(rowIndex,j,center,dx,dy,isBlocked);
    }
    rowIndex++;
  }
  infile.close();
  initialized = true;
}


void Environment::Draw() {
  if (!initialized) return;
  for(int i=0; i<rows; i++) 
    for(int j=0; j<columns; j++) {
      cells[i][j].Draw();
    }
}
pair<int,int> Environment::GetCellIndices(Vector3d p) {
  if(envDebug) cout << "Env::GCI p= " << p << " [" << xmin << "," << xmax << "|" << ymin << "," << ymax << "] dx="<<dx<<" dy="<<dy<< endl;
  int j = int((p[0]-xmin)/dx);
  int i = int((p[1]-ymin)/dy);
  //cout << "1i " << int((p[0]-xmin)/(xmax-xmin)) << " j " << j << endl;
  if( i<0 ) i = 0;
  if( j<0 ) j = 0;
  if( j>=columns) j = columns-1;
  if( i>=rows ) i = rows-1;
  //cout << "2i " << i << " j " << j << endl;
  return make_pair(i,j);
}

pair<int,int> Environment::GetClosestBlocked(pair<int,int> curCellInd, Vector3d pos, double radius) {
  int numXToOffset = int(dx/radius + 0.5) + 1;
  int numYToOffset = int(dy/radius + 0.5) + 1;
  int xIndexStart = curCellInd.second-numXToOffset;
  int yIndexStart = curCellInd.first-numYToOffset;
  if( xIndexStart < 0 ) xIndexStart = 0;
  if( yIndexStart < 0 ) yIndexStart = 0;
  int xIndexEnd = curCellInd.second+numXToOffset;
  int yIndexEnd = curCellInd.first+numYToOffset;
  if( xIndexEnd >= columns ) xIndexEnd = columns - 1;
  if( yIndexEnd >= rows ) yIndexEnd = rows - 1;
  if(envDebug)cout << "GetClosestBlocked xStart " << xIndexStart << " xEnd " << xIndexEnd << " yStart " << yIndexStart << " yEnd " << yIndexEnd << " nRows " << rows << " nColumns " << columns << endl;
  pair<int,int> closestCellIndex(-1,-1);
  double closeDist = 1e6;
  for(int i=yIndexStart; i<=yIndexEnd; i++) {
    for(int j=xIndexStart; j<=xIndexEnd; j++) {
      if(envDebug) cout << " checking cell i,j: [" << i << "," << j << "]" << endl;
      if(cells[i][j].blocked) {
	double dist = (cells[i][j].GetCenter()-pos).norm();
	if( dist < closeDist ) {
	  closeDist = dist;
	  closestCellIndex = make_pair(i,j);
	}
      }
    }//endfor j
  }//endfor i
  return closestCellIndex;
}

Vector3d Environment::GetWrappedPosition(Vector3d p, bool& updated) {
  Vector3d pNew(p);
  if( pNew.GetX()>xmax ) {
    pNew.SetX( pNew.GetX() - (xmax-xmin) );
    updated=true;
  }
  else if( pNew.GetX()<xmin ) {
    //pNew[0] += (xmax-xmin);
	pNew.SetX(pNew.GetX() + (xmax - xmin));
    updated=true;
  }

  if( pNew.GetY()>ymax ) {
    //pNew[1] -= (ymax-ymin);
	pNew.SetY(pNew.GetY() - (ymax - ymin));
    updated=true;
  }
  else if( pNew.GetY()<ymin ) {
	  //pNew[1] += (ymax - ymin);
	  pNew.SetY(pNew.GetY() + (ymax - ymin));
    updated=true;
  }
  return pNew;
}

/*
Vector3d Environment::GetValidPosition(Vector3d p, Vector3d oldP, double radius, Vector3d& vel, bool& updated) {
  Vector3d pNew(p);
  if( pNew.GetX()+radius>xmax ) {
    pNew = oldP;
    pNew[0] = xmax - radius;
    vel[0] *=-1;
    updated=true;
  }
  else if( pNew.GetX()-radius<xmin ) {
    pNew = oldP;
    pNew[0] = xmin + radius;
    vel[0] *=-1;
    updated=true;
  }

  if( pNew.GetY()+radius>ymax ) {
    pNew[1] = ymax - radius;
    vel[1] *=-1;
    updated=true;
  }
  else if( (pNew.GetY()-radius)<ymin ) {
    pNew[1] = ymin + radius;
    vel[1] *=-1;
    updated=true;
  }

  //return pNew;
  //////////////////////////////////////////////////////////////
  //find current cell
  //find nearest blocked cell
  //if p is in collision set to oldP and reflect
  pair<int,int> curCellInd = GetCellIndices(pNew);
  if(envDebug) cout << "Point pNew: " << pNew << " in cell: " << curCellInd.first << "," << curCellInd.second << endl;
  pair<int,int> closestBlocked = GetClosestBlocked(curCellInd, pNew, radius);
  if(envDebug) cout << "Point pNew: " << pNew << " closest blocked cell: " << closestBlocked.first << "," << closestBlocked.second << endl;

  if( closestBlocked.first != -1 && closestBlocked.second != -1 ) {
    //here is where the position may be updated
    //check if collision with the cell
    int j = closestBlocked.first;
    int i = closestBlocked.second;
    Cell& cell = cells[j][i];
    //top part of cell
    if( cell.IsCollision(pNew, radius) ) {
      //found collision
      cout << "found collision!!! oldVel=" << vel << endl;
      
      updated=true;
      bool flipX=false;
      bool flipY=false;
      double txmin, txmax, tymin, tymax; 
      int tcondX, tcondY;
      cell.EdgeCheck(pNew,radius, flipX, flipY, txmin, txmax, tymin, tymax, tcondX, tcondY);
	  cout << "pNew: " << pNew << endl;
	  cout << "flipX: " << flipX << " flipY: " << flipY << " txmin: " << txmin << " txmax: " << txmax << " tcondX: " << tcondX
		  << " tymin: " << tymin << " tymax: " << tymax << " tcondY: " << tcondY << endl;
      if( flipX ) {
	      //vel[0] *= -1.0;
		  double xVel = vel.GetX();
		  xVel *= -1;
		  vel.SetX(xVel);
	      if( tcondX==1 ) pNew[0] = txmax;
	      else if( tcondX== 0) pNew[0] = txmin;
      }
      if( flipY ) {
	      //vel[1] *= -1.0;
		  double yVel = vel.GetY();
		  yVel *= -1;
		  vel.SetY(yVel);
	      if( tcondY==1 ) pNew[1] = tymax;
	      else if( tcondY==0) pNew[1] = tymin;
      }
      cout << " new vel = " << vel << endl;
    }
  }

  return pNew;
}
*/

Vector3d Environment::GetValidPosition2(Vector3d p, Vector3d oldP, double radius, Vector3d& vel, bool& updated) {
  if( envDebug ) {
    cout << "Env::GetValidPosition2 p: " << p << " oldP " << oldP << " radius: " << radius << " vel: " << vel << endl;
  }
  Vector3d pNew(p);
  bool velUpdated = false;
  if( pNew.GetX()+radius>xmax ) {
    pNew = oldP;
    pNew[0] = xmax - radius;
    vel[0] *=-1;
    updated=true;
    velUpdated=true;
  }
  else if( pNew.GetX()-radius<xmin ) {
    pNew = oldP;
    pNew[0] = xmin + radius;
    vel[0] *=-1;
    updated=true;
    velUpdated=true;
  }

  if( pNew.GetY()+radius>ymax ) {
    pNew[1] = ymax - radius;
    vel[1] *=-1;
    updated=true;
    velUpdated=true;
  }
  else if( (pNew.GetY()-radius)<ymin ) {
    pNew[1] = ymin + radius;
    vel[1] *=-1;
    updated=true;
    velUpdated=true;
  }

  //return pNew;
  //////////////////////////////////////////////////////////////
  //find current cell
  //find nearest blocked cell
  //if p is in collision set to oldP and reflect
  pair<int,int> curCellInd = GetCellIndices(pNew);
  if(envDebug) cout << "Point pNew: " << pNew << " in cell (curCellInd): " << curCellInd.first << "," << curCellInd.second << endl;
  pair<int,int> closestBlocked = GetClosestBlocked(curCellInd, pNew, radius);
  if(envDebug) cout << "Point pNew: " << pNew << " closest blocked cell: " << closestBlocked.first << "," << closestBlocked.second << endl;

  if( closestBlocked.first != -1 && closestBlocked.second != -1 ) {
    //here is where the position may be updated
    //check if collision with the cell
    int i = closestBlocked.first;
    int j = closestBlocked.second;
    Cell& cell = cells[i][j];
    //top part of cell
    if( cell.IsCollision(pNew, radius) ) {
      //found collision
      if( envDebug ) cout << "found collision(2)!!! pNew=" << pNew << " oldVel=" << vel << endl;
      
      updated=true;
      Vector3d cellCenter = cell.GetCenter();
      Vector3d pushDir = (pNew-cellCenter).normalize();
      if(!velUpdated) vel *= -1.0;
      bool stillColliding = true;
      do {
	//push position out by epsilon
	pNew += 0.05 * pushDir;
	stillColliding = cell.IsCollision(pNew, radius);
      }while(stillColliding);

    }//endif cell.IsCollision
  }

  return pNew;
}

void Environment::MakeEmptyEnv() {
  for(int i=0; i<rows; i++) {
    for(int j=0; j<columns; j++) {
      cells[i][j].blocked = false;
    }
  }
}

void Environment::ToggleBlockedCell(double tx, double ty) {
  Vector3d p(tx,ty,0);
  pair<int,int> cellInd = GetCellIndices(p);
  cout << "Cell indices = " << cellInd.first << "," << cellInd.second << endl;
  cells[cellInd.first][cellInd.second].blocked = !cells[cellInd.first][cellInd.second].blocked;
}

void Environment::FindPathToRandomFreeCell(Vector3d p, vector< pair<int,int> >& cellPath) {
  //find the grid cell p is in
  pair<int,int> curCellInd = GetCellIndices(p);
  if( cells[curCellInd.first][curCellInd.second].blocked ) {
    cout << "Attempting to create path from a blocked cell!!! Returning. " << endl;
    return;
  }
  //find a random grid cell
  bool inBlockedCell = false;
  int r_i;
  int r_j;
  do {
    r_i = rand() % rows;
    r_j = rand() % columns;
    inBlockedCell = cells[r_i][r_j].blocked;
  } while( inBlockedCell );
  //call find path from grid cell to grid cell
  cellPath.clear();
  
  if(envDebug) cout << "Finding path from curCellInd: " << curCellInd.first << "," << curCellInd.second << " to goalCell: " << r_i << "," << r_j << endl;

  DijkstrasFindPath(curCellInd, make_pair(r_i,r_j), cellPath);

  if( 1 && envDebug ) {
    cout << "Path Found: ";
    for(int i=0; i<cellPath.size(); i++) {
      cout << "(" << i << ") [ " << cellPath[i].first << "," << cellPath[i].second << "] ";
    }
    cout << endl;
  }
}

void Environment::DijkstrasFindPath(pair<int,int> curCellInd, pair<int,int> goalCellInd, vector< pair<int,int> >& cellPath) {
  ResetCellValues();
  cells[curCellInd.first][curCellInd.second].visited = true;
  cells[curCellInd.first][curCellInd.second].tentativeDist = 0;
  vector<Cell*> unvisited;
  unvisited.push_back( &cells[curCellInd.first][curCellInd.second] );
  bool foundPath = false;
  while(!foundPath && !unvisited.empty()) {
    Cell* cur = unvisited[0];
    unvisited.erase(unvisited.begin());
    if( cur->xi == goalCellInd.first && cur->xj == goalCellInd.second ) {
      foundPath = true;
    }
    else {
      //foreach neighbor - add to unvisited with new tentative distance
      for(int i=cur->xi-1; i<=cur->xi+1; i++) {
	for(int j=cur->xj-1; j<=cur->xj+1; j++) {
	  if( i==cur->xi && j==cur->xj ) continue; //this will be cur node
	  if( i<0 || i>=rows ) continue; //out of bounds
	  if( j<0 || j>=columns ) continue; //out of bounds
          if( cells[i][j].blocked ) continue;//don't add blocked cells

	  if( i!=cur->xi && j!=cur->xj ) { //disallow diagonal moves in certain cases
	    if( cells[cur->xi][j].blocked ) continue;
	    if( cells[i][cur->xj].blocked ) continue;
	  }
          /*
	  if( i<cur->xi && j<cur->xj ) { //disallow diagonal moves in certain cases
	    if( cells[cur->xi][j].blocked ) continue;
	    if( cells[i][cur->xj].blocked ) continue;
	  }
	  if( i<cur->xi && j>cur->xj ) { //disallow diagonal moves in certain cases
	    if( cells[cur->xi][j].blocked ) continue;
	    if( cells[i][cur->xj].blocked ) continue;
	  }
	  if( i>cur->xi && j<cur->xj ) { //disallow diagonal moves in certain cases
	    if( cells[cur->xi][j].blocked ) continue;
	    if( cells[i][cur->xj].blocked ) continue;
	  }
	  */
	  double ijTentativeDist = cur->tentativeDist + (cur->center-cells[i][j].center).norm();
	  if( ijTentativeDist < cells[i][j].tentativeDist ) {
	    //add to unvisited...also save parent
	    cells[i][j].parent = make_pair(cur->xi,cur->xj);
	    cells[i][j].tentativeDist = ijTentativeDist;
	    unvisited.push_back(&cells[i][j]);
	  }
	}//endfor j
      }//endfor i
    }
  }//endwhile

  if( foundPath ) {
    //backtrack
    pair<int,int> curBackTrackCell = goalCellInd;
    while( !((curBackTrackCell.first == curCellInd.first) &&
           (curBackTrackCell.second== curCellInd.second)) ) {
      cellPath.push_back(curBackTrackCell);
      curBackTrackCell = cells[curBackTrackCell.first][curBackTrackCell.second].parent;
    }//endwhile
    cellPath.push_back(curBackTrackCell);

  }
  
}

void Environment::ResetCellValues() {
  for(int i=0; i<rows; i++) {
    for(int j=0; j<columns; j++) {
      cells[i][j].tentativeDist = 1e7;
      cells[i][j].parent = make_pair(-1,-1);
      cells[i][j].visited = false;
    }
  }
}
