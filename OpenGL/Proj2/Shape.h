//
//  Shape.h
//  ECS175_Proj2
//
//  Created by Jacob Ley on 10/20/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#ifndef __ECS175_Proj2__Shape__
#define __ECS175_Proj2__Shape__

#include <stdio.h>

class Shape{
 public:
  Shape();
  void addvert(float x, float y, float z);
  float findmin();
  float findmax();
  int numvert;
  void rotate(float x1, float x2, float y1, float y2, float z1, float z2, float theta);
  void translate(float x, float y, float z);
  void scale(float scal);
  void findvert(float *x, float *y, float *z, int n);
  void matmult44(float A[], float B[], float C[]);
  void matmult41(float A[], float B[], float C[]);

 private:
  void rotate2(float M[], int i);
  float coordinates[50][3];
  void centroid (float *x, float *y, float *z);

};

#endif /* defined(__ECS175_Proj2__Shape__) */
