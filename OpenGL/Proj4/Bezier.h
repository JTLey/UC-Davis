//
//  Bezier.h
//  ECS 175 Proj4
//
//  Created by Jacob Ley on 11/15/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#ifndef __ECS_175_Proj4__Bezier__
#define __ECS_175_Proj4__Bezier__

#include <stdio.h>

class Bezier{
public:
    Bezier(int Res);
    void AddPoint(float x, float y);
    void InsertPoint(float x, float y, int n);
    void MovePoint(int n, float dx, float dy);
    void DelPoint(int n);
    void FindPoint(int n, float *x, float *y);
    int numPoint;   //Total number of points
    int Resolution; // Resolution of curve
    float FindMin();
    float FindMax();
    float Points[50][2];    //Contains the control points
    void AdRes(bool x);
};

//BSpline is very similar to Bezier, includes k value and knots
class BSpline{
public:
    BSpline(int Res, int poly);
    void AddPoint(float x, float y);
    void InsertPoint(float x, float y, int n);
    void MovePoint(int n, float dx, float dy);
    void DelPoint(int n);
    void FindPoint(int n, float *x, float *y);
    int numPoint;
    int k;  //Value of K
    int Resolution;
    float FindMin();
    float FindMax();
    float Points[50][2];
    float U[100]; //Knots
    void AdRes(bool x);
    void AdK(bool x);
};

#endif /* defined(__ECS_175_Proj4__Bezier__) */
