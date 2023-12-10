#ifndef CONSTANTS_H
#define CONSTANTS_H

enum Object {
    SERVER,
    CLIENT
};

enum ServerStatus {
    RUNNING,
    CLOSED,
};

enum ClientStatus {
    LOGIN,
    LOGOUT,
    REGISTER,
}

#endif