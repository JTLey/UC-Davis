//
//  Coord.h
//  ECS175_Proj3
//
//  Created by Jacob Ley on 11/2/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#ifndef __ECS175_Proj3__Coord__
#define __ECS175_Proj3__Coord__

#include <iostream>
#include <cmath>

class Coord{
 public:
  Coord(){
    x=y=z=r=g=b=0;
  }
  float x, y, z, r, g, b;
  int corxy, corxz, coryz;
};

class Tri{
 public:
  Tri(){
    KA = KD = KS = n =1;
  }
  Coord *C1, *C2, *C3;
  int num1, num2, num3;
  int sort;
  int n;
  float normvec[3], KA, KD, KS;
  float plane;
  int renderfirst[100];
  int render;
  void findlimits(Tri *L, float *minx, float *maxx, float *miny, float *maxy, float *minz, float *maxz){
    *minx=*maxx = L->C1->x;
    if (L->C2->x<*minx)
      *minx = L->C2->x;
    else *maxx = L->C2->x;
    if (L->C3->x<*minx)
      *minx = L->C3->x;
    if (L->C3->x>*maxx)
      *maxx = L->C3->x;
    *miny=*maxy = L->C1->y;
    if (L->C2->y<*miny)
      *miny = L->C2->y;
    else *maxy = L->C2->y;
    if (L->C3->y<*miny)
      *miny = L->C3->y;
    if (L->C3->y>*maxy)
      *maxy = L->C3->y;
    *minz=*maxz = L->C1->z;
    if (L->C2->z<*minz)
      *minz = L->C2->z;
    else *maxz = L->C2->z;
    if (L->C3->z<*minz)
      *minz = L->C3->z;
    if (L->C3->z>*maxz)
      *maxz = L->C3->z;
  }

  bool order(Tri *L){ //Should L be rendered before this
    float minAX, minAY, minAZ, maxAX, maxAY, maxAZ, minBX, minBY, minBZ, maxBX, maxBY, maxBZ;
    findlimits(this, &minAX, &maxAX, &minAY, &maxAY, &minAZ, &maxAZ);
    findlimits(L, &minBX, &maxBX, &minBY, &maxBY, &minBZ, &maxBZ);
    if (sort==0){
      if (maxAX<minBX)
	return true;
      else if ((minAX>maxBX) || (maxAY<minBY) || (minAY>maxBY) || (maxAZ<minBZ) || (minAZ>maxBZ))
	return false;
      float avgy = (C1->y + C2->y + C3->y)/3;
      float avgz = (C1->z + C2->z + C3->z)/3;
      float avgy2 = (L->C1->y + L->C2->y + L->C3->y)/3;
      float avgz2 = (L->C1->z + L->C2->z + L->C3->z)/3;
      float xx1 = (-(avgy * normvec[1] + avgz * normvec[2] + plane))/normvec[0];
      float xx2 = (-(avgy * L->normvec[1] + avgz * L->normvec[2] + L->plane))/L->normvec[0];
      float xx3 = (-(avgy2 * normvec[1] + avgz2 * normvec[2] + plane))/normvec[0];
      float xx4 = (-(avgy2 * L->normvec[1] + avgz2 * L->normvec[2] + L->plane))/L->normvec[0];
      if (xx1<xx2 && xx3<xx4)
	return true;
      else return false;
    }
    if (sort==1){
      if (maxAY<minBY)
	return true;
      else if ((minAX>maxBX) || (maxAX<minBX) || (minAY>maxBY) || (maxAZ<minBZ) || (minAZ>maxBZ))
	return false;
      float avgx = (C1->x + C2->x + C3->x)/3;
      float avgz = (C1->z + C2->z + C3->z)/3;
      float avgx2 = (L->C1->x + L->C2->x + L->C3->x)/3;
      float avgz2 = (L->C1->z + L->C2->z + L->C3->z)/3;
      float yy1 = (-(avgx * normvec[0] + avgz * normvec[2] + plane))/normvec[1];
      float yy2 = (-(avgx * L->normvec[0] + avgz * L->normvec[2] + L->plane))/L->normvec[1];
      float yy3 = (-(avgx2 * normvec[0] + avgz2 * normvec[2] + plane))/normvec[1];
      float yy4 = (-(avgx2 * L->normvec[0] + avgz2 * L->normvec[2] + L->plane))/L->normvec[1];
      if (yy1<yy2 && yy3<yy4)
	return true;
      else return false;
    }
    else {
      if (maxAZ<minBZ){
	return true;
      }
      else if ((minAX>maxBX) || (maxAX<minBX) || (minAY>maxBY) || (maxAY<minBY) || (minAZ>maxBZ)){
	return false;
      }
      float avgx = (C1->x + C2->x + C3->x)/3;
      float avgy = (C1->y + C2->y + C3->y)/3;
      float avgx2 = (L->C1->x + L->C2->x + L->C3->x)/3;
      float avgy2 = (L->C1->y + L->C2->y + L->C3->y)/3;
      float zz1 = (-(avgx * normvec[0] + avgy * normvec[1] + plane))/normvec[2];
      float zz2 = (-(avgx * L->normvec[0] + avgy * L->normvec[1] + L->plane))/L->normvec[2];
      float zz3 = (-(avgx2 * normvec[0] + avgy2 * normvec[1] + plane))/normvec[2];
      float zz4 = (-(avgx2 * L->normvec[0] + avgy2 * L->normvec[1] + L->plane))/L->normvec[2];
      if (zz1<zz2 && zz3<zz4)
	return true;
      else return false;
    }
  }

