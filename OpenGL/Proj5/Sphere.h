//
//  Sphere.h
//  ECS175_Proj5
//
//  Created by Jacob Ley on 12/3/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#ifndef __ECS175_Proj5__Sphere__
#define __ECS175_Proj5__Sphere__

#include <stdio.h>

class sphere{
public:
    sphere(float *X);
    float Coord[3], Ellipse[3], K[10];
    void AddK(float *X);
    float NearHit(float Source[3], float Ray[3]);
    void getSource(float Ray[3], float newSource[3], float newNormal[3], float GetK[10]);
};

#endif /* defined(__ECS175_Proj5__Sphere__) */
