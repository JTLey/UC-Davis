//
//  Light.h
//  ECS175_Proj5
//
//  Created by Jacob Ley on 12/3/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#ifndef __ECS175_Proj5__Light__
#define __ECS175_Proj5__Light__

#include <stdio.h>

class light{
public:
    light(float *X);
    float Coord[3], Color[3];
};

#endif /* defined(__ECS175_Proj5__Light__) */
