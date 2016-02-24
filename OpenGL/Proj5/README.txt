README
FOR JACOB LEY ECS175 PROJ5
998251725
jtley@ucdavis.edu

NOTES
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
There is the option to move ellispoids/spheres around the screen using arrows
	However, due to high recursion levels and resolution
	This is not necessarily a smooth graphic, and is just for visual
	understanding of the scene
All other inputs (new angle, point, etc.) are command line inputs
The scene supports a maximum of 10 ellipsoids, 10 planes, and 10 light sources
	in addition to an ambient light source
The scene is normalized. If there is an object that is incredibly bright
	(High K values, close to light source) other objects may appear dim

RUNNING THE FILE
~~~~~~~~~~~~~~~~~~~
Compile the program using the provide "make" command
Then run the program by typing "main"
The program will read from the text file "3drayshapes.txt" and draw the scene
During runtime, you may give the program commands using the RIGHT MOUSE BUTTON
	To bring up a menu. This will either activate arrows or the command line
To close the program, select EXIT from the menu
	This will save the current scene to the text file

UNDERSTANDING THE TEXT FILE
~~~~~~~~~~~~~~~~~~~~~~~
The text file "3drayshapes.txt" is provided to load all pertinent info
That includes the scene dimensions, angles, and all objects in the scene

Attached is a sample text file with explanation. // Comments are not actually included in the text file.

500	//Resolution of scene (both height and width)
45	//Scene angle (how far from base viewing vector) the higher->more fisheye
2	//Recursion level. The higher this is, the slower the program 
0 0 0	//From Point (x y z)
1 0 0	// At Point
0 0 1	//Up Vector 
0.5 0.5 0.5	//Ambient light (R G B)
3	//Number of light sources
3 3 1 5 5 1	//First light source location and color (x y z r g b)
3 -4 0 3 2 2	//Second light source...
0 -3 -1 6 6 6
2	//Number of planes
1 1 1 2	//Definition of 1st plane (a b c d) => Ax + By + Cz = D
5 -4 1 5 1 -4 2 0 0	//Points on 1st plane (x1 y1 z1, x2 y2 z2, x3 y3 z3)
1 0.8 1 0.7 0.7 0.7 1 0.8 1 1	//K values for phong (Ka (RGB), Kd (RGB), Ks (RGB), alpha)
1 0 0 3	//Definition of 2nd plane...
3 -0.5 -0.5 3 -4 -3 3 -3 -4
0.8 0.9 1 0.8 1 0.9 0.8 1 0.8 3
1	//Number of Ellipsoids
5.82 0.3 0.62 0.5 1 0.7 //Location and defition of 1st (x, y, z, a, b, c)
1 0.3 0.7 1 0.9 0.9 1 0.5 1 2 //K values for Phong

This file is rewritten at the closing of the program

HOW THE PROGRAM WORKS:
~~~~~~~~~~~~~~~~~~~~~~
The at and from points are used to make a baseray vector (Display() in main)
This is crossed with the up vector to get a right vector
Then the right vector is recrossed with the baseray to get a guarenteed perpendicular up vector.
Then all vectors are normalized

Depending on the coordinates on the screen, those 3 vectors are combined with sine+cosine functions of the angle to create a normalized ray that "shoots out" (RayTrace() in main)

The ray is the checked if it hits anything 
	(Checkhit() in main combined with Nearhit() in Sphere/Plane.cpp)

The closest object it hits becomes the new surface. The phong model is applied there, then two new rays (reflection and refraction) are sent out recursively

The phong model applies the ambient light to the object.
Then it tests for each light source, if there is an object in the way (using checkhit() again)
If there is no obstruction, the diffuse and specular values are included. 

Once all the rays are calculated, the values are normalized and the screen displays. Larger resolutions and recursions mean longer frame times (easily a few seconds per frame)




USING THE MENU
~~~~~~~~~~~~~~~~~
press the RIGHT MOUSE BUTTON to activate the menu
If you choose to move an ellipsoid:
	press DOWN ARROW to move in the x direction
	press RIGHT ARROW to move in the y direction
	press UP ARROW to move in the z direction
	tap the LEFT ARROW to invert the direction of movement
If you choose to have a new From/At point or Up vector:
	Input the values into the command line: "0 0 1" would represent a point/vector of (0, 0, 1)
Choose new angle:
	Input the value into the command line "60" would mean 60 degrees of visibity in both directions from the base viewing vector
Choosing exit will save the scene and close the program

DEFINITION OF PLANE
~~~~~~~~~~~~~~~~~~~~~
To view a plane one must be added to the text file
It involves defining the plane as (Ax + By + Zz = D)
Then the three points must be added to that plane.
NOTE: THERE IS NO CHECK IF THE POINTS ACTUALLY LIE ON THAT PLANE
	So if the added points are wrong, the scene may fail
Then the plane is defined by its K values
That is broken down into the Ambient, Diffuse, and Specular values for RGB, as well as the alpha value. The values should be between 0 and 1, and alpha should be a whole number greater than 0

DEFINITION OF Ellipsoid
~~~~~~~~~~~~~~~~~~~~
A ellipsoid can be moved around in the program, but not reshaped.
The definition of an ellipsoid is:
	((x-x1)*a)^2 + ((y-y1)*b)^2 + ((z-z1)*c)^2 = 1
So the user must specify the base coordinates and a,b,c values
The smaller the a,b,c values, the longer the ellipsoid will be in that direction




