//
//  main.cpp
//  ECS 175 Proj4
//
//  Created by Jacob Ley on 11/15/15.
//  Copyright (c) 2015 Jacob Ley. All rights reserved.
//

#include <iostream>
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/glut.h>
#  include <GL/freeglut.h>
#endif
#include <fstream>
#include <cstring>
#include <cmath>
#include "Bezier.h"
#include <sstream>

using namespace std;

int MainWindow, numBez=0, numBspline=0, menvalue=0, oldvalue, id_menu[350], Bez_Menu, BSpline_Menu, Bez_Insert, Spline_Insert, knot=0; //Main variables
Bezier* Bez[100]; //Contains all Beziers
BSpline* Spline[100]; //Contains all BSplines
int Menu_id, Bez_point_id, Del_Bez_id, Del_Bez_point_id, Move_Bez_id, Res_Bez_id, Knot_Spline_id; //Menu ids
int Spline_point_id, Del_Spline_id, Del_Spline_point_id, Move_Spline_id, Res_Spline_id, K_Spline_id;
float MasterMin, MasterMax, scale; //For NDC
float deltax = 0, deltay = 0; //Keyboard and Mouse
int xOrigin = -1, yOrigin = -1;
bool UP, DOWN, UP2, DOWN2, RIGHT, RIGHT2, LEFT, LEFT2, menuchange, push=false, ToggleControl=false;
char s[100]; //Printing output

void load(){ // Read the Text File
    string line;
    ifstream myfile("2dcoordinates.txt");
    int n, k;
    Bezier* B;
    BSpline* BS;
    float x,y;
    size_t pos;
    if (myfile.is_open()){
        getline(myfile,line);
        numBez = stoi(line); // Number of Bezier
        for (int i=0; i<numBez; i++){
            getline(myfile,line);
            n=stoi(line);   //Number of Points
            getline(myfile,line);
            B = new Bezier(stoi(line)); //Resolution
            Bez[i] = B;
            for (int j=0; j<n; j++){
                getline(myfile,line);
                pos = line.find(" ");   //Each Point
                x = stof(line.substr(0,pos));
                line.erase(0, pos+1);
                y = stof(line);
                B->AddPoint(x, y);
            }
        }
        getline(myfile,line);
        numBspline = stoi(line); //Number of BSpline
        for (int i=0; i<numBspline; i++){
            getline(myfile,line);
            n=stoi(line);   //Number of Points
            getline(myfile,line);
            k = stoi(line); //K value
            getline(myfile,line);
            BS = new BSpline(stoi(line), k);    //Resolution
            getline(myfile,line);
            for (int j=0; j<99; j++){
                pos = line.find(" ");
                BS->U[j] = stof(line.substr(0,pos));
                line.erase(0, pos+1);
            }
            BS->U[99]=stof(line);
            Spline[i] = BS;
            for (int j=0; j<n; j++){
                getline(myfile,line);   //Each Point
                pos = line.find(" ");
                x = stof(line.substr(0,pos));
                line.erase(0, pos+1);
                y = stof(line);
                BS->AddPoint(x, y);
            }
        }
    }
    myfile.close();
}


void save(){    //Writes the file, same set up as load
    Bezier* B;
    BSpline* BS;
    ofstream myfile ("2dcoordinates.txt");
    float x,y;
    if (myfile.is_open()){
        myfile<<numBez<<"\n";
        for (int i=0; i<numBez; i++){
            B = Bez[i];
            myfile << B->numPoint << "\n";
            myfile << B->Resolution << "\n";
            for (int j=0; j<B->numPoint; j++){
                B->FindPoint(j, &x, &y);
                myfile << x << " " << y << "\n";
            }
        }
        myfile<<numBspline<<"\n";
        for (int i=0; i<numBspline; i++){
            BS= Spline[i];
            myfile << BS->numPoint << "\n";
            myfile << BS->k << "\n";
            myfile << BS->Resolution << "\n";
            for (int j=0; j<100; j++)
                myfile << BS->U[j] << " ";
            myfile << "\n";
            for (int j=0; j<BS->numPoint; j++){
                BS->FindPoint(j, &x, &y);
                myfile << x << " " << y << "\n";
            }
        }
    }
    myfile.close();
}

