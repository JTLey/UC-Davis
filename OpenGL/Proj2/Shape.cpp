//
//  Shape.cpp
//  ECS175_Proj2
//
//  Created by Jacob Ley on 10/20/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//
#include <iostream>
#include "Shape.h"
#include <cmath>
using namespace std;

Shape::Shape(){
  numvert = 0; //Initialize shape
}

void Shape::addvert(float x, float y, float z){
  coordinates[numvert][0] = x;
  coordinates[numvert][1] = y; //Add a new vertex
  coordinates[numvert][2] = z;
  numvert++;
}

float Shape::findmin(){ //Calculate the total minimum value
  float min=coordinates[0][0];
  for(int i=0; i<numvert; i++){
    for (int n=0; n<3; n++){
      min = (coordinates[i][n]<min)?coordinates[i][n]:min;
    }
  }
  return min;
}

float Shape::findmax(){ //Calculate the total maximum value (for MasterMax)
  float max=coordinates[0][0];
  for(int i=0; i<numvert; i++){
    for (int n=0; n<3; n++){
      max = (coordinates[i][n]>max)?coordinates[i][n]:max;
    }
  }
  return max;
}

void Shape::rotate(float x1, float x2, float y1, float y2, float z1, float z2, float theta){ //Rotation Function
  float dz, dy, dx;
  dz = z2-z1;
  dy = y2-y1; //First Create the rotation matrix
  dx = x2-x1;
  float M[16];
  float l = pow(pow(x1-x2,2)+pow(y1-y2,2)+pow(z1-z2,2),0.5);
  float in42=1/(l*l+dx*dx);
  float in41=pow(dz,2)+pow(dy,2);
  float M1[] = {1,0,0,0,0,1,0,0,0,0,1,0,-x1,-y1,-z1,1}; //First Translation Matrix
  float M21[] = {1,0,0,0,0,dz/l,dy/l,0,0,-dy/l,dz/l,0,0,0,0,1}; //Rotate over x-axis to YZ plane
  float M22[] = {l,0,dx,0,0,1,0,0,-dx,0,l,0,0,0,0,1}; //Rotate over Y axis to Z axis
  float M3[] = {float (cos(theta)),float (sin(theta)),0,0,float (-sin(theta)),float (cos(theta)),0,0,0,0,1,0,0,0,0,1}; //Rotate about Z
  float M42in[]= {l*in42,0,-dx*in42,0,0,1,0,0,dx*in42,0,l*in42,0,0,0,0,1}; //Inverse of M22
  float M41in[]= {1,0,0,0,0,l*dz/in41,-l*dy/in41,0,0,l*dy/in41,l*dz/in41,0,0,0,0,1}; //Inverse of M21
  float M5[] = {1,0,0,0,0,1,0,0,0,0,1,0,x1,y1,z1,1}; //Translate back to normal
  matmult44(M5, M41in,M);
  matmult44(M, M42in,M5);
  matmult44(M5,M3,M);
  matmult44(M,M22,M5);
  matmult44(M5,M21,M); //Create one single matrix
  matmult44(M,M1,M5);
  for (int i=0; i<numvert;i++){ //Call matrix to each vertex
    rotate2(M5,i);
  }
}

void Shape::rotate2(float M[], int i){
  float in_vert[] = {coordinates[i][0],coordinates[i][1],coordinates[i][2],1};
  float out_vert[4];
  matmult41(M, in_vert, out_vert); //Apply rotation matrix to vertex
  coordinates[i][0]=out_vert[0];
  coordinates[i][1]=out_vert[1];
  coordinates[i][2]=out_vert[2];
}
void Shape::translate(float x, float y, float z){
  for(int i=0; i<numvert; i++){
    coordinates[i][0]+=x; //Translation function
    coordinates[i][1]+=y;
    coordinates[i][2]+=z;
  }
}

void Shape::scale(float scal){
  float x,y,z;
  centroid(&x, &y, &z); //Scaling function, about centroid
  translate(-x,-y,-z);
  for(int i=0; i<numvert; i++){
    coordinates[i][0]*=scal;
    coordinates[i][1]*=scal;
    coordinates[i][2]*=scal;
  }
  translate(x,y,z);
}

void Shape::findvert(float *x, float *y, float *z, int n){
  *x=coordinates[n][0];
  *y=coordinates[n][1]; //Returns the nth vertex
  *z=coordinates[n][2];
}

void Shape::centroid(float *x, float *y, float *z){
  *x=0, *y=0, *z=0;
  for(int i=0; i<numvert; i++){
    *x+=coordinates[i][0];
    *y+=coordinates[i][1]; //Averages the coordinates, returns central point
    *z+=coordinates[i][2];
  }
  *x = *x/numvert;
  *y = *y/numvert;
  *z = *z/numvert;
}

void Shape::matmult44(float A[], float B[],float C[]){ //4x4 x 4x4 matrix multiplier
  for (int i=0; i<4; i++){
    for (int j=0; j<4; j++){
      C[i*4+j]=0;
      for (int k=0; k<4; k++){
	C[i*4+j]+=A[j+k*4]*B[i*4+k];
      }
    }
  }
}

void Shape::matmult41(float A[], float B[], float C[]){ //4x4 x 4x1 matrix multiplier
  for (int i=0; i<4; i++){
    C[i]=0;
    for (int k=0; k<4; k++){
      C[i]+=A[i+k*4] * B[k];
    }
  }
}
