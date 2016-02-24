ECS 175 PROJ 4
JACOB LEY
998251725
jtley@ucdavis.edu

README FILE for ECS 175 PROJ 4

OVERALL NOTES
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
!!The command Line is unused. All inputs are by the KEYBOARD ARROWS and the MOUSE!!.
Bezier Curves are RED, BSpline Curves are BLUE
The Control Polygons are initially toggled off (So that my sample file draws a cute picture), select toggle in the menu to turn it on
The sample file is called 2dcoordinates.txt, it is written to with the "Exit" command, it loads automatically
Moving points displays the coordinate values.
There is a maximum of 50 curves for each type, with 50 points for each curve
Curves and Points are labeled in chronological order (If there are 3 Bezier curves, adding a new one will be Bezier curve 4)
The current point being edited is displayed in the top left corner
Line functions connecting points are implemented using OpenGL GL_LINES.

The menu is dynamically created, so it can be quite slow. Commands that change the number of curves or points will have a delay.
	Commands that only change a value of a pre-existing point should not experience a slow down.

Movement is animated (moves as you drag with mouse).

IMPLEMENTED ALGORITHMS
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
DeBoor Algorithm: Main.cpp 589-627

De Casteljau Algorithm: Main.cpp 552-587

NDC : Main.cpp Findmaster() 512-549, implemented in all draw functions


RUNNING THE FILE
~~~~~~~~~~~~~~~~~~~~~~~
Compile the program by typing "make" using the provided makefile
Run the Program by typing "main"
At that point, the program will load the 2dcoordinates.txt and allow user input

TEXT FILE
~~~~~~~~~~~~~
Curves are stored in 2dcoordinates.txt
It stores whether a curve is Bezier or BSpline, all control points, resolution, and k values
Attached is the sample file provided, note that the //comments are not actually in the file

3   //NUMBER OF BEZIER
3       //First Bezier, number of points
4       //First Bezier, resolution
1.14079 1.44945     //x,y values for First Bezier
1.80241 1.24652
2.43522 1.47805
7       //Second Bezier, number of points...
25
1.12154 1.44611
1.15772 1.53636
1.54158 1.50574
1.81395 1.35246
2.03956 1.51937
2.4115 1.53787
2.45466 1.47529
3       //Third Bezier...
25
1.78419 2.02468
1.57693 1.71282
1.94159 1.79885
2   //NUMBER OF BSPLINES
5   //First BSpline, numer of points
3   //First BSPline, k value
2   //First BSpline, resolution
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 //knot values (all declared)
1.22547 2.26752 //x,y values for First BSpline
1.46548 2.32775
1.65905 2.16644
1.22808 2.0758
1.16926 2.27202
8   //Second BSPline...
3
20
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99
2.32775 2.24563
2.1811 2.37174
1.86435 2.19577
2.16351 2.15471
1.94647 2.27202
1.93474 2.13711
2.34534 2.06672
2.27499 2.43559


USING THE MENU
~~~~~~~~~~~~~~~~
Use the RIGHT MOUSE BUTTON to bring up the menu
then use the LEFT MOUSE BUTTON to select your action.

"Toggle Control Polygons" Means that it will either hide or display the polygons
"Exit" Will quit the program and save the curves

The other two options bring up new submenus for the selected curve.

MOVE POINT
~~~~~~~~~~~~
This command will default after certain other commmands
Selecting this command will print the current coordinates to the top left corner
It will also draw a small green X on the selected coordinate

Drag the point to the desired position to move it
Do so by clicking the point the LEFT MOUSE BUTTON, and dragging (You can actually click anywhere)
Release the LEFT MOUSE BUTTON to drop the point

You can drag the point outside the window, but the scaling will not be as smooth (Still entirely functional)

To quickly move to another point on a curve of the SAME TYPE use the ARROW KEYS: (must tap, not hold)
    UP ARROW moves to the first point of the next curve
    DOWN ARROW moves to the first point of the previous curve
    RIGHT ARROW moves to the next point in same curve
    LEFT ARROW moves to the previous point in the same curve

It is not reccomended to hold the mouse button and click the arrows.
    (Shouldn't cause errors, just makes selecting points hard)

ADD BEZIER/BSPLINE
~~~~~~~~~~~~~~~~~~~
Functionally the same for both curves.
Creates a brand new curve labled appropriately.
The Bezier will create a triangle with resolution=25
BSpline will create a Square with k=3, resolution=20

It will also give the user the ability to control the first point. (See MOVE)

ADD/INSERT BEZIER/BSPLINE
~~~~~~~~~~~~~~~~~~~~~~~
Adding a point is esentially inserting a point at the end of the curve
Inserting is used to place a new point before the selected point

For example, if there are 4 points A B C D, and insert point 2 is selected:
        A new point X will be inserted such that the control polygon is defined as A X B C D.
        That means X is the new point 2, and B, C, D increase by 1

!!The new point will default to a value that places it in the middle of the screen!!

After adding and inserting, the user has ability to move that new point. (See MOVE)

DELETE BEZIER/BSPLINE
~~~~~~~~~~~~~~~~~~~~~~
Running this command deletes the entire selected Curve
It will upgrade all other curves as well:
So if there are Bezier Curves A, B, C, D and you choose to (Delete Bezier 2):
        The resulting curves are A, C, and D with C=Bezier 2, D = Bezier 3

This option will not have any print output in the top corner, nor will it default to any point movement

DELETE BEZIER/BSPLINE POINT
~~~~~~~~~~~~~~~~~~~~~~~
This command will delete the selected point
You cannot delete all the points (1 will remain)
!!For BSplines, if the number of points is now less than k, it will print and error and not display the curve!!

Like Deleting entire curves, deleting a point will upgrad the other points
So if you delete point 2, the old point 3 becomes the new point 2.

ADJUST BEZIER/BSPLINE RESOLUTION
~~~~~~~~~~~~~~~~~~~~~~~~~
The resolution is the number of iterations between intervals, the higher the "smoother" the curve
Selecting this option will print the resolution to the top left corner
Use the Keyboard UP BUTTON to increase Resolution
Use the Keyboard DOWN BUTTON to decrease Resolution

You can either click the button multiple times, or simply hold it down.

Resolution will not decrease past 1.

ADJUST BSPLINE K
~~~~~~~~~~~~~~~~~
Just like resolution, K is adjusted using the keyboard.
K cannot be less than 2, and cannot be more than the number of control points

Tap the Keyboard UP BUTTON to increase K
Tap the Keyboard DOWN BUTTON to decrease K

You cannot hold down the button, you must tap the key for each change.

ADJUST BSPLINE KNOT VALUES
~~~~~~~~~~~~~~~~~~~~~~~~~
Selecting this option changes the text in the top left corner.
You have the option to make the changes in factors of .01, .1, or 1
Use the LEFT and RIGHT BUTTONS to select a knot. Only valid and necessary knots are selectable
Use the UP and DOWN BUTTONS to change the knot value. The knots must stay in increasing order (will not allow error)
BUTTONS must be tapped to change value, holding will not do more
These changed values will be saved upon exiting
