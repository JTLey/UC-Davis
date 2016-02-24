//
//  main.cpp
//  ECS175_Proj2
//
//  Created by Jacob Ley on 10/20/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#include <iostream>
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#  include <GL/freeglut.h>
#endif
#include "Shape.h"
#include <fstream>
#include <cmath>
using namespace std;
void draw();
bool x;
Shape S1, S2, S3, S4, S5, *S;
int numshape;
int MainWindow, xyWindow, xzWindow, yzWindow;
int verts[6][50][2];
int Vec_id, Tran_id, Scal_id, Rot_id, Menu_id, Cor_id, File_id, sign=1;
int menvalue=31, oldvalue=31;
float MasterMax, MasterMin, scalfac, DeltaZ, DeltaY, DeltaX;
float X1, X2, Y1, Y2, Z1, Z2;
string filestring = "3dcoordinates.txt"; //Initial File Name

void load(){ //Load function, reads in data from 3dcoordinates.txt
  string line;
  int n,a,b;
  float x,y,z;
  ifstream myfile(filestring);
  if (myfile.is_open()){
    getline(myfile,line);
    numshape=stoi(line);
    for (int i=0;i<numshape;i++){
  if (i==0) {
    S=&S1;
  }
  else if (i==1) {
    S=&S2;
  }
  else if (i==2) {
    S=&S3;
  }
  else if (i==3) {
    S=&S4;
  }
  else if (i==4) {
    S=&S5;
  }
  getline(myfile,line);
  n=stoi(line);
  for (int j=0; j<n;j++){
    getline(myfile,line);
    size_t pos = line.find(" ");
    x = stof(line.substr(0,pos));
    line.erase(0 , pos+1);
    pos = line.find(" ");
    y = stof(line.substr(0,pos));
    line.erase(0, pos+1);
    z = stof(line);
    S->addvert(x, y, z);
  }
  getline(myfile,line);
  n=stoi(line);
  verts[5][i][0]=n;
  for (int j=0;j<n;j++){
    getline(myfile,line);
    size_t pos = line.find(" ");
    a = stoi(line.substr(0,pos))-1;
    line.erase(0 , pos+1);
    b = stoi(line)-1;
    verts[i][j][0]=a;
    verts[i][j][1]=b;
  }
          }
getline(myfile,line);
size_t pos = line.find(" ");
X1 = stof(line.substr(0,pos));
line.erase(0 , pos+1);
pos = line.find(" ");
Y1 = stof(line.substr(0,pos));
line.erase(0, pos+1);
Z1 = stof(line);
getline(myfile,line);
pos = line.find(" ");
X2 = stof(line.substr(0,pos));
line.erase(0,pos+1);
pos = line.find(" ");
Y2 = stof(line.substr(0,pos));
line.erase(0,pos+1);
Z2 = stof(line);
}
    else { //A default file just in case
      cout << "BAD FILE, USING BACKUP\n";
      numshape=1;
      S1.addvert(0, 0, 0);
      S1.addvert(1, 0, 0);
      S1.addvert(0, 1, 0);
      S1.addvert(0, 0, 1);
      verts[5][0][0]=6;
      verts[0][0][0]=0;
      verts[0][0][1]=1;
      verts[0][1][0]=0;
      verts[0][1][1]=2;
      verts[0][2][0]=0;
      verts[0][2][1]=3;
      verts[0][3][0]=1;
      verts[0][3][1]=2;
      verts[0][4][0]=1;
      verts[0][4][1]=3;
      verts[0][5][0]=2;
      verts[0][5][1]=3;
      X1=Y1=Z1=.25;
      X2=Y2=Z2=.75;
    }
}

