Program made on top of UniShader, simulating the wave propagation on analytic 2D surface - wave plane

Controls:
W,S,A,D,Q,E - movement of camera (up, down, left, right, forward, backward)
V,B - zoom (in, out) camera
I,K,J,L - rotate camera (up, down, left, right)
Pressing shift for above will increase the ammount of movement

1 (top row of keyboard) - switch on/off displaying of points that creates wave plane (default is off)
2 (top row of keyboard) - switch between filled and wireframe wave plane (default is filled) 
Space - pause simulation
F10 - outputs information about openGL to text file
X,Y,Z - sets camera rotation along the cardinal axis

Using Settings window:
The Settings window is used for changing the parameters of wave plane and the waves.
Selection is made by inputing number written next to menu item and pressing Enter key. Similarly works inputing of values for parameters.

Mesh (wave plane) parameters:
For increasing size of wave plane (surface), set higher values for width and height. For better resolution, set lower values for spacing (it will also decrease the size)
Defaultly the wave plane consist of 10000(100x100) points with spacing of 0.5.
However even the oldest graphic cards supporting openGL 3.2 should smoothly simulate wave plane made of million points. Recommended spacing is 0.2.

Wave parameters:
Each wave have it's unique name (identifier) specified when creating the wave. Creating new wave with existing name results in selecting the existing wave.
After selecting name with identifier it is possible to change it's parameters or delete it.

Any changes made to parameters will immediatly result in change of wave plane.