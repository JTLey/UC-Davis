//
//  main.cpp
//  ECS175_Proj5
//
//  Created by Jacob Ley on 12/3/15.
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
#include "Plane.h"
#include "Light.h"
#include "Sphere.h"

#define RADIAN 0.01745 // Degree to radian conversion

using namespace std;

int MainWindow, Menu_id, Sphere_id, menvalue;
int resolution, numlight, numplane, numsphere, Recursions;
light *Lights[10];
sphere *Spheres[10];
plane *Planes[10];
float angle, From[3], At[3], Up[3], *PixelBuffer, ambient[3];
bool UP, DOWN, RIGHT, LEFT=false;

void load(){ // Read the Text File
    string line;
    ifstream myfile("3drayshapes.txt");
    size_t pos;
    float X[10];
    if (myfile.is_open()){
        getline(myfile,line);
        resolution = stoi(line);
        getline(myfile, line);
        angle = stof(line);
        getline(myfile,line);
        Recursions = stoi(line);
        
        getline(myfile,line);
        pos = line.find(" ");
        From[0] = stof(line.substr(0,pos)); //From Point
        line.erase(0, pos+1);
        pos = line.find(" ");
        From[1] = stof(line.substr(0,pos));
        line.erase(0, pos+1);
        From[2] = stof(line);
        
        getline(myfile,line);
        pos = line.find(" ");
        At[0] = stof(line.substr(0,pos)); //At Point
        line.erase(0, pos+1);
        pos = line.find(" ");
        At[1] = stof(line.substr(0,pos));
        line.erase(0, pos+1);
        At[2] = stof(line);
    
        getline(myfile,line);
        pos = line.find(" ");
        Up[0] = stof(line.substr(0,pos)); //Up Vector
        line.erase(0, pos+1);
        pos = line.find(" ");
        Up[1] = stof(line.substr(0,pos));
        line.erase(0, pos+1);
        Up[2] = stof(line);
        
        getline(myfile,line);
        pos = line.find(" ");
        ambient[0] = stof(line.substr(0,pos)); //ambient Light location and value
        line.erase(0, pos+1);
        pos = line.find(" ");
        ambient[1] = stof(line.substr(0,pos));
        line.erase(0, pos+1);
        ambient[2] = stof(line);
        
        getline(myfile,line);
        numlight = stoi(line);
        for(int i=0; i<numlight; i++){
            getline(myfile,line);
            pos = line.find(" ");
            X[0] = stof(line.substr(0,pos)); //Light location and value
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[1] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[2] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[3] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[4] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            X[5] = stof(line);
            Lights[i] = new light(X);
        }
        
        getline(myfile, line);
        numplane = stoi(line);
        for (int i=0; i<numplane; i++){
            getline(myfile,line);
            pos = line.find(" ");
            X[0] = stof(line.substr(0,pos)); //Plane definition and points
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[1] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[2] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            X[3] = stof(line);
            Planes[i] = new plane(X);
            getline(myfile,line);
            pos = line.find(" ");
            X[0] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[1] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[2] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[3] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[4] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[5] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[6] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[7] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            X[8] = stof(line);
            Planes[i] -> AddPoints(X);
            
            getline(myfile,line);
            pos = line.find(" ");
            X[0] = stof(line.substr(0,pos)); //K values
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[1] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[2] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[3] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[4] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[5] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[6] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[7] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[8] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            X[9] = stoi(line);
            Planes[i]->AddK(X);
        }
        
        getline(myfile, line);
        numsphere = stoi(line);
        for (int i=0; i<numsphere; i++){
            getline(myfile,line);
            pos = line.find(" ");
            X[0] = stof(line.substr(0,pos)); //Light location and value
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[1] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[2] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[3] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[4] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            X[5] = stof(line);
            Spheres[i] = new sphere(X);
            
            getline(myfile,line);
            pos = line.find(" ");
            X[0] = stof(line.substr(0,pos)); //K values
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[1] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[2] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[3] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[4] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[5] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[6] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[7] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            pos = line.find(" ");
            X[8] = stof(line.substr(0,pos));
            line.erase(0, pos+1);
            X[9] = stoi(line);
            Spheres[i]->AddK(X);
        }
    }
    myfile.close();
}

