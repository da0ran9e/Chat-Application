PORT = 0;
rtd = 9999;
username = "username";
// char g_user[MAX_CLIENTS][50];
onlineUsers = [
    {name: "user1", friend: true},
    {name: "user2", friend: false}
];

friends = [
    {name: "friend1"},
    {name: "friend2"},
    {name: "user1"}
];

chatRooms = [
    {
        name: "Room1",
        member: [
            {memberName: "member1"},
            {membername: "member2"},
            {memberName: "username"}
        ],
        messages: [
            {
                user: "member1",
                timestamp: "12-26-2023 16:02:44",
                content: "Hey! How are you!"
            },
            {
                user: "username",
                timestamp: "12-26-2023 16:04:30",
                content: "Hi!"
            }
        ]
    },
    {
        name: "Room2",
        member: [
            {memberName: "member1"},
            {membername: "member2"},
            {memberName: "username"}
        ],
        messages: [
            {
                user: "member1",
                timestamp: "12-26-2023 16:02:44",
                content: "Hey! How are you!"
            },
            {
                user: "username",
                timestamp: "12-26-2023 16:04:30",
                content: "Hi!"
            },
            {
                user: "member1",
                timestamp: "12-26-2023 16:02:44",
                content: "Hey! How are you!"
            }
        ]
    }
]

function sendMessage() {
    var messageText = document.getElementById("messageInput").value;
    if (messageText.trim() !== "") {
        var timestamp = new Date().toLocaleString();
 
        var sentMessageContainer = document.createElement("div");
        sentMessageContainer.className = "flex flex-row justify-end mb-4";
        sentMessageContainer.innerHTML = `
            <div class="messages text-sm text-white grid grid-flow-row gap-2">
                <div class="flex items-center flex-row-reverse group">
                    <p class="px-6 py-3 rounded-t-full rounded-l-full bg-blue-700 max-w-xs lg:max-w-md">${messageText}</p>
                    <button type="button" class="hidden group-hover:block">
                        <p class="p-4 text-center text-sm text-gray-500">${timestamp}</p>
                    </button>
                </div> 
            </div>
        `;


        document.getElementById("chat-body").appendChild(sentMessageContainer);
        document.getElementById("messageInput").value = "";
    }
}
function displayChatRooms(){
    const chatRoomList = document.getElementById('contact-list');
    chatRooms.forEach(room => {
        var chatRoomBtn = document.createElement("div");
        chatRoomBtn.className = "flex justify-between items-center p-3 hover:bg-gray-800 rounded-lg relative";
        chatRoomBtn.innerHTML = `
        <div class="w-16 h-16 relative flex flex-shrink-0">
            <img class="shadow-md rounded-full w-full h-full object-cover"
                name="chat-room-name"
                src="img.png"
                alt=""
            />
        </div>
        <div class="flex-auto min-w-0 ml-4 mr-6 hidden md:block group-hover:block">
            <p name="chat-room-name">${room.name}</p>
            <div class="flex items-center text-sm text-gray-600">
                <div class="min-w-0">
                    <p name="chat-room-recent" class="truncate">${room.messages.pop().content}</p>
                </div>
            </div>
        </div>
        `;
        chatRoomBtn.onclick = () => changeChatRoom(room);
        chatRoomList.appendChild(chatRoomBtn);
    });
}

function displayOnlineUsers(){
    const onlineUserList = document.getElementById('active-list');
    onlineUserList.innerHTML = '';

    onlineUsers.forEach(user => {
        btnDisplay = "block";
        if(user.friend == true) btnDisplay = "hidden"
        const onlineUserContainer = document.createElement('div');
        onlineUserContainer.className = "flex justify-between items-center p-3 hover:bg-gray-800 rounded-lg relative";

        onlineUserContainer.innerHTML = `
                        <div class="w-16 h-16 relative flex flex-shrink-0">
                            <img class="shadow-md rounded-full w-full h-full object-cover"
                                 src="img.png"
                                 alt=""
                            />
                        </div>
                        <div name="online-name" class="flex-auto min-w-0 ml-4 mr-6 hidden md:block group-hover:block">
                            <p>${user.name}</p>
                        </div>
                        <a onclick=addFriend("${user.name}") href="#" name="online-add-friend-bnt" class="${btnDisplay} rounded-full hover:bg-gray-700 bg-gray-800 w-10 h-10 p-2">
                            <svg class="w-full h-full fill-current" viewBox="0 0 24 24">
                                <path d="M17 11a1 1 0 0 1 0 2h-4v4a1 1 0 0 1-2 0v-4H7a1 1 0 0 1 0-2h4V7a1 1 0 0 1 2 0v4h4z"></path>
                            </svg>
                        </a>
        `
                        
        onlineUserList.appendChild(onlineUserContainer);
    });
}

