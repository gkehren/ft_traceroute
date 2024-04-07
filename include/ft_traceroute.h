#ifndef FT_TRACEROUTE_H
# define FT_TRACEROUTE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <errno.h>

#define MAX_HOPS 30
#define PACKET_SIZE 60

typedef struct	s_packet
{
	struct icmphdr	header;
	char			data[PACKET_SIZE - sizeof(struct icmphdr)];
}	t_packet;

typedef struct	s_env
{
	int					sockfd;
	struct sockaddr_in	dest_addr;
	int					ttl;
	struct timeval		start_time;
	struct timeval		end_time;
	double				rtt;
}	t_env;

int		send_icmp_packet(t_env *env);
int		recv_icmp_packet(t_env *env, struct sockaddr_in *from_addr);
double	get_elapsed_time(struct timeval *start_time, struct timeval *end_time);

#endif
