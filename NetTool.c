#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<libgen.h>
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/********************************************************************/
/* A simple set of colored message functions. (blue,green,red)      */
/********************************************************************/

void promptM(char * message){
    fprintf(stdout,ANSI_COLOR_CYAN "[*]" ANSI_COLOR_RESET " %s\n",message);
}
void promptS(char * message){
    fprintf(stdout,ANSI_COLOR_GREEN "[*]" ANSI_COLOR_RESET " %s\n",message);
}

void promptE(char * message){
    fprintf(stderr,ANSI_COLOR_RED "[*] %s\n" ANSI_COLOR_RESET ,message);
}

/********************************************************************/
/*Return the string containing  the hostname of a machine           */
/********************************************************************/
char * getHost(struct sockaddr aE,socklen_t lg){
    char * hbuf = malloc(NI_MAXHOST* sizeof(char));
    getnameinfo((struct sockaddr*)&aE,lg,hbuf,sizeof(hbuf),NULL,0,0);
    return hbuf;
}

/********************************************************************/
/* Create a socket in SOCK_STREAM or SOCK_DGRAM                     */
/* 0 = TCP ; 1 = UDP                                                */
/********************************************************************/
int CreateSocket(int protocol){
    promptM("Creating socket...");
    int dS = -1;
    switch(protocol){
        case 0:
            dS = socket(AF_INET,SOCK_STREAM,0);
            break;
        case 1:
            dS = socket(AF_INET,SOCK_DGRAM,0);
            break;
        default:
            fprintf(stderr,ANSI_COLOR_RED "[*] Error please set correct numbers for the function (0 = TCP, 1 = UDP)\n" ANSI_COLOR_RESET);
            exit(1);
            break;
    }
    if(dS == -1){
        perror(ANSI_COLOR_RED "[*] Error while creating socket : " ANSI_COLOR_RESET);
        exit(1);
    }
    promptS("Done.");
    return dS;
}

/********************************************************************/
/* Bind a socket with the givin port                                */
/********************************************************************/
int BindSocket(int dS,char * argv){
    promptM("Binding...");
    struct sockaddr_in ad;
    ad.sin_family = AF_INET;
    ad.sin_addr.s_addr = INADDR_ANY;
    ad.sin_port = htons((short)atoi(argv));
    int dB = bind(dS,(struct sockaddr*)&ad,sizeof(ad));
    if(dB == -1){
        perror("Error while binding socket : ");
        exit(1);
    }
    promptS("Done.");
    return dB;
}
/********************************************************************/
/* Listen on a giving socket                                        */
/********************************************************************/
int Listen(int dS){
    promptS("Starting listener...\n");
    int dL = listen(dS,10);
    if(dL == -1){
        perror("Error listening on the socket : ");
        exit(1);
    }
    return dL;
}

/********************************************************************/
/* Accepting a TCP connection on a socket with an empty addrinfo    */
/********************************************************************/
int AcceptTCP(int dS,struct sockaddr_storage adCli, socklen_t adCliLength){
    int dSC = accept(dS,(struct sockaddr *)&adCli,&adCliLength);
    if(dSC == -1){
        perror("Error accepting a client : ");
        exit(1);
    }
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    getnameinfo((struct sockaddr *)&adCli,adCliLength,hbuf,sizeof(hbuf), sbuf,sizeof(sbuf),0);
    printf("[*] Starting interaction with %s:%s...\n",hbuf,sbuf);

    return dSC;
}

/********************************************************************/
/* Connect to a TCP server with a hostname/IP and a port            */
/********************************************************************/
int CConnect(int dS,char* server, char* port){
    struct addrinfo * addressInfo;
    getaddrinfo(server,port,NULL,&addressInfo);
    int dC = connect(dS,addressInfo->ai_addr,addressInfo->ai_addrlen);

    if(dC == -1){
        perror("Error while connecting : ");
        exit(1);
    }
    return dC;
}

/********************************************************************/
/* Send "message" string to a TCP server                            */
/********************************************************************/
void SendMessageTCP(int dS, char * message,int sizeM){
    char buffer[sizeM+1];
    strcpy(buffer,message);
    buffer[sizeM+1] = '\0';
    int dSend = send(dS,buffer,sizeM,0);
    if(dSend == -1){
        perror("Error while sending message : ");
        exit(1);
    }
    return;
}
/********************************************************************/
/* Send an integer to a TCP server                                  */
/********************************************************************/
void SendIntTcp(int dS,int entier){
    int dSend = send(dS,&entier,sizeof(int),0);
    if(dSend == -1){
           perror("Error while sending message : ");
           exit(1);
       }
    return;
}
/********************************************************************/
/* Recive an integer from a TCP server/client                       */
/********************************************************************/
int ReciveInt(int dS){
    int entier;
    int dR = recv(dS,&entier,sizeof(int),0);
    if(dR == -1){
        perror("Error while reciving confirmation : ");
        exit(1);
    }
    return entier;
}

