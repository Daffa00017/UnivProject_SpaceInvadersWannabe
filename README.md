# Space Invaders Wannabe

This simple project act as me and my friend lab000 attempt to learn about design pattern in developing a c++ simple game project. It is fully inspired by a popular retro game that is "Space Invaders" with the help of a youtube tutorial for development. 


## Getting Started

### What design pattern that is used in this project?

* Singleton pattern
  * Input manager, Sound manager and UI Manager.
* Observer pattern
  * UI Observer that is used for broadcasting score and high score to UI Manager.
* Command pattern
  * Shoot command, this command pattern is simply to more encapsulate the function inside Input manager. So instead of the shoot function being in the Input manager, we make that a command pattern by making a new class just for this function.
* Factory pattern
  * Aliens.


## Acknowledgments

The tutorial that i used to create this project
* [Programming With Nick]([https://github.com/matiassingers/awesome-readme](https://www.youtube.com/@programmingwithnick))
