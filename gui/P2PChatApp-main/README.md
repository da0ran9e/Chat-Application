# P2P Chatting Application
## Description
Apply P2P network into chatting
- [x] Send multiple consecutive chat message
- [x] Private chat with multiple user (multiple 1-1 conversations)
## Prerequisite
Linux 4.4+
## Instruction
### Build
Run following command to build both client and server:
`make -C src`
To build only one server or client, run one of these commands:
`make -C src server`
`make -C src client`
### Run
From root folder, run:
`./bin/server`
or
`./bin/client`
Cannot read `db/account.txt` if run from other folder.
### Debug
We will debug via `syslog`. Monitor syslog file with those commands:
* For server: `tail -f -n -300 /var/log/syslog | grep "P2P Chat Server"`
* For client: `tail -f -n -300 /var/log/syslog | grep "P2P Chat Client"`
### Demo
https://youtu.be/U_aO7b1js8g