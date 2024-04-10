#include "../include/ft_traceroute.h"

void	init_default_env(t_env *env)
{
	env->ttl = 1; // time to live
	env->rtt = 0; // round-trip time
	env->max_hops = DEFAULT_MAX_HOPS; // max number of hops
	env->nqueries = DEFAULT_NQUERIES; // number of queries per hop
	env->tos = 8; // 8 = IPTOS_LOWDELAY | 16 = IPTOS_THROUGHPUT | 32 = IPTOS_RELIABILITY | 128 = IPTOS_MINCOST
	env->seq = 0; // ICMP sequence number
	env->source.s_addr = INADDR_ANY; // source address
}

void	parse_args(t_env *env, int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <host>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	init_default_env(env);
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--help") == 0)
		{
			printf("Usage:\n");
			printf(" ft_traceroute [options] <host>\n");
			printf("Options:\n");
			printf(" <host> Destination IP address or hostname\n");
			printf(" --help       Display this information\n");
			printf(" -m=<max_hops> Set the max number of hops\n");
			exit(EXIT_SUCCESS);
		}
		else if (strncmp(argv[i], "-m=", 3) == 0)
		{
			env->max_hops = atoi(argv[i] + 3);
			if (env->max_hops <= 0)
			{
				fprintf(stderr, "Invalid max hops value\n");
				exit(EXIT_FAILURE);
			}
		}
		else if (strncmp(argv[i], "-q=", 3) == 0)
		{
			env->nqueries = atoi(argv[i] + 3);
			if (env->nqueries <= 0)
			{
				fprintf(stderr, "Invalid number of queries value\n");
				exit(EXIT_FAILURE);
			}
		}
		else if (strncmp(argv[i], "-t=", 3) == 0)
		{
			env->tos = atoi(argv[i] + 3);
			if (env->tos < 0 || env->tos > 255)
			{
				fprintf(stderr, "Invalid type of service value\n");
				exit(EXIT_FAILURE);
			}
		}
		else if (strncmp(argv[i], "-p=", 3) == 0)
		{
			env->seq = atoi(argv[i] + 3);
			if (env->seq <= 0 || env->seq > 65535)
			{
				fprintf(stderr, "Invalid port value\n");
				exit(EXIT_FAILURE);
			}
		}
		else if (strncmp(argv[i], "-s=", 3) == 0)
		{
			if (inet_pton(AF_INET, argv[i] + 3, &env->source) != 1)
			{
				fprintf(stderr, "Invalid source address\n");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			env->host = argv[i];
		}
		//else if (strncmp(argv[i], "-i=", 3) == 0)
		//{
		//	env->interface = argv[i] + 3;
		//}
		// -i=<interface> Set the interface to use
		// -p=<port> Set the port to use
		// -s=<source> Set the source address
		// -q=<nprobes> Set the number of probes per hop
		// -N=<nqueries> Set the number of queries per probe
		// -t=<tos> Set the type of service
	}
}
