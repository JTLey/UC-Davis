//
//  main.cpp
//  ECS175_Proj1
//
//  Created by Jacob Ley on 9/29/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "Line.h"
#include "Poly.h"
#include <fstream>
#include <string>
using namespace std;

float *PixelBuffer; // Declaring all global variables and functions
void displaywindow();
void displaydraw();
void rasterize();
void getcommand();
void GetFile();
void SaveFile();
void getcor(string point, int *x, int *y);
string filename;
int width, height, linenum, polynum;
bool first, BorD1, BorD2, BorD3, color=false; //0 is Bresenham, 1 is DDA;
polygon *Poly1, *Poly2, *Poly3;
line *line1, *line2, *line3;
int main(int argc, char *argv[])
{
  cout << "Height:"; // Take Height and Width inputs
  cin >> height;
  cout << "Width:";
  cin >> width;
  PixelBuffer = new float[height * width * 3];
  Poly1 = new polygon; // Create new Shapes
  Poly2 = new polygon;
  Poly3 = new polygon;
  line1 = new line;
  line2 = new line;
  line3 = new line;
  fill_n(PixelBuffer, height*width*3, 0); //Resets the Buffer to all 0s (black)
  linenum = 0; // Initially the number of lines is 0
  polynum=0; // Same with polygons
  filename = argv[1]; // Read in the load file
  Poly1->window(width , height); // Clip each shape to the window
  Poly2->window(width , height);
  Poly3->window(width , height);
  line1->window(width , height);
  line2->window(width , height);
  line3->window(width , height);
  glutInit(&argc, argv); 
  glutInitDisplayMode(GLUT_SINGLE);
  glutInitWindowSize(height, width);
  //set window position
  glutInitWindowPosition(100, 100);

  //create and set main window title
  glutCreateWindow("ECS175 Proj1 JLey");
  glClearColor(0, 0, 0, 0); //clears the buffer of OpenGL
  //sets display function
  glutDisplayFunc(displaydraw);
  glutIdleFunc(displaydraw);
  GetFile(); // Read in the given coordinates file
  first = true;
  glutMainLoop();//main display loop, will display until terminate
  return 0;
}

void GetFile(){
  string line, point1, point2;
  int num, vertex, x1, y1, x2, y2;
  ifstream myfile;
  myfile.open(filename); // Open the file
  if (myfile.is_open())
    {
      if ( getline (myfile,line) )
	{
	  num = stoi(line); //How many polygons?
	  for (int i=0; i<num; i++){
	    getline (myfile,line); // How many vertices
	    vertex = stoi(line);
	    if (polynum == 0){
	      Poly1->numvert=vertex;
	      for (int i=0; i<vertex; i++){
		getline(myfile,point1);
		getcor(point1, &x1, &y1); // Add each vertex to polygon
		Poly1->addvert(x1, y1, i);
	      }
	      polynum++; // Increase polynum (now it is 1)
	    }
	    else if (polynum == 1){ //Repeat if there is a second polyong
	      Poly2->numvert=vertex;
	      for (int i=0; i<vertex; i++){
		getline(myfile,point1);
		getcor(point1, &x1, &y1);
		Poly2->addvert(x1, y1, i);
	      }
	      polynum++;
	    }
	    else{ // Repeat again for a third
	      Poly3->numvert=vertex;
	      for (int i=0; i<vertex; i++){
		getline(myfile,point1);
		getcor(point1, &x1, &y1);
		Poly3->addvert(x1, y1, i);
	      }
	    }
	  }
	}
      if (getline (myfile,line) ){
	num = stoi(line); //How many lines are there
	for (int i=0; i<num; i++){
	  getline (myfile,line);
	  vertex = stoi(line); // Is it DDA or Bresenham?
	  getline (myfile,point1);
	  getline (myfile,point2); // Take in the two points
	  getcor(point1, &x1, &y1);
	  getcor(point2, &x2, &y2);
	  if (linenum==0){
	    line1->x1 = x1;  //Add the two points to line 1
	    line1->x2 = x2;
	    line1->y1 = y1;
	    line1->y2 = y2;
	    BorD1=vertex; // Declare if it is DDA or Bresenham
	    linenum++; //Increase the number of lines (now 1)
	  }
	  else if (linenum == 1){ //repeat if there is a second line
	    line2->x1 = x1;
	    line2->x2 = x2;
	    line2->y1 = y1;
	    line2->y2 = y2;
	    BorD2=vertex;
	    linenum++;
	  }
	  else { //Repeat again for a third
	    line3->x1 = x1;
	    line3->x2 = x2;
	    line3->y1 = y1;
	    line3->y2 = y2;
	    BorD3=vertex;
	  }
	}
      }
      myfile.close(); //Close file
    }
  else cout << "File did not open\n";

}