  /* bool operator<(const Tri& L){
        float maxa, maxb;
        if (sort==0){
            maxa =C1->x;
            if (C2->x>maxa)
                maxa=C2->x;
            if (C3->x>maxa)
                maxa = C3->x;
            maxb =L.C1->x;
            if (L.C2->x>maxb)
                maxb=L.C2->x;
            if (L.C3->x>maxb)
                maxb = L.C3->x;
            if(maxa < maxb)
                return true;
            else if (maxa > maxb)
                return false;
            else return (std::abs(normvec[0]) < std::abs(L.normvec[0]));
            float avgy = (C1->y + C2->y + C3->y)/3;
            float avgz = (C1->z + C2->z + C3->z)/3;
            float xx1 = (-(avgy * normvec[1] + avgz * normvec[2] + plane))/normvec[0];
            float xx2 = (-(avgy * L.normvec[1] + avgz * L.normvec[2] + L.plane))/L.normvec[0];
            if (xx1<xx2)
                return true;
            else return false;
        }
        else if (sort==1){
            maxa =C1->y;
            if (C2->y>maxa)
                maxa=C2->y;
            if (C3->y>maxa)
                maxa = C3->y;
            maxb =L.C1->y;
            if (L.C2->y>maxb)
                maxb=L.C2->y;
            if (L.C3->y>maxb)
                maxb = L.C3->y;
            if(maxa < maxb)
                return true;
            else if (maxa > maxb)
                return false;
            else return (std::abs(normvec[1]) < std::abs(L.normvec[1]));
        }
        else {
            maxa =C1->z;
            if (C2->z>maxa)
                maxa=C2->z;
            if (C3->z>maxa)
                maxa = C3->z;
            maxb =L.C1->z;
            if (L.C2->z>maxb)
                maxb=L.C2->z;
            if (L.C3->z>maxb)
                maxb = L.C3->z;
            if(maxa < maxb)
                return true;
            else if (maxa > maxb)
                return false;
            else return (std::abs(normvec[2]) < std::abs(L.normvec[2]));
        }
    }
    bool operator>(const Tri& L){
    float maxa, maxb;
    if (sort==0){
    maxa =C1->x;
    if (C2->x>maxa)
    maxa=C2->x;
    if (C3->x>maxa)
    maxa = C3->x;
    maxb =L.C1->x;
    if (L.C2->x>maxb)
    maxb=L.C2->x;
    if (L.C3->x>maxb)
    maxb = L.C3->x;
    if(maxa > maxb)
    return true;
    else if (maxa < maxb)
    return false;
    else return (std::abs(normvec[0]) > std::abs(L.normvec[0]));
    float avgy = (C1->y + C2->y + C3->y)/3;
    float avgz = (C1->z + C2->z + C3->z)/3;
    float xx1 = (-(avgy * normvec[1] + avgz * normvec[2] + plane))/normvec[0];
    float xx2 = (-(avgy * L.normvec[1] + avgz * L.normvec[2] + L.plane))/L.normvec[0];
    if (xx1<xx2)
    return true;
    else return false;
    }
    else if (sort==1){
    maxa =C1->y;
    if (C2->y>maxa)
    maxa=C2->y;
    if (C3->y>maxa)
    maxa = C3->y;
    maxb =L.C1->y;
    if (L.C2->y>maxb)
    maxb=L.C2->y;
    if (L.C3->y>maxb)
    maxb = L.C3->y;
    if(maxa > maxb)
    return true;
    else if (maxa < maxb)
    return false;
    else return (std::abs(normvec[1]) > std::abs(L.normvec[1]));
    }
    else {
    maxa =C1->z;
    if (C2->z>maxa)
    maxa=C2->z;
    if (C3->z>maxa)
    maxa = C3->z;
    maxb =L.C1->z;
    if (L.C2->z>maxb)
    maxb=L.C2->z;
    if (L.C3->z>maxb)
    maxb = L.C3->z;
    if(maxa > maxb)
    return true;
    else if (maxa < maxb)
    return false;
    else return (std::abs(normvec[2]) > std::abs(L.normvec[2]));
    }
    }*/
};


#endif /* defined(__ECS175_Proj3__Coord__) */
