//
//  Plane.h
//  ECS175_Proj5
//
//  Created by Jacob Ley on 12/3/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#ifndef __ECS175_Proj5__Plane__
#define __ECS175_Proj5__Plane__

#include <stdio.h>

class plane{
public:
    plane(float *X);
    float ABC[3] , D, K[10], AB[3], BC[3], CA[3];
    float Point1[3], Point2[3], Point3[3];
    void AddPoints(float *X);
    void AddK(float *X);
    float NearHit(float Source[3], float Ray[3]);
    void getSource(float OldRay[3], float newNormal[3], float getK[10]);
};

#endif /* defined(__ECS175_Proj5__Plane__) */
