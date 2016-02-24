//
//  main.cpp
//  ECS175_Proj3
//
//  Created by Jacob Ley on 11/1/15.
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
#include <fstream>
#include <cstring>
#include "Coord.h"
#include "Sort.h"
#include <cmath>
using namespace std;
int MainWindow, xyWindow, xzWindow, yzWindow;
Coord* Coordinates[5][20]; // Shape, coordinate (x,y,z)+(r,g,b)
Tri* Lines[5][20]; // i = Shape, j = Triangles in shape
int numTri[6][2]; // i = Shape, j=0 = # of coordinates, j=1 = # of triangles
string filestring = "3dcoordinates.txt";
float PixelBuffer1[300*300*3], PixelBuffer2[300*300*3], PixelBuffer3[300*300*3];
int numshape, lineloc[300][2];
float MasterMin, MasterMax, scalfac, DeltaX=0, DeltaY=0, DeltaZ=0;
TriHeap PaintOrder;
bool Halftone=false;
Coord Light;
float IA[3], Viewer[3];
int menvalue=24, Menu_id, Phong_id, Light_id, View_id, Move_id, id_1, id_2, id_3, id_4, id_5, id_12, id_22, id_32, id_42, id_52, sign=1;

void load(){
  string line;
  Coord *C;
  Tri *L;
  int n,num;
  ifstream myfile(filestring);
  if (myfile.is_open()){
    getline(myfile,line);
    numshape=stoi(line);
    for (int i=0;i<numshape;i++){
      getline(myfile,line);
      n=stoi(line);
      numTri[i][0]=n;
      for (int j=0; j<n;j++){
	C = new Coord();
	getline(myfile,line);
	size_t pos = line.find(" ");
	C->x = stof(line.substr(0,pos));
	line.erase(0 , pos+1);
	pos = line.find(" ");
	C->y = stof(line.substr(0,pos));
	line.erase(0 , pos+1);
	pos = line.find(" ");
	C->z = stof(line.substr(0,pos));
	line.erase(0 , pos+1);
	pos = line.find(" ");
	C->r = stof(line.substr(0,pos));
	line.erase(0 , pos+1);
	pos = line.find(" ");
	C->g = stof(line.substr(0,pos));
	line.erase(0 , pos+1);
	C->b = stof(line);
	Coordinates[i][j]=C;
      }
      getline(myfile,line);
      n=stoi(line);
      numTri[i][1]=n;
      for (int j=0;j<n;j++){
	L = new Tri();
	getline(myfile,line);
	size_t pos = line.find(" ");
	num = stoi(line.substr(0,pos));
	L->C1 = Coordinates[i][num-1];
	L->num1=num;
	line.erase(0 , pos+1);
	pos = line.find(" ");
	num = stoi(line.substr(0,pos));
	L->C2 = Coordinates[i][num-1];
	L->num2=num;
	line.erase(0 , pos+1);
	num=stoi(line);
	L->C3 = Coordinates[i][num-1];
	L->num3=num;
	Lines[i][j]=L;
      }
    }
    getline(myfile,line);
    size_t pos = line.find(" ");
    Light.x = stof(line.substr(0,pos));
    line.erase(0 , pos+1);
    pos = line.find(" ");
    Light.y = stof(line.substr(0,pos));
    line.erase(0 , pos+1);
    pos = line.find(" ");
    Light.z = stof(line.substr(0,pos));
    line.erase(0 , pos+1);
    pos = line.find(" ");
    Light.r = stof(line.substr(0,pos));
    line.erase(0 , pos+1);
    pos = line.find(" ");
    Light.g = stof(line.substr(0,pos));
    line.erase(0 , pos+1);
    Light.b =stof(line);
    getline(myfile,line);
    pos = line.find(" ");
    Viewer[0] = stof(line.substr(0,pos));
    line.erase(0 , pos+1);
    pos = line.find(" ");
    Viewer[1] = stof(line.substr(0,pos));
    line.erase(0 , pos+1);
    Viewer[2] =stof(line);
    getline(myfile,line);
    pos = line.find(" ");
    IA[0] = stof(line.substr(0,pos));
    line.erase(0 , pos+1);
    pos = line.find(" ");
    IA[1] = stof(line.substr(0,pos));
    line.erase(0 , pos+1);
    IA[2] =stof(line);
    myfile.close();
  }
}

void save(){
  // Save the data to 3dcoordinates.txt , runs before quitting
  int n;
  Coord *C;
  Tri *L;
  ofstream myfile (filestring);
  if (myfile.is_open()){
    myfile<<numshape<<"\n";
    for (int i=0; i<numshape; i++){
      n = numTri[i][0];
      myfile << n << "\n";
      for (int j=0; j<n;j++){
	C = Coordinates[i][j];
	myfile << C->x << " "<< C->y << " "<< C->z << " "<< C->r << " "<< C->g << " "<< C->b << "\n";
      }
      n= numTri[i][1];
      myfile << n << "\n";
      for (int j=0; j<n; j++){
	L = Lines[i][j];
	myfile << (L->num1) << " " << (L->num2) << " " << (L->num3) << "\n";
      }
    }
    myfile << Light.x << " "<< Light.y << " "<< Light.z << " "<< Light.r << " "<< Light.b << " "<< Light.b << "\n";
    myfile << Viewer[0] << " " << Viewer[1] << " " << Viewer[2] << "\n";
    myfile << IA[0] << " " << IA[1] << " " << IA[2] << "\n";
    myfile.close();
  }
}

float findmax(float x,float y,float z, float extra){
  float temp;
  if (x>y)
    temp = x;
  else temp= y;
  if (z>temp)
    temp = z;
  if (extra>temp)
    temp = extra;
  return temp;
}

float findmin(float x, float y, float z, float extra){
  float temp;
  if (x<y)
    temp = x;
  else temp= y;
  if (z<temp)
    temp = z;
  if (extra<temp)
    temp=extra;
  return temp;
}

float Dist(float *vec){
  return pow(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2], .5);
}

void Normalize(float *normvec){
  float length = Dist(normvec);
  if (length!=0){
    normvec[0] = normvec[0]/length;
    normvec[1] = normvec[1]/length;
    normvec[2] = normvec[2]/length;
  }
}

void menu(int num){
  if(num == 0){
    save();
    glutDestroyWindow(MainWindow);
    exit(0);
  }else{
    menvalue=num;
  }
  glutPostRedisplay();
}

