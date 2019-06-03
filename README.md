# Orbit3D
A 3D adaptation of the Exploration mode in my orbit-sandbox project, created using Unreal Engine 4.


02/06/19: The current version of the game includes a number of untextured spheres which spawn over time and exert gravitational forces on each other. Just as in the 2D version, when two spheres collide they combine into one larger sphere. A sky sphere, created using the [Spacescape](http://alexcpeterson.com/spacescape/) tool, encapsulates the player's vision. The player currently controls the pre-made spaceship included in the example "Flying" C++ project.

![Basic Gravity And Collisions](Screenshots/BasicGravityAndCollisions3.png)


03/06/19: Each sphere that spawns has a chance to be given a point light component, much more realistically illuminating all other objects within the sky sphere.

![Point Lights](Screenshots/PointLights.png)