#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
char * get_message(int seq_num, long double millisecondsSinceEpoch);
void print_rtt(long double rtt, int seq_num, char *ip);
long double get_time();

int main(int argc, char * argv[]) {
	struct sockaddr_in server_address, return_address;
	
	char * server_ip = malloc(400); 
	strcpy(server_ip, argv[1]);
	
	int port = atoi(argv[2]);
	int i = 1;
	int socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr( server_ip );
 	server_address.sin_port = htons(port);
	long double total_rtt = 0;
	int total_received = 0;
	int packets_lost = 0;
	long double min_rtt = 10000;
	long double max_rtt = 0;


	for(int i=0; i<10; i++){
		long double send_time = get_time(); 
		const char * message = get_message(i, send_time);
	
		// printf("Sending '%s' to %s:%i\n", message, server_ip, port);
		sendto( socket_id, message, strlen(message), 0,
			(struct sockaddr *) &server_address, sizeof(struct sockaddr_in) );
		//printf("Waiting for Response\n");
		
		struct timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		setsockopt(socket_id, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv));
		unsigned int return_len;
		int result = recvfrom( socket_id, message, strlen(message), 0,
			(struct sockaddr *) &return_address, &return_len );
		if (result<0)
		{
			printf("Request timeout for seq#=%d\n", i);
			packets_lost++;
		}
		else
		{
			
			long double return_time = get_time();
			long double rtt = return_time - send_time;

			printf("PING received from %s: seq #=%d time=%.3Lf ms\n", server_ip, i, rtt);
			total_rtt+= rtt;
			total_received++;
			if (rtt<min_rtt)
				min_rtt = rtt;
			if (rtt>max_rtt)
				max_rtt = rtt;

		}
	}
	int packet_loss = 100*(packets_lost/10.00);
	long double average = total_rtt/((double)total_received);
	
	if (total_received>0)
		printf("10 packets transmitted, %d recieved, %d%% packet loss rtt min/avg/max = %.3Lf %.3Lf %.3Lf ms\n", total_received, packet_loss, min_rtt, average, max_rtt);	
	else
		printf("10 packets transmitted, %d recieved, 100%% packet loss rtt min/avg/max = undefined undefined undefined ms\n", total_received);	

	close(socket_id);
}

long double get_time(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
 	long double millisecondsSinceEpoch =
		(long double)(tv.tv_sec) * 1000 +
 		(long double)(tv.tv_usec) / 1000;
 	return millisecondsSinceEpoch;
}


char * get_message(int seq_num, long double millisecondsSinceEpoch){
	char* time = malloc(100);
	sprintf(time, "%Lf", millisecondsSinceEpoch);
	char * message= malloc(400);
	char * number = malloc(4);
	sprintf(number, "%d", seq_num);
	strcpy(message, "PING ");
	strcat(message, number);
	strcat(message, " ");
	strcat(message, time);
	

	return message;
}