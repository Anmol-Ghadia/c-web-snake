mkdir -p bin/web
docker run \
  --rm \
  -v $(pwd):/src \
  -u $(id -u):$(id -g) \
  emscripten/emsdk \
  emcc src/*.c -o bin/web/main.html -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os -I. -I include -L. -L include -s USE_GLFW=3 libWeb/libraylib.a -DPLATFORM_WEB -s EXPORTED_FUNCTIONS='["_main","_give_touch_input","_give_key_input","_pause_playing_game"]' -s EXPORTED_RUNTIME_METHODS=ccall,cwrap

echo "Compilation for WASM Complete"

# mkdir -p bin/desktop/linux/i386
# docker run \
#     --rm \
#     -v $(pwd):/project \
#     -w /project \
#     gcc:latest \
#     gcc -o bin/desktop/linux/i386/snake src/*.c -Wall -std=c99 -Iinclude -L libDesktop/i386 -lopengl32 -lgdi32 -lwinmm