function addFriend(friendname){
    console.log("add friend" + friendname)
    alert("add friend "+ friendname)
}

function displayFriends(){
    const onlineFriendList = document.getElementById('friend-list');
    onlineFriendList.innerHTML = '';
    
    friends.forEach(friend => {
        activeStatus = ""
        onlineUsers.forEach(onlineStatus =>{
            if (onlineStatus.name == friend.name) {
                activeStatus = `
        <div class="absolute bg-gray-900 p-1 rounded-full bottom-0 right-0">
            <div class="bg-green-500 rounded-full w-3 h-3"></div>
        </div>`

            }
        })
        friendContainer = document.createElement("div")
        friendContainer.className="text-sm text-center mr-4";
        friendContainer.innerHTML = `
                    <div class="p-1 border-4 border-transparent rounded-full"><div class="w-16 h-16 relative flex flex-shrink-0">
                        <img class="shadow-md rounded-full w-full h-full object-cover"
                             src="img.png"
                             alt=""
                        />
                        ${activeStatus}
                    </div></div><p>${friend.name}</p></div>
                    `
        onlineFriendList.appendChild(friendContainer);
    })
}

function displayMessages(chatRoom){
    const messageContainer = document.getElementById('chat-body');
    messageContainer.innerHTML = '';
    chatRoom.messages.forEach(message => {
        if (message.user == username){
            var sentMessageContainer = document.createElement("div");
        sentMessageContainer.className = "flex flex-row justify-end mb-4";
        sentMessageContainer.innerHTML = `
            <div class="messages text-sm text-white grid grid-flow-row gap-2">
                <div class="flex items-center flex-row-reverse group">
                    <p class="px-6 py-3 rounded-t-full rounded-l-full bg-blue-700 max-w-xs lg:max-w-md">${message.content}</p>
                    <button type="button" class="hidden group-hover:block">
                        <p class="p-4 text-center text-sm text-gray-500">${message.timestamp}</p>
                    </button>
                </div> 
            </div>
        `;
        messageContainer.appendChild(sentMessageContainer);
        } else {
            const receivedMessageContainer = document.createElement("div");
            receivedMessageContainer.className ="messages text-sm text-gray-700 grid grid-flow-row gap-2";
            receivedMessageContainer.innerHTML = `
                            <p class="p-0 text-left text-sm text-gray-50">${message.user}</p>
                            <div class="flex items-center group">
                                <p class="px-6 py-3 rounded-b-full rounded-r-full bg-gray-800 max-w-xs lg:max-w-md text-gray-200">${message.content}</p>
                                
                                <button type="button" class="hidden group-hover:block">
                                    <p class="p-4 text-center text-sm text-gray-500">${message.timestamp}</p>
                                </button>
                            </div>
            `;              
            messageContainer.appendChild(receivedMessageContainer);
        }
    });
}

function changeChatRoom(chatRoom) {
    document.getElementById('chat-name').textContent = chatRoom.name;
    displayMessages(chatRoom);
}

document.getElementById("messageInput").addEventListener("keydown", function (event) {
    if (event.key === "Enter") {
        sendMessage();
    }
});

// // Change the current chat room
// function changeChatRoom(chatRoom) {
//     document.getElementById('chatRoomName').textContent = chatRoom.name;
//     displayMessages(chatRoom);
// }

// // Send a message to the current chat room
// function sendMessage() {
//     const messageInput = document.getElementById('messageInput');
//     const messageText = messageInput.value.trim();
//     if (messageText !== '') {
//         const currentChatRoom = chatRooms.find(room => room.name === document.getElementById('chatRoomName').textContent);
//         const newMessage = { sender: 'CurrentUser', text: messageText };
//         currentChatRoom.messages.push(newMessage);
//         displayMessages(currentChatRoom);
//         messageInput.value = '';
//     }
// }

displayChatRooms();
displayOnlineUsers();
displayFriends();

