//
//  Poly.cpp
//  ECS175_Proj1
//
//  Created by Jacob Ley on 10/1/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#include "Poly.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <iostream>
using namespace std;

polygon::polygon(){ //constructor
  int i;
  for (i=0; i<40; i++)
    truevert[i]=999;
  numvert=0;
  xmin=0;
  xmaxclip=999;
  xmax = xmaxwin =300;
  ymin=0;
  ymaxclip=999;
  ymax=ymaxwin=300;
}

void polygon::draw(float *Buffer, int num){
  xmax = min(xmaxclip, xmaxwin-1); //Find actual clipping window 
  ymax = min(ymaxclip, ymaxwin-1);
  int a, b, c, x1, x2, y1, y2, tempvert[2*30];
  bool inclip;
  //////////////////////////
  // The start of 4 part clipping function
  for (a =0; a<numvert ; a++){
    if (truevert[2*a]>=xmin) break; //First find a vertex that is within xmin
  }
  inclip = true;
  if (a==numvert) return;
  for (b=0,c=0;c<numvert;a++, c++){ //From that vertex, go vertex to vertex 
    x1= truevert[2*(a%numvert)]; 
    y1 = truevert[2*(a%numvert)+1];
    if (x1>=xmin){
      if (inclip){ //Add valid vertexes to the temporary vertex list
	tempvert[2*b]=x1; //tempvert will update with each iteration
	tempvert[2*b+1]=y1;
	b++;
      }
      else{ //If the last vertex was bad, but this one was good
	x2 = truevert[2*((a-1)%numvert)]; //Find intersection vertex first
	y2 = truevert[2*((a-1)%numvert)+1]; //and add it
	tempvert[2*b+1] = (float (y2-y1))/(float (x2-x1))*(float (xmin-x1))+y1;
	tempvert[2*b]=xmin;
	tempvert[2*b+2]=x1;
	tempvert[2*b+3]=y1;
	inclip = true;
	b+=2;
      }
    }
    else if (inclip){ //If the last vertex was good but this one was bad
      x2 = truevert[2*((a-1)%numvert)]; //only add intersection vertex
      y2 = truevert[2*((a-1)%numvert)+1];
      tempvert[2*b+1] = (float (y2-y1))/(float (x2-x1))*(float (xmin-x1))+y1;
      tempvert[2*b]=xmin;
      inclip = false;
      b++;
    }
    if ((c==numvert-1)&&(x1<xmin)){ //Special case when last vertex in list is bad
      x2 = truevert[2*((a+1)%numvert)]; //add intersection vertex instead
      y2 = truevert[2*((a+1)%numvert)+1];
      tempvert[2*b+1] = (float (y2-y1))/(float (x2-x1))*(float (xmin-x1))+y1;
      tempvert[2*b]=xmin;
      b++;
    }
  }
  drawnum=b; //This is the new amount of vertices (could increase or decrease)

  ////////////////////////////////////
  for (a =0; a<drawnum ; a++){ //REPEAT for xmax, now drawing vertices from 
    if (tempvert[2*a]<=xmax) break; //the new tempvert list
  }
  inclip = true;
  if (a==drawnum) return;
  for (b=0,c=0;c<drawnum;a++, c++){
    x1= tempvert[2*(a%drawnum)];
    y1 = tempvert[2*(a%drawnum)+1];
    if (x1<=xmax){
      if (inclip){
	drawvert[3*b]=x1;
	drawvert[3*b+1]=y1;
	b++;
      }
      else{
	x2 = tempvert[2*((a-1)%drawnum)];
	y2 = tempvert[2*((a-1)%drawnum)+1];
	drawvert[3*b+1] = (float (y2-y1))/(float (x2-x1))*(float (xmax-x1))+y1;
	drawvert[3*b]=xmax;
	drawvert[3*b+3]=x1;
	drawvert[3*b+4]=y1;
	inclip = true;
	b+=2;
      }
    }
    else if (inclip){
      x2 = tempvert[2*((a-1)%drawnum)];
      y2 = tempvert[2*((a-1)%drawnum)+1];
      drawvert[3*b+1] = (float (y2-y1))/(float (x2-x1))*(float (xmax-x1))+y1;
      drawvert[3*b]=xmax;
      inclip = false;
      b++;
    }
    if ((c==drawnum-1)&&(x1>xmax)){
      x2 = tempvert[2*((a+1)%drawnum)];
      y2 = tempvert[2*((a+1)%drawnum)+1];
      drawvert[3*b+1] = (float (y2-y1))/(float (x2-x1))*(float (xmax-x1))+y1;
      drawvert[3*b]=xmax;
      b++;
    }
  }
  drawnum=b;
  /////////////////////////////////////
  for (a =0; a<drawnum ; a++){ //Repeat for YMAX
    if (drawvert[3*a+1]<=ymax) break;
  }
  inclip = true;
  if (a==drawnum) return;
  for (b=0,c=0;c<drawnum;a++, c++){
    x1= drawvert[3*(a%drawnum)];
    y1 = drawvert[3*(a%drawnum)+1];
    if (y1<=ymax){
      if (inclip){
	tempvert[2*b]=x1;
	tempvert[2*b+1]=y1;
	b++;
      }
      else{
	x2 = drawvert[3*((a-1)%drawnum)];
	y2 = drawvert[3*((a-1)%drawnum)+1];
	tempvert[2*b] = (float (x2-x1))/(float (y2-y1))*(float (ymax-y1))+x1;
	tempvert[2*b+1]=ymax;
	tempvert[2*b+2]=x1;
	tempvert[2*b+3]=y1;
	inclip = true;
	b+=2;
      }
    }
    else if (inclip){
      x2 = drawvert[3*((a-1)%drawnum)];
      y2 = drawvert[3*((a-1)%drawnum)+1];
      tempvert[2*b] = (float (x2-x1))/(float (y2-y1))*(float (ymax-y1))+x1;
      tempvert[2*b+1]=ymax;
      inclip = false;
      b++;
    }
    if ((c==drawnum-1)&&(y1>ymax)){
      x2 = drawvert[3*((a+1)%drawnum)];
      y2 = drawvert[3*((a+1)%drawnum)+1];
      tempvert[2*b] = (float (x2-x1))/(float (y2-y1))*(float (ymax-y1))+x1;
      tempvert[2*b+1]=ymax;
      b++;
    }
  }
  drawnum=b;
  /////////////////////////////////////

  for (a =0; a<drawnum ; a++){ //Finally REPEAT for YMIN
    if (tempvert[2*a+1]>=ymin) break;
  }
  inclip = true;
  if (a==drawnum) return;
  for (b=0,c=0;c<drawnum;a++, c++){
    x1= tempvert[2*(a%drawnum)];
    y1 = tempvert[2*(a%drawnum)+1];
    if (y1>=ymin){
      if (inclip){
	drawvert[3*b]=x1;
	drawvert[3*b+1]=y1;
	b++;
      }
      else{
	x2 = tempvert[2*((a-1)%drawnum)];
	y2 = tempvert[2*((a-1)%drawnum)+1];
	drawvert[3*b] = (float (x2-x1))/(float (y2-y1))*(float (ymin-y1))+x1;
	drawvert[3*b+1]=ymin;
	drawvert[3*b+3]=x1;
	drawvert[3*b+4]=y1;
	inclip = true;
	b+=2;
      }
    }
    else if (inclip){
      x2 = tempvert[2*((a-1)%drawnum)];
      y2 = tempvert[2*((a-1)%drawnum)+1];
      drawvert[3*b] = (float (x2-x1))/(float (y2-y1))*(float (ymin-y1))+x1;
      drawvert[3*b+1]=ymin;
      inclip = false;
      b++;
    }
    if ((c==drawnum-1)&&(y1<ymin)){
      x2 = tempvert[2*((a+1)%drawnum)];
      y2 = tempvert[2*((a+1)%drawnum)+1];
      drawvert[3*b] = (float (x2-x1))/(float (y2-y1))*(float (ymin-y1))+x1;
      drawvert[3*b+1]=ymin;
      b++;
    }
  }
  drawnum=b;

  ////////
  for (a=0;a<drawnum-1;a++){ //Certain cases have repeat vertices, delete them
    if ((drawvert[3*a]==drawvert[3*(a+1)]) && (drawvert[3*a+1]==drawvert[3*(a+1)+1])){
      for (b=a; b<drawnum; b++){
	drawvert[3*b]=drawvert[3*(b+1)];
	drawvert[3*b+1]=drawvert[3*(b+1)+1];
      }
      drawnum--;
    }
  }
  if ((drawvert[3*a]==drawvert[0]) && (drawvert[3*a+1]==drawvert[1])){
    drawnum--;
  }

  for (a=0;a<drawnum;a++){ //Declare each vertex as a horizontal, max/min, or neither

    x1=drawvert[3*a];
    x2=drawvert[3*((a+1)%drawnum)];
    y1=drawvert[3*a+1];
    y2=drawvert[3*((a+1)%drawnum)+1];
    b=drawvert[3*((a-1+drawnum)%drawnum)+1];
    c=drawvert[3*((a-1+drawnum)%drawnum)];

    drawline(x1, y1, x2, y2, Buffer, num); //Also use oppurtunity to draw each edge
    
    /// 1 is a local max or min, -1 is a point but not a max/min
    if(b>y1){
      if (y2>y1)
	drawvert[a*3+2]=1;
      else if (y2<y1)
	drawvert[a*3+2]=-1;
    }
    else if(b<y1){
      if (y2<y1){
	drawvert[a*3+2]=1;
      }
      else if (y2>y1){
	drawvert[a*3+2]=-1;
      }
    }
    /// 2,3 is a horizontal line with the point on the left, -2,-3 the point is on the right
    /// 2 is the non-horizontal line slopes upwards (either direction)
    /// 3 the line slopes downwards
    if (b==y1){
      if (c>x1){
	if (y2>y1)
	  drawvert[a*3+2]=2;
	else drawvert[a*3+2]=3;
      }
      else{
	if (y2>y1)
	  drawvert[a*3+2]=-2;
	else drawvert[a*3+2]=-3;
      }
    }
    if (y2==y1){
      if (x2>x1){
	if (b>y1)
	  drawvert[a*3+2]=2;
	else drawvert[a*3+2]=3;
      }
      else{
	if (b>y1)
	  drawvert[a*3+2]=-2;
	else drawvert[a*3+2]=-3;
      }
    }
    if ((y2==y1)&&(y1==b)){
      drawvert[a*3+2]=-1;
    }
  }
}

