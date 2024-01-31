mkdir build
g++ -c webview.cc -std=c++11 -DWEBVIEW_STATIC $(pkg-config --cflags gtk+-3.0 webkit2gtk-4.0) -o build/webview.o
gcc -c basic.c -std=c99 -Ilibs/webview -o build/basic.o
g++ build/basic.o build/webview.o $(pkg-config --libs gtk+-3.0 webkit2gtk-4.0) -o build/basic && build/basic
