#include "../include/ft_traceroute.h"

void	display_result(t_env *env, struct sockaddr_in *from_addr)
{
	struct	addrinfo	hints;
	struct	addrinfo	*res;
	char	host[NI_MAXHOST];

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(inet_ntoa(from_addr->sin_addr), NULL, &hints, &res) != 0)
		strncpy(host, inet_ntoa(from_addr->sin_addr), NI_MAXHOST);
	else
		if (getnameinfo(res->ai_addr, res->ai_addrlen, host, sizeof(host), NULL, 0, NI_NAMEREQD) != 0)
			strncpy(host, inet_ntoa(from_addr->sin_addr), NI_MAXHOST);

	freeaddrinfo(res);

	printf(" %d  %s (%s)  %.3f ms\n", env->ttl, host, inet_ntoa(from_addr->sin_addr), env->rtt);
}

void	ft_traceroute(t_env* env)
{
	env->ttl = 1;
	while (env->ttl <= MAX_HOPS)
	{
		// Send ICMP packet
		if (send_icmp_packet(env) < 0)
		{
			perror("send_icmp_packet");
			close(env->sockfd);
			exit(EXIT_FAILURE);
		}

		// Receive ICMP packet
		struct sockaddr_in from_addr;
		if (recv_icmp_packet(env, &from_addr) < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				printf(" *  *  *\n"); // Timeout
			}
			else
			{
				perror("recv_icmp_packet");
				close(env->sockfd);
				exit(EXIT_FAILURE);
			}
		}

		// Print result
		display_result(env, &from_addr);

		// Check if destination reached
		if (from_addr.sin_addr.s_addr == env->dest_addr.sin_addr.s_addr)
			break;
		env->ttl++;
	}
}

void	init_socket(t_env *env, char *arg)
{
	struct addrinfo		hints;
	struct addrinfo		*result;
	int					err;
	struct sockaddr_in	*ipv4;

	env->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (env->sockfd < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}

	struct timeval	timeout = {1, 0};
	if (setsockopt(env->sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
	{
		perror("setsockopt");
		close(env->sockfd);
		exit(EXIT_FAILURE);
	}

	hints = (struct addrinfo){0};
	hints.ai_family = AF_INET;
	err = getaddrinfo(arg, NULL, &hints, &result);
	if (err != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
		close(env->sockfd);
		exit(EXIT_FAILURE);
	}
	ipv4 = (struct sockaddr_in *)result->ai_addr;
	memset(&env->dest_addr, 0, sizeof(struct sockaddr_in));
	env->dest_addr.sin_family = AF_INET;
	env->dest_addr.sin_addr.s_addr = ipv4->sin_addr.s_addr;
	freeaddrinfo(result);
}

int main(int argc, char **argv)
{
	t_env	env;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <destination>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	env.rtt = 0;
	init_socket(&env, argv[1]);
	printf("traceroute to %s (%s), %d hops max, %d byte packets\n", argv[1], inet_ntoa(env.dest_addr.sin_addr), MAX_HOPS, PACKET_SIZE);
	ft_traceroute(&env);
	close(env.sockfd);
	return (EXIT_SUCCESS);
}
