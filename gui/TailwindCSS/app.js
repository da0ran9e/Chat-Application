
PORT = 0;
rtd = 9999;
username = "username";
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
        id: 0,
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
        id: 2,
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
    chatRoomList.innerHTML = "";
    chatRooms.forEach(room => {
        var chatRoomBtn = document.createElement("div");
        chatRoomBtn.className = "flex justify-between items-center p-3 hover:bg-gray-800 rounded-lg relative";
        chatRoomBtn.innerHTML = `
        <div class="w-16 h-16 relative flex flex-shrink-0">
        <img class="shadow-md rounded-full w-full h-full object-cover"
        src="img3.png"
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
                             src="img2.png"
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
                    src="img1.png"
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

function displayChatMemberx(room){
    var memberListBody = document.createElement("div");
    memberListBody.className = "relative z-10";
    memberListBody.innerHTML = `<div class="fixed inset-0 bg-gray-900 bg-opacity-0 transition-opacity"></div>
  
    <div class="fixed inset-0 z-10 w-screen overflow-y-auto">
        <div class="flex min-h-full items-end justify-center p-4 text-center sm:items-center sm:p-0">
            <div class="relative transform overflow-hidden rounded-lg bg-gray-800 text-left shadow-xl transition-all sm:my-8 sm:80 sm:max-w-lg">
                <div class="flex min-h-full flex-1 flex-col justify-center px-6 py-12 lg:px-8">
                    
                    <ul id="mem_list" class="max-w-md divide-y divide-gray-200 dark:divide-gray-700">
                        <li class="pb-3 sm:pb-4">
                            <div class="flex items-center space-x-4 rtl:space-x-reverse">
                                <div class="flex-shrink-0">
                                    <img class="w-8 h-8 rounded-full" src="img.png" alt="Neil image">
                                </div>
                                <div class="flex-1 min-w-0">
                                    <p class="text-sm font-medium text-white truncate dark:text-white">
                                        Member name
                                    </p>
                                    
                                </div>
                                <div class="inline-flex items-center text-base font-semibold text-gray-700 dark:text-white">
                                    member
                                </div>
                            </div>
                        </li>
                    </ul>
                    <div class="bg-gray-50 px-4 py-3 sm:flex sm:flex-row-reverse sm:px-6">
                <button onclick=list_close() id="close_button" type="button" class="inline-flex w-full justify-center rounded-md bg-green-600 px-3 py-2 text-sm font-semibold text-white shadow-sm hover:bg-gray-900 sm:ml-3 sm:w-auto">Close</button>
            </div>
                </div>
            </div>
        </div>
    </div>`

    document.body.appendChild(memberListBody);
    
    var memberListTable = document.getElementById("memlist");
    chatRooms.forEach(chatRoom=>{
        if(chatRoom.name == room){
            chatRoom.member.forEach(m=>{
                
            })
        }
    })
}

function list_close(){
    var closeBtn = document.getElementById()
}

function changeChatRoom(chatRoom) {
    document.getElementById('chat-name').textContent = chatRoom.name;
    document.getElementById('chat-status').textContent = chatRoom.id;
    displayMessages(chatRoom);
}

document.getElementById("messageInput").addEventListener("keydown", function (event) {
    if (event.key === "Enter") {
        sendMessage();
    }
});

function showNotification(friend){
    document.getElementById("friend-request-name").innerHTML = friend;
    document.getElementById("friend-notification").style.display = "block";
}

function updateData(){

}

function displayChatMember(){
    document.getElementById("member-list-window").style.display = "block"

}

function addMemberDisplay(){

}

function removeMemberDisplay(){

}

function exitChat(){

}

function addMember(member, room){

}

function removeMember(member, room){

}

function accept_friend(name){

}

function deny_friend(name){

}

function createRoom(name){

}

displayChatRooms();
displayOnlineUsers();
displayFriends();
//showNotification("quinn");
setInterval(displayOnlineUsers, 1000);
setInterval(displayFriends, 1000);
setInterval(testList, 2000);

function testList(){   
    const data = '{ "friend": ['+
        '{"name": "friend1"},'+
        '{"name": "friend2"},'+
        '{"name": "friend3"} ]}';
    const obj = JSON.parse(data);
    friends = obj.friend;
    onlineUsers = [
        {name: "friend3", friend: true},
        {name: "user2", friend: false},
        {name: "user4", friend: false}
    ];
}