void SaveFile(){
  ofstream myfile (filename);
  int a, b, n, x, y;
  if (myfile.is_open()) //Open the coordinates file for writing
    {
      myfile << polynum << "\n"; //Declare how many polygons
      for (a =0; a<polynum; a++){
	if (a==0){
	  n=Poly1->getvertnum(); // How many vertices in this polygon
	  myfile << n << "\n";
	  for (b=0; b<n; b++){
	    Poly1->getverts(&x, &y, b); // Return the values of each vertex
	    myfile << "(" << x << "," << y << ")\n";
	  }
	}
	else if (a==1){ //Repeat
	  n=Poly2->getvertnum();
	  myfile << n << "\n";
	  for (b=0; b<n; b++){
	    Poly2->getverts(&x, &y, b);
	    myfile << "(" << x << "," << y << ")\n";
	  }
	}
	else if (a==2){ //Repeat
	  n=Poly3->getvertnum();
	  myfile << n << "\n";
	  for (b=0; b<n; b++){
	    Poly3->getverts(&x, &y, b);
	    myfile << "(" << x << "," << y << ")\n";
	  }
	}
      }
      myfile << linenum << "\n"; //How many lines?
      for (a=0; a<linenum; a++){
	if (a==0){
	  if (BorD1){ //Declare if it is DDA or Bresenham
	    myfile << 1 << "\n";
	  }
	  else myfile << 0 << "\n"; //Get each coordinate
	  myfile << "(" << line1->x1 << "," << line1->y1 << ")\n";
	  myfile << "(" << line1->x2 << "," << line1->y2 << ")\n";
	}
	if (a==1){ //Repeat
	  if (BorD2){
	    myfile << 1 << "\n";
	  }
	  else myfile << 0 << "\n";
	  myfile << "(" << line2->x1 << "," << line2->y1 << ")\n";
	  myfile << "(" << line2->x2 << "," << line2->y2 << ")\n";
	}
	if (a==2){ //Repeat
	  if (BorD3){
	    myfile << 1 << "\n";
	  }
	  else myfile << 0 << "\n";
	  myfile << "(" << line3->x1 << "," << line3->y1 << ")\n";
	  myfile << "(" << line3->x2 << "," << line3->y2 << ")\n";
	}
      }
      myfile.close();
    }
  else cout << "Unable to open file";
}

void displaydraw(){

  if (first==false)
    cout << ">>";

  fill_n(PixelBuffer, height*width*3, 0); // Clear the buffer
  if (first==false)
    getcommand(); //Take input from user
  else first = false;

  if (polynum>0)
    Poly1->draw(PixelBuffer, 0);
  if (polynum>1)
    Poly2->draw(PixelBuffer, 1);
  if (polynum>2)
    Poly3->draw(PixelBuffer, 2); // Only draw polygons that have been declared
  rasterize(); //Color in each polygon

  if (linenum>0) //Draw all declared lines
    line1->draw(PixelBuffer, BorD1);
  if (linenum>1)
    line2->draw(PixelBuffer, BorD2);
  if (linenum>2)
    line3->draw(PixelBuffer, BorD3);

  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  //draws pixel on screen, width and height must match pixel buffer dimension
  glDrawPixels(width, height, GL_RGB, GL_FLOAT, PixelBuffer);
  //glutReshapeWindow(width,height);
  //window refresh

  glEnd();
  glFlush();
}

