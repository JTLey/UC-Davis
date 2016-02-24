//
//  Light.cpp
//  ECS175_Proj5
//
//  Created by Jacob Ley on 12/3/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#include "Light.h"

light::light(float *X){
    Coord[0] = X[0];
    Coord[1] = X[1];
    Coord[2] = X[2];
    
    Color[0] = X[3];
    Color[1] = X[4];
    Color[2] = X[5];
}