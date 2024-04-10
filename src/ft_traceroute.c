#include "../include/ft_traceroute.h"

double	get_elapsed_time(struct timeval *start_time, struct timeval *end_time)
{
	return ((end_time->tv_sec - start_time->tv_sec) * 1000.0
		+ (end_time->tv_usec - start_time->tv_usec) / 1000.0);
}

uint16_t	calculate_checksum(uint16_t *data, int length)
{
	uint32_t	sum;

	sum = 0;
	while (length > 1)
	{
		sum += *data++;
		length -= 2;
	}

	if (length == 1)
		sum += *(uint8_t *)data;

	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);

	return (uint16_t)~sum;
}

int send_icmp_packet(t_env *env)
{
	t_packet	packet;

	memset(&packet, 0, sizeof(t_packet));

	// Set ICMP header
	packet.header.type = ICMP_ECHO; // Type=
	packet.header.un.echo.id = htons(getpid()); // ID
	packet.header.un.echo.sequence = env->seq++; // Sequence

	packet.header.checksum = calculate_checksum((uint16_t *)&packet, sizeof(t_packet));

	int optval = env->ttl;
	if (setsockopt(env->sockfd, IPPROTO_IP, IP_TTL, &optval, sizeof(int)) < 0)
		return -1;

	gettimeofday(&env->start_time, NULL);

	// Send packet
	if (sendto(env->sockfd, &packet, PACKET_SIZE, 0, (struct sockaddr *)&env->dest_addr, sizeof(struct sockaddr_in)) < 0)
		return -1;

	return 0;
}

int	recv_icmp_packet(t_env *env, struct sockaddr_in *from_addr)
{
	char			buffer[PACKET_SIZE];
	struct iphdr	*ip_header;
	struct icmphdr	*icmp_header;

	memset(buffer, 0, PACKET_SIZE);

	// Receive packet
	if (recvfrom(env->sockfd, buffer, PACKET_SIZE, 0, NULL, NULL) < 0)
		return -1;

	// Get IP header
	ip_header = (struct iphdr *)buffer;
	icmp_header = (struct icmphdr *)(buffer + ip_header->ihl * 4);

	// Get source address
	from_addr->sin_addr.s_addr = ip_header->saddr;

	// Check if response is ICMP_ECHOREPLY
	if (icmp_header->type == ICMP_ECHOREPLY)
		return (0);

	return (1);
}
