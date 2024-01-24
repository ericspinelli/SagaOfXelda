# README.md

## Installation

Requires SFML: `sudo apt-get install libsfml-dev`

Compile: `g++ -c *.cpp && g++ *.o -o Xelda -lsfml-graphics -lsfml-window -lsfml-system`

## Menu Controls

* **Level Select:** UP arrow, DOWN arrow

* **Start Level:** ENTER

* **Quit:** ESC

## Gameplay Controls

* **Move:** UP, DOWN, LEFT, RIGHT arrows

* **Attack:** SPACE

* **Pause:** P

* **Quit to menu:** ESC

## Developer Mode

* Press `T` to toggle textures

* Press `C` to toggle bounding boxes and line of sight

* Press `F` to toggle camera (default: room view, alternative: follow player view)

## Enemies

Enemies have two modes: pre-set patrol or follow player.  Enemies that follow the player will do so only when the player is in their line-of-sight and will return to their base position when the player is not.
