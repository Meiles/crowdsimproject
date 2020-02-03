//#include <windows.h>
#pragma comment(lib, "user32.lib") 

#include <GLUT/glut.h>  // (or others, depending on the system in use)
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
using namespace std;

#include "Color.h"
#include "Vector.h"
using namespace mathtool;
#include "Simulator.h"
#include "Environment.h"

double theta = 0;
double speedTheta = 0.1;
int drawMode=1;


int numDraws=0;
int windowW = 700;
int windowH = 700;

static int window_id;
static int menu_id;
MyColor bColor(0,0,0);
bool isConvex=true;

Simulator gSim;
Environment* gEnv = NULL;
bool isSimulating=false;
bool follow = false;
int agentsToAdd = 1;

void init() {
    glClearColor(bColor.r, bColor.g, bColor.b, 0.0); //Set display-window color to white.

     if (!follow){
          glMatrixMode(GL_PROJECTION);      //Set projection parameters.
          gluOrtho2D(-windowW / 2, windowW / 2, -windowH / 2, windowH / 2);
          gEnv = new Environment("env.txt", -windowW / 2, windowW / 2, -windowH / 2, windowH / 2);
     }
     else {
          glMatrixMode(GL_PROJECTION);      //Set projection parameters.
          gluOrtho2D(-windowW / 2, windowW / 2, -windowH / 2, windowH / 2);
     }

     if (gEnv == NULL){
          gEnv = new Environment("env.txt", -windowW / 2, windowW / 2, -windowH / 2, windowH / 2);
     }
}


void menu(int num) {
     if (num == 0) {
          glutDestroyWindow(window_id);
          exit(0);
     }

     else if (num == 1){
          agentsToAdd = 1;
     }

     else if (num == 2){
          agentsToAdd = 5;
     }

     else if (num == 3){
          agentsToAdd = 10;
     }
    
     else if (num == 4){
         agentsToAdd = 25;
     }
        
}

void createMenu() {
    menu_id = glutCreateMenu(menu);
    glutAddMenuEntry("Quit",0);
     glutAddMenuEntry("Add Single Agent", 1);
     glutAddMenuEntry("Add 5 Agents", 2);
     glutAddMenuEntry("Add 10 Agents", 3);
    glutAddMenuEntry("Add 25 Agents", 4);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int homeX, homeY;
bool mousePressed=false;
int timeSincePress=0;
int changeY=0;
int changeX=0;
double alpha = 0.5;

void customDraw();
void update() {
  if( isSimulating ) 
    gSim.Update();
  //cout << "update mousePressed=" << mousePressed << endl;
  if( mousePressed ) {
    speedTheta = changeY*0.2;
    timeSincePress += 1;
  }
  customDraw();
}
void customDraw() {
  if( ++numDraws % 100 == 0 ) {
    cout << " customDraw numDraws = " << numDraws << " drawMode: " << drawMode << endl;
  }


  glClearColor(bColor.r, bColor.g, bColor.b, 0.0);
  glClear(GL_COLOR_BUFFER_BIT);

  gEnv->Draw();
  gSim.Draw();

  glFlush();
}

void mousebutton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if( !mousePressed ) {
            homeX = x;
            homeY = y;
    
            
            
            //Using this and passing in agents to add is selected from the menu
            gSim.addAgent(agentsToAdd, 0.0, 0.0);

            timeSincePress = 0;
        }
        mousePressed = true;
        glutPostRedisplay();
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        mousePressed = false;
        timeSincePress = 0;
    }
    if( button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN ) {
      cout << "glut_middle_button == down" << endl;
    }


}

void keyboard(unsigned char key, int x, int y) {
    switch (key)
    {
        case 'q':
        case 27:             // ESCAPE key
            exit (0);
            break;
          case 'f':
            follow = !follow; break;
        case ' ':
            isSimulating = !isSimulating; break;
    }
}

void otherKeyInput(int key, int x, int y) {
  glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);                         //Initialize GLUT.
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);   //Set display mode.
    glutInitWindowSize(windowW, windowH);                  //Set display-window width and height.
    
    window_id = glutCreateWindow("Flocking in 2D!");  //Create display window.
    init();                                        //Execute initialization procedure.
    createMenu();
    
    glutIdleFunc(update);
    glutDisplayFunc(customDraw);                   //Send graphics to display window.
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mousebutton);                    //How to handle mouse press events.
    glutSpecialFunc(otherKeyInput);
    glutMainLoop();                                //Display everything and wait.
}
