#include "ft_ping.h"
#include <math.h>

void	interrupt_handler(int sig) {
	(void)sig;
	running = 0;
}

float calculateSD(float data[MAX_MSG], int max) {
    float sum = 0.0, mean, SD = 0.0;
    int i;
    for (i = 0; i < max; ++i) {
        sum += data[i];
    }
    mean = sum / max;
    for (i = 0; i < max; ++i) {
        SD += pow(data[i] - mean, 2);
    }
    return sqrt(SD / max);
}


float average(float arr[MAX_MSG], int max) {
   float sum = 0;
   int i;

   for (i = 0; i < max; i++) {
      sum += arr[i];
   }
   return (float)sum/5;
}

float find_max(float arr[MAX_MSG], int max) {
	float ret = -1;

	for (int i = 0; i < max; i++) {
		if (ret < arr[i] || ret == -1)
			ret = arr[i];
	}
	return (ret);
}

float find_min(float arr[MAX_MSG], int max) {
	float ret = -1;
	
	for (int i = 0; i < max; i++) {
		if (ret > arr[i] || ret == -1)
			ret = arr[i];
	}
	return (ret);
}

void	print_end(int sent, int received, char *target, float history[MAX_MSG]) {
	printf("--- %s ping statistics ---\n", target);
	printf("%d packets transmitted, %d packets received, %d packet loss\n", sent, received, sent - received);
	printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", find_min(history, received), average(history, received), find_max(history, received), calculateSD(history, received));
}