/********************************************************************/
/* Send "message" string to a UDP server with the Hostname/IP and   */
/* port of the UDP server                                           */
/********************************************************************/
void SendMessageUDP(int dS,char * server, char * port,char  * message,int sizeM){
    struct sockaddr_in aD;
    aD.sin_family = AF_INET;
    inet_pton(AF_INET,server,&(aD.sin_addr));
    aD.sin_port = htons(atoi(port));
    socklen_t lgA = sizeof(aD);
    int dSend;
    dSend = sendto(dS,message,sizeM,0,(struct sockaddr*)&aD,lgA);

    if(dSend == -1){
        perror(ANSI_COLOR_RED "Error while sending message :" ANSI_COLOR_RESET);
        exit(1);
    }
    promptS("Success.");
    return;
}
/********************************************************************/
/* Recive one message from a UDP client/server                      */
/********************************************************************/
    int ReciveUDP(int dS,char * buffer){
    struct sockaddr_in aE;
    socklen_t lg = sizeof(aE);
    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
    int dR = recvfrom(dS,buffer,sizeof(buffer),0,(struct sockaddr*)&aE,&lg);
    if(dR == -1){
        perror(ANSI_COLOR_RED "Error while sending message :" ANSI_COLOR_RESET);
        exit(1);
    }
    getnameinfo((struct sockaddr*)&aE,lg,hbuf,sizeof(hbuf), sbuf,sizeof(sbuf),0);
    printf("Receving message from %s:%s. (size : %d)\n",hbuf,sbuf,dR);
    printf("Data : %s\n",buffer);
    return dR;
}
/********************************************************************/
/* Recive a message in TCP form a server/client                     */
/* length is the size of your receving buffer, if set to 0 the size */
/* will be the highest that your socket can recv.                   */
/********************************************************************/
char * ReciveMessage(int dS,socklen_t length){
    if(length == 0){
        int dG = getsockopt(dS,SOL_SOCKET,SO_RCVBUF,NULL,&length);
        if(dG == -1){
            perror("Error while getting socketMAXLENGTH :");
            exit(1);
        }
    }
    char * buffer = malloc(length*sizeof(char));
    memset(buffer,0,length*sizeof(char));
    int dR = recv(dS,buffer,length,0);
    if(dR == -1){
        perror("Error while reciving confirmation : ");
        exit(1);
    }
    printf("[RECIVING] %d Bytes > %s \n",dR,buffer);
    return buffer;
}

/********************************************************************/
/* CURRENTLY IN DEV                 DO NOT LOOK AT THIS MESS XD     */
/********************************************************************/


//Send A file to a server, respecting our protocol
void sendFile(int dS, char *filename){
    FILE *handler = fopen(filename, "r");
    filename = basename(filename);
    char ligne[256];
    bzero(ligne,sizeof(ligne));
    strcat(strcat(ligne,filename),"|");
    //Send the name of the file first...
    int dSend = send(dS,ligne,strlen(ligne),0);
    if(dSend == -1){
        perror("Error while sending message : ");
        exit(1);
    }
    bzero(ligne,sizeof(ligne));
    if(handler){
        int i = 0;
        char c = ' ';

        fseek(handler, 0, SEEK_END);
        //Offset from the first to the last byte, or in other words, filesize
        int file_size = ftell(handler); //Get the file size
        //go back to the start of the file
        rewind(handler);

        //CHECK IF OUR FILESIZE IS HIGHER THAN THE BUFFER
        if(file_size <=256){
            char * buffer = (char*) malloc(sizeof(char) * (file_size + 1) );
            int read_size = fread(buffer, sizeof(char), file_size, handler);
            if (file_size != read_size){
                // Something went wrong, throw away the memory and set the buffer to NULL
                free(buffer);
                buffer = NULL;
            }
            else{
                strcat(ligne,buffer);
                int dSend = send(dS,ligne,strlen(ligne),0);
                if(dSend == -1){
                    perror("Error while sending message : ");
                    exit(1);
                }
            }
        }
        //WE SEND 256 BYTES OF THE FILE UNTIL IT'S EMPTY
        else {
            while((c = fgetc(handler)) != EOF){
                if(i == 255){
                    int dSend = send(dS,ligne,strlen(ligne),0);
                    if(dSend == -1){
                        perror("Error while sending message : ");
                        exit(1);
                    }
                    printf("[SENDING] %s",ligne);
                    bzero(ligne,sizeof(ligne));
                    i = 0;
                }
                ligne[i] = c;
                i++;
            }
            int dSend = send(dS,ligne,strlen(ligne),0);
            if(dSend == -1){
                perror("Error while sending message : ");
                exit(1);
            }
            printf("[SENDING] %s",ligne);
        }
        fclose(handler);
    }
}
//IN OUR FTP,THE NAME OF THE FILE IS LOCATED AT THE BEGENNING FOLLOWED BY '|'
char * getFileName(char * buffer){
    char * fileName = malloc(256*sizeof(char));
    for(int i = 0;i<strlen(buffer);i++){
        if(buffer[i] == '|'){
            return fileName;
        }
        fileName[i] = buffer[i];
    }
    return fileName;
}

//Recive a file respecting our protocol
void Recivefile(int dS){
    char buffer[256];
    bzero(buffer,sizeof(buffer));
    //We know the the first 256 bytes will contain the filename because we are in TCP !
    int dR = recv(dS,buffer,256,0);
    if(dR == -1){
        perror("Error while reciving confirmation : ");
        exit(1);
    }
    char * filename = getFileName(buffer);
    char rest[strlen(buffer)];
    bzero(rest,sizeof(rest));
    int j = 0;
    //Getting the rest of the buffer without the name
    for(int i = strlen(filename)+1; i<strlen(buffer);i++){
        rest[j] = buffer[i];
        j++;
    }
    char path[261];
    bzero(path,sizeof(path));
    strcat(strcat(path,"./loot/"),filename);
    FILE * handler = fopen(path,"w");
    bzero(buffer,sizeof(buffer));
    strcat(buffer,rest);
    if(handler){
        while(dR != 0){
            printf("[RECIVING] %d Bytes > %s \n",(int)strlen(buffer),buffer);
            fprintf(handler,"%s",buffer);
            bzero(buffer,sizeof(buffer));
            dR = recv(dS,buffer,256,0);
            if(dR == -1){
                perror("Error while reciving confirmation : ");
                exit(1);
            }
        }
        fclose(handler);
    }
    else{
        perror("Error while opening file : ");
        exit(1);
    }
}
