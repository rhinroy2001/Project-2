/*
** listener.c -- a datagram sockets "server" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/stat.h>

#define MAXBUFLEN 1000

struct client_info{
    char host[INET_ADDRSTRLEN];
    int port;
};

struct client_info get_client_info(struct sockaddr_in *sa){
    struct client_info info = {};
    info.port = ntohs(sa->sin_port);
    inet_ntop(sa->sin_family, &(sa->sin_addr), info.host, INET_ADDRSTRLEN);

    return info;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void* communicateWithSender(char* smtpPortNumber){
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in their_addr;
    socklen_t addr_len;
    char buf[MAXBUFLEN];
    int rv;
    int numbytes;
    char* clientMessage;
    char* replyCode;
    char* helo;
    char* mailFrom;
    char* rcptTo;
    char* data;
    char* parse;
    char* parseToken;
    char* temp;
    FILE *fp;
    int num = 1;
    char path[40];
    char recipient[40];
    char dateTime[40];
    char from[40];
    char to[40];
    char s[INET_ADDRSTRLEN];
    time_t now = time(&now);
    struct tm *ptm = gmtime(&now);
    int portNumberi = atoi(smtpPortNumber);
    char portNumber[40];
    char* mailFromMessage = "MAIL FROM";
    char* rcptToMessage = "RCPT TO";
    char* emptyMessage = "";
    char* dataMessage = "DATA";
    char host[256];
    struct hostent *host_entry;
    char* ip;
    char* prevMessage;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, smtpPortNumber, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        exit(1);
    }

    gethostname(host, sizeof(host));
    host_entry = gethostbyname(host);
    ip = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

    freeaddrinfo(servinfo);

    addr_len = sizeof(their_addr);

    

    


    rv = mkdir("db", 0755);
    
    // while(1){
    //     bzero(buf, MAXBUFLEN);
    //     if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
    //             (struct sockaddr *)&their_addr, &addr_len)) == -1) {
    //             perror("recvfrom");
    //             exit(1);
    //     }
    //     while(strncmp("HELO", buf, 4) != 0){
    //         if(strncmp("HELO", buf, 4) == 0){
    //             replyCode = "250 OK";
    //             if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
    //                 perror("sendto");
    //                 exit(1);
    //             }
    //             break;
    //         }
    //         if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
    //             (struct sockaddr *)&their_addr, &addr_len)) == -1) {
    //             perror("recvfrom");
    //             exit(1);
    //         }
    //     }
    // }

        for(;;){

            replyCode = "";
            bzero(buf, sizeof(buf));
            if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
                (struct sockaddr *)&their_addr, &addr_len)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            struct client_info client = get_client_info(&their_addr);
            if(strncmp("HELO", buf, 4) == 0){
                helo = buf;
                parse = strtok(helo, " ");
                parse = strtok(NULL, " ");
                if(strncmp(parse, "447f22.edu", 10) == 0){
                    replyCode = "250 OK";
                    bzero(buf, sizeof(buf));
                    sprintf(buf, "%s %s greets %s", replyCode, ip, client.host);
                    if((rv = sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                        perror("sendto");
                        exit(1);
                    }
                    printf("%s\n", buf);
                }else{
                    replyCode = "501 DOMAIN NOT SUPPORTED\n";
                    if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                        perror("sendto");
                        exit(1);
                    }
                    printf("%s\n", replyCode);
                }
            
            }else if(strncmp("MAIL FROM", buf, 9) == 0){
                prevMessage = "MAIL FROM";
                replyCode = "250 OK";
                mailFrom = buf;
                parse = strtok(mailFrom, "<");
                while(parse != NULL){
                    temp = parse;
                    parse = strtok(NULL, "<");
                    if(parse == NULL){
                        break;
                    }
                }
                parse = strtok(temp, "@");
                sprintf(from, "From: <%s@447f22.edu>\n", parse);
                parse = strtok(NULL, "@");
                if(strncmp(parse, "447f22.edu>", 11) == 0){
                    if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                        perror("sendto");
                        exit(1);
                    }
                    printf("%s\n", replyCode);
                }else{
                    replyCode = "501 DOMAIN NOT SUPPORTED\n";
                    if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                        perror("sendto");
                        exit(1);
                    }
                    printf("%s\n", replyCode);
                }
            }else if(strncmp("RCPT TO", buf, 7) == 0){
                if(strncmp("MAIL FROM", prevMessage, 10) == 0){
                    prevMessage = "RCPT TO";
                    replyCode = "250 OK";
                    rcptTo = buf;
                    parse = strtok(rcptTo, "<");
                    while(parse != NULL){
                        temp = parse;
                        parse = strtok(NULL, "<");
                        if(parse == NULL){
                            break;
                        }
                    }
                    parse = strtok(temp, "@");
                    strcpy(recipient, parse);
                    sprintf(path, "db/%s", parse);
                    sprintf(to, "To: <%s@447f22.edu>\n", recipient);
                    rv = mkdir(path, 0755);
                    if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                        perror("sendto");
                        exit(1);
                    }
                    printf("%s\n", replyCode);
                }else{
                    replyCode = "503 BAD SEQUENCE OF COMMANDS";
                    if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                        perror("sendto");
                        exit(1);
                    }
                    printf("%s\n", replyCode);
                }
            }else if(strncmp("DATA", buf, 4) == 0){
                if(strncmp("RCPT TO", prevMessage, 8) == 0){
                    prevMessage = "DATA";
                    replyCode = "354 OK";
                    data = buf;
                    if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                        perror("sendto");
                        exit(1);
                    }
                    printf("%s\n", replyCode);
                }else{
                    replyCode = "503 BAD SEQUENCE OF COMMANDS";
                    if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                        perror("sendto");
                        exit(1);
                    }
                    printf("%s\n", replyCode);
                }
            }else if(strncmp(prevMessage, "DATA", 4) == 0){
                prevMessage = "";
                replyCode = "250 OK\n";
                sprintf(path, "db/%s/%d.email", recipient, num);
                num++;
                sprintf(dateTime, "Date: %s", asctime(ptm));
                fp = fopen(path, "w");
                fputs(dateTime, fp);
                fputs(from, fp);
                fputs(to, fp);
                for(int i = 0; i < MAXBUFLEN; i++){
                    fputc(buf[i], fp);
                }
                fclose(fp);
                if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                    perror("sendto");
                    exit(1);
                }
                printf("%s\n", replyCode);

            }else if(strncmp("HELP", buf, 4) == 0){
                prevMessage = "";
                replyCode = "214 OK"; // WRITE HELP MENU
                if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                    perror("sendto");
                    exit(1);
                }
                printf("%s\n", replyCode);
            }else if(strncmp("QUIT", buf, 4) == 0){
                replyCode = "221 BYE";
                if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                    perror("sendto");
                    exit(1);
                }
                printf("%s\n", replyCode);
            }else{
                replyCode = "500 command unrecognized";
                if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                    perror("sendto");
                    exit(1);
                }
                printf("%s\n", replyCode);
            }
        }
    
    close(sockfd);
}

void* commincateWithReceiver(char* httpPortNumber){
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    int rv;
    int numbytes;
    char buf[MAXBUFLEN];
    char s[INET_ADDRSTRLEN];
    int n = 0;
    char* replyCode;
    char dateTime[40];
    char path[40];
    char db[10];
    char recipient[20];
    char* request;
    char* parse;
    char* get;
    char* server;
    char* count;
    char temp[20];
    char host[256];
    char hostname[258];
    int emailCount;
    time_t now = time(&now);
    struct tm *ptm = gmtime(&now);
    
    

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP   

    if ((rv = getaddrinfo(NULL, httpPortNumber, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        exit(1);
    }

    const int so_reuseaddr = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(int));

    freeaddrinfo(servinfo);

    addr_len = sizeof(their_addr); 
    gethostname(host, sizeof(host));
    sprintf(hostname, "<%s>", host);
    
    for(;;){
        replyCode = "";
        bzero(buf, sizeof(buf));
        if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
            perror("recvfrom");
            exit(1);
        }
        
        if(strncmp("GET", buf, 3) == 0){
            request = buf;
            char prev[20];
            parse = strtok(request, "\n");
            while(parse != NULL){
                if(strncmp("GET", parse, 3) == 0){
                    get = parse;
                }
                if(strncasecmp("Server", parse, 6) == 0){
                    server = parse;
                }
                if(strncasecmp("count", parse, 5) == 0){
                    count =  parse;
                }
                parse = strtok(NULL, "\n");
            }

            parse = strtok(get, "/");
            while(parse != NULL){
                strcpy(temp, parse);
                if(strncmp("db", prev, 2) == 0){
                    strcpy(recipient, temp);
                    break;
                }
                if(strncmp("db", temp, 2) == 0){
                    strcpy(db, temp);
                    strcpy(prev, temp);
                }
                parse = strtok(NULL, "/");
            }

            parse = strtok(server, " ");
            parse = strtok(NULL, " ");
            server = parse;

            parse = strtok(count, " ");
            parse = strtok(NULL, " ");
            count = parse;
            emailCount = atoi(count);
            sprintf(dateTime, "%s\n", asctime(ptm));

            if(strcmp(server, hostname) == 0){
                bzero(buf, sizeof(buf));
                
                sprintf(buf, "HTTP/1.1 200 OK\nLast-Modified: %s\nCount: %d\nContent-Type: text/plain\n\n", dateTime, emailCount);
                for(int i = 1; i <= emailCount; i++){
                    int n = 0;
                    sprintf(path, "%s/%s/%d.email", db, recipient, i);
                    char fbuf[1000];
                    FILE *fp;
                    fp = fopen(path, "r");
                    if(fp != NULL){
                        do{
                            fbuf[n] = fgetc(fp);
                            if(feof(fp)){
                                break;
                            }
                            n++;
                        }while(1);
                        fclose(fp);
                        strcat(buf, fbuf);
                    }else{
                        replyCode = "404 FILE DOES NOT EXIST\n";
                        if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                            perror("sendto");
                            exit(1);
                        }
                        printf("%s\n", replyCode);
                    }
                }
                if((rv = sendto(sockfd, buf, MAXBUFLEN, 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                    perror("sendto");
                    exit(1);
                }
                printf("%s\n", buf);
            }else{
                replyCode = "400 BAD REQUEST\n";
                if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                    perror("sendto");
                    exit(1);
                }
                printf("%s\n", replyCode);
            }
        }else{
            replyCode = "400 BAD REQUEST\n";
            if((rv = sendto(sockfd, replyCode, strlen(replyCode), 0, (struct sockaddr *)&their_addr, addr_len)) == -1){
                perror("sendto");
                exit(1);
            }
            printf("%s\n", replyCode);
        }
    }
    close(sockfd);
}

int main(int argc, char* argv[])
{
    char smtpPort[50];
    char httpPort[50];
    char* temp;
    char* smtpPortToken;
    char* smtpPortNumber;
    char* httpPortToken;
    char* httpPortNumber;
    char* ipAddress;
    int count = 0;
    pthread_t tid[10];

    FILE* file = fopen(argv[1], "r");
    if(file == NULL){
    	perror("Cannot open the file");
	    exit(1);
    }
    fscanf(file, "%s %s", smtpPort, httpPort);
    smtpPortToken = strtok(smtpPort, "=");
    while(smtpPortToken != NULL){
        temp = smtpPortToken;
        smtpPortToken = strtok(NULL, "=");
        if(smtpPortToken == NULL){
            smtpPortNumber = temp;
        }
    }
    httpPortToken = strtok(httpPort, "=");
    while(httpPortToken != NULL){
        temp = httpPortToken;
        httpPortToken = strtok(NULL, "=");
        if(httpPortToken == NULL){
            httpPortNumber = temp;
        }
    }

    // char *httpSocket = malloc(sizeof(char));
    // *httpSocket = httpPortNumber;

    // int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    // int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    // struct sockaddr_in sockaddr_to;
    // fill sockaddr

    
    // pthread_create(&tid[count++], NULL, commincateWithReceiver, (void*)httpSocket);
    // pthread_create(&tid[count++], NULL, communicateWithSender, (void*)smtpPortNumber);

    communicateWithSender(smtpPortNumber);
    commincateWithReceiver(httpPortNumber);


    // close(sockfd);

    return 0;
}