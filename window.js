function login(usn, psw){
    window.login(usn, psw).then(result => {
        if (result == 201){
            username = getElementById("username").value
            updateData();
            showMainBody();
        } else {
            document.getElementById("username").value = "";
            document.getElementById("password").value = "";
        }
    })
}

function register(usn, psw){
    window.register(usn, psw).then(result => {
        if (result == 211){
            username = getElementById("username").value
            updateData();
            showMainBody();
        } else {
            document.getElementById("username").value = "";
            document.getElementById("password").value = "";
        }
    })
}

function updateData(){
    window.updateData().then(result => {
        PORT = result.port;
        rtd = result.rtd;
        onlineUsers = result.onlineUsers;
        friends = result.friends;
        chatRooms = result.chatRooms;
    });
}

function addMember(member, room){
    window.addMember(member, room).then(result=>{});
}

function removeMember(member, room){
    window.removeMember(member, room).then(result=>{});
}

function leaveChat(){
    removeMember(username, room);
}

function logout(){
    window.logout();
}

function sendMessage(message, room){
    window.sendMessage(message, room).then(result=>{});
}

