## Build Guide

### Using CLI
Using git bash is the recommeded CLI on `windows`

1) Web build
    ```BASH
    emcc src/main.c -o bin/main.html -Iinclude -Llib -lraylib -s USE_GLFW=3 -s WASM=1 -s FULL_ES2=1 -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY
    ```

1) Desktop build
    
    `Windows:`
    ```BASH
    gcc src/main.c -o bin/desktop/main -Iinclude -LlibDesktop -lraylib -lopengl32 -lgdi32 -lwinmm
    ```

    `Unix:`
    ```BASH
    gcc src/main.c -o bin/desktop/main -Iinclude -LlibDesktop -lraylib -lm -lpthread -ldl -lrt -lX11
    ```

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
1) add two triangles on corners instead of masking using fake square
1) fix painting when snake is going offscreen and wrapping on opposite edge
1) Figure out window resizing
1) Add points system
1) show death screen
