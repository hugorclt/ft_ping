#include "ft_ping.h"

float	timestamp_history[MAX_MSG];
int	total_received = 0;

int		packet_received = 0;
int		running = 1;	
struct timeval 	start_time;
int		options = 0;

float	find_time_difference(struct timeval *t1, struct timeval *t2) {
	return ((t2->tv_sec - t1->tv_sec) * 1000000 + t2->tv_usec - t1->tv_usec);
}

char	*dns_lookup(char *host) {
	struct hostent	*host_entity;

	host_entity = gethostbyname(host);
	if (!host_entity) {
		return (NULL);
	}

	struct in_addr **address_list = (struct in_addr **)host_entity->h_addr_list;
	return (inet_ntoa(*address_list[0]));
}

t_ping	create_packet(int id) {
	t_ping	ping;

	bzero(&ping, sizeof(ping));
	ping.icmp_hdr.type = 8;
	ping.icmp_hdr.code = 0;
	ping.icmp_hdr.un.echo.id = getpid();
	ping.icmp_hdr.un.echo.sequence = htons(id);
	ping.icmp_hdr.checksum = calculate_checksum((unsigned short*)(&ping.icmp_hdr), sizeof(ping.icmp_hdr));	
	return (ping);
}

int	bind_socket(int sockfd) {
	struct sockaddr_in source;

	source.sin_family = AF_INET;
	source.sin_addr.s_addr = INADDR_ANY;
	if (bind(sockfd, (struct sockaddr *)&source, (socklen_t)sizeof(source)) == -1)
		return (0);
	return (1);
}

struct sockaddr_in config_dest(char *ip) {
	struct sockaddr_in dest;

	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr(ip);
	return (dest);
}

int	send_request(int sockfd, struct sockaddr_in *dest, int id) {
	t_ping	ping = create_packet(id);
	
	gettimeofday(&start_time, NULL);
	if (sendto(sockfd, &ping, sizeof(ping), 0, (struct sockaddr *)dest, (socklen_t)sizeof(*dest)) == -1)
		return (0);
	return (-1);
}

void	display_reply( char *ip, char *buffer, int size) {
	struct ip *reply = (struct ip *)buffer;
	struct icmp_echo* icmp = (struct icmp_echo*)(buffer + IP_HEADER_SIZE);
	struct timeval end_time;

	gettimeofday(&end_time, NULL);
	float time_diff = find_time_difference(&start_time, &end_time) / 1000;

	timestamp_history[total_received] = time_diff;
	total_received += 1;
	printf("%d bytes from %s: icmp seq=%d ttl=%d time=%.3f ms\n", size - IP_HEADER_SIZE, ip, ntohs(icmp->seq), reply->ip_ttl, time_diff);
}

int	recv_response( char *ip, int sockfd, struct sockaddr_in *dest) {
	char	buffer[100];
	socklen_t len = sizeof(struct sockaddr_in);

	bzero(&buffer, sizeof(buffer));
	int size = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)dest, &len); 
	if (size == -1)
		return (0);

	packet_received += 1;
	display_reply( ip, buffer, size);
	return (1);
}

int check_argument(int ac, char **av) {
	if (ac == 2)
		return (0);
	if (ac == 3) {
		if (strcmp(av[1], "-?") == 0 || strcmp(av[2], "-?") == 0)
			return (1);
		if (strcmp(av[1], "-v") == 0 || strcmp(av[2], "-v") == 0)
			return (2);
	}
	return (-1);
}

char	*find_ip(int ac, char **av) {
	if (ac == 2)
		return (av[1]);
	if (ac == 3) {
		if (strcmp(av[1], "-?") == 0 || strcmp(av[1], "-v") == 0)
			return (av[2]);
		else
			return (av[1]);
	}
	return (NULL);
}

void	print_help() {
	printf("Usage: ping [OPTION...] HOST ...\nSend ICMP ECHO_REQUEST packets to network hosts.\n\n-? open the help menu\n-v verbose\n");
}

int main(int ac, char **av) {	
	signal(SIGINT, interrupt_handler);

	char	*host = find_ip(ac, av);
	if (host == NULL)
		return (0);

	options = check_argument(ac, av);
	if (options == -1)
		return (0);

	if (options == 1) {
		print_help();
		return (1);
	}

	int	id = 0;
	char	*ip_dest = dns_lookup(host);
	
	if (!ip_dest) {
		printf("Invalid hostname\n");
		return (0);
	}
	
	int	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (!bind_socket(sockfd)) {
		close(sockfd);
		return (0);
	}


	struct sockaddr_in dest = config_dest(ip_dest);
	if (options == 0)
		printf("PING %s (%s): 56 data bytes\n", host, ip_dest);
	else
		printf("PING %s (%s): 56 data bytes, id 0x%x = %d\n", host, ip_dest, getpid(), getpid());
	while (running) {
		if (!send_request(sockfd, &dest, id)) {
			close(sockfd);
			return (0);
		}

		if (!recv_response( ip_dest, sockfd, &dest)) {
			close(sockfd);
			return (0);
		}
		id = id + 1;
		sleep(1);
	}
	print_end(id, packet_received, av[1], timestamp_history);
	close(sockfd);
	return (1);
}
