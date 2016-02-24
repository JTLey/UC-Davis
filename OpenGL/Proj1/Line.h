//
//  Line.h
//  ECS175_Proj1
//
//  Created by Jacob Ley on 10/1/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#ifndef __ECS175_Proj1__Line__
#define __ECS175_Proj1__Line__

#include <stdio.h>

class line{
 public:
  line();
  int x1, x2, xmaxclip, y1, y2, ymaxclip, xmin;
  int xmax, xmaxwin, ymin, ymax, ymaxwin;
  float m, w;
  void draw(float *Buffer, bool BorD);
  void clip(int xminimum, int xmaximum, int yminimum, int ymaximum);
  void window( int xmaximum, int ymaximum);
};

#endif /* defined(__ECS175_Proj1__Line__) */