void save(){ // Save the data to 3dcoordinates.txt , runs before quitting
  int n;
  float x,y,z;
  ofstream myfile (filestring);
  if (myfile.is_open()){
    myfile<<numshape<<"\n";
    for (int i=0; i<numshape; i++){
      if (i==0) {
	S=&S1;
      }
      else if (i==1) {
	S=&S2;
      }
      else if (i==2) {
	S=&S3;
      }
      else if (i==3) {
	S=&S4;
      }
      else if (i==4) {
	S=&S5;
      }
      n = S->numvert;
      myfile << n << "\n";
      for (int j=0; j<n;j++){
	S->findvert(&x,&y,&z,j);
	myfile<<x<<" "<<y<<" "<<z<<"\n";
      }
      n= verts[5][i][0];
      myfile << n << "\n";
      for (int j=0; j<n; j++){
	myfile << (verts[i][j][0]+1) << " " << (verts[i][j][1]+1) << "\n";
      }
    }
    myfile << X1 << " " << Y1 << " " << Z1 << "\n";
    myfile << X2 << " " << Y2 << " " << Z2 << "\n";
    myfile.close();
  }
}

void menu(int num){
  if(num == 0){
    save();
    glutDestroyWindow(MainWindow);
    exit(0);
  }else{
    if (num<40){
      oldvalue=menvalue = num;
    }
    else menvalue=num;
  }
  glutPostRedisplay();
}

void createmenu(){ //All possible menu options
  Cor_id = glutCreateMenu(menu);
  glutAddMenuEntry("Shape 1", 41);
  glutAddMenuEntry("Shape 2", 42);
  glutAddMenuEntry("Shape 3", 43);
  glutAddMenuEntry("Shape 4", 44);
  glutAddMenuEntry("Shape 5", 45);
  glutAddMenuEntry("Rotation Vector", 46);
  Vec_id = glutCreateMenu(menu);
  glutAddMenuEntry("X1 ; Y1 ; Z1", 1);
  glutAddMenuEntry("X2 ; Y2 ; Z2", 2);
  Scal_id = glutCreateMenu(menu);
  glutAddMenuEntry("Shape 1", 11);
  glutAddMenuEntry("Shape 2", 12);
  glutAddMenuEntry("Shape 3", 13);
  glutAddMenuEntry("Shape 4", 14);
  glutAddMenuEntry("Shape 5", 15);
  Tran_id = glutCreateMenu(menu);
  glutAddMenuEntry("Shape 1", 21);
  glutAddMenuEntry("Shape 2", 22);
  glutAddMenuEntry("Shape 3", 23);
  glutAddMenuEntry("Shape 4", 24);
  glutAddMenuEntry("Shape 5", 25);
  Rot_id = glutCreateMenu(menu);
  glutAddMenuEntry("Shape 1", 31);
  glutAddMenuEntry("Shape 2", 32);
  glutAddMenuEntry("Shape 3", 33);
  glutAddMenuEntry("Shape 4", 34);
  glutAddMenuEntry("Shape 5", 35);
  Menu_id = glutCreateMenu(menu);
  glutAddSubMenu("Rotate", Rot_id);
  glutAddSubMenu("Translate", Tran_id);
  glutAddSubMenu("Scale", Scal_id);
  glutAddSubMenu("Rotation Vector", Vec_id);
  glutAddSubMenu("Print Vertex", Cor_id);
  glutAddMenuEntry("New File",50);
  glutAddMenuEntry("Exit", 0);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void display1(){ //This is the subwindow for XY
  glutSetWindow(xyWindow); //Display2 and Display3 are essentially the same (except for XZ, YZ)
  glClear(GL_COLOR_BUFFER_BIT);
  int i,j,a,b;
  float x1, x2, y1, y2, z;
  glColor3f(1.0,1.0,1.0);
  glRasterPos2f(0.85,.9);
  glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_10, (const unsigned char *)"XY");
  glBegin(GL_LINES); //Draw rotation vector
  glColor3f(0.0, 0.0, 1.0);
  glVertex2f((X1-MasterMin)*scalfac-.99, (Y1-MasterMin)*scalfac-.99);
  glColor3f(1.0, 0.0, 0.0);
  glVertex2f((X2-MasterMin)*scalfac-.99, (Y2-MasterMin)*scalfac-.99);
  glEnd();
  glColor3f(.5, 0.0, 0.0);
  glBegin(GL_LINE_LOOP); //Draw a square
  glVertex2f(-.99,-.99);
  glVertex2f(1,-.99);
  glVertex2f(1,1);
  glVertex2f(-.99,1);
  glEnd();
  for (i=0;i<numshape;i++){
    glColor3f(1.0, 0.2*float(i), 0.0);
    if (i==0) {
      S=&S1;
    }
    else if (i==1) {
      S=&S2;
    }
    else if (i==2) {
      S=&S3;
    }
    else if (i==3) {
      S=&S4;
    }
    else if (i==4) {
      S=&S5;
    }
    for (j=0;j<verts[5][i][0];j++){
      a=verts[i][j][0];
      b=verts[i][j][1];
      S->findvert(&x1, &y1, &z, a);
      S->findvert(&x2, &y2, &z, b);
      glBegin(GL_LINES); //Draw all the lines
      glVertex2f((x1-MasterMin)*scalfac-.99, (y1-MasterMin)*scalfac-.99);
      glVertex2f((x2-MasterMin)*scalfac-.99, (y2-MasterMin)*scalfac-.99);
      glEnd();
    }
  }
  glutSwapBuffers();
}

