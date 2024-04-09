#include "../include/ft_traceroute.h"

void	parse_args(t_env *env, int argc, char **argv)
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <host>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	env->rtt = 0;
	env->max_hops = DEFAULT_MAX_HOPS;
	env->nqueries = DEFAULT_NQUERIES;
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
		else
		{
			env->host = argv[i];
		}
		//else if (strncmp(argv[i], "-t=", 3) == 0)
		//{
		//	env->tos = atoi(argv[i] + 3);
		//	if (env->tos < 0 || env->tos > 255)
		//	{
		//		fprintf(stderr, "Invalid type of service value\n");
		//		exit(EXIT_FAILURE);
		//	}
		//}
		//else if (strncmp(argv[i], "-l=", 3) == 0)
		//{
		//	env->flow_label = atoi(argv[i] + 3);
		//	if (env->flow_label < 0 || env->flow_label > 1048575)
		//	{
		//		fprintf(stderr, "Invalid flow label value\n");
		//		exit(EXIT_FAILURE);
		//	}
		//}
		//else if (strncmp(argv[i], "-i=", 3) == 0)
		//{
		//	env->interface = argv[i] + 3;
		//}
		//else if (strncmp(argv[i], "-p=", 3) == 0)
		//{
		//	env->port = atoi(argv[i] + 3);
		//	if (env->port <= 0 || env->port > 65535)
		//	{
		//		fprintf(stderr, "Invalid port value\n");
		//		exit(EXIT_FAILURE);
		//	}
		//}
		//else if (strncmp(argv[i], "-s=", 3) == 0)
		//{
		//	if (inet_pton(AF_INET, argv[i] + 3, &env->source) != 1)
		//	{
		//		fprintf(stderr, "Invalid source address\n");
		//		exit(EXIT_FAILURE);
		//	}
		//}
		// -i=<interface> Set the interface to use
		// -p=<port> Set the port to use
		// -s=<source> Set the source address
		// -q=<nprobes> Set the number of probes per hop
		// -N=<nqueries> Set the number of queries per probe
		// -t=<tos> Set the type of service
		// -l=<flow_label> Set the flow label
	}
}
