# database setting up
DB_NAME="chat"
DB_USER="postgres"
SQL_FILE="../database/setup/setup.sql"

# check postgre sql existance
if ! command -v psql &> /dev/null; then
    sudo apt update
    #install postgresql-16
    sudo apt install postgresql-16
    sudo apt-get install postgresql-client
    #start posgresql service
    sudo systemctl start postgresql.service
fi

# check database
if ! sudo -u postgres psql -lqt | cut -d \| -f 1 | grep -qw $DB_NAME; then

    # Create a database and user
    sudo -u postgres createdb $DB_NAME
    sudo -u postgres createuser --interactive --username=$DB_USER
    # change user password  
    ALTER USER postgres WITH PASSWORD 'anquynh123';

    # Connect to the database and execute SQL script
    sudo -u postgres psql -d $DB_NAME -U $DB_USER -f $SQL_FILE
fi

# install C lib for posgresql
sudo apt-get install libpq-dev

# server compile
gcc -o server main.c features.c log.c authentication.c connection.c conversations.c relation.c rooms.c ../database/db_connection.c ../database/database.c ../database/sql_queries.c ../utility.c ../protocol/protocol.c -L/usr/include/postgresql/libpq -lpq
./server 5500