#include <linux/input.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>               //SERVER
#include <netdb.h>                    //RECEIVE EVENT
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <malloc.h>

struct input_event ev[64];
size_t ies = sizeof(struct input_event);

struct recv_num_b{
	int num;
	int received;
};

int sock_err(const char *function){

	int err = errno;
	fprintf(stderr, "%s: socket error: %d\n", function, err);
	return 0;

}

int create_socket(int argc, char* argv[]){

	unsigned int ip_addr = INADDR_NONE;
	struct sockaddr_in addr;

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(fd < 0)
		return sock_err("socket");

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(argv[1]));
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		return sock_err("bind");

	return fd;

}

void recv_ev(int fd, struct recv_num_b *recv_num_b){

	struct sockaddr_in addr;
	int addrlen = sizeof(addr), count;
	char num[32];

	count  = recvfrom(fd, &num, 32, 0, (struct sockaddr*)&addr, &addrlen);
	recv_num_b->num = atoi(num);
	//printf("recvfrom : %d\n", count);

	recv_num_b->received = recvfrom(fd, &ev, ies * 64, 0, (struct sockaddr*)&addr, &addrlen);
	//printf("recvfrom : %d\n", recv_num_b->received);

}

int main(int argc, char* argv[]){


    size_t rb, wb, received;
    int fd_socket;
    int fd[100];

    if(argc < 2)
    	    return sock_err("few arguments");
    
    fd_socket = create_socket(argc, argv);
    if(fd_socket == 0){
	    perror("create socket");
	    exit(1);
    }
   
    char dev[50] = "/dev/input/";
    int size_dev = strlen(dev);

    for(int i = 2; i < argc; i++){

    	strcat(dev, argv[i]);
		if ((fd[i - 2] = open(dev, O_WRONLY)) < 0) {
			perror("evdev open");
       			exit(1);
		}
		dev[size_dev] = '\0';

    }

    struct recv_num_b recv_num_b;

    while(1){
        
		recv_ev(fd_socket, &recv_num_b);
		write(fd[recv_num_b.num], &ev, recv_num_b.received);

    }

    return 0;
}	