void getcommand(){
  string command, identify, point1, point2, input;
  int x1, x2, y1, y2, vertex;
  float scal;
  command = identify = point1 = point2 =  "";
  cin >> command; //Take in the command and compare to possible inputs
  if (command == "Bresenham"){
    cin >> point1 >> point2; //Drawing a line, take in two points
    getcor(point1, &x1, &y1);
    getcor(point2, &x2, &y2);
    if (linenum==0){ //Assign the points to the correct line
      line1->x1 = x1;
      line1->x2 = x2;
      line1->y1 = y1;
      line1->y2 = y2;
      BorD1=0; //Bresenham, so this is 0 (1 if DDA)
      linenum++;
    }
    else if (linenum == 1){
      line2->x1 = x1;
      line2->x2 = x2;
      line2->y1 = y1;
      line2->y2 = y2;
      BorD2=0;
      linenum++;
    }
    else {
      line3->x1 = x1;
      line3->x2 = x2;
      line3->y1 = y1;
      line3->y2 = y2;
      BorD3=0;
    }
  }
  else if (command == "DDA"){
    cin >> point1 >> point2;
    getcor(point1, &x1, &y1);
    getcor(point2, &x2, &y2);
    if (linenum==0){
      line1->x1 = x1;
      line1->x2 = x2;
      line1->y1 = y1;
      line1->y2 = y2;
      BorD1=1; //Same as Bresenham, just now it is 1
      linenum++;
    }
    else if (linenum == 1){
      line2->x1 = x1;
      line2->x2 = x2;
      line2->y1 = y1;
      line2->y2 = y2;
      BorD2=1;
      linenum++;
    }
    else {
      line3->x1 = x1;
      line3->x2 = x2;
      line3->y1 = y1;
      line3->y2 = y2;
      BorD3=1;
    }
  }
  else if (command == "Polygon"){
    cin >> vertex; //Take in the number of vertices
    if (polynum == 0){ //Choose the correct polygon to edit
      Poly1->numvert=vertex;
      for (int i=0; i<vertex; i++){
	cin >> point1;
	getcor(point1, &x1, &y1);
	cout << x1 << y1; // Add each vertex to the polygon
	Poly1->addvert(x1, y1, i);
      }
    }
    else if (polynum == 1){
      Poly2->numvert=vertex;
      for (int i=0; i<vertex; i++){
	cin >> point1;
	getcor(point1, &x1, &y1);
	Poly2->addvert(x1, y1, i);
      }
    }
    else{
      Poly3->numvert=vertex;
      for (int i=0; i<vertex; i++){
	cin >> point1;
	getcor(point1, &x1, &y1);
	Poly3->addvert(x1, y1, i);
      }
    }
    polynum++;
  }
  else if (command == "Translate"){
    cin >> vertex; // Which polygon to edit
    if (vertex<=polynum){
      cin >> point1; //Take in translation vector
      getcor(point1, &x1, &y1);
      if (vertex == 1) //Call correct polygons functin
	Poly1->tran(x1, y1);
      else if (vertex == 2)
	Poly2->tran(x1, y1);
      else if (vertex == 3)
	Poly3->tran(x1, y1);
      else cout << "Out of Range\n";
    }
    else cout << "Out of Range\n";
  }
  else if (command == "Rotate"){
    cin >> vertex; //Which polygon to edit
    if (vertex<=polynum){
      cin >> scal; //By how much?
      if (vertex == 1)
	Poly1->rot(scal); //Call correct polygons function
      else if (vertex == 2)
	Poly2->rot(scal);
      else if (vertex == 3)
	Poly3->rot(scal);
      else cout << "Out of Range\n";
    }
    else cout << "Out of Range\n";
  }
  else if (command == "ScaleX"){
    cin >> vertex;// Which polygon to edit
    if (vertex<=polynum){
      cin >> scal; //By how much?
      if (scal==0) 
	cout << "Must be non-zero\n";
      else if (vertex == 1)
	Poly1->scaleX(scal); //Call the correct polygons function
      else if (vertex == 2)
	Poly2->scaleX(scal);
      else if (vertex == 3)
	Poly3->scaleX(scal);
      else cout << "Out of Range\n";
    }
    else cout << "Out of Range\n";
  }
  else if (command == "ScaleY"){ //Same as ScaleX
    cin >> vertex;
    if (vertex<=polynum){
      cin >> scal;
      if (vertex == 1)
	Poly1->scaleY(scal);
      else if (vertex == 2)
	Poly2->scaleY(scal);
      else if (vertex == 3)
	Poly3->scaleY(scal);
      else cout << "Out of Range\n";
    }
    else cout << "Out of Range\n";
  }
  else if (command == "Clip"){
    cin >> vertex >> x1 >> x2 >> y1 >> y2; //Take in the polygon #, and window specs
    if (x1<0)
      x1=0;
    if (x2>999)
      x2=999; //Ensure that bad input is fixed
    if (y1<0)
      y1=0;
    if (y2>999)
      y2=999;
    if ((vertex == 1) && (polynum >0)) //Call the correct polygons function
      Poly1->clip(x1, x2, y1, y2);
    else if ((vertex == 2) && (polynum >1))
      Poly2->clip(x1, x2, y1, y2);
    else if ((vertex == 3) && (polynum >2))
      Poly3->clip(x1, x2, y1, y2);
    else if ((vertex == 4) && (linenum >0))
      line1->clip(x1, x2, y1, y2);
    else if ((vertex == 5) && (linenum >1))
      line2->clip(x1, x2, y1, y2);
    else if ((vertex == 6) && (linenum >2))
      line3->clip(x1, x2, y1, y2);
    else cout << "Out of Range\n";
  }
  else if (command == "Save"){
    SaveFile();
  }

  else if (command == "Color"){
    cin >> point1;
    if (point1 == "ON"){ //Turn coloring on
      color = true;
    }
    else color = false;
  }

  else
    cout << "Please refer to README for proper input\n";
}

