//
//  Sort.cpp
//  ECS175_Proj3
//
//  Created by Jacob Ley on 11/2/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#include "Sort.h"
#include <iostream>
using namespace std;

/**
 * Construct the binary heap.
 * capacity is the capacity of the binary heap.
 */
TriHeap::TriHeap()
  : currentSize(0)
{
}

/**
 * Insert item x into the priority queue, maintaining heap order.
 * Duplicates are allowed.
 * Throw Overflow if container is full.
 */
void TriHeap::insert( Tri* x )
{

  // Percolate up
  array[currentSize]=x;
  returned[currentSize]=false;
  currentSize++;
}

bool TriHeap::isEmpty(){
  for (int i=0; i<currentSize; i++){
    if (!returned[i])
      return false;
  }
  return true;
}

/**
 * Remove the smallest item from the priority queue.
 * Throw Underflow if empty.
 */
void TriHeap::sort(){
  for(int i=0; i<currentSize; i++){
    returned[i]=false;
    array[i]->render=0;
  }
  for(int i=0; i<currentSize; i++){
    for (int j=0; j<currentSize; j++){
      if(i!=j){
	if (array[i]->order(array[j])){
	  array[i]->renderfirst[array[i]->render]=j;
	  array[i]->render+=1;
	}
      }
    }
  }
}

Tri* TriHeap::deleteMax( )
{
  bool found;
  for (int i=0; i<currentSize; i++){
    found=true;
    if (returned[i])
      continue;
    for (int j=0; j<array[i]->render; j++){
      if(!returned[array[i]->renderfirst[j]]){
	found=false;
	j+=100;
      }
      else found=true;
    }
    if(found){
      returned[i]=true;
      return array[i];
    }
  }
  makeEmpty();
  return array[0];
}

/**
 * Make the priority queue logically empty.
 */
void TriHeap::makeEmpty( )
{
  currentSize = 0;
}

/**
 * Internal method to percolate down in the heap.
 * hole is the index at which the percolate begins.
 */
