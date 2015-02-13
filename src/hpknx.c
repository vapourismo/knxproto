#include "tests/testfw.h"

// #include <unistd.h>
// #include <signal.h>

externtest(msgbuilder)
externtest(knxnetip)

// inline void display_mem(uint8_t* data, size_t len) {
// 	for (size_t i = 0; i < len; i++)
// 		printf("%02X ", data[i]);

// 	printf("\n");
// }

// typedef struct {
// 	dgramsock sock;
// 	queue* queue;
// } dgram_receiver_info;

// static volatile bool dgram_active = true;

// void* dgram_receiver(dgram_receiver_info* info) {
// 	uint8_t buffer[100];

// 	puts("Receiver started ...");
// 	while (dgram_active) {
// 		if (!dgramsock_ready(info->sock, 1, 0)) {
// 			sleep(1);
// 			continue;
// 		}

// 		ssize_t rv = dgramsock_recv(info->sock, buffer, 100, NULL, 0);

// 		if (rv > 0)
// 			queue_enqueue(info->queue, buffer, rv);
// 		else if (rv < 0)
// 			break;
// 	}

// 	puts("Receiver exited");
// 	pthread_exit(NULL);
// }

// void dgram_signal(int sig) {
// 	if (sig == SIGINT) {
// 		puts("Caught SIGINT");
// 		dgram_active = false;
// 	}
// }

int main(void) {
	runtest(msgbuilder);
	runtest(knxnetip);

	// signal(SIGINT, &dgram_signal);

	// ip4addr local;
	// ip4addr_from_string(&local, "127.0.0.1", 1337);

	// dgramsock sock = dgramsock_create(&local, true);

	// if (!sock)
	// 	return 1;

	// queue incoming;
	// queue_init(&incoming);

	// dgram_receiver_info info = {sock, &incoming};

	// pthread_t receiver_thread;
	// pthread_create(&receiver_thread, NULL, (void* (*)(void*)) &dgram_receiver, &info);

	// while (dgram_active) {
	// 	uint8_t* data;
	// 	ssize_t len = queue_dequeue(&incoming, (void**) &data);

	// 	if (len > 0)
	// 		display_mem(data, len);
	// 	else
	// 		sleep(1);
	// }

	// pthread_join(receiver_thread, NULL);
	// dgramsock_close(sock);

	return 0;
}