void display2(){
  glutSetWindow(xzWindow);
  glClear(GL_COLOR_BUFFER_BIT);
  int i,j,a,b;
  float x1, x2, y, z1, z2;
  glColor3f(1.0,1.0,1.0);
  glRasterPos2f(0.85,-.9);
  glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_10, (const unsigned char *)"XZ");
  glBegin(GL_LINES);
  glColor3f(0.0, 0.0, 1.0);
  glVertex2f((X1-MasterMin)*scalfac-.99, (Z1-MasterMin)*scalfac-.99);
  glColor3f(1.0, 0.0, 0.0);
  glVertex2f((X2-MasterMin)*scalfac-.99, (Z2-MasterMin)*scalfac-.99);
  glEnd();
  glColor3f(0.0, 0.5, 0.0);
  glBegin(GL_LINE_LOOP);
  glVertex2f(-.99,-.99);
  glVertex2f(1,-.99);
  glVertex2f(1,1);
  glVertex2f(-.99,1);
  glEnd();
  for (i=0;i<numshape;i++){
    glColor3f(0.0, 1.0, 0.2*float (i));
    if (i==0) {
      S=&S1;
    }
    else if (i==1) {
      S=&S2;
    }
    else if (i==2) {
      S=&S3;
    }
    else if (i==3) {
      S=&S4;
    }
    else if (i==4) {
      S=&S5;
    }
    for (j=0;j<verts[5][i][0];j++){
      a=verts[i][j][0];
      b=verts[i][j][1];
      S->findvert(&x1, &y, &z1, a);
      S->findvert(&x2, &y, &z2, b);
      glBegin(GL_LINES);
      glVertex2f((x1-MasterMin)*scalfac-.99, (z1-MasterMin)*scalfac-.99);
      glVertex2f((x2-MasterMin)*scalfac-.99, (z2-MasterMin)*scalfac-.99);
      glEnd();
    }
  }
  glutSwapBuffers();
}

