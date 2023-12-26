// Sample chat data (replace with your implementation using a server and database)
const chatRooms = [
    { name: 'Friend but not online', online: true, messages: [] },
    { name: 'Group chat', online: false, messages: [] },
    { name: 'Friend online', online: false, messages: [] }
];

const onlineUser = [
    {name: 'user1'},
    {name: 'user2'},
    {name: 'user3'}
]

const onlineUsers = ['User1', 'User2', 'User3'];

// Display chat rooms
function displayChatRooms() {
    const chatRoomList = document.getElementById('chatRoomList');
    chatRoomList.innerHTML = '';
    chatRooms.forEach(room => {
        const li = document.createElement('li');
        li.textContent = room.name;
        li.onclick = () => changeChatRoom(room);
        chatRoomList.appendChild(li);
    });
}

// Display online users
function displayOnlineUsers() {
    const onlineUserList = document.getElementById('onlineUserList');
    onlineUserList.innerHTML = '';
    onlineUsers.forEach(user => {
        const li = document.createElement('li');
        li.textContent = user;
        onlineUserList.appendChild(li);
    });
}

// Display messages for the selected chat room
function displayMessages(chatRoom) {
    const messageContainer = document.getElementById('messageContainer');
    messageContainer.innerHTML = '';
    chatRoom.messages.forEach(message => {
        const div = document.createElement('div');
        div.textContent = `${message.sender}: ${message.text}`;
        messageContainer.appendChild(div);
    });
}

// Change the current chat room
function changeChatRoom(chatRoom) {
    document.getElementById('chatRoomName').textContent = chatRoom.name;
    displayMessages(chatRoom);
}

// Send a message to the current chat room
function sendMessage() {
    const messageInput = document.getElementById('messageInput');
    const messageText = messageInput.value.trim();
    if (messageText !== '') {
        const currentChatRoom = chatRooms.find(room => room.name === document.getElementById('chatRoomName').textContent);
        const newMessage = { sender: 'CurrentUser', text: messageText };
        currentChatRoom.messages.push(newMessage);
        displayMessages(currentChatRoom);
        messageInput.value = '';
    }
}

// Initialize the app
displayChatRooms();
displayOnlineUsers();
changeChatRoom(chatRooms[0]);
