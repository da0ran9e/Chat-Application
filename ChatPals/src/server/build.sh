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

# database setting up
DB_NAME="chat"
DB_USER="postgres"
SQL_FILE="setup.sql"

# check postgre sql existance
if ! command -v psql &> /dev/null; then
    sudo apt update
    #install postgresql-16
    sudo apt install postgresql-16
    sudo apt-get install postgresql-client
    #start posgresql service
    sudo systemctl start postgresql.service
    # Create a database and user
    sudo -u postgres createdb $DB_NAME
    sudo -u postgres createuser --interactive --username=$DB_USER
    # change user password  
    sudo -u postgres psql -c "ALTER USER postgres WITH PASSWORD 'anquynh123';"

    # Connect to the database and execute SQL script
    cd ../database/setup
    sudo -u postgres psql -d $DB_NAME -U $DB_USER -f $SQL_FILE
    cd ../../server
fi

# install C lib for posgresql
sudo apt-get install libpq-dev

# server compile
mkdir ../../bin
gcc -o ../../bin/server main.c features.c log.c authentication.c connection.c conversations.c relation.c rooms.c ../database/db_connection.c ../database/database.c ../database/sql_queries.c ../utility.c ../protocol/protocol.c -L/usr/include/postgresql/libpq -lpq
cd ../../bin
./server 5500