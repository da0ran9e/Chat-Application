## Binary Protocol Structure:
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
| 1 | 2 | ["destination", "username", "response"] | Respond to Request |
| 2 | 0 | ["username"] | Get Room List |
| 2 | 1 | ["room_id"] | Get people list |
| 2 | 2 | ["room_name"] | Create Room |
| 2 | 3 | ["room_id", "guest_username"] | Add User to Room |
| 2 | 4 | ["room_id", "guest_username"] | Remove User from Room |
| 3 | 0 | ["room_id"] | Get Conversation |
| 3 | 1 | ["room_id", "message"] | Send Message |
