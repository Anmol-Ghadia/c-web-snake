# Snake Game
A simple snake that targets Web using [`wasm`](https://webassembly.org/) and standard `desktop` platform. 

## Demo
Find the Web version hosted here: [Demo](https://www.students.cs.ubc.ca/~aghadia/trialWasm/main.html)
> Note: since the project is under development, the demo might not be the latest version.
## Build Guide
1) Make sure the following dependencies are installed first
    1) emsdk
    1) raylib is installed and compiled for web
    1) GCC and Make if windows
    `Note:` make file looks for raylib in D drive but modify it as per your installation
### Using Make
1) Web build
    ```BASH
    make web
    ```
1) Desktop build
    ```BASH
    make desktop
    ```
    `Note:` make file is configured to build for Windows, modify the file using command from above to build for Unix systems

## TODO
1) Refactor code
1) ~~Design Snake Head~~
1) ~~Add invisible buttons for movement on touch screens~~
1) ~~Add some padding to pause button~~
1) ~~Add Dark and light mode button on menu button~~
1) ~~Fix margin_width, it should be a ratio instead of number of pixels~~
1) ~~Add rounded cap on tail~~
1) ~~Fix snake coloring which turns black after certain length~~
1) ~~Add styling for head~~
1) ~~show death screen~~
1) ~~Add menu~~
1) ~~Add touch screen controls~~
1) ~~add styling~~
1) ~~Add points system~~
1) ~~depricate struct boolean as stdbool.h is now included~~
1) ~~fix painting when snake is going offscreen and wrapping on opposite edge~~
1) ~~add two triangles on corners instead of masking using fake square~~
1) ~~Figure out window resizing~~
