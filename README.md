# Archers Duel

A game made in C++ where you are an archer defending your watchtower from the enemy archers. Destroy their tower before they destroy yours!

## Gameplay

- Charge your shots to increase the speed of your arrows.
- Stun enemy archers by hitting them with your projectiles. Gain some time by making them fall from their tower!
- Randomly selected power ups will appear during the battle. Use them to increase your damage output!
- Once your cannon bar is full, you can charge a powerful cannon shot to devastate your enemies' tower. While charging your cannon, you won't be able to shoot if you get hit by an arrow , though.

## Features

- **Single Player**. Play a quick match against and along AI players.
- **Multiplayer** up to 16 total players. Connection requirements will increase proportionally with the higher the number of players.
- **Tutorial**. Quick tutorial to explain the basic and simple mechanics of the game.

## In-game pictures

# ![menu](https://img.itch.zone/aW1hZ2UvNDQ3MTQzLzIyNTg2MTYucG5n/original/h7daeW.png)
# ![archer1](https://img.itch.zone/aW1nLzIyNTg2MzIucG5n/original/MthHmG.png)
# ![archer2](https://img.itch.zone/aW1hZ2UvNDQ3MTQzLzIyNTg2MTcucG5n/original/f15%2B57.png)

## Download and play the game
 
If you want the game's build for Windows, download just the zip file it from the *Releases* tab.

## Server Setup Information

- When playing server, you'll always play as a red archer of the red team.
- LAN Multiplayer support only. (Advanced) It can be played through the Internet by configuring port forwarding on your router.
- Frame buffer size helps to optimize online gameplay performance. This value should be configured according the connection quality. It should also be slightly increased by each extra player. Examples: 
    
## Build Requirments
 
In order to build or modify properly this source code you will need the following libraries:

- SDL2 (https://www.libsdl.org/download-2.0.php)
- SDL2 Image extension (https://www.libsdl.org/projects/SDL_image/)
- SDL2 Net (https://www.libsdl.org/projects/SDL_net/)

You will also need the following programs:

- CMake (https://cmake.org/)
- Git (Recommended)(https://git-scm.com/)

## Build steps

Once you have downloaded all the dependencies you can build the project by following these steps:

  1a. Clone this repository using git: git clone https://github.com/MartGon/ArcherDuel

  1b. Download the source code from the *Releases* tab

  2a. (Linux) Run CMake, CMake should find the libraries for you when running on Linux. You'll have to link the libraries manually otherwise

  2b. (Windows) You'll neeed to tell CMake where to look for each one of the SDL libraries that you have downloaded

  3a. You can now compile the project with your preferred IDE (or *make*, in case of Linux)
  
 ## About us

This game was made for learning and entertainment purposes.
