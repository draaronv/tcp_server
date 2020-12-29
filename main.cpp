#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
using namespace std;

int main()
{
    //Create socket
    int listening=socket(AF_INET,SOCK_STREAM,0);
    if(listening==-1)
    {
        cerr<<"Can't create a socket";
        return -1;
    }
    //Binding Socket
    sockaddr_in hint;
    hint.sin_family=AF_INET;
    hint.sin_port=htons(54000);
    inet_pton(AF_INET,"0.0.0.0",&hint.sin_addr);
    if( bind(listening,(sockaddr*)&hint,sizeof(hint)) == -1)
    {
        cerr <<"Can't bind to IP/port ";
        return -2;
    }

    //Listening
    if(listen(listening,SOMAXCONN)==-1)
    {
        cerr<<"Can't listen"<<endl;
        return -3;
    }

    // Accept the Call
    sockaddr_in client;
    socklen_t clientSize=sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    int clientSocket=accept(listening,(sockaddr *)&client,&clientSize);
    if(clientSocket==-1)
    {
        cerr<<"Problem with client connecting";
        return -4;
    }
    close(listening);
    memset(host,0,NI_MAXHOST);
    memset(svc,0,NI_MAXSERV);
    int result=getnameinfo((sockaddr*)&client,clientSize,host,NI_MAXHOST,svc,NI_MAXSERV,0);
    if(result)
    {
        cout<<host<<"  connected on "<<svc<<endl;
    }
    else
    {
        inet_ntop(AF_INET,&client.sin_addr,host,NI_MAXHOST);
        cout<<host<< " connected on "<<ntohs(client.sin_port)<<endl;
    }

    //Sending Messages to the server and displaying the message
    char buf[4096];
    while(true)
    {
        // Clear the message
        memset(buf,0,4096);
        //wait for messages
        int bytesReceived=recv(clientSocket,buf,4096,0);
        if(bytesReceived==-1)
        {
            cerr<<"There was a connection issue"<<endl;
            break;
        }
        if(bytesReceived==0)
        {
            cout<<"The client disconnected"<<endl;
            break;
        }
        // Display message
        cout<<"Received: "<<string(buf,0,bytesReceived)<<endl;
        //Resend Message
        send(clientSocket,buf,bytesReceived+1,0);
    }

    // Close the socket
    close(clientSocket);
    return 0;

}