int polygon::getvertnum(){
  return numvert;
}
//Used for save function
void polygon::getverts(int *x, int *y, int vert){
  *x=truevert[vert*2];
  *y=truevert[vert*2+1];
}

void polygon::tran(int x, int y){
  for (int i=0; i<numvert; i++){ //Iterate through vertices and shift by x,y
    truevert[2*i] = truevert[2*i] + x; //Note that the actual vertices are changed
    truevert[2*i+1] = truevert[2*i+1] + y; 
  }
}

void polygon::rot(float theta){
  int x, y, x1, y1;
  x=0;
  y=0;
  for (int i=0; i<numvert; i++){
    x+=truevert[2*i]; //find centroid
    y+=truevert[2*i+1];
  }
  x=x/numvert;
  y=y/numvert;
  for (int i=0; i<numvert; i++){
    x1 = truevert[2*i] - x; //shift polygon to center
    y1 = truevert[2*i+1] - y;
    truevert[2*i] = x1 * cos(theta) - y1 * sin(theta) + x; //rotate points, then return to original centroid
    truevert[2*i+1] = x1 * sin(theta) + y1 * cos(theta) + y;
  }
}

void polygon::scaleX(float x){
  int x1, y1;
  centroid(&x1, &y1); //Find centroid
  tran(-x1, -y1); // Move to origin
  for (int i=0; i<numvert; i++)
    truevert[2*i] = truevert[2*i] *x;  //Change each x value
  tran(x1, y1);
}

