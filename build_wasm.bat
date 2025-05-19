call emsdk_env.bat

emcc -o build/web/index.html src/main.c -Os -Wall ./raylib/wasm/lib/libraylib.a -I. -I./raylib/wasm/include -L. -L./raylib/wasm/lib/ -s USE_GLFW=3 -s TOTAL_MEMORY=67108864 -s SINGLE_FILE=1 --embed-file build/assets --shell-file ./raylib/wasm/minshell.html -DPLATFORM_WEB -s EXPORTED_RUNTIME_METHODS=ccall,HEAPF32