void display3(){
  glutSetWindow(yzWindow);
  glClear(GL_COLOR_BUFFER_BIT);
  int i,j,a,b;
  float x, y1, y2, z1, z2;
  glColor3f(1.0,1.0,1.0);
  glRasterPos2f(-.9,-.9);
  glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_10, (const unsigned char *)"YZ");
  glBegin(GL_LINES);
  glColor3f(0.0, 0.0, 1.0);
  glVertex2f((Y1-MasterMin)*scalfac-.99, (Z1-MasterMin)*scalfac-.99);
  glColor3f(1.0, 0.0, 0.0);
  glVertex2f((Y2-MasterMin)*scalfac-.99, (Z2-MasterMin)*scalfac-.99);
  glEnd();
  glColor3f(0.0, 0.0, 0.5);
  glBegin(GL_LINE_LOOP);
  glVertex2f(-.99,-.99);
  glVertex2f(1,-.99);
  glVertex2f(1,1);
  glVertex2f(-.99,1);
  glEnd();
  for (i=0;i<numshape;i++){
    glColor3f(0.2*float (i), 0.0, 1.0);
    if (i==0) {
      S=&S1;
    }
    else if (i==1) {
      S=&S2;
    }
    else if (i==2) {
      S=&S3;
    }
    else if (i==3) {
      S=&S4;
    }
    else if (i==4) {
      S=&S5;
    }
    for (j=0;j<verts[5][i][0];j++){
      a=verts[i][j][0];
      b=verts[i][j][1];
      S->findvert(&x, &y1, &z1, a);
      S->findvert(&x, &y2, &z2, b);
      glBegin(GL_LINES);
      glVertex2f((y1-MasterMin)*scalfac-.99, (z1-MasterMin)*scalfac-.99);
      glVertex2f((y2-MasterMin)*scalfac-.99, (z2-MasterMin)*scalfac-.99);
      glEnd();
    }
  }
  glutSwapBuffers();
}

void SetVertex(){ // Command to change the rotation vector
  if (menvalue==1){
    X1+=DeltaX/((MasterMax-MasterMin)*200);
    Y1+=DeltaY/((MasterMax-MasterMin)*200);
    Z1+=DeltaZ/((MasterMax-MasterMin)*200);
  }
  else if (menvalue==2){
    X2+=DeltaX/((MasterMax-MasterMin)*200);
    Y2+=DeltaY/((MasterMax-MasterMin)*200);
    Z2+=DeltaZ/((MasterMax-MasterMin)*200);
  }
}

void SetScale(){ //Function to scale each shape
  if (menvalue==11){
    if (DeltaZ!=0)
      S1.scale(1+DeltaZ*sign/200);
    else if (DeltaX!=0)
      S1.scale(1-DeltaX*sign/200);
  }
  else if (menvalue==12){
    if (DeltaZ!=0)
      S2.scale(1+DeltaZ*sign/200);
    else if (DeltaX!=0)
      S2.scale(1-DeltaX*sign/200);
  }
  else if (menvalue==13){
    if (DeltaZ!=0)
      S3.scale(1+DeltaZ*sign/200);
    else if (DeltaX!=0)
      S3.scale(1-DeltaX*sign/200);
  }
  else if (menvalue==14){
    if (DeltaZ!=0)
      S4.scale(1+DeltaZ*sign/200);
    else if (DeltaX!=0)
      S4.scale(1-DeltaX*sign/200);
  }
  else if (menvalue==15){
    if (DeltaZ!=0)
      S5.scale(1+DeltaZ*sign/200);
    else if (DeltaX!=0)
      S5.scale(1-DeltaX*sign/200);
  }
}

void SetTran(){ //Translate each shape
  if (menvalue==21){
    S1.translate(DeltaX/((MasterMax-MasterMin)*200), DeltaY/((MasterMax-MasterMin)*200), DeltaZ/((MasterMax-MasterMin)*200));
  }
  else if (menvalue==22){
    S2.translate(DeltaX/((MasterMax-MasterMin)*200), DeltaY/((MasterMax-MasterMin)*200), DeltaZ/((MasterMax-MasterMin)*200));
  }
  else if (menvalue==23){
    S3.translate(DeltaX/((MasterMax-MasterMin)*200), DeltaY/((MasterMax-MasterMin)*200), DeltaZ/((MasterMax-MasterMin)*200));
  }
  else if (menvalue==24){
    S4.translate(DeltaX/((MasterMax-MasterMin)*200), DeltaY/((MasterMax-MasterMin)*200), DeltaZ/((MasterMax-MasterMin)*200));
  }
  else if (menvalue==25){
    S5.translate(DeltaX/((MasterMax-MasterMin)*200), DeltaY/((MasterMax-MasterMin)*200), DeltaZ/((MasterMax-MasterMin)*200));
  }
}