void polygon::scaleY(float y){
  int x1, y1;
  centroid(&x1, &y1);
  tran(-x1, -y1);
  for (int i=0; i<numvert; i++)
    truevert[2*i+1] = truevert[2*i+1]*y; //Same as ScaleX but change Y
  tran(x1,y1);
}

void polygon::clip(int x1, int x2, int y1, int y2){
  xmin = x1;
  xmaxclip = x2;
  ymin = y1; //Edit clipping values
  ymaxclip = y2;
}

void polygon::window(int xmaximum, int ymaximum){
  xmaxwin = xmaximum;
  ymaxwin = ymaximum;
}

void polygon::addvert(int x, int y, int z){
  truevert[2*z]=x;
  truevert[2*z+1]=y; //Add a new vertex
}

void polygon::centroid(int *x, int *y){
  *x=*y=0;
  for (int i=0; i <numvert; i++){
    *x+=truevert[2*i]; //Average values of all x and y coordinates
    *y+=truevert[2*i+1];
  }
  *x = *x / numvert;
  *y = *y / numvert;
}

int polygon::staycolor(int x, int y){
  for (int i=0; i<drawnum; i++){
    if (drawvert[3*i]==x){ //Called by rasterization function
      if (drawvert[3*i+1]==y){ // Returns whether the point is a vertex
	return drawvert[3*i+2]; // Also if point is max/min or normal line
      }
    }
  }
  return 0;
}

