#include "../include/ft_traceroute.h"

void	display_result(t_env *env, struct sockaddr_in *from_addr)
{
	static char prev_host[NI_MAXHOST];
	char	host[NI_MAXHOST];
	struct	addrinfo	hints;
	struct	addrinfo	*res;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(inet_ntoa(from_addr->sin_addr), NULL, &hints, &res) != 0)
		strncpy(host, inet_ntoa(from_addr->sin_addr), NI_MAXHOST);
	else
		if (getnameinfo(res->ai_addr, res->ai_addrlen, host, sizeof(host), NULL, 0, NI_NAMEREQD) != 0)
			strncpy(host, inet_ntoa(from_addr->sin_addr), NI_MAXHOST);

	freeaddrinfo(res);

	if (strcmp(prev_host, host) != 0)
	{
		printf(" %s (%s)  %.3f ms ", host, inet_ntoa(from_addr->sin_addr), env->rtt);
		strncpy(prev_host, host, NI_MAXHOST);
	}
	else
		printf(" %.3f ms ", env->rtt);
}

void	ft_traceroute(t_env* env)
{
	int reached = 0;
	while (env->ttl <= env->max_hops)
	{
		if (reached)
			break;
		for (int i = 0; i < env->nqueries; i++)
		{
			// Send ICMP packet
			if (send_icmp_packet(env) < 0)
			{
				perror("send_icmp_packet");
				close(env->sockfd);
				exit(EXIT_FAILURE);
			}

			printf(" %d ", env->ttl);
			// Receive ICMP packet
			struct sockaddr_in from_addr;
			if (recv_icmp_packet(env, &from_addr) < 0)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					printf(" * "); // Timeout
				else
				{
					perror("recv_icmp_packet");
					close(env->sockfd);
					exit(EXIT_FAILURE);
				}
			}
			else
			{
				// Print result
				gettimeofday(&env->end_time, NULL);
				env->rtt = get_elapsed_time(&env->start_time, &env->end_time);
				display_result(env, &from_addr);
			}

			// Check if destination reached
			if (from_addr.sin_addr.s_addr == env->dest_addr.sin_addr.s_addr)
				reached = 1;
		}
		printf("\n");
		env->ttl++;
	}
}

int main(int argc, char **argv)
{
	t_env	env;

	parse_args(&env, argc, argv);
	init_socket(&env);
	printf("traceroute to %s (%s), %d hops max, %d byte packets\n", env.host, inet_ntoa(env.dest_addr.sin_addr), env.max_hops, PACKET_SIZE);
	ft_traceroute(&env);
	close(env.sockfd);
	return (EXIT_SUCCESS);
}