void SetRot(){ //Or rotate each shape
  if (menvalue==31){
    if (DeltaZ!=0)
      S1.rotate(X1, X2, Y1, Y2, Z1, Z2, DeltaZ*sign/200);
    else if (DeltaX!=0)
      S1.rotate(X1, X2, Y1, Y2, Z1, Z2,-DeltaX*sign/200);
  }
  else if (menvalue==32){
    if (DeltaZ!=0)
      S2.rotate(X1, X2, Y1, Y2, Z1, Z2, DeltaZ*sign/200);
    else if (DeltaX!=0)
      S2.rotate(X1, X2, Y1, Y2, Z1, Z2, -DeltaX*sign/200);
  }
  else if (menvalue==33){
    if (DeltaZ!=0)
      S3.rotate(X1, X2, Y1, Y2, Z1, Z2, DeltaZ*sign/200);
    else if (DeltaX!=0)
      S3.rotate(X1, X2, Y1, Y2, Z1, Z2, -DeltaX*sign/200);
  }
  else if (menvalue==34){
    if (DeltaZ!=0)
      S4.rotate(X1, X2, Y1, Y2, Z1, Z2, DeltaZ*sign/200);
    else if (DeltaX!=0)
      S4.rotate(X1, X2, Y1, Y2, Z1, Z2, -DeltaX*sign/200);
  }
  else if (menvalue==35){
    if (DeltaZ!=0)
      S5.rotate(X1, X2, Y1, Y2, Z1, Z2, DeltaZ*sign/200);
    else if (DeltaX!=0)
      S5.rotate(X1, X2, Y1, Y2, Z1, Z2, -DeltaX*sign/200);
  }
  float x, y, z;
  S1.findvert(&x, &y, &z, 2);
}

void ShowCor(){ //Print coordinates of selected shape
  if (menvalue == 46){
    cout<< "Vector Coordinate 1 : X: "<< X1 << " Y: " << Y1 << " Z: " << Z1 << "\n";
    cout<< "Vector Coordinate 2 : X: "<< X2 << " Y: " << Y2 << " Z: " << Z2 << "\n";
  }
  else{
    int num = menvalue%10-1;
    if (num==0) {
      S=&S1;
    }
    else if (num==1) {
      S=&S2;
    }
    else if (num==2) {
      S=&S3;
    }
    else if (num==3) {
      S=&S4;
    }
    else if (num==4) {
      S=&S5;
    }
    float x,y,z;
    cout << "Shape " << num+1 << ":\n";
    for (int i=0; i<S->numvert;i++){
      S->findvert(&x, &y, &z, i);
      cout << "Vertex " << i+1 << ":    X: " << x << " Y: " << y << " Z: " << z << "\n";
    }
  }
  menvalue=oldvalue;
}

void drawall(){ //Main Draw function, called every update
  if (menvalue<10){
    SetVertex();
  }
  else if (menvalue<20){ //Choose what function to run
    SetScale();
  }
  else if (menvalue<30){
    SetTran();
  }
  else if (menvalue<40){
    SetRot();
  }
  else if (menvalue<50){
    ShowCor();
  }
  else if (menvalue==50){
    cout << "Enter Name of New Text File:";
    cin >> filestring;
    load();
    menvalue=oldvalue;
  }
  MasterMax=X1;
  MasterMin=X1;
  MasterMax = (X2>MasterMax) ? X2:MasterMax;
  MasterMax = (Y1>MasterMax) ? Y1:MasterMax;
  MasterMax = (Y2>MasterMax) ? Y2:MasterMax;
  MasterMax = (Z1>MasterMax) ? Z1:MasterMax;
  MasterMax = (Z2>MasterMax) ? Z2:MasterMax;
  MasterMin = (X2<MasterMin) ? X2:MasterMin;
  MasterMin = (Y1<MasterMin) ? Y1:MasterMin;
  MasterMin = (Y2<MasterMin) ? Y2:MasterMin;
  MasterMin = (Z1<MasterMin) ? Z1:MasterMin;
  MasterMin = (Z2<MasterMin) ? Z2:MasterMin; //Calculate Master Values
  for (int i=0; i<numshape;i++){
    if (i==0) {
      S=&S1;
    }
    else if (i==1) {
      S=&S2;
    }
    else if (i==2) {
      S=&S3;
    }
    else if (i==3) {
      S=&S4;
    }
    else if (i==4) {
      S=&S5;
    }
    MasterMax = (S->findmax()>MasterMax)?S->findmax():MasterMax;
    MasterMin = (S->findmin()<MasterMin)?S->findmin():MasterMin;
  }
  scalfac =1.98/(MasterMax-MasterMin);
  display1();
  display2();
  display3(); //Display each shape
  draw();
  if((DeltaZ!=0) || (DeltaY!=0) || (DeltaX!=0)){
    glutSetWindow(MainWindow);
    glutPostRedisplay();
  }
}

