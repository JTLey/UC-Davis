//
//  Bezier.cpp
//  ECS 175 Proj4
//
//  Created by Jacob Ley on 11/15/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#include "Bezier.h"
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
#include <iostream>

//////////////BEZIER//////////////

Bezier::Bezier(int Res){ //Initialize with a default Resolution
    numPoint=0;
    Resolution = Res;
}

void Bezier::AddPoint(float x, float y){
    Points[numPoint][0]=x;
    Points[numPoint][1]=y;  //Add a point on to the end
    numPoint++;
}

void Bezier::InsertPoint(float x, float y, int n){
    for(int i=numPoint; i>n; i--){
        Points[i][0]=Points[i-1][0];
        Points[i][1]=Points[i-1][1];    //Insert a point in the middle of the curve
    }
    Points[n][0]=x;
    Points[n][1]=y;
    numPoint++;
}

void Bezier::MovePoint(int n, float dx, float dy){
    Points[n][0]+=dx;
    Points[n][1]+=dy;   //Adjust the cooridinate values
}

void Bezier::DelPoint(int n){
    for (int i=n; i<numPoint;i++){
        Points[i][0]=Points[i+1][0];
        Points[i][1]=Points[i+1][1];    //Removes a point from the curve
    }
    numPoint--;
}

float Bezier::FindMax(){
    float max=Points[0][0];
    max = (Points[0][1]>max) ? Points[0][1] : max;
    for (int i=1; i<numPoint; i++){
        max = (Points[i][0]>max) ? Points[i][0] : max;
        max = (Points[i][1]>max) ? Points[i][1] : max; //Returns max point value (x and y)
    }
    return max;
}

void Bezier::AdRes(bool x){
    if (x)
        Resolution++;
    else if (Resolution>1){ //Adjusts resolution, cannot be less than 1
        Resolution--;
    }
}

float Bezier::FindMin(){
    float min=Points[0][0];
    min = (Points[0][1]<min) ? Points[0][1] : min;
    for (int i=1; i<numPoint; i++){
        min = (Points[i][0]<min) ? Points[i][0] : min;  //Returns min point value (x and y)
        min = (Points[i][1]<min) ? Points[i][1] : min;
    }
    return min;
}

void Bezier::FindPoint(int n, float *x, float *y){  //Returns the coordinates of a point
    *x = Points[n][0];
    *y = Points[n][1];
}

////////////////BSPLINE/////////////////

//BSpline is very similar to Bezier, with addition of K
//See Bezier for descriptions

BSpline::BSpline(int Res, int poly){ //Initialize with resolution and K
    numPoint=0;
    Resolution = Res;
    k = poly;
    for (int i =0; i<100; i++) //Default U values
        U[i]=i;
    
}

void BSpline::AddPoint(float x, float y){
    Points[numPoint][0]=x;
    Points[numPoint][1]=y;
    numPoint++;
}

void BSpline::InsertPoint(float x, float y, int n){
    for(int i=numPoint; i>n; i--){
        Points[i][0]=Points[i-1][0];
        Points[i][1]=Points[i-1][1];
    }
    Points[n][0]=x;
    Points[n][1]=y;
    numPoint++;
}

void BSpline::MovePoint(int n, float dx, float dy){
    Points[n][0]+=dx;
    Points[n][1]+=dy;
}

void BSpline::DelPoint(int n){
    for (int i=n; i<numPoint;i++){  //Deletes Point, note that it does not check k!
        Points[i][0]=Points[i+1][0];
        Points[i][1]=Points[i+1][1];
    }
    numPoint--;
}

float BSpline::FindMax(){
    float max=Points[0][0];
    max = (Points[0][1]>max) ? Points[0][1] : max;
    for (int i=1; i<numPoint; i++){
        max = (Points[i][0]>max) ? Points[i][0] : max;
        max = (Points[i][1]>max) ? Points[i][1] : max;
    }
    return max;
}

void BSpline::AdRes(bool x){
    if (x)
        Resolution++;
    else if (Resolution>1){
        Resolution--;
    }
}

void BSpline::AdK(bool x){  //Change K value, cannot be less than 2
    if (x && (k < numPoint))
        k++;
    else if ((!x) && k>2){
        k--;
    }
}

float BSpline::FindMin(){
    float min=Points[0][0];
    min = (Points[0][1]<min) ? Points[0][1] : min;
    for (int i=1; i<numPoint; i++){
        min = (Points[i][0]<min) ? Points[i][0] : min;
        min = (Points[i][1]<min) ? Points[i][1] : min;
    }
    return min;
}

void BSpline::FindPoint(int n, float *x, float *y){
    *x = Points[n][0];
    *y = Points[n][1];
}
