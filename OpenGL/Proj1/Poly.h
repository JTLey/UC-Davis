//
//  Poly.h
//  ECS175_Proj1
//
//  Created by Jacob Ley on 10/1/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#ifndef __ECS175_Proj1__Poly__
#define __ECS175_Proj1__Poly__

#include <stdio.h>

class polygon{
 public:
  polygon();
  void draw(float *Buffer, int num);
  void addvert(int x, int y, int z);
  void tran(int x, int y);
  void rot(float theta);
  void scaleX(float x);
  void scaleY(float y);
  void clip(int x1, int x2, int y1, int y2);
  int numvert, xmin, xmax, xmaxclip, xmaxwin, ymin, ymax, ymaxwin, ymaxclip,drawnum;
  int staycolor(int x, int y);
  void window( int xmaximum, int ymaximum);
  int getvertnum();
  void getverts (int *x, int *y, int vert);

 private:
  void drawline(int x1, int y1, int x2, int y2, float *Buffer, int num);
  int truevert[2*20], drawvert[3*30];
  void centroid(int *x, int *y);
};

#endif /* defined(__ECS175_Proj1__Poly__) */
