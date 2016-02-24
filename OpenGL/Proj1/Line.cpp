//  ECS175_Proj1
//
//  Created by Jacob Ley on 10/1/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#include "Line.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
using namespace std;

line::line(){ //Constructor, set max window sizes
  x1=x2=y1=y2=999;
  xmin = ymin = 0;
  xmax=ymax= xmaxclip = ymaxclip = 999;
  xmaxwin = ymaxwin = 300;
}

void line::draw(float *Buffer, bool BorD){
  xmax = min(xmaxwin,xmaxclip);
  ymax = min(ymaxwin, ymaxclip);
  int x, y;
  x = y = 0;
  if (x1 == x2){ //Horizontal and Vertical lines do not use DDA or Bresenham
    if((x1>xmin) && (x1<xmax)){
      if (y1>y2){
	x=y1;
	y1=y2;
	y2=x;
      }
      for (y=max(y1,ymin); y<=min(y2,ymax); y++){ //Simply a single for loop
	Buffer[(y*xmaxwin+x1)*3]=1;
	Buffer[(y*xmaxwin+x1)*3+1]=1;
	Buffer[(y*xmaxwin+x1)*3+2]=1;
      }
    }
    return;
  }
  else if (y1 == y2){
    if ((y1>ymin) && (y1<ymax)){
      if (x1 > x2){
	x=x1;
	x1=x2;
	x2=x;
      }
      for (x=max(x1,xmin); x<=min(x2,xmax); x++){ //Loops go from low to high values
	Buffer[(y1*xmaxwin+x)*3]=1;
	Buffer[(y1*xmaxwin+x)*3+1]=1;
	Buffer[(y1*xmaxwin+x)*3+2]=1;
      }
    }
    return;
  }

  if (BorD){ //If DDA
    m = float ((y2 - y1))/ float ((x2 - x1)); //Find slope
    if ((m>=-1) && (m<=1)){ //Determine whether x loop or y loop is used
      if (x1>x2){ //x loop, make sure x1 is smaller x
	x=x1;
	x1=x2;
	x2=x;
	x=y1;
	y1=y2;
	y2=x;
      }
      for (x=max(x1,xmin); x<=min(x2, xmax); x++){ //Only draw within clip window
	y = round(m * (float (x - x1))) + y1;
	if ((y<=ymax) && (y>=ymin)){
	  Buffer[(y*xmaxwin+x)*3]=1;
	  Buffer[(y*xmaxwin+x)*3+1]=1;
	  Buffer[(y*xmaxwin+x)*3+2]=1;
	}
      }
    }
    else{
      if (y1>y2){//Same as with x, make sure y1 is smaller y
	x=x1;
	x1=x2;
	x2=x;
	x=y1;
	y1=y2;
	y2=x;
      }
      w = 1/m; //Take an inverse slope
      for (y=max(y1,ymin); y<=min(y2, ymax); y++){ //Only draw within clip
	x = round(w * (float (y-y1)) + x1);
	if ((x<=xmax) && (x>=xmin)){
	  Buffer[(y*xmaxwin+x)*3]=1;
	  Buffer[(y*xmaxwin+x)*3+1]=1;
	  Buffer[(y*xmaxwin+x)*3+2]=1;
	}
      }
    }
  }
  else { //Bresenhams algorithm
    int P;
    int dx, dy; //note that no floats are declared
    if (abs(y2-y1)<abs(x2-x1)){ //Is slope more horizontal or vertical
      if (x1>x2){ //More horizontal, make x1 smaller
	x=x1;
	x1=x2;
	x2=x;
	x=y1;
	y1=y2;
	y2=x;
      }
      dx = x2-x1;
      if (y2>y1){ //Must decide whether P makes y increase or decrease
	y=y1;
	dy = y2-y1;
	P = 2 * dy - dx;
	for (x=x1; (x<=min(xmax,x2)) && (y<=min(y2, ymax)); x++){
	  if((x>=xmin)&&(y>=max(ymin, y1))){ //Only draw within clip
	    Buffer[(y*xmaxwin+x)*3]=1;
	    Buffer[(y*xmaxwin+x)*3+1]=1;
	    Buffer[(y*xmaxwin+x)*3+2]=1;
	  }
	  if (P>0){
	    y++; //Increase y if P>0
	    P = P + 2 * dy - 2 * dx;
	  }
	  else P = P + 2 * dy;

	}
      }
      else { //Repeat that y must decrease
	y=y1;
	dy = y1-y2;
	P = 2 * dy - dx;
	for (x=x1; (x<=min(xmax,x2)) && (y>=max(y2, ymin)); x++){
	  if((x>=xmin)&&(y<=min(ymax, y1))){
	    Buffer[(y*xmaxwin+x)*3]=1;
	    Buffer[(y*xmaxwin+x)*3+1]=1;
	    Buffer[(y*xmaxwin+x)*3+2]=1;
	  }
	  if (P>0){
	    y--;
	    P = P + 2 * dy - 2 * dx;
	  }
	  else P = P + 2 * dy;

	}
      }

    }
    else{ //Repeat for the case that dy>dx
      if (y1>y2){//make y1 smaller value
	x=x1;
	x1=x2;
	x2=x;
	x=y1;
	y1=y2;
	y2=x;
      }
      dy = y2-y1;
      if (x2>x1){
	x=x1;
	dx = x2-x1;
	P = 2 * dx - dy;
	for (y=y1; (y<=min(ymax,y2)) && (x<=min(x2, xmax)); y++){
	  if((y>=ymin)&&(x>=max(xmin, x1))){
	    Buffer[(y*xmaxwin+x)*3]=1;
	    Buffer[(y*xmaxwin+x)*3+1]=1;
	    Buffer[(y*xmaxwin+x)*3+2]=1;
	  }
	  if (P>=0){
	    x++;
	    P = P + 2 * dx - 2 * dy;
	  }
	  else P = P + 2 * dx;

	}
      }
      else {
	x=x1;
	dx = x1-x2;
	P = 2 * dx - dy;
	for (y=y1; (y<=min(ymax,y2)) && (x<=max(x2, xmin)); y++){
	  if((y>=ymin)&&(x>=min(xmax, x1))){
	    Buffer[(y*xmaxwin+x)*3]=1;
	    Buffer[(y*xmaxwin+x)*3+1]=1;
	    Buffer[(y*xmaxwin+x)*3+2]=1;
	  }
	  if (P>0){
	    x--;
	    P = P + 2 * dx - 2 * dy;
	  }
	  else P = P + 2 * dx;

	}
      }
    }
  }
}

void line::clip(int xminimum, int xmaximum, int yminimum, int ymaximum){
  xmin =xminimum; //Input clipping window
  xmaxclip =xmaximum;
  ymin =yminimum;
  ymaxclip =ymaximum;
}

void line::window(int xmaximum, int ymaximum){
  xmaxwin = xmaximum; //input window size, acts like clip
  ymaxwin = ymaximum;
}
