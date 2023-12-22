#include "../../include/client/feature.h"

void handle_message(const char * messsge){
    int op = getProtocolOpcode(messsge);
    int func = getProtocolFunctionCode(messsge);
    Parameters params;
    char payload[BUFFER-8];
    int plSize=0;
    getProtocolPayload(messsge, payload, plSize);
    getProtocolParameters(payload, params);

    //handle
}

void _authentication(int func, ){
    //login

    //register
    //changepass
}

void _start_client(){
    //get initial data
    
}
