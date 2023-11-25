#include "../include/ft_traceroute.h"

int main(int argc, char **argv)
{
	t_env	env;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <destination>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	env.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (env.sockfd < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	memset(&env.dest_addr, 0, sizeof(env.dest_addr));
	env.dest_addr.sin_family = AF_INET;
	if (inet_pton(AF_INET, argv[1], &env.dest_addr.sin_addr) != 1)
	{
		perror("inet_pton");
		close(env.sockfd);
		exit(EXIT_FAILURE);
	}

	printf("traceroute to %s (%s), %d hops max, %d byte packets\n", argv[1], inet_ntoa(env.dest_addr.sin_addr), MAX_HOPS, PACKET_SIZE);

	env.ttl = 1;
	while (env.ttl <= MAX_HOPS)
	{
		// Send ICMP packet
		if (send_icmp_packet(&env) < 0)
		{
			perror("send_icmp_packet");
			close(env.sockfd);
			exit(EXIT_FAILURE);
		}

		// Receive ICMP packet
		struct sockaddr_in from_addr;
		if (recv_icmp_packet(&env, &from_addr) < 0)
		{
			perror("recv_icmp_packet");
			close(env.sockfd);
			exit(EXIT_FAILURE);
		}

		// Print result
		printf(" %d\t%s (%s)\t%.3f ms\n", env.ttl, inet_ntoa(from_addr.sin_addr), inet_ntoa(env.dest_addr.sin_addr), env.rtt);

		// Check if destination reached
		if (from_addr.sin_addr.s_addr == env.dest_addr.sin_addr.s_addr)
			break;
		env.ttl++;
	}

	close(env.sockfd);
	return (EXIT_SUCCESS);
}
