#pragma once

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <netdb.h>
#include <netinet/in.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <signal.h>
#include <errno.h>

typedef struct s_ping {
	struct icmphdr 	icmp_hdr;
	char		msg[56];
}	t_ping;

struct icmp_echo {
    // header
    uint8_t type;
    uint8_t code;
    uint16_t checksum;

    uint16_t ident;
    uint16_t seq;

    // data
    double sending_ts;
};

extern int running;
#define IP_HEADER_SIZE 20

#define MAX_MSG 512

unsigned short calculate_checksum(unsigned short* b, int len); 
void	interrupt_handler(int sig);
void	print_end(int sent, int received, char *target,  float history[MAX_MSG]);