void createmenu(){
  id_12 = glutCreateMenu(menu);
  glutAddMenuEntry("Rotate", 80);
  glutAddMenuEntry("Translate", 90);
  id_22 = glutCreateMenu(menu);
  glutAddMenuEntry("Rotate", 81);
  glutAddMenuEntry("Translate", 91);
  id_32 = glutCreateMenu(menu);
  glutAddMenuEntry("Rotate", 82);
  glutAddMenuEntry("Translate", 92);
  id_42 = glutCreateMenu(menu);
  glutAddMenuEntry("Rotate", 83);
  glutAddMenuEntry("Translate", 93);
  id_52 = glutCreateMenu(menu);
  glutAddMenuEntry("Rotate", 84);
  glutAddMenuEntry("Translate", 94);
  Move_id = glutCreateMenu(menu);
  glutAddSubMenu("Shape 1", id_12);
  if (numshape>1)
    glutAddSubMenu("Shape 2", id_22);
  if (numshape>2)
    glutAddSubMenu("Shape 3", id_32);
  if (numshape>3)
    glutAddSubMenu("Shape 4", id_42);
  if (numshape==5)
    glutAddSubMenu("Shape 5", id_52);
  View_id = glutCreateMenu(menu);
  glutAddMenuEntry("New Viewer Coordinates (input)", 11);
  glutAddMenuEntry("Move Viewer (Arrow Keys)", 12);
  Light_id = glutCreateMenu(menu);
  glutAddMenuEntry("New Light Coordinates (input)", 21);
  glutAddMenuEntry("Move Light (Arrow Keys)", 22);
  glutAddMenuEntry("New Light Values (input)", 23);
  glutAddMenuEntry("Adjust Light Values (Arrow Keys)", 24);
  glutAddMenuEntry("New Ambient Light Values (input)", 25);
  glutAddMenuEntry("Adjust Ambient Light Values (Arrow Keys)", 26);
  id_1 = glutCreateMenu(menu);
  glutAddMenuEntry("Ka",31);
  glutAddMenuEntry("Kd",32);
  glutAddMenuEntry("Ks",33);
  glutAddMenuEntry("n (reflection)",34);
  id_2 = glutCreateMenu(menu);
  glutAddMenuEntry("Ka",41);
  glutAddMenuEntry("Kd",42);
  glutAddMenuEntry("Ks",43);
  glutAddMenuEntry("n (reflection)",44);
  id_3 = glutCreateMenu(menu);
  glutAddMenuEntry("Ka",51);
  glutAddMenuEntry("Kd",52);
  glutAddMenuEntry("Ks",53);
  glutAddMenuEntry("n (reflection)",54);
  id_4 = glutCreateMenu(menu);
  glutAddMenuEntry("Ka",61);
  glutAddMenuEntry("Kd",62);
  glutAddMenuEntry("Ks",63);
  glutAddMenuEntry("n (reflection)",64);
  id_5 = glutCreateMenu(menu);
  glutAddMenuEntry("Ka",71);
  glutAddMenuEntry("Kd",72);
  glutAddMenuEntry("Ks",73);
  glutAddMenuEntry("n (reflection)",74);
  Phong_id = glutCreateMenu(menu);
  glutAddSubMenu("Shape 1", id_1);
  if (numshape>1)
    glutAddSubMenu("Shape 2", id_2);
  if (numshape>2)
    glutAddSubMenu("Shape 3", id_3);
  if (numshape>3)
    glutAddSubMenu("Shape 4", id_4);
  if (numshape==5)
    glutAddSubMenu("Shape 5", id_5);
  Menu_id = glutCreateMenu(menu);
  glutAddSubMenu("Move Shape", Move_id);
  glutAddSubMenu("Phong", Phong_id);
  glutAddSubMenu("Light", Light_id);
  glutAddSubMenu("Viewer", View_id);
  glutAddMenuEntry("Coordinate Color", 1);
  glutAddMenuEntry("Halftone", 2);
  glutAddMenuEntry("Exit", 0);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

float Dot(float *vec1, float *vec2){
  return (vec1[0]*vec2[0] + vec1[1]*vec2[1] + vec1[2]*vec2[2]);
}

void Phong(float *vertcol, Tri *L){
  float lightvec[3]={Light.x - L->C1->x, Light.y - L->C1->y, Light.z - L->C1->z};
  float viewvec[3] ={Viewer[0] - L->C1->x, Viewer[1] - L->C1->y, Viewer[2] - L->C1->z};
  float length = (Dist(viewvec)+Dist(lightvec));
  Normalize(lightvec);
  Normalize(viewvec);
  float Reflectvec[3]={2*(Dot(L->normvec, lightvec)*L->normvec[0])-lightvec[0], 2*(Dot(L->normvec, lightvec)*L->normvec[1])-lightvec[1], 2*(Dot(L->normvec, lightvec)*L->normvec[2])-lightvec[2]};
  Normalize(Reflectvec);
  vertcol[8]=((L->KD*Dot(lightvec,L->normvec)) + L->KS*pow(abs(Dot(Reflectvec,viewvec)),L->n))/length;
  vertcol[0]=L->C1->r * (Light.r * vertcol[8] + IA[0] * L->KA);
  vertcol[1]=L->C1->g * (Light.g * vertcol[8] + IA[1] * L->KA);
  vertcol[2]=L->C1->b * (Light.b * vertcol[8] + IA[2] * L->KA);
  lightvec[0]=Light.x - L->C2->x;
  lightvec[1]=Light.y - L->C2->y;
  lightvec[2]=Light.z - L->C2->z;
  viewvec[0]=Viewer[0] - L->C2->x;
  viewvec[1]=Viewer[1] - L->C2->y;
  viewvec[2]=Viewer[2] - L->C2->z;
  length = Dist(viewvec)+Dist(lightvec);
  Normalize(lightvec);
  Normalize(viewvec);
  Reflectvec[0] = 2*(Dot(L->normvec, lightvec)*L->normvec[0])-lightvec[0];
  Reflectvec[1] = 2*(Dot(L->normvec, lightvec)*L->normvec[1])-lightvec[1];
  Reflectvec[2] = 2*(Dot(L->normvec, lightvec)*L->normvec[2])-lightvec[2];
  Normalize(Reflectvec);
  vertcol[8]=((L->KD*Dot(lightvec,L->normvec)) + L->KS*pow(abs(Dot(Reflectvec,viewvec)),L->n))/length;
  vertcol[3]=L->C2->r * (Light.r * vertcol[8] + IA[0] * L->KA);
  vertcol[4]=L->C2->g * (Light.g * vertcol[8] + IA[1] * L->KA);
  vertcol[5]=L->C2->b * (Light.b * vertcol[8] + IA[2] * L->KA);
  lightvec[0]=Light.x - L->C3->x;
  lightvec[1]=Light.y - L->C3->y;
  lightvec[2]=Light.z - L->C3->z;
  viewvec[0]=Viewer[0] - L->C3->x;
  viewvec[1]=Viewer[1] - L->C3->y;
  viewvec[2]=Viewer[2] - L->C3->z;
  length = Dist(viewvec)+Dist(lightvec);
  Normalize(lightvec);
  Normalize(viewvec);
  Reflectvec[0] = 2*(Dot(L->normvec, lightvec)*L->normvec[0])-lightvec[0];
  Reflectvec[1] = 2*(Dot(L->normvec, lightvec)*L->normvec[1])-lightvec[1];
  Reflectvec[2] = 2*(Dot(L->normvec, lightvec)*L->normvec[2])-lightvec[2];
  Normalize(Reflectvec);
  vertcol[8]=((L->KD*Dot(lightvec,L->normvec)) + L->KS*pow(abs(Dot(Reflectvec,viewvec)),L->n))/length;
  vertcol[6]=L->C3->r * (Light.r * vertcol[8] + IA[0] * L->KA);
  vertcol[7]=L->C3->g * (Light.g * vertcol[8] + IA[1] * L->KA);
  vertcol[8]=L->C3->b * (Light.b * vertcol[8] + IA[2] * L->KA);
}

void BinaryDisplay(float *Buffer){
  float avg;
  for(int i=0; i<270000;i+=2700){
    for(int j=0; j<900; j+=9){
      avg=0;
      for(int a=i; a<i+2700; a+=900){
	for(int b=j; b<j+9; b+=1){
	  avg+=Buffer[a+b];
	}
      }
      avg=avg/27;
      if(avg<0.1){
	Buffer[i+j]=Buffer[i+j+1]=Buffer[i+j+2]=            0;
	Buffer[i+j+3]=Buffer[i+j+4]=Buffer[i+j+5]=          0;
	Buffer[i+j+6]=Buffer[i+j+7]=Buffer[i+j+8]=          0;
	Buffer[i+j+900]=Buffer[i+j+901]=Buffer[i+j+902]=    0;
	Buffer[i+j+903]=Buffer[i+j+904]=Buffer[i+j+905]=    0;
	Buffer[i+j+906]=Buffer[i+j+907]=Buffer[i+j+908]=    0;
	Buffer[i+j+1800]=Buffer[i+j+1801]=Buffer[i+j+1902]= 0;
	Buffer[i+j+1803]=Buffer[i+j+1804]=Buffer[i+j+1805]= 0;
	Buffer[i+j+1806]=Buffer[i+j+1807]=Buffer[i+j+1808]= 0;
      }
      else if(avg<0.2){
	Buffer[i+j]=Buffer[i+j+1]=Buffer[i+j+2]=            0;
	Buffer[i+j+3]=Buffer[i+j+4]=Buffer[i+j+5]=          0;
	Buffer[i+j+6]=Buffer[i+j+7]=Buffer[i+j+8]=          0;
	Buffer[i+j+900]=Buffer[i+j+901]=Buffer[i+j+902]=    0;
	Buffer[i+j+903]=Buffer[i+j+904]=Buffer[i+j+905]=    1;
	Buffer[i+j+906]=Buffer[i+j+907]=Buffer[i+j+908]=    0;
	Buffer[i+j+1800]=Buffer[i+j+1801]=Buffer[i+j+1902]= 0;
	Buffer[i+j+1803]=Buffer[i+j+1804]=Buffer[i+j+1805]= 0;
	Buffer[i+j+1806]=Buffer[i+j+1807]=Buffer[i+j+1808]= 0;
      }
      else if(avg<0.3){
	Buffer[i+j]=Buffer[i+j+1]=Buffer[i+j+2]=            0;
	Buffer[i+j+3]=Buffer[i+j+4]=Buffer[i+j+5]=          0;
	Buffer[i+j+6]=Buffer[i+j+7]=Buffer[i+j+8]=          0;
	Buffer[i+j+900]=Buffer[i+j+901]=Buffer[i+j+902]=    1;
	Buffer[i+j+903]=Buffer[i+j+904]=Buffer[i+j+905]=    0;
	Buffer[i+j+906]=Buffer[i+j+907]=Buffer[i+j+908]=    1;
	Buffer[i+j+1800]=Buffer[i+j+1801]=Buffer[i+j+1902]= 0;
	Buffer[i+j+1803]=Buffer[i+j+1804]=Buffer[i+j+1805]= 0;
	Buffer[i+j+1806]=Buffer[i+j+1807]=Buffer[i+j+1808]= 0;
      }
      else if(avg<0.4){
	Buffer[i+j]=Buffer[i+j+1]=Buffer[i+j+2]=            0;
	Buffer[i+j+3]=Buffer[i+j+4]=Buffer[i+j+5]=          0;
	Buffer[i+j+6]=Buffer[i+j+7]=Buffer[i+j+8]=          1;
	Buffer[i+j+900]=Buffer[i+j+901]=Buffer[i+j+902]=    0;
	Buffer[i+j+903]=Buffer[i+j+904]=Buffer[i+j+905]=    1;
	Buffer[i+j+906]=Buffer[i+j+907]=Buffer[i+j+908]=    0;
	Buffer[i+j+1800]=Buffer[i+j+1801]=Buffer[i+j+1902]= 1;
	Buffer[i+j+1803]=Buffer[i+j+1804]=Buffer[i+j+1805]= 0;
	Buffer[i+j+1806]=Buffer[i+j+1807]=Buffer[i+j+1808]= 0;
      }
      else if(avg<0.5){
	Buffer[i+j]=Buffer[i+j+1]=Buffer[i+j+2]=            1;
	Buffer[i+j+3]=Buffer[i+j+4]=Buffer[i+j+5]=          0;
	Buffer[i+j+6]=Buffer[i+j+7]=Buffer[i+j+8]=          1;
	Buffer[i+j+900]=Buffer[i+j+901]=Buffer[i+j+902]=    0;
	Buffer[i+j+903]=Buffer[i+j+904]=Buffer[i+j+905]=    0;
	Buffer[i+j+906]=Buffer[i+j+907]=Buffer[i+j+908]=    0;
	Buffer[i+j+1800]=Buffer[i+j+1801]=Buffer[i+j+1902]= 1;
	Buffer[i+j+1803]=Buffer[i+j+1804]=Buffer[i+j+1805]= 0;
	Buffer[i+j+1806]=Buffer[i+j+1807]=Buffer[i+j+1808]= 1;
      }
      else if(avg<0.6){
	Buffer[i+j]=Buffer[i+j+1]=Buffer[i+j+2]=            1;
	Buffer[i+j+3]=Buffer[i+j+4]=Buffer[i+j+5]=          0;
	Buffer[i+j+6]=Buffer[i+j+7]=Buffer[i+j+8]=          1;
	Buffer[i+j+900]=Buffer[i+j+901]=Buffer[i+j+902]=    0;
	Buffer[i+j+903]=Buffer[i+j+904]=Buffer[i+j+905]=    1;
	Buffer[i+j+906]=Buffer[i+j+907]=Buffer[i+j+908]=    0;
	Buffer[i+j+1800]=Buffer[i+j+1801]=Buffer[i+j+1902]= 1;
	Buffer[i+j+1803]=Buffer[i+j+1804]=Buffer[i+j+1805]= 0;
	Buffer[i+j+1806]=Buffer[i+j+1807]=Buffer[i+j+1808]= 1;
      }
      else if(avg<0.7){
	Buffer[i+j]=Buffer[i+j+1]=Buffer[i+j+2]=            1;
	Buffer[i+j+3]=Buffer[i+j+4]=Buffer[i+j+5]=          1;
	Buffer[i+j+6]=Buffer[i+j+7]=Buffer[i+j+8]=          1;
	Buffer[i+j+900]=Buffer[i+j+901]=Buffer[i+j+902]=    0;
	Buffer[i+j+903]=Buffer[i+j+904]=Buffer[i+j+905]=    1;
	Buffer[i+j+906]=Buffer[i+j+907]=Buffer[i+j+908]=    0;
	Buffer[i+j+1800]=Buffer[i+j+1801]=Buffer[i+j+1902]= 1;
	Buffer[i+j+1803]=Buffer[i+j+1804]=Buffer[i+j+1805]= 0;
	Buffer[i+j+1806]=Buffer[i+j+1807]=Buffer[i+j+1808]= 1;
      }
      else if(avg<0.8){
	Buffer[i+j]=Buffer[i+j+1]=Buffer[i+j+2]=            1;
	Buffer[i+j+3]=Buffer[i+j+4]=Buffer[i+j+5]=          1;
	Buffer[i+j+6]=Buffer[i+j+7]=Buffer[i+j+8]=          1;
	Buffer[i+j+900]=Buffer[i+j+901]=Buffer[i+j+902]=    0;
	Buffer[i+j+903]=Buffer[i+j+904]=Buffer[i+j+905]=    1;
	Buffer[i+j+906]=Buffer[i+j+907]=Buffer[i+j+908]=    0;
	Buffer[i+j+1800]=Buffer[i+j+1801]=Buffer[i+j+1902]= 1;
	Buffer[i+j+1803]=Buffer[i+j+1804]=Buffer[i+j+1805]= 1;
	Buffer[i+j+1806]=Buffer[i+j+1807]=Buffer[i+j+1808]= 1;
      }
      else if(avg<0.9){
	Buffer[i+j]=Buffer[i+j+1]=Buffer[i+j+2]=            1;
	Buffer[i+j+3]=Buffer[i+j+4]=Buffer[i+j+5]=          1;
	Buffer[i+j+6]=Buffer[i+j+7]=Buffer[i+j+8]=          1;
	Buffer[i+j+900]=Buffer[i+j+901]=Buffer[i+j+902]=    1;
	Buffer[i+j+903]=Buffer[i+j+904]=Buffer[i+j+905]=    0;
	Buffer[i+j+906]=Buffer[i+j+907]=Buffer[i+j+908]=    1;
	Buffer[i+j+1800]=Buffer[i+j+1801]=Buffer[i+j+1902]= 1;
	Buffer[i+j+1803]=Buffer[i+j+1804]=Buffer[i+j+1805]= 1;
	Buffer[i+j+1806]=Buffer[i+j+1807]=Buffer[i+j+1808]= 1;
      }
      else {
	Buffer[i+j]=Buffer[i+j+1]=Buffer[i+j+2]=            1;
	Buffer[i+j+3]=Buffer[i+j+4]=Buffer[i+j+5]=          1;
	Buffer[i+j+6]=Buffer[i+j+7]=Buffer[i+j+8]=          1;
	Buffer[i+j+900]=Buffer[i+j+901]=Buffer[i+j+902]=    1;
	Buffer[i+j+903]=Buffer[i+j+904]=Buffer[i+j+905]=    1;
	Buffer[i+j+906]=Buffer[i+j+907]=Buffer[i+j+908]=    1;
	Buffer[i+j+1800]=Buffer[i+j+1801]=Buffer[i+j+1902]= 1;
	Buffer[i+j+1803]=Buffer[i+j+1804]=Buffer[i+j+1805]= 1;
	Buffer[i+j+1806]=Buffer[i+j+1807]=Buffer[i+j+1808]= 1;
      }
    }
  }
}

void drawline(float *Buffer, int x, int y, bool flat){  // not only for xy plotting, just easiest syntax
  if (flat){
    float r=Buffer[x], g=Buffer[x+1], b=Buffer[x+2];
    int dx=(y-x)/3;
    float dr=(Buffer[y]-r)/dx, dg=(Buffer[y+1]-g)/dx, db=(Buffer[y+2]-b)/dx;
    for (int i=0; i<dx; i++){
      x+=3;
      r+=dr;
      Buffer[x]=r;
      g+=dg;
      Buffer[x+1]=g;
      b+=db;
      Buffer[x+2]=b;
    }
  }
  else{
    int P;
    int dx, dy, x1=(x%900)/3, x2=(y%900)/3, y1=x/900, y2=y/900;
    if (abs(y2-y1)<abs(x2-x1)){
      if (x1>x2){
	dx=x1;
	x1=x2;
	x2=dx;
	dx=y1;
	y1=y2;
	y2=dx;
	dx=x;
	x=y;
	y=dx;
      }
      float r=Buffer[x], g=Buffer[x+1], b=Buffer[x+2];
      dx = x2-x1;
      float dr=(Buffer[y]-r)/dx, dg=(Buffer[y+1]-g)/dx, db=(Buffer[y+2]-b)/dx;
      if (y2>y1){
	dy = y2-y1;
	P = 2 * dy - dx;
	while(x<y){
	  Buffer[x]=r;
	  Buffer[x+1]=g;
	  Buffer[x+2]=b;
	  if(lineloc[y1][0]==-1)
	    lineloc[y1][0]=lineloc[y1][1]=x;
	  else if(lineloc[y1][0]>x)
	    lineloc[y1][0]=x;
	  else if(lineloc[y1][1]<x)
	    lineloc[y1][1]=x;
	  x+=3;
	  r+=dr;
	  g+=dg;
	  b+=db;
	  if (P>0){
	    x+=900;
	    y1++;
	    P = P + 2 * dy - 2 * dx;
	  }
	  else P = P + 2 * dy;
	}
      }
      else {
	dy = y1-y2;
	P = 2 * dy - dx;
	while((x%900)<(y%900)){
	  Buffer[x]=r;
	  Buffer[x+1]=g;
	  Buffer[x+2]=b;
	  if(lineloc[y1][0]==-1)
	    lineloc[y1][0]=lineloc[y1][1]=x;
	  else if(lineloc[y1][0]>x)
	    lineloc[y1][0]=x;
	  else if(lineloc[y1][1]<x)
	    lineloc[y1][1]=x;
	  x+=3;
	  r+=dr;
	  g+=dg;
	  b+=db;
	  if (P>0){
	    x-=900;
	    y1--;
	    P = P + 2 * dy - 2 * dx;
	  }
	  else P = P + 2 * dy;
	}
      }

    }
    else{
      if (y1>y2){
	dx=x1;
	x1=x2;
	x2=dx;
	dx=y1;
	y1=y2;
	y2=dx;
	dx=x;
	x=y;
	y=dx;
      }
      float r=Buffer[x], g=Buffer[x+1], b=Buffer[x+2];
      dy = y2-y1;
      float dr=(Buffer[y]-r)/dy, dg=(Buffer[y+1]-g)/dy, db=(Buffer[y+2]-b)/dy;
      if (x2>x1){
	dx = x2-x1;
	P = 2 * dx - dy;
	while(x<y){
	  Buffer[x]=r;
	  Buffer[x+1]=g;
	  Buffer[x+2]=b;
	  if(lineloc[y1][0]==-1)
	    lineloc[y1][0]=lineloc[y1][1]=x;
	  else if(lineloc[y1][0]>x)
	    lineloc[y1][0]=x;
	  else if(lineloc[y1][1]<x)
	    lineloc[y1][1]=x;
	  x+=900;
	  y1++;
	  r+=dr;
	  g+=dg;
	  b+=db;
	  if (P>=0){
	    x+=3;
	    P = P + 2 * dx - 2 * dy;
	  }
	  else P = P + 2 * dx;
	}
      }
      else {
	dx = x1-x2;
	P = 2 * dx - dy;
	while(x<y){
	  Buffer[x]=r;
	  Buffer[x+1]=g;
	  Buffer[x+2]=b;
	  if(lineloc[y1][0]==-1)
	    lineloc[y1][0]=lineloc[y1][1]=x;
	  else if(lineloc[y1][0]>x)
	    lineloc[y1][0]=x;
	  else if(lineloc[y1][1]<x)
	    lineloc[y1][1]=x;
	  x+=900;
	  y1++;
	  r+=dr;
	  g+=dg;
	  b+=db;
	  if (P>0){
	    x-=3;
	    P = P + 2 * dx - 2 * dy;
	  }
	  else P = P + 2 * dx;
	}
      }
    }
  }
}

void display1(){
  glutSetWindow(xyWindow); //Display2 and Display3 are essentially the same (except for XZ, YZ)
  glClear(GL_COLOR_BUFFER_BIT);
  for (int i=0; i<270000; i++){
    PixelBuffer1[i]=0;
  }
  int drawmin, drawmax, cor;
  PaintOrder.makeEmpty();
  Tri *L;
  float vertcol[9]; // vertcol[i][j] ==> The color of vertex i, j=0 ==> r, j=1 ==> g, j=2 ==> b
  for (int i=0; i<numshape; i++){
    for (int j=0; j<numTri[i][1]; j++){
      L = Lines[i][j];
      L->sort = 2;
      PaintOrder.insert(L);
    }
  }
  PaintOrder.sort();
  int y;
  while (!PaintOrder.isEmpty()){
    for(int i=0; i<300;i++)
      lineloc[i][0]=lineloc[i][1]=-1;
    L = PaintOrder.deleteMax();
    drawmin = (findmin(L->C1->y, L->C2->y, L->C3->y, L->C3->y)-MasterMin)*scalfac;
    drawmax = (findmax(L->C1->y, L->C2->y, L->C3->y, L->C3->y)-MasterMin)*scalfac;
    Phong(vertcol, L);
    cor=L->C1->corxy;
    PixelBuffer1[cor] = vertcol[0];
    PixelBuffer1[cor+1] = vertcol[1];
    PixelBuffer1[cor+2] = vertcol[2];
    y = cor/900;
    lineloc[y][0]=lineloc[y][1]=cor;
    cor=L->C2->corxy;
    PixelBuffer1[cor] = vertcol[3];
    PixelBuffer1[cor+1] = vertcol[4];
    PixelBuffer1[cor+2] = vertcol[5];
    y = cor/900;
    if(lineloc[y][0]==-1)
      lineloc[y][0]=lineloc[y][1]=cor;
    if(lineloc[y][0]>cor)
      lineloc[y][0]=cor;
    if(lineloc[y][1]<cor)
      lineloc[y][1]=cor;
    cor=L->C3->corxy;
    PixelBuffer1[cor] = vertcol[6];
    PixelBuffer1[cor+1] = vertcol[7];
    PixelBuffer1[cor+2] = vertcol[8];
    y = cor/900;
    if(lineloc[y][0]==-1)
      lineloc[y][0]=lineloc[y][1]=cor;
    if(lineloc[y][0]>cor)
      lineloc[y][0]=cor;
    if(lineloc[y][1]<cor)
      lineloc[y][1]=cor;
    drawline(PixelBuffer1, L->C1->corxy, L->C2->corxy, false);
    drawline(PixelBuffer1, L->C2->corxy, L->C3->corxy, false);
    drawline(PixelBuffer1, L->C1->corxy, L->C3->corxy, false);
    for (int i=drawmin; i<=drawmax; i++){
      drawline(PixelBuffer1, lineloc[i][0], lineloc[i][1], true);
    }
  }
  y = (int (((Light.y)-MasterMin)*scalfac)*300+ int(((Light.x)-MasterMin)*scalfac))*3;
  if ((y%900>0) && (y%900<897) && (y/900>0) && (y/900<299)){
    PixelBuffer1[y]=PixelBuffer1[y-903]=PixelBuffer1[y-897]=PixelBuffer1[y+897]=PixelBuffer1[y+903]=Light.r;
    PixelBuffer1[y+1]=PixelBuffer1[y-902]=PixelBuffer1[y-896]=PixelBuffer1[y+898]=PixelBuffer1[y+904]=Light.g;
    PixelBuffer1[y+2]=PixelBuffer1[y-901]=PixelBuffer1[y-895]=PixelBuffer1[y+899]=PixelBuffer1[y+905]=Light.b;
  }
  y = (int ((Viewer[1]-MasterMin)*scalfac)*300+ int((Viewer[0]-MasterMin)*scalfac))*3;
  if ((y%900>0) && (y%900<897) && (y/900>0) && (y/900<299)){
    PixelBuffer1[y-3]=PixelBuffer1[y-903]=PixelBuffer1[y-897]=PixelBuffer1[y+897]=PixelBuffer1[y+903]=1;
    PixelBuffer1[y+3]=PixelBuffer1[y+900]=PixelBuffer1[y-900]=1;
    PixelBuffer1[y-2]=PixelBuffer1[y-902]=PixelBuffer1[y-896]=PixelBuffer1[y+898]=PixelBuffer1[y+904]=1;
    PixelBuffer1[y+4]=PixelBuffer1[y+901]=PixelBuffer1[y-899]=1;
    PixelBuffer1[y+2]=PixelBuffer1[y-901]=PixelBuffer1[y-895]=PixelBuffer1[y+899]=PixelBuffer1[y+905]=1;
    PixelBuffer1[y+5]=PixelBuffer1[y+902]=PixelBuffer1[y-898]=1;
  }
  if (Halftone)
    BinaryDisplay(PixelBuffer1);
  for(int i=0; i<900; i+=3)
    PixelBuffer1[i]=1;
  for(int i=0; i<270000; i+=900)
    PixelBuffer1[i]=1;
  for(int i=897; i<270000; i+=900)
    PixelBuffer1[i]=1;
  for(int i=269100; i<270000; i+=3)
    PixelBuffer1[i]=1;
  glLoadIdentity();
  glDrawPixels(300, 300, GL_RGB, GL_FLOAT, PixelBuffer1);
  glEnd();
  glFlush();
}

void display2(){
  glutSetWindow(xzWindow);
  glClear(GL_COLOR_BUFFER_BIT);
  for (int i=0; i<270000; i++){
    PixelBuffer2[i]=0;
  }
  int drawmin, drawmax, cor;
  PaintOrder.makeEmpty();
  Tri *L;
  float vertcol[9]; // vertcol[i][j] ==> The color of vertex i, j=0 ==> r, j=1 ==> g, j=2 ==> b
  for (int i=0; i<numshape; i++){
    for (int j=0; j<numTri[i][1]; j++){
      L = Lines[i][j];
      L->sort = 1;
      PaintOrder.insert(L);
    }
  }
  PaintOrder.sort();
  int z;
  while (!PaintOrder.isEmpty()){
    for(int i=0; i<300;i++)
      lineloc[i][0]=lineloc[i][1]=-1;
    L = PaintOrder.deleteMax();
    drawmin = (findmin(L->C1->z, L->C2->z, L->C3->z, L->C3->z)-MasterMin)*scalfac;
    drawmax = (findmax(L->C1->z, L->C2->z, L->C3->z, L->C3->z)-MasterMin)*scalfac;
    Phong(vertcol, L);
    cor=L->C1->corxz;
    PixelBuffer2[cor] = vertcol[0];
    PixelBuffer2[cor+1] = vertcol[1];
    PixelBuffer2[cor+2] = vertcol[2];
    z = cor/900;
    lineloc[z][0]=lineloc[z][1]=cor;
    cor=L->C2->corxz;
    PixelBuffer2[cor] = vertcol[3];
    PixelBuffer2[cor+1] = vertcol[4];
    PixelBuffer2[cor+2] = vertcol[5];
    z = cor/900;
    if(lineloc[z][0]==-1)
      lineloc[z][0]=lineloc[z][1]=cor;
    if(lineloc[z][0]>cor)
      lineloc[z][0]=cor;
    if(lineloc[z][1]<cor)
      lineloc[z][1]=cor;
    cor=L->C3->corxz;
    PixelBuffer2[cor] = vertcol[6];
    PixelBuffer2[cor+1] = vertcol[7];
    PixelBuffer2[cor+2] = vertcol[8];
    z = cor/900;
    if(lineloc[z][0]==-1)
      lineloc[z][0]=lineloc[z][1]=cor;
    if(lineloc[z][0]>cor)
      lineloc[z][0]=cor;
    if(lineloc[z][1]<cor)
      lineloc[z][1]=cor;
    drawline(PixelBuffer2, L->C1->corxz, L->C2->corxz, false);
    drawline(PixelBuffer2, L->C2->corxz, L->C3->corxz, false);
    drawline(PixelBuffer2, L->C1->corxz, L->C3->corxz, false);
    for (int i=drawmin; i<=drawmax; i++){
      drawline(PixelBuffer2, lineloc[i][0], lineloc[i][1], true);
    }
  }
  z = (int (((Light.z)-MasterMin)*scalfac)*300+ int(((Light.x)-MasterMin)*scalfac))*3;
  if ((z%900>0) && (z%900<897) && (z/900>0) && (z/900<299)){
    PixelBuffer2[z]=PixelBuffer2[z-903]=PixelBuffer2[z-897]=PixelBuffer2[z+897]=PixelBuffer2[z+903]=Light.r;
    PixelBuffer2[z+1]=PixelBuffer2[z-902]=PixelBuffer2[z-896]=PixelBuffer2[z+898]=PixelBuffer2[z+904]=Light.g;
    PixelBuffer2[z+2]=PixelBuffer2[z-901]=PixelBuffer2[z-895]=PixelBuffer2[z+899]=PixelBuffer2[z+905]=Light.b;
  }
  z = (int ((Viewer[2]-MasterMin)*scalfac)*300+ int((Viewer[0]-MasterMin)*scalfac))*3;
  if ((z%900>0) && (z%900<897) && (z/900>0) && (z/900<299)){
    PixelBuffer2[z-3]=PixelBuffer2[z-903]=PixelBuffer2[z-897]=PixelBuffer2[z+897]=PixelBuffer2[z+903]=1;
    PixelBuffer2[z+3]=PixelBuffer2[z+900]=PixelBuffer2[z-900]=1;
    PixelBuffer2[z-2]=PixelBuffer2[z-902]=PixelBuffer2[z-896]=PixelBuffer2[z+898]=PixelBuffer2[z+904]=1;
    PixelBuffer2[z+4]=PixelBuffer2[z+901]=PixelBuffer2[z-899]=1;
    PixelBuffer2[z+2]=PixelBuffer2[z-901]=PixelBuffer2[z-895]=PixelBuffer2[z+899]=PixelBuffer2[z+905]=1;
    PixelBuffer2[z+5]=PixelBuffer2[z+902]=PixelBuffer2[z-898]=1;
  }
  if (Halftone)
    BinaryDisplay(PixelBuffer2);
  for(int i=1; i<900; i+=3)
    PixelBuffer2[i]=1;
  for(int i=1; i<270000; i+=900)
    PixelBuffer2[i]=1;
  for(int i=898; i<270000; i+=900)
    PixelBuffer2[i]=1;
  for(int i=269101; i<270000; i+=3)
    PixelBuffer2[i]=1;
  glLoadIdentity();
  glDrawPixels(300, 300, GL_RGB, GL_FLOAT, PixelBuffer2);
  glEnd();
  glFlush();
}

void display3(){
  glutSetWindow(yzWindow);
  glClear(GL_COLOR_BUFFER_BIT);
  for (int i=0; i<270000; i++){
    PixelBuffer3[i]=0;
  }
  int drawmin, drawmax, cor;
  PaintOrder.makeEmpty();
  Tri *L;
  float vertcol[9]; // vertcol[i][j] ==> The color of vertex i, j=0 ==> r, j=1 ==> g, j=2 ==> b
  for (int i=0; i<numshape; i++){
    for (int j=0; j<numTri[i][1]; j++){
      L = Lines[i][j];
      L->sort = 0;
      PaintOrder.insert(L);
    }
  }
  int z;
  PaintOrder.sort();
  while (!PaintOrder.isEmpty()){
    for(int i=0; i<300;i++)
      lineloc[i][0]=lineloc[i][1]=-1;
    L = PaintOrder.deleteMax();
    drawmin = (findmin(L->C1->z, L->C2->z, L->C3->z, L->C3->z)-MasterMin)*scalfac;
    drawmax = (findmax(L->C1->z, L->C2->z, L->C3->z, L->C3->z)-MasterMin)*scalfac;
    Phong(vertcol, L);
    cor=L->C1->coryz;
    PixelBuffer3[cor] = vertcol[0];
    PixelBuffer3[cor+1] = vertcol[1];
    PixelBuffer3[cor+2] = vertcol[2];
    z = cor/900;
    lineloc[z][0]=lineloc[z][1]=cor;
    cor=L->C2->coryz;
    PixelBuffer3[cor] = vertcol[3];
    PixelBuffer3[cor+1] = vertcol[4];
    PixelBuffer3[cor+2] = vertcol[5];
    z = cor/900;
    if(lineloc[z][0]==-1)
      lineloc[z][0]=lineloc[z][1]=cor;
    if(lineloc[z][0]>cor)
      lineloc[z][0]=cor;
    if(lineloc[z][1]<cor)
      lineloc[z][1]=cor;
    cor=L->C3->coryz;
    PixelBuffer3[cor] = vertcol[6];
    PixelBuffer3[cor+1] = vertcol[7];
    PixelBuffer3[cor+2] = vertcol[8];
    z = cor/900;
    if(lineloc[z][0]==-1)
      lineloc[z][0]=lineloc[z][1]=cor;
    if(lineloc[z][0]>cor)
      lineloc[z][0]=cor;
    if(lineloc[z][1]<cor)
      lineloc[z][1]=cor;
    drawline(PixelBuffer3, L->C1->coryz, L->C2->coryz, false);
    drawline(PixelBuffer3, L->C2->coryz, L->C3->coryz, false);
    drawline(PixelBuffer3, L->C1->coryz, L->C3->coryz, false);
    for (int i=drawmin; i<=drawmax; i++){
      drawline(PixelBuffer3, lineloc[i][0], lineloc[i][1], true);
    }
  }
  z = (int (((Light.z)-MasterMin)*scalfac)*300+ int(((Light.y)-MasterMin)*scalfac))*3;
  if ((z%900>0) && (z%900<897) && (z/900>0) && (z/900<299)){
    PixelBuffer3[z]=PixelBuffer3[z-903]=PixelBuffer3[z-897]=PixelBuffer3[z+897]=PixelBuffer3[z+903]=Light.r;
    PixelBuffer3[z+1]=PixelBuffer3[z-902]=PixelBuffer3[z-896]=PixelBuffer3[z+898]=PixelBuffer3[z+904]=Light.g;
    PixelBuffer3[z+2]=PixelBuffer3[z-901]=PixelBuffer3[z-895]=PixelBuffer3[z+899]=PixelBuffer3[z+905]=Light.b;
  }
  z = (int ((Viewer[2]-MasterMin)*scalfac)*300+ int((Viewer[1]-MasterMin)*scalfac))*3;
  if ((z%900>0) && (z%900<897) && (z/900>0) && (z/900<299)){
    PixelBuffer3[z-3]=PixelBuffer3[z-903]=PixelBuffer3[z-897]=PixelBuffer3[z+897]=PixelBuffer3[z+903]=1;
    PixelBuffer3[z+3]=PixelBuffer3[z+900]=PixelBuffer3[z-900]=1;
    PixelBuffer3[z-2]=PixelBuffer3[z-902]=PixelBuffer3[z-896]=PixelBuffer3[z+898]=PixelBuffer3[z+904]=1;
    PixelBuffer3[z+4]=PixelBuffer3[z+901]=PixelBuffer3[z-899]=1;
    PixelBuffer3[z+2]=PixelBuffer3[z-901]=PixelBuffer3[z-895]=PixelBuffer3[z+899]=PixelBuffer3[z+905]=1;
    PixelBuffer3[z+5]=PixelBuffer3[z+902]=PixelBuffer3[z-898]=1;
  }
  if (Halftone)
    BinaryDisplay(PixelBuffer3);
  for(int i=2; i<900; i+=3)
    PixelBuffer3[i]=1;
  for(int i=2; i<270000; i+=900)
    PixelBuffer3[i]=1;
  for(int i=899; i<270000; i+=900)
    PixelBuffer3[i]=1;
  for(int i=269102; i<270000; i+=3)
    PixelBuffer3[i]=1;
  glLoadIdentity();
  glDrawPixels(300, 300, GL_RGB, GL_FLOAT, PixelBuffer3);
  glEnd();
  glFlush();
}

void findmaster(){
  Coord *C;
  MasterMax = MasterMin = Viewer[0];
  MasterMax = (Viewer[1]>MasterMax) ? Viewer[1]:MasterMax;
  MasterMax = (Viewer[2]>MasterMax) ? Viewer[2]:MasterMax;
  MasterMax = (Light.x>MasterMax) ? Light.x:MasterMax;
  MasterMax = (Light.y>MasterMax) ? Light.y:MasterMax;
  MasterMax = (Light.z>MasterMax) ? Light.z:MasterMax;
  MasterMin = (Viewer[1]<MasterMin) ? Viewer[1]:MasterMin;
  MasterMin = (Viewer[2]<MasterMin) ? Viewer[2]:MasterMin;
  MasterMin = (Light.x<MasterMin) ? Light.x:MasterMin;
  MasterMin = (Light.y<MasterMin) ? Light.y:MasterMin;
  MasterMin = (Light.z<MasterMin) ? Light.z:MasterMin;
  for (int i=0; i< numshape; i++){
    for (int j=0; j<numTri[i][0]; j++){
      C = Coordinates[i][j];
      MasterMax = findmax(C->x, C->y, C->z, MasterMax);
      MasterMin = findmin(C->x, C->y, C->z, MasterMin);
    }
  }
  MasterMax++;
  MasterMin--;
  scalfac =299/(MasterMax-MasterMin);
}

void normcross(Tri *L, float vec1[3], float vec2[3]){ //normalized vec1 x vec2
  L->normvec[0] = vec1[1]*vec2[2] - vec1[2]*vec2[1];
  L->normvec[1] = vec1[2]*vec2[0] - vec1[0]*vec2[2];
  L->normvec[2] = vec1[0]*vec2[1] - vec1[1]*vec2[0];
  Normalize(L->normvec);
  float lightvec[3]={Light.x - L->C1->x, Light.y - L->C1->y, Light.z - L->C1->z};
  if (Dot(L->normvec, lightvec) < 0){
    L->normvec[0]=0-L->normvec[0];
    L->normvec[1]=0-L->normvec[1];
    L->normvec[2]=0-L->normvec[2];
  }
  L->plane = -(L->normvec[0]*L->C1->x + L->normvec[1]*L->C1->y + L->normvec[2]*L->C1->z);
}

void norm(){
  Coord *C;
  Tri *L;
  float vec1[3], vec2[3];
  for (int i=0; i< numshape; i++){
    for (int j=0; j<numTri[i][0]; j++){
      C = Coordinates[i][j];
      if (C->r > 1)
	C->r = 1;
      else if (C->r < 0)
	C->r = 0;
      if (C->g > 1)
	C->g = 1;
      else if (C->g < 0)
	C->g = 0;
      if (C->b > 1)
	C->b = 1;
      else if (C->b < 0)
	C->b = 0;
      vec1[0] = (((C->y)-MasterMin)*scalfac);
      vec2[0] = ((C->x)-MasterMin)*scalfac;
      C->corxy = (int (vec1[0])*300+int (vec2[0]))*3;
      vec1[1] = (((C->z)-MasterMin)*scalfac);
      C->corxz = (int (vec1[1])*300.0+int (vec2[0]))*3.0;
      C->coryz = (int (vec1[1])*300+int (vec1[0]))*3;
      C->corxy -= (C->corxy%3);
      C->corxz -= (C->corxz%3);
      C->coryz -= (C->coryz%3);
    }
    for (int j=0; j<numTri[i][1]; j++){
      L = Lines[i][j];
      vec1[0] = L->C1->x-L->C2->x;
      vec1[1] = L->C1->y-L->C2->y;
      vec1[2] = L->C1->z-L->C2->z;
      vec2[0] = L->C1->x-L->C3->x;
      vec2[1] = L->C1->y-L->C3->y;
      vec2[2] = L->C1->z-L->C3->z;
      normcross(L, vec1, vec2);
    }
  }
}

void setCor(){
  int i,j;
  cout << "Shape Number (Int 1 - "<< numshape << "): ";
  cin >> i;
  i--;
  if (i>numshape)
    i=numshape;
  else if (i<0)
    i=0;
  cout << "Coordinate Number (Int 1 - "<< numTri[i][0]<< "): ";
  cin >> j;
  j--;
  if (j>numTri[i][0])
    j=numTri[i][0];
  else if (j<0)
    j=0;
  cout << "Coordinate R Value: ";
  cin >> Coordinates[i][j]->r;
  cout << "Coordinate G Value: ";
  cin >> Coordinates[i][j]->g;
  cout << "Coordinate B Value: ";
  cin >> Coordinates[i][j]->b;
  menvalue = 24;
}

void setView(){
  if (menvalue==11){
    cout << "Viewer X coordinate: ";
    cin >> Viewer[0];
    cout << "Viewer Y coordinate: ";
    cin >> Viewer[1];
    cout << "Viewer Z coordinate: ";
    cin >> Viewer[2];
    menvalue++;
  }
  else{
    Viewer[0]+=DeltaX/100*sign*(MasterMax-MasterMin);
    Viewer[1]+=DeltaY/100*sign*(MasterMax-MasterMin);
    Viewer[2]+=DeltaZ/100*sign*(MasterMax-MasterMin);
  }
}

void setLight(){
  if (menvalue==22){
    Light.x+=DeltaX/100*sign*(MasterMax-MasterMin);
    Light.y+=DeltaY/100*sign*(MasterMax-MasterMin);
    Light.z+=DeltaZ/100*sign*(MasterMax-MasterMin);
  }
  else if(menvalue==24){
    Light.r+=DeltaX/100*sign;
    Light.g+=DeltaY/100*sign;
    Light.b+=DeltaZ/100*sign;
  }
  else if (menvalue==26){
    IA[0]+=DeltaX/100*sign;
    IA[1]+=DeltaY/100*sign;
    IA[2]+=DeltaZ/100*sign;
  }
  else if (menvalue ==21){
    cout << "Light X coordinate: ";
    cin >> Light.x;
    cout << "Light Y coordinate: ";
    cin >> Light.y;
    cout << "Light Z coordinate: ";
    cin >> Light.z;
    menvalue++;
  }
  else if (menvalue == 23){
    cout << "Light R value: ";
    cin >> Light.r;
    cout << "Light G value: ";
    cin >> Light.g;
    cout << "Light B value: ";
    cin >> Light.b;
    menvalue++;
  }
  else if (menvalue == 25){
    cout << "Ambient Light R value: ";
    cin >> IA[0];
    cout << "Ambient Light G value: ";
    cin >> IA[1];
    cout << "Ambient Light B value: ";
    cin >> IA[2];
    menvalue++;
  }
  if (Light.r >1.0)
    Light.r=1.0;
  else if (Light.r < 0.0)
    Light.r = 0.0;
  if (Light.g >1.0)
    Light.g=1.0;
  else if (Light.g < 0.0)
    Light.g = 0.0;
  if (Light.b >1.0)
    Light.b=1.0;
  else if (Light.b < 0.0)
    Light.b = 0.0;

  if (IA[0] >1.0)
    IA[0]=1.0;
  else if (IA[0] < 0.0)
    IA[0] = 0.0;
  if (IA[1] >1.0)
    IA[1]=1.0;
  else if (IA[1] < 0.0)
    IA[1] = 0.0;
  if (IA[2] >1.0)
    IA[2]=1.0;
  else if (IA[2] < 0.0)
    IA[2] = 0.0;
}

void setShape(int num){
  float K;
  int n;
  if ((menvalue%10) == 1){
    int i=(menvalue/10)-3;
    cout << "Shape "<< i+1<< " Ka Value (0 - 1): ";
    cin >> K;
    K = (K>1) ? 1 : K;
    K = (K<0) ? 0 : K;
    for (int j=0; j<numTri[i][1] ; j++){
      Lines[i][j]->KA = K;
    }
  }
  if ((menvalue%10) == 2){
    int i=(menvalue/10)-3;
    cout << "Shape "<< i+1<< " Kd Value (0 - 1): ";
    cin >> K;
    K = (K>1) ? 1 : K;
    K = (K<0) ? 0 : K;
    for (int j=0; j<numTri[i][1] ; i=j++){
      Lines[i][j]->KD = K;
    }
  }
  if ((menvalue%10) == 3){
    int i=(menvalue/10)-3;
    cout << "Shape "<< i+1<< " Ks Value (0 - 1): ";
    cin >> K;
    K = (K>1) ? 1 : K;
    K = (K<0) ? 0 : K;
    for (int j=0; j<numTri[i][1] ; j++){
      Lines[i][j]->KS = K;
    }
  }
  if ((menvalue%10) == 4){
    int i=(menvalue/10)-3;
    cout << "Shape "<< i+1<< " n Value (1, 2, 3..): ";
    cin >> n;
    n = (n<1) ? 1 : n;
    for (int j=0; j<numTri[i][1] ; j++){
      Lines[i][j]->n = n;
    }
  }
  menvalue=24;
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
    glEnd();
  glFlush();

}

void matmult44(float A[], float B[],float C[]){ //4x4 x 4x4 matrix multiplier
  for (int i=0; i<4; i++){
    for (int j=0; j<4; j++){
      C[i*4+j]=0;
      for (int k=0; k<4; k++){
	C[i*4+j]+=A[j+k*4]*B[i*4+k];
      }
    }
  }
}

void matmult41(float A[], float B[], float C[]){ //4x4 x 4x1 matrix multiplier
  for (int i=0; i<4; i++){
    C[i]=0;
    for (int k=0; k<4; k++){
      C[i]+=A[i+k*4] * B[k];
    }
  }
}

void rotate2(float M[], int i, int j){
  float in_vert[] = {Coordinates[i][j]->x,Coordinates[i][j]->y,Coordinates[i][j]->z,1};
  float out_vert[4];
  matmult41(M, in_vert, out_vert); //Apply rotation matrix to vertex
  Coordinates[i][j]->x=out_vert[0];
  Coordinates[i][j]->y=out_vert[1];
  Coordinates[i][j]->z=out_vert[2];
}

void Rotate(int i){ //Rotation Function
  float theta = .03;
  if (DeltaX || DeltaY || DeltaZ){
    float M[16];
    //    float l = pow(pow(dx,2)+pow(dy,2)+pow(dz,2),0.5);
    //    float in42=1/(l*l+dx*dx);
    //    float in41=pow(dz,2)+pow(dy,2);
    //  float M21[] = {1,0,0,0,0,dz/l,dy/l,0,0,-dy/l,dz/l,0,0,0,0,1}; //Rotate over x-axis to YZ plane
    //  float M22[] = {l,0,dx,0,0,1,0,0,-dx,0,l,0,0,0,0,1}; //Rotate over Y axis to Z axis
    if (DeltaX){
      float avg=0;
      int n;
      for (n=0; n<numTri[i][0]; n++){
	avg+=Coordinates[i][n]->x;
      }
      avg/=n;
      float M1[] = {1,0,0,0,0,1,0,0,0,0,1,0,-avg,0,0,1}; //First Translation Matrix
      float M3[] = {1,0,0,0,0,float (cos(theta)),float (sin(theta*sign)),0,0,float (-sin(theta*sign)),float (cos(theta)),0,0,0,0,1}; //Rotate about X
      float M5[] = {1,0,0,0,0,1,0,0,0,0,1,0,avg,0,0,1}; //Translate back to normal
      matmult44(M5, M3,M);
      matmult44(M, M1,M5);
      for (int j=0; j<numTri[i][0];j++){ //Call matrix to each vertex
	rotate2(M5,i, j);
      }
    }
    else if (DeltaZ){
      float avg=0;
      int n;
      for (n=0; n<numTri[i][0]; n++){
	avg+=Coordinates[i][n]->z;
      }
      avg/=n;
      float M1[] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,-avg,1}; //First Translation Matrix
      float M3[] = {float (cos(theta)),float (sin(theta*sign)),0,0,float (-sin(theta*sign)),float (cos(theta)),0,0,0,0,1,0,0,0,0,1}; //Rotate about Z
      float M5[] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,avg,1}; //Translate back to normal
      matmult44(M5, M3,M);
      matmult44(M, M1,M5);
      for (int j=0; j<numTri[i][0];j++){ //Call matrix to each vertex
	rotate2(M5,i, j);
      }
    }
    else if (DeltaY){
      float avg=0;
      int n;
      for (n=0; n<numTri[i][0]; n++){
	avg+=Coordinates[i][n]->y;
      }
      avg/=n;
      float M1[] = {1,0,0,0,0,1,0,0,0,0,1,0,0,-avg,0,1}; //First Translation Matrix
      float M3[] = {float (cos(theta)),0,float (-sin(theta*sign)),0,0,1,0,0,float (sin(theta*sign)),0,float (cos(theta)),0,0,0,0,1}; //Rotate about X
      float M5[] = {1,0,0,0,0,1,0,0,0,0,1,0,0,avg,0,1}; //Translate back to normal
      matmult44(M5, M3,M);
      matmult44(M, M1,M5);
      for (int j=0; j<numTri[i][0];j++){ //Call matrix to each vertex
	rotate2(M5,i, j);
      }
    }
    //  float M42in[]= {l*in42,0,-dx*in42,0,0,1,0,0,dx*in42,0,l*in42,0,0,0,0,1}; //Inverse of M22
    //  float M41in[]= {1,0,0,0,0,l*dz/in41,-l*dy/in41,0,0,l*dy/in41,l*dz/in41,0,0,0,0,1}; //Inverse of M21
  }
}

