# Binary Protocol Structure:
## Client - Server:
| Opcode | Function | Parameters | 
| :---: | :---: | :---: |
| [ 4 bytes ] | [ 4 bytes ] | [ variable length ] |


| Opcode | Function | Parameters | Description |
| :---: | :---: | :---: | :--: |
| 0 | 0 | 0 | Ping and get list of online users |
| 0 | 1 | ["username", "password"] | Login | 
| 0 | 2 | ["username", "password"] | Register |
| 0 | 3 | ["username", "old_password", "new_password"] | Change Password |
| 1 | 0 | None | Get Friend List |
| 1 | 1 | ["username", "destination", "timeout"] | Send Friend Request |
| 1 | 2 | ["destination", "username", "response"] | Response to Request |
| 1 | 3 | None | Get Request List |
| 2 | 0 | ["username"] | Get Room List |
| 2 | 1 | ["room_id"] | Get member list |
| 2 | 2 | ["room_name, "admin username"] | Create Room |
| 2 | 3 | ["room_id", "guest_username"] | Add User to Room |
| 2 | 4 | ["room_id", "guest_username"] | Remove User from Room |
| 3 | 0 | ["room_id"] | Get Conversation |
| 3 | 1 | ["username", "room_id", "message"] | Send Message |

For each parameter in the "Parameters" field, use a length prefix followed by the actual parameter bytes. For example:

| Length param1 | Actual param1 | Length param2 | Actual param2 | Length param3 | Actual param3 |
| :---: | :---: | :---: | :---: | :---: | :---: |
| [ 4 bytes ] | [ variable length ] | [ 4 bytes ] | [ variable length ] | [ 4 bytes ] | [ variable length ] |
| 5 | admin | 8 | password | 0 | 0 |
| 0x00 0x05 | 0x61 0x64 0x6d 0x69 0x6e | 0x00 0x08 | 0x70 0x61 0x73 0x73 0x77 0x6f 0x72 0x64 | 0 | 0 | 

- Expample: LOGIN as "admin" with "password": 
```
0000 0001 0005 6164
6d69 6e00 0870 6173
7377 6f72 6400 0000
```

## Server-client:
| Opcode | Function | Parameters | Status | Response |
| :---: | :---: | :---: | :--: | :--: |
| [ 4 bytes ] | [ 4 bytes ] | [ variable length ] | [ 4 bytes ] | [ variable length ] |

| Opcode | Function | Parameters | Description |
| :---: | :---: | :---: | :--: |
| 0 | 0 | ["username", "ping"] | get online |
| 0 | 1 | ["logged_in_timestamp"] | login done |
| 0 | 2 | ["user_id"] | register done |
| 0 | 3 | ["user_id"] | change password done |
| 1 | 0 | ["username", "ping"] | get friend list |
| 1 | 1 | ["destination", "username"] | request added |
| 1 | 2 | ["response"] | response to request |
| 2 | 0 | [ "room_id", "roomname" ] | get room list |
| 2 | 1 | ["username"] | Send member list |
| 2 | 2 | [ "room_id", "roomname" ] | create done |
| 2 | 3 | [ "room_id", "guess username" ] | add done |
| 2 | 4 | [ "room_id", "guess username" ] | remove done |
| 3 | 0 | ["room_id", "timestamp", "username"] | get conversation |
| 3 | 1 | ["response"] | response message |

- Response Status: Success (0) or Error (non-zero)
- Response Data: Success message or an error message
