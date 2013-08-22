Program made on top of UniShader, simulating the movement and collisions of balls in 3D space.

Controls:
W,S,A,D,Q,E - movement of camera (up, down, left, right, forward, backward)
V,B - zoom (in, out) camera
I,K,J,L - rotate camera (up, down, left, right)
Pressing shift for above will increase the ammount of movement

1,2,3 (top row of keyboard) - loading of setup files, numbers correspond with file names (setup1.txt,...)
R - resets the scene
+,- - speed up/down the simulation (maximum speed is 1 - realtime, minimum is not defined)
Space - pause simulation
Enter - switch on/off the output of the update time in miliseconds (default is off)
F10 - outputs information about openGL to text file
X,Y,Z - sets camera rotation along the cardinal axis

Before simulation can start, a scene must be loaded. There are sample scenes but new one can be simply made
in text editor using this structure:

First number: 0 for balls, 1 for planes, 2 for vortices

Parameters (Number in parenthesis means the count of values used for parameter. If not specified, it is 1):
Balls: position(3), speed vector(3), radius, mass, bounce factor, deformation factor
Planes: position(3), normal vector(3), deformation factor, color(3)
Vortices: position(3), direction vector(3), length, radius of effect, intensity(acceleration), color(3)
Positions and vectors are along cardinal axis (x,y,z) and colors are in RGB format


Bounce and deformation factors are values from 0-1.
Bounce factor is used to control how much velocity is kept during collision for that object.
Setting 1 for that parameter means that object is fully elastic and now velocity is lost due the material it is made of.

Deformation factor is used to control how much velocity is lost during collision for object that collides with this object.
Setting 1 for that parameter means object means that object that will collide with this object will loose all velocity no matter what it is made of.

For example we can have fully elastic ball, but when it hits fully deformable plane, it won't bounce off.
These parameters are not specifying real properties of material, they are only approximations allowing us to introduce inelastic collisions.
The equation for resulting velocity is V = V*B*(1-D), where B is bouncing factor for object1 with speed V and D is deformation factor of object with which
is the object1 colliding.