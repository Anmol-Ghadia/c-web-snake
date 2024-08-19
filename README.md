# Snake Game
![GitHub Actions Pages Deployment Workflow Status](https://img.shields.io/github/actions/workflow/status/Anmol-Ghadia/c-web-snake/gh-pages.yml?label=Deployment%20Build)
[![Main Branch Deployment](https://img.shields.io/website?url=https%3A%2F%2Fanmol-ghadia.github.io%2Fc-web-snake%2F&up_message=Live&up_color=brightgreen&down_message=down&down_color=red&label=View%20Demo&link=https%3A%2F%2Fanmol-ghadia.github.io%2Fc-web-snake%2F)](https://anmol-ghadia.github.io/c-web-snake/)


This repository contains a simple implementation of the classic Snake game, developed in C and compiled to [WebAssembly](https://webassembly.org/) using the Emscripten toolchain. The game supports both touch and keyboard input and features a dark/light mode toggle.

## Features
- Touch Support: Play the game using touch controls on mobile devices.
- Keyboard Support: Control the snake using arrow keys on desktop.
- Dark/Light Mode: Switch between dark and light themes for comfortable gameplay.

### Controls
1) Touch Devices: Tap on the screen to control the direction of the snake.
1) Desktop: Use the arrow keys (Up, Down, Left, Right) to control the snake.

### Theme Toggle
Light mode is activated by default. Theme can be changed on the main Menu screen or the pause screen

## Github Pages Deployment
1) Commits to the main branch will be automatically built and deployed to GitHub Pages using GitHub Actions configured in the .github/workflows/gh-pages.yml file.

1) click on the above demo link to view the deployed app

## Getting Started
To build and run the Snake game locally, follow these steps:

### Prerequisites
- Docker: Required for building the project using Emscripten.
- Git: For cloning the repository.

### Building the Project
1) Clone the Repository:
    ```bash
    git clone https://github.com/yourusername/snake-game.git
    cd snake-game
    ```

1) Run the Build Script:

    > Note: Ensure you have the build script (build.sh) set as executable:

    ```bash
    chmod +x build.sh
    ```

    Then, build the project:

    ```bash
    ./build.sh
    ```

    This script will compile the C code into WebAssembly, create necessary directories, and organize the build output.

### Running the Game Locally
After building the project, you can serve the build directory using a local web server. Here’s how you can do it with Python:

1) Navigate to the Build Directory:

    ```bash
    cd build
    ```

1) Start a Local Web Server:
    Using Python 3.x:

    ```bash
    python -m http.server 8000
    ```

    > Note: serving files over any other server will work as well.

1) Access the Game:

    Open a web browser and go to [http://localhost:8000](http://localhost:8000) to play the game.

## Acknowledgments
- [Emscripten](https://emscripten.org/): For compiling C code to WebAssembly.