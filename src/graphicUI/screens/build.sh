mkdir ../../build/gui
g++ -c ../../include/webview/webview.cc -std=c++11 -DWEBVIEW_STATIC $(pkg-config --cflags gtk+-3.0 webkit2gtk-4.0) -o ../../build/gui/webview.o
gcc -c gui.c -std=c99 -Ilibs/webview -o ../../build/gui/gui.o
g++ ../../build/gui/gui.o ../../build/gui/webview.o $(pkg-config --libs gtk+-3.0 webkit2gtk-4.0) -o gui && gui

gcc -c auth.c -std=c99 -Ilibs/webview -o ../../build/gui/auth.o
g++ ../../build/gui/auth.o ../../build/gui/webview.o $(pkg-config --libs gtk+-3.0 webkit2gtk-4.0) -o auth && auth
