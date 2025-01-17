#include <linux/input.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <poll.h>
#include <netdb.h>
#include <errno.h>                   //CLIENT
#include <string.h>                  //SEND EVENT TO SERVER
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <malloc.h>
#include <inttypes.h>

#pragma pack(push, 1)
#define EV_MAX_READ 64
#define FD_STR_SIZE 32
#define FREV_SIZE 8

struct input_event ev[64];
struct sockaddr_in addr;

int32_t ies = sizeof(struct input_event);
uint32_t addrlen = sizeof(struct sockaddr_in);

struct frinput_event {
	uint16_t type;
	uint16_t code;
	uint32_t value;
};

int32_t sock_err(const char *function){

	int32_t err = errno;
	fprintf(stderr, "%s: socket error: %" PRId32 "\n", function, err);
	return 0;

}

int32_t create_socket(int32_t argc, char* argv[]){

	uint32_t ip_addr = INADDR_NONE;

	char *addr_str = argv[1]; //IP address + Port;
	char *tmp = strtok(addr_str, ":"); //IP address;
	
	if(tmp != NULL){
		ip_addr = inet_addr(tmp);
		if(ip_addr == INADDR_NONE)
			return sock_err("IP address");
	}else{
		return sock_err("IP address");
	}

	tmp = strtok(NULL, ""); //Port
	if(tmp == NULL)
		return sock_err("No port");

	int32_t fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(fd < 0)
		return sock_err("socket");

	memset(&addr, 0, sizeof(addr)); // information about socket
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(tmp));
	addr.sin_addr.s_addr = ip_addr;

	return fd;

}

void send_ev(int32_t fd, const char *num, size_t rb){

	struct frinput_event frev[64];
	uint32_t n = (uint32_t)(rb / ies);

	for(size_t i = 0; i < n; i++){
		frev[i].type = (uint64_t)ev[i].type;
		frev[i].code = (uint16_t)ev[i].code;
		frev[i].value = (uint32_t)ev[i].value;
	}
	
	sendto(fd, num, FD_STR_SIZE, 0, (struct sockaddr*)&addr, addrlen);
	sendto(fd, &frev, n * FREV_SIZE, 0, (struct sockaddr*)&addr, addrlen);

}

int main(int argc, char* argv[]){


    size_t rb;
    int32_t fd_socket, fd[100];
    int32_t ret;

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
	if ((fd[i - 2] = open(dev, O_RDONLY)) < 0) {
		perror("evdev open");
	    	exit(1);
	}
	dev[size_dev] = '\0';

    }
	    
  
    struct pollfd fds[100];
    int32_t nfds = fd[0];
    char num[32];

    for(size_t i = 0; i < argc - 2; i++){
	    fds[i].fd = fd[i];
	    fds[i].events = POLLIN;
	    if(nfds < fd[i])
		    nfds = fd[i];
    }

    while(1){
	
	ret = poll(fds, nfds, NULL);
	if(ret == -1){
		perror("poll");
		return 1;
	}

	for(size_t i = 0; i < argc - 2; i++){
		if(fds[i].revents & POLLIN){
 			rb = read(fd[i], ev, ies * EV_MAX_READ);
			sprintf(num, "%" PRId32, (int32_t)i);
			send_ev(fd_socket, num, rb);
		}
	}
    
    }


    return 0;
}	
