function windowCreateRoom(roomName){
    console.log("Created room: "+roomName);
}

function windowSendMessage(roomId, message){
    console.log("Sent: "+message+" to "+roomId);
}

function windowAddMember(user, roomId){
    console.log("add done");
}

function windowRemoveMember(user, roomId){
    console.log("remove done");
}

function windowSendFriendRequest(friend){
    console.log("Sent request to: "+friend);
}

function windowResponseRequest(friend, response){
    console.log("Just "+ response+" the request from "+ friend);
}

/////////////////////////////////////////////////////////////////

