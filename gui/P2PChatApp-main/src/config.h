#ifndef CONFIG_H
#define CONFIG_H

#define MAXLINE 255
#define TOKENLEN 32
#define MAXRETRY 5
#define BUFFSIZE 1024

#define UDPTRACKINGADDR "127.0.0.1"
#define UDPTRACKINGPORT 2105

#define SERVERADDR "127.0.0.1"
#define SERVERPORT 1999

#define CLIADDR "127.0.0.1"

#define MAXOFFLINE 4        // MAINTAININTV count
#define MAINTAININTV 2      // Update online list each 2 secs
#define HIMSGINTV 2         // Send HI message each 2 secs
#define ONLINELSTINTV 8    // Retrieve online list 8 secs

#define STX 2               // Start of text character
#define ETX 3               // End of text character

const char * const accountFilePath;

const char * const msgFilePath;

#endif // !CONFIG_H
