mkdir ../../build/gui
g++ -c ../../include/webview/webview.cc -std=c++11 -DWEBVIEW_STATIC $(pkg-config --cflags gtk+-3.0 webkit2gtk-4.0) -o ../../build/gui/webview.o

#gcc -c ui_app.c -std=c99 -Ilibs/webview -o ../../build/gui/app.o
#g++ ../../build/gui/app.o ../../build/gui/webview.o $(pkg-config --libs gtk+-3.0 webkit2gtk-4.0) -o app

g++ -o main.c connection.c feature.c ../utility.c ../protocol/protocol.c ../../build/gui/webview.o $(pkg-config --libs gtk+-3.0 webkit2gtk-4.0) -Ilibs/webview app
