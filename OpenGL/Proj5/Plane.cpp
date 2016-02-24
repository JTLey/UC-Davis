//
//  Plane.cpp
//  ECS175_Proj5
//
//  Created by Jacob Ley on 12/3/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#include "Plane.h"
#include <cmath>
#include <iostream>
using namespace std;

float dots(float X[3], float Y[3]){
    return X[0]*Y[0] + X[1]*Y[1] + X[2]*Y[2];
}

void crossP(float X[3], float Y[3], float Z[3]){
    Z[0] = X[1]*Y[2] - Y[1]*X[2];
    Z[1] = X[2]*Y[0] - Y[2]*X[0];
    Z[2] = X[0]*Y[1] - Y[0]*X[1];
}

plane::plane(float *X){
    ABC[0] = X[0];
    ABC[1] = X[1];
    ABC[2] = X[2];
    D = X[3];
}

void plane::AddPoints(float *X){
    Point1[0] = X[0];
    Point1[1] = X[1];
    Point1[2] = X[2];
    
    Point2[0] = X[3];
    Point2[1] = X[4];
    Point2[2] = X[5];
    
    Point3[0] = X[6];
    Point3[1] = X[7];
    Point3[2] = X[8];
    
    for (int i=0; i<3; i++){
        AB[i] = Point2[i] - Point1[i];
        BC[i] = Point3[i] - Point2[i];
        CA[i] = Point1[i] - Point3[i];
    }
}

void plane::AddK(float *X){
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

float plane::NearHit(float Source[3], float Ray[3]){
    float x = dots(ABC, Ray);
    if (x==0){
        return -1;
    }
    else {
        float testcross[3], intersection[3], BI[3], CI[3];
        float t = (D - dots(ABC,Source))/x;
        intersection[0] = Source[0] + t*Ray[0];
        intersection[1] = Source[1] + t*Ray[1];
        intersection[2] = Source[2] + t*Ray[2];
        BI[0] = intersection[0] - Point2[0];
        BI[1] = intersection[1] - Point2[1];
        BI[2] = intersection[2] - Point2[2];
        CI[0] = intersection[0] - Point3[0];
        CI[1] = intersection[1] - Point3[1];
        CI[2] = intersection[2] - Point3[2];
        crossP(AB, BI, testcross);
        if ( dots(testcross,ABC)>0){
            crossP(BC, CI, testcross);
            if ( dots(testcross,ABC)>0){
                float AI[3];
                AI[0] = intersection[0] - Point1[0];
                AI[1] = intersection[1] - Point1[1];
                AI[2] = intersection[2] - Point1[2];
                crossP(CA, AI, testcross);
                if (dots(testcross,ABC)>0){
                    return t;
                }
                else return -1;
            }
            else return -1;
        }
        else{
            crossP(BC, CI, testcross);
            if ( dots(testcross,ABC)<0){
                float AI[3];
                AI[0] = intersection[0] - Point1[0];
                AI[1] = intersection[1] - Point1[1];
                AI[2] = intersection[2] - Point1[2];
                crossP(CA, AI, testcross);
                if (dots(testcross,ABC)<0){
                    return t;
                }
                else return -1;
            }
            else return -1;
        }
    }
}

void plane::getSource(float OldRay[3], float newNormal[3], float GetK[10]){
    if (dots(OldRay,ABC)<0){
        newNormal[0]=ABC[0];
        newNormal[1]=ABC[1];
        newNormal[2]=ABC[2];
    }
    else {
        newNormal[0]=-ABC[0];
        newNormal[1]=-ABC[1];
        newNormal[2]=-ABC[2];
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

