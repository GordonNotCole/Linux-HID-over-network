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
#include <inttypes.h>


#pragma pack(push, 1)
#define EV_MAX_READ 64
#define FD_STR_SIZE 32

struct input_event ev[64];
int32_t ies = sizeof(struct input_event);

struct recv_num_b{
	int32_t num;
	int32_t received;
};

int32_t sock_err(const char *function){

	int32_t err = errno;
	fprintf(stderr, "%s: socket error: %" PRId32 "\n", function, err);
	return 0;

}

int32_t create_socket(int32_t argc, char* argv[]){

	struct sockaddr_in addr;

	int32_t fd = socket(AF_INET, SOCK_DGRAM, 0);
	
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

void recv_ev(int32_t fd, struct recv_num_b *recv_num_b){

	struct sockaddr_in addr;
	uint32_t addrlen = sizeof(addr);
	char num[32];

	recvfrom(fd, &num, FD_STR_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
	
	recv_num_b->num = (int32_t)atoi(num);
	recv_num_b->received = (int32_t)recvfrom(fd, &ev, ies * EV_MAX_READ, 0, (struct sockaddr*)&addr, &addrlen);

}

int main(int argc, char* argv[]){


    int32_t fd_socket, fd[100];

    if(argc < 2)
    	    return sock_err("few arguments");
    
    fd_socket = create_socket(argc, argv);
    if(fd_socket == 0){
	    perror("create socket");
	    exit(1);
    }
   
    char dev[50] = "/dev/input/";
    int32_t size_dev = strlen(dev);

    for(size_t i = 2; i < argc; i++){

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

