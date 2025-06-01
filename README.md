# bwapi-linux

A basic Linux version for developing starcraft bots using bwapi.

Get started with Starcraft: BroodWar AI Development as fast as possible.

New to StarCraft or AI Programming? Watch the tutorial video(s):
* Starcraft AI Intro: https://www.youtube.com/watch?v=czhNqUxmLks
* STARTcraft Tutorial: https://www.youtube.com/watch?v=FEEkO6__GKw

# Setup Instructions:

1. Download / Clone this repo to your computer.
2. Download [Starcraft Broodwar 1.16.1](https://drive.google.com/file/d/1q4gib8OQ0CMDMBorhwWwta135uqwlCSk/view?usp=sharing) to the root directory of your project.
3. Unzip starcraft.tar.xz using: `tar -xJf starcraft.tar.xz`.
4. Install Mingw-w64 using: `sudo apt install build-essential mingw-w64`.
5. Install wine using: `sudo dpkg --add-architecture i386 && sudo apt update && sudo apt install wine`.
6. Build the provided bot source code via the `make` command.
7. Run `bash run.sh`.
8. You can clean up the binary files by running the `make clean` command.

This repository is a repo for Linux developers, based on https://github.com/davechurchill/STARTcraft.

bwapi repository: https://github.com/bwapi/bwapi
base repository: https://github.com/davechurchill/STARTcraft