void Translate(int i){
  for(int j=0; j<numTri[i][0]; j++){
    Coordinates[i][j]->x+=DeltaX*sign*(MasterMax-MasterMin)/100; //Translation function
    Coordinates[i][j]->y+=DeltaY*sign*(MasterMax-MasterMin)/100;
    Coordinates[i][j]->z+=DeltaZ*sign*(MasterMax-MasterMin)/100;
  }
}

void drawall(){
  if (menvalue==1)
    setCor();
  else if (menvalue==2){
    Halftone=!Halftone;
    menvalue=24;
  }
  else if (menvalue<20)
    setView();
  else if (menvalue<30)
    setLight();
  else if (menvalue<40)
    setShape(0);
  else if (menvalue<50)
    setShape(1);
  else if (menvalue<60)
    setShape(2);
  else if (menvalue<70)
    setShape(3);
  else if (menvalue<80)
    setShape(4);
  else if (menvalue<90)
    Rotate(menvalue%10);
  else Translate(menvalue%10);
  findmaster();
  norm();
  draw();
  display1();
  display2();
  display3();
  if((DeltaZ!=0) || (DeltaY!=0) || (DeltaX!=0)){
    glutSetWindow(MainWindow);
    glutPostRedisplay();
  }
}

void pressKey(int key, int xx, int yy) {
  switch (key) {
  case GLUT_KEY_UP : DeltaZ = 1.0; break;
  case GLUT_KEY_DOWN : DeltaX = 1.0; break;
  case GLUT_KEY_RIGHT : DeltaY = 1.0; break; //Key inputs for animated movements
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

int main(int argc, char ** argv) {
  load();
  glutInit(&argc, argv);                                  //initializes the GLUT framework
  glutInitDisplayMode(GLUT_SINGLE);
  glutInitWindowSize(600,600);
  glutInitWindowPosition(100,100);
  MainWindow = glutCreateWindow("Jacob Ley Proj3");
  glutSpecialFunc(pressKey);
  glutSpecialUpFunc(releaseKey);
  createmenu();
  glutDisplayFunc(drawall);
  xyWindow = glutCreateSubWindow(MainWindow, 0, 300, 300, 300);
  glutSpecialFunc(pressKey);
  glutSpecialUpFunc(releaseKey);
  glutDisplayFunc(display1);

  xzWindow = glutCreateSubWindow(MainWindow, 0, 0, 300, 300);
  glutSpecialFunc(pressKey);
  glutSpecialUpFunc(releaseKey);
  glutDisplayFunc(display2);

  yzWindow = glutCreateSubWindow(MainWindow, 300, 0, 300, 300);
  glutSpecialFunc(pressKey);
  glutSpecialUpFunc(releaseKey);
  glutDisplayFunc(display3);
  glutMainLoop();           //the main loop of the GLUT framework
}
