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

gcc -o ../../bin/client client_terminal.c connection.c feature.c ../utility.c ../protocol/protocol.c
cd ../../bin
./client