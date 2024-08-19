mkdir -p bin
docker run \
  --rm \
  -v $(pwd):/src \
  -u $(id -u):$(id -g) \
  emscripten/emsdk \
  emcc src/*.c -o bin/main.html -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os -I. -I include -L. -L include -s USE_GLFW=3 libWeb/libraylib.a -DPLATFORM_WEB -s EXPORTED_FUNCTIONS='["_main","_give_touch_input","_give_key_input","_pause_playing_game"]' -s EXPORTED_RUNTIME_METHODS=ccall,cwrap

echo "Compilation for WASM Complete"

mkdir -p build
cp web/index.html build/index.html
cp web/script.js build/script.js

rm bin/main.html
mv bin/main.js build/main.js
mv bin/main.wasm build/main.wasm

echo "Build complete"