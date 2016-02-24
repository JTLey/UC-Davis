//
//  Sphere.cpp
//  ECS175_Proj5
//
//  Created by Jacob Ley on 12/3/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#include "Sphere.h"
#include <cmath>
#include <iostream>

using namespace std;

float dotP(float X[3], float Y[3]){
    return (X[0]*Y[0] + X[1]*Y[1] + X[2]*Y[2]);
}

sphere::sphere(float *X){
    Coord[0] = X[0];
    Coord[1] = X[1];
    Coord[2] = X[2];
    
    Ellipse[0] = X[3];
    Ellipse[1] = X[4];
    Ellipse[2] = X[5];
    
}

void sphere::AddK(float *X){
    K[0] = X[0];
    K[1] = X[1];
    K[2] = X[2];
    K[3] = X[3];
    K[4] = X[4];
    K[5] = X[5];
    K[6] = X[6];
    K[7] = X[7];
    K[8] = X[8];
    K[9] = X[9];
}

float sphere::NearHit(float Source[3], float Ray[3]){
    float P1[3],V1[3], A, B, C;
    P1[0] = Source[0]*Ellipse[0]-Coord[0]*Ellipse[0];
    P1[1] = Source[1]*Ellipse[1]-Coord[1]*Ellipse[1];
    P1[2] = Source[2]*Ellipse[2]-Coord[2]*Ellipse[2];
    V1[0] = Ray[0]*Ellipse[0];
    V1[1] = Ray[1]*Ellipse[1];
    V1[2] = Ray[2]*Ellipse[2];
    A = dotP(V1, V1);
    B = 2 * dotP(V1, P1);
    C = dotP(P1, P1) - 1;
    P1[2]=B*B-4*A*C;
  //  cout << "determinant " << P1[2] << "\n";
    if((P1[2])>0){
        P1[1] =(pow(P1[2],.5));
        P1[0] = ((-B) + P1[1])/(2*A);
        P1[1] = ((-B) - P1[1])/(2*A);
        if ((P1[0]>0.03) &&  ((P1[0]<P1[1])||(P1[1]<=0.03))){
            return P1[0];
        }
        else if(P1[1]>.03) return P1[1];
        else return -1;
    }
    else return -1;
}

void sphere::getSource(float Ray[3], float newSource[3], float newNormal[3], float GetK[10]){
    newNormal[0] = 2 * (newSource[0]-Coord[0]) * Ellipse[0]*Ellipse[0];
    newNormal[1] = 2 * (newSource[1]-Coord[1]) * Ellipse[1]*Ellipse[1];
    newNormal[2] = 2 * (newSource[2]-Coord[2]) * Ellipse[2]*Ellipse[2];
    if (dotP(newNormal,Ray)>0){
        newNormal[0] = (-newNormal[0]);
        newNormal[1] = (-newNormal[1]);
        newNormal[2] = (-newNormal[2]);
    }
    GetK[0]=K[0];
    GetK[1]=K[1];
    GetK[2]=K[2];
    GetK[3]=K[3];
    GetK[4]=K[4];
    GetK[5]=K[5];
    GetK[6]=K[6];
    GetK[7]=K[7];
    GetK[8]=K[8];
    GetK[9]=K[9];
}

