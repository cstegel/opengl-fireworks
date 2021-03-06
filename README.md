# Project Overview: Fireworks

This is an OpenGL project that aims to model fireworks.  It is my final project for UW's CS 488 course.

![fireworks](screenshot.png)

# Dependencies

The only external dependencies are OpenAL and ALUT which are are already installed
on UW's graphics lab computers.  On Ubuntu 14.04 you can add these with the following command:

```
sudo apt-get install libopenal-dev libalut-dev
```

The rest of the dependencies are pulled in automatically when building the project and my
course submission has these dependencies already pulled in.


# Compilation

To compile the program, run the command "make" within the root directory of the project.

The program will be compiled to `bin/Release/fireworks`.
The program can then be run with `./bin/Release/fireworks`.

This program was compiled/run successfully on graphics lab computer 13 (gl13).

# Controls

The mouse can be toggled on with the "~" key (the one to the left of the "1").  You can then
open up the debug menu to toggle various options as well as change the display mode.

You can move around in the world with the WASD keys, spacebar, CTRL, and by moving the mouse.

Clicking the mouse will launch a firework into the air.
Scrolling the mouse wheel will increase or decrease the distance that fireworks
spawn in front of you.

# Objectives

  1. modelling the scene
  2. UI
  3. texture mapping
  4. bump mapping
  5. shadows using shadow maps (incomplete)
  6. shadow map antialiasing using Percentage-Closer Filtering (incomplete)
  7. volumetric light scattering as a postprocess effect
  8. triggered sound where volume decays with distance
  9. kinematics system (gravity, velocity, acceleration)
  10. particle emitters (collections of temporary 2D sprites affected by kinematics) (incomplete)

# License

Check the individual LICENSE files inside of `assets/models/` directories before
using the models for something other than personal use. Most of the models
only have a license permitting personal use.

Files under the `src/cs488/` directory are copyright of the instructors of
the University of Waterloo's CS 488 course.

All other files are Licensed under the MIT license as explained in `LICENSE`.