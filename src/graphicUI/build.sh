# Check if gcc is installed
if ! command -v gcc &> /dev/null; then
    echo "Installing gcc..."
    sudo apt-get update
    sudo apt-get install gcc
else
    echo "gcc is already installed."
fi

# Check if g++ is installed
if ! command -v g++ &> /dev/null; then
    echo "Installing g++..."
    sudo apt-get update
    sudo apt-get install g++
else
    echo "g++ is already installed."
fi

# install gtk webview
sudo apt install libgtk-3-dev libwebkit2gtk-4.0-dev

# create folder for object files
mkdir ../../build
mkdir ../../bin
mkdir ../../build/gui
g++ -c ../../include/webview/webview.cc -std=c++11 -DWEBVIEW_STATIC $(pkg-config --cflags gtk+-3.0 webkit2gtk-4.0) -o ../../build/gui/webview.o

# compile GUI
gcc -c ui_auth.c -std=c99 -Ilibs/webview -o ../../build/gui/auth.o
g++ ../../build/gui/auth.o ../../build/gui/webview.o $(pkg-config --libs gtk+-3.0 webkit2gtk-4.0) -o ../../bin/auth

gcc -c ui_app.c -std=c99 -Ilibs/webview -o ../../build/gui/app.o
g++ ../../build/gui/app.o ../../build/gui/webview.o $(pkg-config --libs gtk+-3.0 webkit2gtk-4.0) -o ../../bin/app

cd ../../bin
./auth
./app