void polygon::drawline(int x1, int y1, int x2, int y2, float *Buffer, int num){
  bool sameline=false;
  int x, y; //Draw using Bresenham Algorithm
  x = y = 0;
  if (x1 == x2){ //Like line.cpp, there are 6 cases for slope  >1, 1>0, 0>-1, -1>, =0, =inf
    if (y1>y2){
      y=y1;
      y1=y2;
      y2=y;
    }
    for (y=y1; y<=y2; y++){
      Buffer[(y*xmaxwin+x1)*3+num]=1; //Only edit the buffer for that polygon
    }
    return; //So polygon 1 only draws in red
  }
  else if (y1 == y2){
    if (x1 > x2){
      x=x1;
      x1=x2;
      x2=x;
    }
    for (x=x1; x<=x2; x++){
      if (sameline){
	if (Buffer[(y1*xmaxwin+x)*3+num]!=1)
	  Buffer[(y1*xmaxwin+x)*3+num]=.5; //Special case accounting for scan line algorithm
      } //Essentailly it prevents one line from appearing as two
      else {
	Buffer[(y1*xmaxwin+x)*3+num]=1;
	sameline=true;
      }
    }
    return;
  }
  int P; //For more detail on Bresenham see line.cpp
  int dx, dy;
  if (abs(y2-y1)<abs(x2-x1)){ //Works the same but without fear of going
    if (x1>x2){ //outside clipping window, becuase that has been checked
      swap(x1, x2);
      swap(y1, y2);
    }
    dx = x2-x1;
    if (y2>y1){
      y=y1;
      dy = y2-y1;
      P = 2 * dy - dx;
      for (x=x1; x<=x2; x++){
	if (sameline){
	  if (Buffer[(y*xmaxwin+x)*3+num]!=1)
	    Buffer[(y*xmaxwin+x)*3+num]=.5; //Special case for Rasterization
	}
	else {
	  Buffer[(y*xmaxwin+x)*3+num]=1;
	}
	if (P>0){
	  y++;
	  P = P + 2 * dy - 2 * dx;
	  sameline = false;
	}
	else {
	  P = P + 2 * dy;
	  sameline = true;
	}
      }
    }
    else {
      y=y1;
      dy = y1-y2;
      P = 2 * dy - dx;
      for (x=x1; x<=x2; x++){
	if (sameline){
	  if (Buffer[(y*xmaxwin+x)*3+num]!=1)
	    Buffer[(y*xmaxwin+x)*3+num]=.5;
	}
	else {
	  Buffer[(y*xmaxwin+x)*3+num]=1;
	}
	if (P>0){
	  y--;
	  P = P + 2 * dy - 2 * dx;
	  sameline = false;
	}
	else {
	  P = P + 2 * dy;
	  sameline = true;
	}
      }
    }
  }
  else{
    if (y1>y2){
      swap(x1, x2);
      swap(y1, y2);
    }
    dy = y2-y1;
    if (x2>x1){
      x=x1;
      dx = x2-x1;
      P = 2 * dx - dy;
      for (y=y1; y<=y2; y++){
	Buffer[(y*xmaxwin+x)*3+num]=1;
	if (P>0){
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
      for (y=y1; y<=y2; y++){
	Buffer[(y*xmaxwin+x)*3+num]=1;
	if (P>0){
	  x--;
	  P = P + 2 * dx - 2 * dy;
	}
	else P = P + 2 * dx;

      }
    }
  }
}
