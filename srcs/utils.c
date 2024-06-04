#include "ft_ping.h"

void	interrupt_handler(int sig) {
	(void)sig;
	running = 0;
}

void	print_end(int sent, int received, char *target) {
	printf("--- %s ping statistics ---\n", target);
	printf("%d packets transmitted, %d packets received, %d packet loss\n", sent, received, sent / received);
	printf("round-trip min/avg/max/stddev = 0.134/0.148/0.161/0.000 ms\n");
}
