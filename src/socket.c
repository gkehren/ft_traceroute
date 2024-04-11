#include "../include/ft_traceroute.h"

void	set_sockopts(t_env *env)
{
	struct timeval	timeout = {1, 0};
	if (setsockopt(env->sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
	{
		perror("setsockopt");
		close(env->sockfd);
		exit(EXIT_FAILURE);
	}
	if (setsockopt(env->sockfd, IPPROTO_IP, IP_TOS, &env->tos, sizeof(env->tos)) < 0)
	{
		perror("setsockopt");
		close(env->sockfd);
		exit(EXIT_FAILURE);
	}
	if (env->interface)
	{
		if (setsockopt(env->sockfd, SOL_SOCKET, SO_BINDTODEVICE, env->interface, strlen(env->interface)) < 0)
		{
			perror("setsockopt");
			close(env->sockfd);
			exit(EXIT_FAILURE);
		}
	}
}

void	resolve_hostname(t_env *env)
{
	struct addrinfo		hints;
	struct addrinfo		*result;
	int					err;
	struct sockaddr_in	*ipv4;

	hints = (struct addrinfo){0};
	hints.ai_family = AF_INET;
	err = getaddrinfo(env->host, NULL, &hints, &result);
	if (err != 0)
	{
		fprintf(stderr, "%s: %s\n", env->host, gai_strerror(err));
		close(env->sockfd);
		exit(EXIT_FAILURE);
	}
	ipv4 = (struct sockaddr_in *)result->ai_addr;
	memset(&env->dest_addr, 0, sizeof(struct sockaddr_in));
	env->dest_addr.sin_family = AF_INET;
	env->dest_addr.sin_addr.s_addr = ipv4->sin_addr.s_addr;
	freeaddrinfo(result);
}

void	init_socket(t_env *env)
{
	env->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (env->sockfd < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	set_sockopts(env);

	struct sockaddr_in src_addr;
	memset(&src_addr, 0, sizeof(struct sockaddr_in));
	src_addr.sin_family = AF_INET;
	src_addr.sin_addr = env->source;
	if (bind(env->sockfd, (struct sockaddr *)&src_addr, sizeof(struct sockaddr_in)) < 0)
	{
		perror("bind");
		close(env->sockfd);
		exit(EXIT_FAILURE);
	}
	resolve_hostname(env);
}
