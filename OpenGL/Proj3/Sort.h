//
//  Sort.h
//  ECS175_Proj3
//
//  Created by Jacob Ley on 11/2/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#ifndef __ECS175_Proj3__Sort__
#define __ECS175_Proj3__Sort__

#include <stdio.h>
#include "Coord.h"

// BinaryHeap class
//
// CONSTRUCTION: with a negative infinity sentinel and
//               optional capacity (that defaults to 100)
//
// ******************PUBLIC OPERATIONS*********************
// void insert( x )       --> Insert x
// deleteMax( maxItem )   --> Remove (and optionally return) smallest item
// bool isEmpty( )        --> Return true if empty; else false
// void makeEmpty( )      --> Remove all items
// ******************ERRORS********************************
// Throws Underflow and Overflow as warranted

class TriHeap
{
 public:
  explicit TriHeap();
  bool isEmpty();
  void insert( Tri *x );
  Tri* deleteMax( );
  void makeEmpty( );
  void sort();

 private:
  int currentSize;  // Number of elements in heap
  Tri* array[100];// The heap array
  bool returned[100];
};

#endif /* defined(__ECS175_Proj3__Sort__) */