void getcor(string point, int *x, int *y){
  point.erase(point.begin()); //Input is string (x,y)
  point.erase(point.end()-1); //remove ()
  size_t pos = point.find(",");
  *x = atoi((point.substr(0,pos)).c_str()); //remove ,
  point.erase(0 , pos+1); // take in x and y values
  *y = atoi(point.c_str());
}

void rasterize(){
  bool in1, in2, in3;
  int x, y;
  for (y=0;y<height;y++){ //start at the bottom, work up
    in1=in2=in3=false; //By default the lines should not be filled in
    for (x=width-1;x>=0;x--){ //Go right to left
      //The first polygon only colors in R pixels, so rasterizations occur indpendantly, then are all combined at the end
      
      if(PixelBuffer[(y*width+x)*3]==1){ //Point is filled in

	if ((Poly1->staycolor(x,y))==2){ //Vertex with horizontal line
	  if ((y>0) && (PixelBuffer[((y-1)*width+x+1)*3]==1)){
	    in1=true; //Check whether filling in should occur, judged by lower points
	  }
	  else in1=false;
	}

	else if ((Poly1->staycolor(x,y))==3){ //Another horizontal line, but the other line goes up
	  if (PixelBuffer[((y-1)*width+(x+3))*3]==1)
	    in1=false;
	  else in1=true;
	}


	else if ((Poly1->staycolor(x,y))==-1){ // Vertex that in not max or min
	  if (x>0){
	    if (PixelBuffer[(y*width+x-1)*3]==0){ //Checking if in shift will occur without this function
	      in1=!in1;
	    }
	  }
	}
	else if ((Poly1->staycolor(x,y))==1){ //Vertex is local min or max
	  if (PixelBuffer[(y*width+x-1)*3]!=0){ //Ensure no lines are on the same level
	    in1=!in1;
	  }
	}
	else{ //Otherwise just flip the coloring
	  in1=!in1;
	}
      }
      else if (PixelBuffer[(y*width+x)*3]==.5){ //Caused by lines that move more horizontally than vertically, so as not to cause double in flip.
	PixelBuffer[(y*width+x)*3]=1;
      }
      else if (in1) //Color in these coordinates
	PixelBuffer[(y*width+x)*3]=1;

      ////////////////////////

      if(PixelBuffer[(y*width+x)*3+1]==1){ //Repeat for the second polygon

	if ((Poly2->staycolor(x,y))==2){
	  if ((y>0) && (PixelBuffer[((y-1)*width+x)*3+1]==1)){
	    in2=true;
	  }
	  else in2=false;
	}

	else if ((Poly2->staycolor(x,y))==3){
	  if (PixelBuffer[((y-1)*width+(x+3))*3+1]==1)
	    in2=false;
	  else in2=true;
	}


	else if ((Poly2->staycolor(x,y))==-1){
	  if (x>0){
	    if (PixelBuffer[(y*width+x-1)*3+1]==0){
	      in2=!in2;
	    }
	  }
	}
	else if ((Poly2->staycolor(x,y))==1){
	  if (PixelBuffer[(y*width+x-1)*3+1]!=0){
	    in2=!in2;
	  }
	}
	else{
	  in2=!in2;
	}
      }
      else if (PixelBuffer[(y*width+x)*3+1]==.5){
	PixelBuffer[(y*width+x)*3+1]=1;
      }
      else if (in2)
	PixelBuffer[(y*width+x)*3+1]=1;

      //////////////////////// 

      if(PixelBuffer[(y*width+x)*3+2]==1){ //Repeat for the third polygon

	if ((Poly3->staycolor(x,y))==2){
	  if ((y>0) && (PixelBuffer[((y-1)*width+x)*3+2]==1)){
	    in3=true;
	  }
	  else in3=false;
	}

	else if ((Poly3->staycolor(x,y))==3){
	  if (PixelBuffer[((y-1)*width+(x+3))*3+2]==1)
	    in3=false;
	  else in3=true;
	}


	else if ((Poly3->staycolor(x,y))==-1){
	  if (x>0){
	    if (PixelBuffer[(y*width+x-1)*3+2]==0){
	      in3=!in3;
	    }
	  }
	}
	else if ((Poly3->staycolor(x,y))==1){
	  if (PixelBuffer[(y*width+x-1)*3+2]!=0){
	    in3=!in3;
	  }
	}
	else{
	  in3=!in3;
	}
      }
      else if (PixelBuffer[(y*width+x)*3+2]==.5){
	PixelBuffer[(y*width+x)*3+2]=1;
      }
      else if (in3)
	PixelBuffer[(y*width+x)*3+2]=1;

    }
  }
  if (!color){ //By default the rasterization is colored. To make all white
    for (y=0;y<height;y++){ //Just scan again and set all colored pixels to white
      for (x=0;x<width;x++){
	if ((PixelBuffer[(y*width+x)*3]==1)||(PixelBuffer[(y*width+x)*3+1]==1)||(PixelBuffer[(y*width+x)*3+2]==1)){
	  PixelBuffer[(y*width+x)*3]=1;
	  PixelBuffer[(y*width+x)*3+1]=1;
	  PixelBuffer[(y*width+x)*3+2]=1;
	}
      }
    }
  }
}
