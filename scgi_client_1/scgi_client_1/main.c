#include <stdio.h>
#include <stdlib.h> //exit()
#include <unistd.h> //close()
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> //inet_aton()
#include <sys/event.h>

#define BUF_SIZE 256

int main(int argc, const char * argv[]) {
    
    char msg[256] = "70:\nCONTENT_LENGTH <00> 27 <00>\nSCGI <00> 1 <00>\nREQUEST_METHOD <00> POST <00>\nREQUEST_URI <00> /deepthought <00>\n,\nWhat is the answer to life?";
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    inet_aton("127.0.0.1", &addr.sin_addr);
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1){
        printf("connect error");
        exit(0);
    }
    
    
    struct kevent event;     /* событие, которое мы хотим отслеживать */
    struct kevent tevent;     /* произошедшее событие */
    int kq = kqueue();
    if (kq == -1){
        printf("kqueue error");
        exit(0);
    }
    
    EV_SET(&event, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
    kevent(kq, &event, 1, NULL, 0, NULL);
    
    
    while(1){
        if (kevent(kq, NULL, 0, &tevent, 1, 0) == 1)
            if (send(fd, msg, sizeof(msg), 0) == -1){
                printf("send error");
                exit(0);
            }
            break;
        //printf("%d\n", res);
    }
    
    EV_SET(&event, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(kq, &event, 1, NULL, 0, NULL);
    
    while(1){
        if (kevent(kq, NULL, 0, &tevent, 1, 0) == 1){
            if (recv(fd, msg, sizeof(msg), 0) == -1){
                printf("recv error");
                exit(0);
            }else{
                printf("%s\n", msg);
                break;
            }
        }
    }
    
    //printf("%s\n", msg);
    
    close(fd);
    return 0;
}
