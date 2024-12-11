Kidus Zegeye, kmz25, CS 5152

I used XCode to program and build my solution

Process:

Task 1: My first time working in C++, had to study the syntax of AsteroidSet to figure out how to work on PhotonSet. Took a lot of inspiration from AsteroidSet to create the PhotonSet and Photon classes

Task 2: Took inspiration from how Asteroids updated, traversed the screen, and deleted themselves, in order to implement update for Photons

Task 3: Took inspiration from Asteroids' sprite code to draw the Photons, but modified it to only use 1 sprite. At first nothing would show up when I fired, and this ended up being caused by the Photon constructor not being configured correctly to setup the maxage and other parameters. This caused the photons to delete instantly

Task 4: The sound effects worked pretty easily by just adding a new one in a similar way that the "bang" sound effect was created

Task 5: I heavily used the collision code for ships and asteroids, but instead looped through all of the Photons in the PhotonSet. When spawning smaller asteroids after shooting a big one, I made a unit vector parallel to the photon's velocity and used the rotate method for the velocities of the smaller asteroids.

Task 6 (7): I took the code for creating the health text and used the GameScene's draw method to write them in red or green, based on a boolean flag that is set when the game is won or lost. I used a 2d affine transform to scale the text size by 3 and translate it to the center of the screen

Note: Had to upload SLShip because there wasn't a way to get/set the private _refire variable, so I made getter/setter functions for the _refire variable