void menu(int num){ //Called from the menu, sets value of menvalue
    if(num == -1){
        save();
        glutDestroyWindow(MainWindow);
        exit(0);
    }else{
        oldvalue = menvalue;
        menvalue=num;
    }
    glutPostRedisplay();
}

void createmenu(){ //Create the popup menu
    string line;
    ostringstream oss;
    char cLine[100];
    
    // MOVE BEZIER POINT
    for (int i=0; i<numBez; i++){
        id_menu[i] = glutCreateMenu(menu);
        for (int j=0; j<Bez[i]->numPoint; j++){
            line = "Point ";
            oss.str("");
            oss.clear();
            oss << j+1;
            line += oss.str();
            strcpy(cLine,line.c_str());
            glutAddMenuEntry(cLine, i*50+j);
        }
    }
    Move_Bez_id = glutCreateMenu(menu);
    for (int i=0; i<numBez; i++){
        line = "Bezier ";
        oss.str("");
        oss.clear();
        oss << i+1;
        line += oss.str();
        strcpy(cLine,line.c_str());
        glutAddSubMenu(cLine, id_menu[i]);
    }
    
    // ADD BEZIER POINT
    Bez_point_id = glutCreateMenu(menu);
    for (int i=0; i<numBez; i++){
        line = "Bezier ";
        oss.str("");
        oss.clear();
        oss << i+1;
        line += oss.str();
        strcpy(cLine,line.c_str());
        glutAddMenuEntry(cLine, 2500 + i);
    }
    
    // DELETE BEZIER
    Del_Bez_id = glutCreateMenu(menu);
    for (int i=0; i<numBez; i++){
        line = "Bezier ";
        oss.str("");
        oss.clear();
        oss << i+1;
        line += oss.str();
        strcpy(cLine,line.c_str());
        glutAddMenuEntry(cLine, 2550 + i);
    }
    
    // DELETE BEZIER POINT
    for (int i=0; i<numBez; i++){
        id_menu[i+50] = glutCreateMenu(menu);
        for (int j=0; j<Bez[i]->numPoint; j++){
            line = "Point ";
            oss.str("");
            oss.clear();
            oss << j+1;
            line += oss.str();
            strcpy(cLine,line.c_str());
            glutAddMenuEntry(cLine, 2600 + i*50 + j);
        }
    }
    Del_Bez_point_id = glutCreateMenu(menu);
    for (int i=0; i<numBez; i++){
        line = "Bezier ";
        oss.str("");
        oss.clear();
        oss << i+1;
        line += oss.str();
        strcpy(cLine,line.c_str());
        glutAddSubMenu(cLine, id_menu[i+50]);
    }
    
    //BEZIER RESOLUTION
    Res_Bez_id = glutCreateMenu(menu);
    for (int i=0; i<numBez; i++){
        line = "Bezier ";
        oss.str("");
        oss.clear();
        oss << i+1;
        line += oss.str();
        strcpy(cLine,line.c_str());
        glutAddMenuEntry(cLine, 5100 + i);
    }
    
    //ADD BSPLINE POINT
    Spline_point_id = glutCreateMenu(menu);
    for (int i=0; i<numBspline; i++){
        line = "BSpline ";
        oss.str("");
        oss.clear();
        oss << i+1;
        line += oss.str();
        strcpy(cLine,line.c_str());
        glutAddMenuEntry(cLine, 5150 + i);
    }
    
    //DELETE BSPLINE POINT
    for (int i=0; i<numBspline; i++){
        id_menu[i+100] = glutCreateMenu(menu);
        for (int j=0; j<Spline[i]->numPoint; j++){
            line = "Point ";
            oss.str("");
            oss.clear();
            oss << j+1;
            line += oss.str();
            strcpy(cLine,line.c_str());
            glutAddMenuEntry(cLine, 5200 + i*50 + j);
        }
    }
    Del_Spline_point_id = glutCreateMenu(menu);
    for (int i=0; i<numBspline; i++){
        line = "BSpline ";
        oss.str("");
        oss.clear();
        oss << i+1;
        line += oss.str();
        strcpy(cLine,line.c_str());
        glutAddSubMenu(cLine, id_menu[i+100]);
    }
    
    //DELETE BSPLINE
    Del_Spline_id = glutCreateMenu(menu);
    for (int i=0; i<numBspline; i++){
        line = "BSpline ";
        oss.str("");
        oss.clear();
        oss << i+1;
        line += oss.str();
        strcpy(cLine,line.c_str());
        glutAddMenuEntry(cLine, 7700 + i);
    }
    
    //MOVE BSPLINE POINT
    for (int i=0; i<numBspline; i++){
        id_menu[i+150] = glutCreateMenu(menu);
        for (int j=0; j<Spline[i]->numPoint; j++){
            line = "Point ";
            oss.str("");
            oss.clear();
            oss << j+1;
            line += oss.str();
            strcpy(cLine,line.c_str());
            glutAddMenuEntry(cLine, 7750 + i*50+j);
        }
    }
    Move_Spline_id = glutCreateMenu(menu);
    for (int i=0; i<numBspline; i++){
        line = "BSpline ";
        oss.str("");
        oss.clear();
        oss << i+1;
        line += oss.str();
        strcpy(cLine,line.c_str());
        glutAddSubMenu(cLine, id_menu[i+150]);
    }
    
    //BSPLINE RESOLUTION
    Res_Spline_id = glutCreateMenu(menu);
    for (int i=0; i<numBspline; i++){
        line = "BSpline ";
        oss.str("");
        oss.clear();
        oss << i+1;
        line += oss.str();
        strcpy(cLine,line.c_str());
        glutAddMenuEntry(cLine, 10250 + i);
    }
    
    //BSPLINE K
    K_Spline_id = glutCreateMenu(menu);
    for (int i=0; i<numBspline; i++){
        line = "BSpline ";
        oss.str("");
        oss.clear();
        oss << i+1;
        line += oss.str();
        strcpy(cLine,line.c_str());
        glutAddMenuEntry(cLine, 10300 + i);
    }
    
    //BEZIER INSERT POINT
    for (int i=0; i<numBez; i++){
        id_menu[i+200] = glutCreateMenu(menu);
        for (int j=0; j<Bez[i]->numPoint; j++){
            line = "Point ";
            oss.str("");
            oss.clear();
            oss << j+1;
            line += oss.str();
            strcpy(cLine,line.c_str());
            glutAddMenuEntry(cLine, 10350 + i*50+j);
        }
    }
    Bez_Insert = glutCreateMenu(menu);
    for (int i=0; i<numBez; i++){
        line = "Bezier ";
        oss.str("");
        oss.clear();
        oss << i+1;
        line += oss.str();
        strcpy(cLine,line.c_str());
        glutAddSubMenu(cLine, id_menu[i+200]);
    }
    
    //BSPLINE INSERT POINT
    for (int i=0; i<numBspline; i++){
        id_menu[i+250] = glutCreateMenu(menu);
        for (int j=0; j<Spline[i]->numPoint; j++){
            line = "Point ";
            oss.str("");
            oss.clear();
            oss << j+1;
            line += oss.str();
            strcpy(cLine,line.c_str());
            glutAddMenuEntry(cLine, 12850 + i*50+j);
        }
    }
    Spline_Insert = glutCreateMenu(menu);
    for (int i=0; i<numBspline; i++){
        line = "BSpline ";
        oss.str("");
        oss.clear();
        oss << i+1;
        line += oss.str();
        strcpy(cLine,line.c_str());
        glutAddSubMenu(cLine, id_menu[i+250]);
    }
    
    //BSPLINE KNOT
    for (int i=0; i<numBspline; i++){
        id_menu[i+300] = glutCreateMenu(menu);
        for (int j=0; j<3; j++){
            if (j==0)
                line = "Adjust Knot by .01";
            else if (j==1)
                line = "Adjust Knot by .1";
            else if (j==2)
                line = "Adjust Knot by 1";
            strcpy(cLine,line.c_str());
            glutAddMenuEntry(cLine, 15350 + i*3+j);
        }
    }
    Knot_Spline_id = glutCreateMenu(menu);
    for (int i=0; i<numBspline; i++){
        line = "BSpline ";
        oss.str("");
        oss.clear();
        oss << i+1;
        line += oss.str();
        strcpy(cLine,line.c_str());
        glutAddSubMenu(cLine, id_menu[i+300]);
    }
    
    Bez_Menu= glutCreateMenu(menu);
    glutAddMenuEntry("Add Bezier", -10);
    if (numBez>0){
        glutAddSubMenu("Add Bezier Point", Bez_point_id);
        glutAddSubMenu("Insert Bezier Point", Bez_Insert);
        glutAddSubMenu("Delete Bezier", Del_Bez_id);
        glutAddSubMenu("Delete Bezier Point", Del_Bez_point_id);
        glutAddSubMenu("Move Bezier Point", Move_Bez_id);
        glutAddSubMenu("Adjust Bezier Resolution", Res_Bez_id);
    }
    
    BSpline_Menu = glutCreateMenu(menu);
    glutAddMenuEntry("Add BSpline", -20);
    if (numBspline>0){
        glutAddSubMenu("Add BSpline Point", Spline_point_id);
        glutAddSubMenu("Insert BSpline Point", Spline_Insert);
        glutAddSubMenu("Delete BSpline", Del_Spline_id);
        glutAddSubMenu("Delete BSpline Point", Del_Spline_point_id);
        glutAddSubMenu("Move BSpline Point", Move_Spline_id);
        glutAddSubMenu("Adjust BSpline Resolution", Res_Spline_id);
        glutAddSubMenu("Adjust BSpline K value", K_Spline_id);
        glutAddSubMenu("Adjust BSpline Knots", Knot_Spline_id);
    }
    
    Menu_id = glutCreateMenu(menu);
    glutAddSubMenu("Bezier", Bez_Menu);
    glutAddSubMenu("BSpline", BSpline_Menu);
    glutAddMenuEntry("Toggle Control Polygons", -2);
    glutAddMenuEntry("Exit", -1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void AddBez(){ //Add a bezier curve, determined by menvalue
    Bezier* B = new Bezier(25);
    B->AddPoint(MasterMin+(MasterMax-MasterMin)/3, MasterMin+(MasterMax-MasterMin)/3);
    B->AddPoint(MasterMin+(MasterMax-MasterMin)/2, MasterMin+2*(MasterMax-MasterMin)/3);
    B->AddPoint(MasterMin+2*(MasterMax-MasterMin)/3, MasterMin+(MasterMax-MasterMin)/3);
    Bez[numBez] = B;
    menvalue = numBez*50;
    numBez++;
}

void mouseMove(int x, int y) { //Used to move objects, scaled to screen size
    
    // this will only be true when the left button is down
    if (push) {
        
        // update deltas
        deltax = (x - xOrigin) / (scale * 250);
        deltay = (yOrigin - y) / (scale * 250);
        xOrigin=x;
        yOrigin=y;
        glutPostRedisplay();
    }
    else {
        deltax=0;
        deltay=0;
    }
}

void mouseButton(int button, int state, int x, int y) { //Decide if mouse button is pressed
    
    // only start motion if the left button is pressed
    if (button == GLUT_LEFT_BUTTON) {
        
        // when the button is released
        if (state == GLUT_UP) {
            push=false;
            deltax=0;
            deltay=0;
        }
        else  {// state = GLUT_DOWN
            push=true;
            xOrigin = x;
            yOrigin = y;
        }
    }
}

void pressKey(int key, int xx, int yy) { //Decide if arrow keys are pressed
    switch (key) {
        case GLUT_KEY_UP : UP = true; break;
        case GLUT_KEY_DOWN : DOWN = true; break;
        case GLUT_KEY_RIGHT : RIGHT = true; break; //Key inputs for animated movements
        case GLUT_KEY_LEFT : LEFT = true; break;
    }
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void releaseKey(int key, int xx, int yy) { // Clear values when arrow keys are released
    
    switch (key) {
        case GLUT_KEY_UP : UP = false; break;
        case GLUT_KEY_DOWN : DOWN = false; break;
        case GLUT_KEY_RIGHT : RIGHT = false; break; //Cancels the animated movements
        case GLUT_KEY_LEFT : LEFT = false; break;
    }
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void MoveBez(){ //Gives mouse control over a certain point
    if ( UP2 == false && UP == true && (menvalue/50)<(numBez-1))
        menvalue = menvalue +50 - menvalue%50;
    else if ( DOWN2 == false && DOWN == true && (menvalue/50)>0)
        menvalue = menvalue -50 - menvalue%50;
    else if ( RIGHT2 == false && RIGHT == true && (menvalue%50)<(Bez[menvalue/50]->numPoint-1))
        menvalue+=1;
    else if ( LEFT2 == false && LEFT == true && (menvalue%50)>0)
        menvalue-=1;
    Bez[menvalue/50]->MovePoint(menvalue%50, deltax, deltay);
    float x, y;
    Bez[menvalue/50]->FindPoint(menvalue%50, &x, &y);
    sprintf(s,"Bezier %d : Point %d : x = %4.3f ; y = %4.3f", //Prints point and coordinates
            menvalue/50+1, menvalue%50+1, x, y);
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(-.9, .9);
    for (char *c = s; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    x = (x - MasterMin) * scale - 1;
    y = (y - MasterMin) * scale - 1;
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex2f(x - .01, y - .01);
    glVertex2f(x + .01, y + .01);   //Places a small X over point
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x - .01, y + .01);
    glVertex2f(x + .01, y - .01);
    glEnd();
}

void AddBezPoint(){ //Append a point to the end of a curve
    int i = menvalue - 2500;
    menvalue = (i*50 + Bez[i]->numPoint);
    Bez[i]->AddPoint(MasterMin+(MasterMax-MasterMin)/2, MasterMin+(MasterMax-MasterMin)/2);
}

void BezInsert(){ // Insert a point to anywhere in the curve
    menvalue = menvalue - 10350;
    Bez[menvalue/50]->InsertPoint(MasterMin+(MasterMax-MasterMin)/2, MasterMin+(MasterMax-MasterMin)/2, menvalue%50);
}

void DelBez(){ //Deletes an entire Bezier Curve
    int i = menvalue - 2550;
    numBez--;
    while (i<numBez){
        Bez[i]=Bez[i+1];
        i++;
    }
    menvalue = -5;
}

void DelBezPoint(){ //Deletes a point from a curve
    int i = menvalue - 2600;
    Bezier *B;
    B = Bez[i/50];
    if (B->numPoint>1)
        B->DelPoint(i%50);
    menvalue = -5;
}

void FindMaster(){ //Finds the largest and smallest points on the screen
    if (numBez<1){  //Does not differentiate between x and y
        MasterMin=Spline[0]->FindMin();
        MasterMax=Spline[0]->FindMax();
        float a,b;
        for (int i=1; i<numBspline; i++){
            a = Spline[i]->FindMin();
            b = Spline[i]->FindMax();
            MasterMin = (a<MasterMin) ? a : MasterMin;
            MasterMax = (b>MasterMax) ? b : MasterMax;
        }
        a = (MasterMax-MasterMin)/20;
        MasterMax+=a;
        MasterMin-=a;
    }
    else{
        MasterMin=Bez[0]->FindMin();
        MasterMax=Bez[0]->FindMax();
        float a,b;
        for (int i=1; i<numBez; i++){
            a = Bez[i]->FindMin();
            b = Bez[i]->FindMax();
            MasterMin = (a<MasterMin) ? a : MasterMin;
            MasterMax = (b>MasterMax) ? b : MasterMax;
        }
        for (int i=0; i<numBspline; i++){
            a = Spline[i]->FindMin();
            b = Spline[i]->FindMax();
            MasterMin = (a<MasterMin) ? a : MasterMin;
            MasterMax = (b>MasterMax) ? b : MasterMax;
        }
        a = (MasterMax-MasterMin)/20;
        MasterMax+=a;
        MasterMin-=a;
    }
    
    scale = 2/(MasterMax-MasterMin);
};


float BezX(Bezier *B, float t0, int i, int j){  // Called from deCasteljau algorithm to find x coordinate
    if (j==0)
        return B->Points[i][0];
    else return ((BezX(B, t0, i, j-1)*(1-t0)) + (BezX(B, t0, i+1, j-1)*t0));
}

float BezY(Bezier *B, float t0, int i, int j){  // Called from deCasteljau algorithm to find y coordinate
    if (j==0)
        return B->Points[i][1];
    else return ((BezY(B, t0, i, j-1)*(1-t0)) + (BezY(B, t0, i+1, j-1)*t0));
}

void DrawBez(Bezier* B){
    if (ToggleControl){
        glColor3f(0.3, 0.3, 0.3);
        for (int i=0; i<B->numPoint-1; i++){//Draw the control polygon in light gray
            glBegin(GL_LINES);
            glVertex2f((B->Points[i][0]-MasterMin)*scale-1, (B->Points[i][1]-MasterMin)*scale-1);
            glVertex2f((B->Points[i+1][0]-MasterMin)*scale-1, (B->Points[i+1][1]-MasterMin)*scale-1);
            glEnd();
        }
    }
    float oldx=B->Points[0][0], oldy=B->Points[0][1], newx, newy, inRes = 1/(float (B->Resolution));
    glColor3f(1.0, 0.0, 0.0);
    for (int i=0; i<B->Resolution; i++){
        newx = BezX(B, (i+1)*inRes, 0, B->numPoint-1);  //THE DECASTELJAU ALGORITHM
        newy = BezY(B, (i+1)*inRes, 0, B->numPoint-1);
        glBegin(GL_LINES);
        glVertex2f((oldx-MasterMin)*scale-1, (oldy-MasterMin)*scale-1);
      //  glColor3f(1.0-(float (i+1)*inRes), 0.0+(float (i+1)*inRes), 0.0);
        glVertex2f((newx-MasterMin)*scale-1, (newy-MasterMin)*scale-1);
        glEnd();
        oldx = newx;
        oldy=newy;
    }
}

float SplineX(BSpline* BS, float x, int i, int j){ //Called from deBoor algorithm to find X coordinate
    if (j==0)
        return BS->Points[i][0];
    return (((BS->U[i + BS->k] - x)/(BS->U[i+BS->k]-BS->U[i+j])) * SplineX(BS, x, i, j-1) + ((x- BS->U[i+j])/(BS->U[i+BS->k]-BS->U[i+j])) * SplineX(BS, x, i+1, j-1));
}

float SplineY(BSpline* BS, float x, int i, int j){ //Called from deBoor algorithm to find Y coordinate
    if (j==0)
        return BS->Points[i][1];
    return (((BS->U[i + BS->k] - x)/(BS->U[i+BS->k]-BS->U[i+j])) * SplineY(BS, x, i, j-1) + ((x- BS->U[i+j])/(BS->U[i+BS->k]-BS->U[i+j])) * SplineY(BS, x, i+1, j-1));
}

void DrawSpline(BSpline* BS){
    float newx, newy, inRes = 1/(float (BS->Resolution));
    float oldx=SplineX(BS, BS->U[BS->k-1], 0, BS->k-1), oldy=SplineY(BS, BS->U[BS->k-1], 0, BS->k-1);
    if (ToggleControl){
        glColor3f(0.3, 0.3, 0.3);
        for (int i=0; i<BS->numPoint-1; i++){ //Draw Control Polygon in light gray
            glBegin(GL_LINES);
            glVertex2f((BS->Points[i][0]-MasterMin)*scale-1, (BS->Points[i][1]-MasterMin)*scale-1);
            glVertex2f((BS->Points[i+1][0]-MasterMin)*scale-1, (BS->Points[i+1][1]-MasterMin)*scale-1);
            glEnd();
        }
    }
    glColor3f(0.0, 0.0, 1.0);
    for (int l=0; l<BS->numPoint+1-BS->k; l++){ //THE BEBOOR ALGORITHM
        for (float i=0; i<BS->Resolution; i++){
            newx = SplineX(BS, BS->U[l+BS->k-1]+(i+1)*inRes*(BS->U[l+BS->k]-BS->U[l+BS->k-1]), l, BS->k-1);
            newy = SplineY(BS, BS->U[l+BS->k-1]+(i+1)*inRes*(BS->U[l+BS->k]-BS->U[l+BS->k-1]), l, BS->k-1);
            glBegin(GL_LINES);
            glVertex2f((oldx-MasterMin)*scale-1, (oldy-MasterMin)*scale-1);
            glVertex2f((newx-MasterMin)*scale-1, (newy-MasterMin)*scale-1);
            glEnd();
            oldx = newx;
            oldy=newy;
        }
    }

}

void ResBez(){  //Sets Resolution for Bezier Curve
    if(UP){
        Bez[menvalue-5100]->AdRes(true);
    }
    else if (DOWN){
        Bez[menvalue-5100]->AdRes(false);
    }
    sprintf(s,"Bezier %d : Resolution = %d",
            menvalue-5099, Bez[menvalue - 5100]->Resolution); //Prints the Resolution
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(-.9, .9);
    for (char *c = s; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void AddSpline(){ //Add New BSpline curve
    BSpline* BS = new BSpline(20, 3);
    BS->AddPoint(MasterMin+(MasterMax-MasterMin)/4, MasterMin+(MasterMax-MasterMin)/4);
    BS->AddPoint(MasterMin+(MasterMax-MasterMin)/4, MasterMin+(MasterMax-MasterMin)*3/4);
    BS->AddPoint(MasterMin+(MasterMax-MasterMin)*3/4, MasterMin+(MasterMax-MasterMin)*3/4);
    BS->AddPoint(MasterMin+(MasterMax-MasterMin)*3/4, MasterMin+(MasterMax-MasterMin)/4);
    Spline[numBspline] = BS;
    menvalue = 7750 + numBspline*50;
    numBspline++;
}

void AddSplinePoint(){  //Append a point to a spline curve
    int i = menvalue - 5150;
    menvalue = (7750+ i*50 + Spline[i]->numPoint);
    Spline[i]->AddPoint(MasterMin+(MasterMax-MasterMin)/2, MasterMin+(MasterMax-MasterMin)/2);
}

void SplineInsert(){ //Insert a point anywhere in the curve
    menvalue = menvalue - 12850;
    Spline[menvalue/50]->InsertPoint(MasterMin+(MasterMax-MasterMin)/2, MasterMin+(MasterMax-MasterMin)/2, menvalue%50);
    menvalue+=7750;
}

void DelSplinePoint(){  //Delete a point on a BSpline
    int i = menvalue - 5200;
    BSpline *BS;
    BS = Spline[i/50];
    if (BS->numPoint>1)
        BS->DelPoint(i%50);
    menvalue = -5;
}

void DelSpline(){   //Delete an entire BSpline
    int i = menvalue - 7700;
    numBspline--;
    while (i<numBspline){
        Spline[i]=Spline[i+1];
        i++;
    }
    menvalue = -5;
}

void MoveSpline(){  //Grant ability to drag points, also print coordinates
    int i = menvalue - 7750;
    if ( UP2 == false && UP == true && (i/50)<(numBspline-1))
        menvalue = menvalue +50 - menvalue%50;
    else if ( DOWN2 == false && DOWN == true && (i/50)>0)
        menvalue = menvalue -50 - menvalue%50;
    else if ( RIGHT2 == false && RIGHT == true && (i%50)<(Spline[i/50]->numPoint-1))
        menvalue+=1;
    else if ( LEFT2 == false && LEFT == true && (i%50)>0)
        menvalue-=1;
    Spline[i/50]->MovePoint(i%50, deltax, deltay);
    float x, y;
    Spline[i/50]->FindPoint(i%50, &x, &y);
    sprintf(s,"BSpline %d : Point %d : x = %4.3f ; y = %4.3f",
            i/50+1, i%50+1, x, y);
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(-.9, .9);
    for (char *c = s; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    x = (x - MasterMin) * scale - 1;
    y = (y - MasterMin) * scale - 1;
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    glVertex2f(x - .01, y - .01);
    glVertex2f(x + .01, y + .01);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(x - .01, y + .01);
    glVertex2f(x + .01, y - .01);
    glEnd();
}

void ResSpline(){   //Change and Proint Spline Resolution
    if(UP){
        Spline[menvalue-10250]->AdRes(true);
    }
    else if (DOWN){
        Spline[menvalue-10250]->AdRes(false);
    }
    sprintf(s,"BSpline %d : Resolution = %d",
            menvalue-10249, Spline[menvalue-10250]->Resolution);
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(-.9, .9);
    for (char *c = s; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void KSpline(){ //Change and print Spline K value
    if(UP2==false && UP==true){
        Spline[menvalue-10300]->AdK(true);
    }
    else if (DOWN2==false && DOWN==true){
        Spline[menvalue-10300]->AdK(false);
    }
    UP2=UP;
    DOWN2=DOWN;
    sprintf(s,"BSpline %d : k = %d",
            menvalue-10299, Spline[menvalue-10300]->k);
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(-.9, .9);
    for (char *c = s; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void ChangeKnot(){
    int i = menvalue-15350;
    float j;
    if (i%3 == 0)
        j=.01;
    else if (i%3 == 1)
        j=.1;
    else j=1;
    i/=3;
    BSpline* BS = Spline[i];
    if (knot<0 || knot>(BS->k+BS->numPoint))
        knot = 0;
    if(LEFT2==false && LEFT==true && knot>0){
        knot--;
    }
    else if (RIGHT2==false && RIGHT==true && knot<(BS->k+BS->numPoint)){
        knot++;
    }
    LEFT2=LEFT;
    RIGHT2=RIGHT;
    if (UP2 == false && UP == true){
        if ((BS->U[knot]+j) <= BS->U[knot+1])
            BS->U[knot]+=j;
    }
    else if (DOWN2 == false && DOWN == true){
        if (knot==0)
            BS->U[0]-=j;
        else if ((BS->U[knot]-j) >= BS->U[knot-1])
            BS->U[knot]-=j;
    }
    UP2=UP;
    DOWN2=DOWN;
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(-.9, .9);
    sprintf(s,"BSpline %d : knot = %d : U[knot] = %4.2f",
            i, knot, BS->U[knot]);
    for (char *c = s; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

void drawWindow(){
    glClear(GL_COLOR_BUFFER_BIT);
    if (menvalue == -5){
      menuchange=false;
    }
    else if (menvalue == -10){  //Iterate through all possible menvalues (edited in createmenu)
        AddBez();
	menuchange=true;
        glutPostRedisplay();
    }
    else if (menvalue == -20){
      menuchange=true;
        AddSpline();
        glutPostRedisplay();
    }
    else if (menvalue == -2){
      menuchange=false;
        ToggleControl = ! ToggleControl;
        menvalue = oldvalue;
        glutPostRedisplay();
    }
    else if (menvalue < 2500){
      menuchange=false;
        MoveBez();
    }
    else if (menvalue < 2550){
      menuchange=true;
        AddBezPoint();
        glutPostRedisplay();
    }
    else if (menvalue < 2600){
      menuchange=true;
        DelBez();
        glutPostRedisplay();
    }
    else if (menvalue < 5100){
      menuchange=true;
        DelBezPoint();
        glutPostRedisplay();
    }
    else if (menvalue < 5150){
      menuchange=false;
        ResBez();
    }
    else if (menvalue < 5200){
      menuchange=true;
        AddSplinePoint();
        glutPostRedisplay();
    }
    else if (menvalue < 7700){
      menuchange=true;
        DelSplinePoint();
        glutPostRedisplay();
    }
    else if (menvalue < 7750){
      menuchange=true;
        DelSpline();
        glutPostRedisplay();
    }
    else if (menvalue < 10250){
      menuchange=false;
        MoveSpline();
    }
    else if (menvalue < 10300){
      menuchange=false;
        ResSpline();
    }
    else if (menvalue < 10350){
      menuchange=false;
        KSpline();
    }
    else if (menvalue < 12850){
      menuchange=true;
        BezInsert();
        glutPostRedisplay();
    }
    else if (menvalue < 15350){
      menuchange=true;
        SplineInsert();
        glutPostRedisplay();
    }
    else if (menvalue < 15500){
      menuchange=false;
        ChangeKnot();
        glutPostRedisplay();
    }
    FindMaster();   //Find Size of Window (For NDC)
    if (menuchange)
      createmenu();      //Update the window for the correct curves
    for (int i=0; i<numBez; i++){
        DrawBez(Bez[i]);    //Draw all beziers
    }
    for (int i=0; i<numBspline; i++){
        if (Spline[i]->numPoint>=Spline[i]->k){ //Draw all correct BSplines
            DrawSpline(Spline[i]);
        }
        else {
            sprintf(s,"BSpline %d not rendered, adjust K and N",i+1);   //Give warning if BSpline is bad
            glColor3f(1.0, 1.0, 1.0);
            glRasterPos2f(-.9, -.9);
            for (char *c = s; *c != '\0'; c++) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
            }
        }
    }
    glutSwapBuffers();
}

int main(int argc, char ** argv) {
    load();
    glutInit(&argc, argv);                                  	//initializes the GLUT framework
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    MainWindow = glutCreateWindow("Jacob Ley Proj4");
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMove);
    glutSpecialFunc(pressKey);
    glutSpecialUpFunc(releaseKey);
    createmenu();
    glutDisplayFunc(drawWindow);
    glutMainLoop();           	//the main loop of the GLUT framework
}