void pressKey(int key, int xx, int yy) {
  switch (key) {
  case GLUT_KEY_UP : DeltaZ = 10*sign; break;
  case GLUT_KEY_DOWN : DeltaX = 10*sign; break;
  case GLUT_KEY_RIGHT : DeltaY = 10*sign; break; //Key inputs for animated movements
  }
  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

void releaseKey(int key, int xx, int yy) {

  switch (key) {
  case GLUT_KEY_UP : DeltaZ = 0;break;
  case GLUT_KEY_DOWN : DeltaX = 0;break;
  case GLUT_KEY_RIGHT : DeltaY = 0; break; //Cancels the animated movements
  case GLUT_KEY_LEFT : sign*=-1; break;
  }
  glutSetWindow(MainWindow);
  glutPostRedisplay();
}

int main(int argc, char **argv)
{
  load();
  glutInit(&argc,argv);                                   //initializes the GLUT framework
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(410,410);
  glutInitWindowPosition(100,100);
  MainWindow = glutCreateWindow("Jacob Ley Proj2");
  glutSpecialFunc(pressKey);
  glutSpecialUpFunc(releaseKey);
  createmenu();
  glutDisplayFunc(drawall);
  //  glutIdleFunc(drawall);
  xyWindow = glutCreateSubWindow(MainWindow, 0, 205, 205, 205);
  glutSpecialFunc(pressKey);
  glutSpecialUpFunc(releaseKey);
  glutDisplayFunc(display1);

  xzWindow = glutCreateSubWindow(MainWindow, 0, 0, 205, 205);
  glutSpecialFunc(pressKey);
  glutSpecialUpFunc(releaseKey);
  glutDisplayFunc(display2);

  yzWindow = glutCreateSubWindow(MainWindow, 205, 0, 205, 205);
  glutSpecialFunc(pressKey);
  glutSpecialUpFunc(releaseKey);
  glutDisplayFunc(display3);
  glutMainLoop();           //the main loop of the GLUT framework
}
 void draw()                           //main window isnt actually used
{
  glutSetWindow(MainWindow);
  glClear(GL_COLOR_BUFFER_BIT);//clears the colour and depth buffers
  glColor3f(1.0, 1.0, 1.0);
  glRasterPos2f(0.1,-.45);
  if (sign==-1)
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_10, (const unsigned char *)"LEFT (Negate)  ~X~");
  else glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_10, (const unsigned char *)"LEFT (Negate)");
  glRasterPos2f(0.4,-.1);
  if (DeltaZ!=0)
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_10, (const unsigned char *)"UP (move Z) ~X~");
  else glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_10, (const unsigned char *)"UP (move Z)");
  glRasterPos2f(0.5,-.55);
  if (DeltaY!=0)
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_10, (const unsigned char *)"RIGHT (move Y)  ~X~");
  else glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_10, (const unsigned char *)"RIGHT (move Y)");
  glRasterPos2f(0.4,-.9);
  if (DeltaX!=0)
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_10, (const unsigned char *)"DOWN (move X)  ~X~");
  else glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_10, (const unsigned char *)"DOWN (move X)");
  glutSwapBuffers();

}                     //The end of program!
