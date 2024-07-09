### command
```BASH
emcc src/main.c -o bin/main.html -Iinclude -Llib -lraylib -s USE_GLFW=3 -s WASM=1 -s FULL_ES2=1 -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY
```