void save(){    //Writes the file, same set up as load
    ofstream myfile ("3drayshapes.txt");
    if (myfile.is_open()){
        myfile << resolution << "\n";
        myfile << angle << "\n";
        myfile << Recursions << "\n";
        myfile << From[0] << " " << From[1] << " " << From[2] << "\n";
        myfile << At[0] << " " << At[1] << " " << At[2] << "\n";
        myfile << Up[0] << " " << Up[1] << " " << Up[2] << "\n";
        myfile << ambient[0] << " " << ambient[1] << " " << ambient[2] << "\n";
        myfile << numlight << "\n";
        for (int i=0; i<numlight; i++){
            myfile << Lights[i]->Coord[0] << " " << Lights[i]->Coord[1] << " " << Lights[i]->Coord[2] << " " << Lights[i]->Color[0] << " " << Lights[i]->Color[1] << " " << Lights[i]->Color[2] << "\n";
        }
        myfile << numplane << "\n";
        for (int i=0; i<numplane; i++){
            myfile << Planes[i]->ABC[0] <<" "<< Planes[i]->ABC[1] << " " << Planes[i]->ABC[2] << " " << Planes[i]->D<<"\n";
            myfile << Planes[i]->Point1[0] << " " << Planes[i]->Point1[1] << " " << Planes[i]->Point1[2] << " " << Planes[i]->Point2[0] << " " << Planes[i]->Point2[1] << " " << Planes[i]->Point2[2] << " " << Planes[i]->Point3[0] << " " << Planes[i]->Point3[1] << " " << Planes[i]->Point3[2] << "\n";
            myfile << Planes[i]->K[0] << " " << Planes[i]->K[1] << " " << Planes[i]->K[2] << " " << Planes[i]->K[3] << " " << Planes[i]->K[4] << " " << Planes[i]->K[5] << " " << Planes[i]->K[6] << " " << Planes[i]->K[7] << " " << Planes[i]->K[8] << " " << Planes[i]->K[9] << "\n";
        }
        myfile << numsphere << "\n";
        for (int i=0; i<numsphere; i++){
            myfile << Spheres[i]->Coord[0] << " " << Spheres[i]->Coord[1] << " " << Spheres[i]->Coord[2] << " " << Spheres[i]->Ellipse[0] << " " << Spheres[i]->Ellipse[1] << " " << Spheres[i]->Ellipse[2] << "\n";
            myfile << Spheres[i]->K[0] << " " << Spheres[i]->K[1] << " " << Spheres[i]->K[2] << " " << Spheres[i]->K[3] << " " << Spheres[i]->K[4] << " " << Spheres[i]->K[5] << " " << Spheres[i]->K[6] << " " << Spheres[i]->K[7] << " " << Spheres[i]->K[8] << " " << Spheres[i]->K[9] << "\n";
        }
    }
    myfile.close();
}

void menu(int num){ //Called from the menu, sets value of menvalue
    if(num == -1){
        save();
        glutDestroyWindow(MainWindow);
        exit(0);
    }
    else{
        if (num==1){
            cout << "PLEASE INPUT NEW FROM POINT: ";
            cin >> From[0] >> From[1] >> From[2];
        }
        else if (num==2){
            cout << "PLEASE INPUT NEW AT POINT: ";
            cin >> At[0] >> At[1] >> At[2];
        }
        else if (num==3){
            cout << "PLEASE INPUT NEW UP POINT: ";
            cin >> Up[0] >> Up[1] >> Up[2];
        }
        else if (num==4){
            cout << "PLEASE INPUT NEW RECURSION LEVEL: ";
            cin >> Recursions;
            if (Recursions<=0){
                Recursions = 1;
            }
        }
        else if (num==5){
            cout << "PLEASE INPUT NEW ANGLE: ";
            cin >> angle;
            if ((angle <=0)||(angle>=90))
                angle = 45;
        }
        else menvalue=num;
    }
    glutPostRedisplay();
}

void createmenu(){
    
    Sphere_id = glutCreateMenu(menu);
    if (numsphere>0){
        glutAddMenuEntry("sphere 1", 20);
    }
    if (numsphere>1){
        glutAddMenuEntry("sphere 2", 21);
    }
    if (numsphere>2){
        glutAddMenuEntry("sphere 3", 22);
    }
    if (numsphere>3){
        glutAddMenuEntry("sphere 4", 23);
    }
    if (numsphere>4){
        glutAddMenuEntry("sphere 5", 24);
    }
    if (numsphere>5){
        glutAddMenuEntry("sphere 6", 25);
    }
    if (numsphere>6){
        glutAddMenuEntry("sphere 7", 26);
    }
    if (numsphere>7){
        glutAddMenuEntry("sphere 8", 27);
    }
    if (numsphere>8){
        glutAddMenuEntry("sphere 9", 28);
    }
    if (numsphere>9){
        glutAddMenuEntry("sphere 10",29);
    }
    
    Menu_id = glutCreateMenu(menu);
    glutAddMenuEntry("New From Point", 1);
    glutAddMenuEntry("New At Point", 2);
    glutAddMenuEntry("New Up Vector", 3);
    glutAddMenuEntry("New Recursion Level",4);
    glutAddMenuEntry("New Angle", 5);
    glutAddSubMenu("Move Sphere", Sphere_id);
    glutAddMenuEntry("Exit", -1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

float normalize(float X[3]){
    float dist = X[0]*X[0] + X[1]*X[1] + X[2]*X[2];
    dist = pow(dist, .5);
    if (dist == 0){
        X[0] = X[1] = 0;
        X[2] = 1;
    }
    else {
        X[0] = X[0]/dist;
        X[1] = X[1]/dist;
        X[2] = X[2]/dist;
    }
    return dist;
}

void cross(float X[3], float Y[3], float Z[3]){
    Z[0] = X[1]*Y[2] - Y[1]*X[2];
    Z[1] = X[2]*Y[0] - Y[2]*X[0];
    Z[2] = X[0]*Y[1] - Y[0]*X[1];
}

float dot(float X[3], float Y[3]){
    return X[0]*Y[0] + X[1]*Y[1] + X[2]*Y[2];
}

void findreflection(float Ray[3], float Normal[3], float Reflection[3]){
    float d = -dot(Ray, Normal);
    Reflection[0] = Ray[0] + 2 * d * Normal[0];
    Reflection[1] = Ray[1] + 2 * d * Normal[1];
    Reflection[2] = Ray[2] + 2 * d * Normal[2];
}

bool checkhit(float Source[3], float Ray[3], float newSource[3], float newNormal[3], float *t, float K[10]){
    *t=-1;
    float newt;
    for (int i=0; i<numplane; i++){
        newt = Planes[i]->NearHit(Source, Ray);
        if (((newt>0.03)&&(newt<*t)) || (*t<.03)){
            Planes[i]->getSource(Ray, newNormal, K);
            newSource[0] = Source[0] + Ray[0]*newt;
            newSource[1] = Source[1] + Ray[1]*newt;
            newSource[2] = Source[2] + Ray[2]*newt;
            *t=newt;
        }
    }
    for (int i=0; i<numsphere; i++){
        newt = Spheres[i]->NearHit(Source, Ray);
        if (((newt>0.03)&&(newt<*t)) || (*t<.03)){
            newSource[0] = Source[0] + Ray[0]*newt;
            newSource[1] = Source[1] + Ray[1]*newt;
            newSource[2] = Source[2] + Ray[2]*newt;
            Spheres[i]->getSource(Ray, newSource, newNormal, K);
            *t=newt;
        }
       /* if (newt>0)
            cout << "NEWT " << newt << "\n";*/
    }
    if (*t>0.03) return true;
    else return false;
}

void Phong(float Surface[3], float ViewerVec[3], float Normal[3], float distance, float RGB[3], float K[10]){
    float reflection[3], lightvec[3], dist;
    light *curlight;
    bool shadow;
    findreflection(ViewerVec, Normal, reflection);
    RGB[0] = ambient[0] * K[0];
    RGB[1] = ambient[1] * K[1];
    RGB[2] = ambient[2] * K[2];
    for (int i=0; i<numlight; i++){ //For every light source
        curlight = Lights[i];
        lightvec[0] = curlight->Coord[0]-Surface[0];
        lightvec[1] = curlight->Coord[1]-Surface[1];
        lightvec[2] = curlight->Coord[2]-Surface[2];
        float newSource[3], newNormal[3], t, X[10];
        dist = normalize(lightvec);
        shadow = false;
        if (checkhit(Surface, lightvec, newSource, newNormal, &t, X)){
            if (t < dist && (dot(lightvec,Normal)<0)){
              //  cout << "DISTANCE " << dist << " TIME: " << t << " \n";
                shadow=true;
            }
        }
        for (int j=0; j<3; j++){
            if (!shadow)
                RGB[j] += (K[3+j]*dot(Normal, lightvec)*curlight->Color[0]+ K[6+j]*pow(dot(reflection,lightvec),K[9])*curlight->Color[0]) / (distance + dist);
        }
    }
}

void RayTrace(float Source[3], float Ray[3], float RGB[3], int depth){
   // cout << "DEPTH " << depth << "\n";
    if (depth == 0){
        RGB[0] = 0;
        RGB[1] = 0;
        RGB[2] = 0;
        return ;
    }
    float t, newSource[3], newNormal[3], rgb[3], K[9];
    if (checkhit(Source, Ray, newSource, newNormal, &t, K)){
        normalize(newNormal);
        Phong(newSource, Ray, newNormal, t, RGB, K);
        float reflection[3];
        findreflection(Ray, newNormal, reflection);
        RayTrace(newSource, reflection, rgb, depth-1);
        RGB[0] += rgb[0]*.08;
        RGB[1] += rgb[1]*.08;
        RGB[2] += rgb[2]*.08;
        RayTrace(newSource, Ray, rgb, depth-1);
        RGB[0] += rgb[0]*.08;
        RGB[1] += rgb[1]*.08;
        RGB[2] += rgb[2]*.08;
        return ;
    }
    else{
        RGB[0] = 0;
        RGB[1] = 0;
        RGB[2] = 0;
        return ;
    }
}

void movesphere(int i){
    if (LEFT){
        if (DOWN)
            Spheres[i]->Coord[0]-=0.02;
        if (RIGHT)
            Spheres[i]->Coord[1]-=0.02;
        if (UP)
            Spheres[i]->Coord[2]-=0.02;
    }
    else{
        if (DOWN)
            Spheres[i]->Coord[0]+=0.02;
        if (RIGHT)
            Spheres[i]->Coord[1]+=0.02;
        if (UP)
            Spheres[i]->Coord[2]+=0.02;
    }
}

void move(){
    if (menvalue >19 && menvalue <30){
        movesphere(menvalue-20);
        glutPostRedisplay();
    }
}

void display(){
    move();
    float BaseRay[3], Right[3], Ray[3], Up2[3], RGB[3], upangle, rightangle;
    float csu, csr, snu, snr, a1, a2, a3; //cosine of upangle, cosine of rightangle, sin of upangle, sin of rightangle
    BaseRay[0] = At[0]-From[0];
    BaseRay[1] = At[1]-From[1];
    BaseRay[2] = At[2]-From[2];
    normalize(BaseRay);
    cross(Up, BaseRay, Right);
    normalize(Right);//Make the up vector perpendicular to ray vector
    cross(BaseRay, Right, Up2);
    for (int i=0; i<resolution; i++){
        upangle = (float(i)/float(resolution)*2 - 1) * angle;
        for (int j=0; j<resolution; j++){
            rightangle = (float(j)/float(resolution)*2 - 1) * angle;
            csr = cos(rightangle*RADIAN);
            csu = cos(upangle*RADIAN);
            snr = sin(rightangle*RADIAN);
            snu = sin(upangle*RADIAN);
            a1 = csr * csu;
            a2 = snr * csu;
            a3 = snu * csr;
            Ray[0] = BaseRay[0] * a1;
            Ray[0] += Right[0] * a2;
            Ray[0] += Up2[0] * a3;
            Ray[1] = BaseRay[1] * a1;
            Ray[1] += Right[1] * a2;
            Ray[1] += Up2[1] * a3;
            Ray[2] = BaseRay[2] * a1;
            Ray[2] += Right[2] * a2;
            Ray[2] += Up2[2] * a3;
            normalize(Ray);
            RGB[0] = RGB[1] = RGB[2] = 0;
            RayTrace(From, Ray, RGB, Recursions); //Recursive call, light values returned in RGB
            PixelBuffer[j*3 + i*resolution*3] = RGB[0];
            PixelBuffer[j*3 + i*resolution*3 +1] = RGB[1];
            PixelBuffer[j*3 + i*resolution*3 +2] = RGB[2];

        }
    }
    csu = 0;
    for (int i=0; i<resolution * resolution * 3; i++){
        csu = (csu<PixelBuffer[i]) ? PixelBuffer[i] : csu;
    }
    if (csu!=0){
        for (int i=0; i<resolution * resolution * 3; i++){
            PixelBuffer[i] /= csu;
        }
    }
    glDrawPixels(resolution, resolution, GL_RGB, GL_FLOAT, PixelBuffer);
    glFlush();
 //   cout << "LOOP\n";
}

void pressKey(int key, int xx, int yy) { //Decide if arrow keys are pressed
    switch (key) {
        case GLUT_KEY_UP : UP = true; break;
        case GLUT_KEY_DOWN : DOWN = true; break;
        case GLUT_KEY_RIGHT : RIGHT = true; break; //Key inputs for animated movements
        case GLUT_KEY_LEFT : LEFT = !LEFT; break;
    }
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void releaseKey(int key, int xx, int yy) { // Clear values when arrow keys are released
    
    switch (key) {
        case GLUT_KEY_UP : UP = false; break;
        case GLUT_KEY_DOWN : DOWN = false; break;
        case GLUT_KEY_RIGHT : RIGHT = false; break; //Cancels the animated movements
    }
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

int main(int argc, char * argv[]) {
    glutInit(&argc, argv);
    load();
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(resolution, resolution);
    //set window position
    glutInitWindowPosition(100, 100);
    
    //create and set main window title
    MainWindow = glutCreateWindow("ECS175 Proj1 JLey");
    glClearColor(0, 0, 0, 0); //clears the buffer of OpenGL
    //sets display function
    glutDisplayFunc(display);
    glutSpecialFunc(pressKey);
    glutSpecialUpFunc(releaseKey);
    // GetFile();
    PixelBuffer = new float[resolution*resolution*3];
    createmenu();
    glutMainLoop();//main display loop, will display until terminate
    return 0;

}
