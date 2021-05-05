// /*
//  * udpclient.c - A simple UDP client
//  * usage: PingClient <host> <port>
//  */
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netdb.h>
// #include <arpa/inet.h>
// #include <netinet/in_systm.h>
// #include <netinet/ip.h>
// #include <netinet/ip_icmp.h>
// #include <fcntl.h>
// #include <signal.h>
// #include <time.h>
// #include <math.h>
// #include <netinet/in.h>
// #define BUFSIZE 1024

// #ifndef SOL_IP
// #define SOL_IP IPPROTO_IP
// #endif
// /*
//  * error - wrapper for perror
//  */
// void error(char *msg)
// {
//   perror(msg);
//   exit(0);
// }

// // Define the Packet Constants
// // ping packet size
// #define PING_PKT_S 64

// // Automatic port number
// #define PORT_NO 0

// // Automatic port number
// #define PING_SLEEP_RATE 1000000

// // Gives the timeout delay for receiving packets
// // in seconds
// #define RECV_TIMEOUT 1

// // Define the Ping Loop
// int pingloop = 1;

// char *toArray(int number)
// {
//   int n = (int)log10(number) + 1;
//   int i;
//   char *numberArray = calloc(n, sizeof(char));
//   for (i = n - 1; i >= 0; --i, number /= 10)
//   {
//     numberArray[i] = (number % 10) + '0';
//   }
//   return numberArray;
// }

// // make a ping request
// void send_ping(int ping_sockfd, struct sockaddr_in *ping_addr, char *rev_host)
// {
//   int ttl_val = 64, msg_count = 0, i, addr_len, flag = 1, msg_received_count = 0;

//   char *message;
//   struct sockaddr_in r_addr;
//   struct timespec time_start, time_end, tfs, tfe;
//   struct timespec time_message;

//   long double rtt_msec = 0, total_msec = 0;
//   struct timeval tv_out;
//   tv_out.tv_sec = RECV_TIMEOUT;
//   tv_out.tv_usec = 0;
//   clock_gettime(CLOCK_MONOTONIC, &time_message);
//   char *t = sprintf("%lld.%.9ld", (long long)time_message.tv_sec, time_message.tv_nsec);

//   // // set socket options at ip to TTL and value to 64,
//   // // change to what you want by setting ttl_val
//   // if (setsockopt(ping_sockfd, SOL_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0)
//   // {
//   //   printf("\nSetting socket options to TTL failed!\n");
//   //   return;
//   // }

//   // else
//   // {
//   //   printf("\nSocket set to TTL..\n");
//   // }

//   // setting timeout of recv setting
//   setsockopt(ping_sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv_out, sizeof tv_out);

//   // send icmp packet in an infinite loop
//   while (pingloop)
//   {
//     // flag is whether packet was sent or not
//     flag = 1;

//     //filling packet
//     bzero(&message, sizeof(message));
//     clock_gettime(CLOCK_MONOTONIC, &time_start);
//     char s[80];
//     strcpy(s, "PING ");
//     strcat(s, toArray(msg_count));
//     strcat(s, t);

//     usleep(PING_SLEEP_RATE);

//     //send packet
//     clock_gettime(CLOCK_MONOTONIC, &time_start);
//     if (sendto(ping_sockfd, &message, sizeof(message), 0,
//                (struct sockaddr *)ping_addr,
//                sizeof(*ping_addr)) <= 0)
//     {
//       printf("\nPacket Sending Failed!\n");
//       flag = 0;
//     }

//     //receive packet
//     addr_len = sizeof(r_addr);

//     if (recvfrom(ping_sockfd, &message, sizeof(message), 0,
//                  (struct sockaddr *)&r_addr, &addr_len) <= 0 &&
//         msg_count > 1)
//     {
//       printf("\nPacket receive failed!\n");
//     }

//     else
//     {
//       clock_gettime(CLOCK_MONOTONIC, &time_end);

//       double timeElapsed = ((double)(time_end.tv_nsec - time_start.tv_nsec)) / 1000000.0;
//       rtt_msec = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + timeElapsed;

//       // if packet was not sent, don't receive
//       if (flag)
//       {
//         printf("%d bytes from %s msg_seq = % d ttl = % d rtt = % Lf ms.\n ", PING_PKT_S, rev_host, msg_count, ttl_val, rtt_msec);

//         msg_received_count++;
//       }
//     }
//   }
//   clock_gettime(CLOCK_MONOTONIC, &tfe);
//   double timeElapsed = ((double)(tfe.tv_nsec - tfs.tv_nsec)) / 1000000.0;

//   total_msec = (tfe.tv_sec - tfs.tv_sec) * 1000.0 + timeElapsed;

//   printf("\n=== ping statistics===\n");
//   printf("\n%d packets sent, %d packets received, %f percent packet loss.Total time : % Lf ms.\n\n ", msg_count, msg_received_count,
//          ((msg_count - msg_received_count) / msg_count) * 100.0, total_msec);
// }

// int main(int argc, char **argv)
// {
//   int sockfd, portno, n;
//   int serverlen;
//   struct sockaddr_in serveraddr;
//   struct hostent *server;
//   char *hostname;
//   char buf[BUFSIZE];

//   /* check command line arguments */
//   if (argc != 3)
//   {
//     fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
//     exit(0);
//   }
//   hostname = argv[1];
//   portno = atoi(argv[2]);

//   /* socket: create the socket */
//   sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//   if (sockfd < 0)
//     error("ERROR opening socket");

//   /* gethostbyname: get the server's DNS entry */
//   server = gethostbyname(hostname);
//   if (server == NULL)
//   {
//     fprintf(stderr, "ERROR, no such host as %s\n", hostname);
//     exit(0);
//   }

//   /* build the server's Internet address */
//   bzero((char *)&serveraddr, sizeof(serveraddr));
//   serveraddr.sin_family = AF_INET;
//   bcopy((char *)server->h_addr,
//         (char *)&serveraddr.sin_addr.s_addr, server->h_length);
//   serveraddr.sin_port = htons(portno);

//   if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
//     error("ERROR connecting");

//   for (int i = 0; i < 10; i++)
//   {
//   }
//   // /* get a message from the user */
//   // bzero(buf, BUFSIZE);
//   // printf("Please enter msg: ");
//   // fgets(buf, BUFSIZE, stdin);

//   // /* send the message to the server */
//   // serverlen = sizeof(serveraddr);
//   // n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);
//   // if (n < 0)
//   //   error("ERROR in sendto");

//   // /* print the server's reply */
//   // n = recvfrom(sockfd, buf, strlen(buf), 0, &serveraddr, &serverlen);
//   // if (n < 0)
//   //   error("ERROR in recvfrom");
//   // printf("Echo from server: %s", buf);
//   // return 0;

//   send_ping(sockfd, &serveraddr, argv[1]);
// }















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
	
		//printf("Sending '%s' to %s:%i\n", message, server_ip, port);
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
			printf("Request timeout for icmp_seq %d\n", i);
			packets_lost++;
		}
		else
		{
			
		  double return_time = get_time();
			double rtt = return_time - send_time;

			printf("PING received from %s: seq #=%d time=%Lf ms\n", server_ip, i, rtt);
			total_rtt+= rtt;
			total_received++;
			if (rtt<min_rtt)
				min_rtt = rtt;
			if (rtt>max_rtt)
				max_rtt = rtt;

		}
	}
	int packet_loss = 100*(packets_lost/10.00);
	double average = total_rtt/((double)total_received);
	
	if (total_received>0)
		printf("10 packets transmitted, %d recieved, %d%% packet loss rtt min/avg/max = %Lf %Lf %Lf ms\n", total_received, packet_loss, min_rtt, average, max_rtt);	
	else
		printf("10 packets transmitted, %d recieved, 100%% packet loss rtt min/avg/max = undefined undefined undefined ms\n", total_received);	

	close(socket_id);
}

double get_time(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
 	double millisecondsSinceEpoch =
		(double)(tv.tv_sec) * 1000 +
 		(double)(tv.tv_usec) / 1000;
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





// #include <sys/types.h>
// #include <sys/socket.h>
// #include <sys/time.h>
// #include <limits.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <netdb.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <string.h>

// void error(const char *);
// int main(int argc, char *argv[])
// {
//    int sock, n;
//    unsigned int length;
//    struct sockaddr_in server, from;
//    struct hostent *hp;
//    char buffer[1024];
   
//    if (argc != 3) { printf("Usage: server port\n");
//                     exit(1);
//    }
//    //create socket
//    sock = socket(AF_INET, SOCK_DGRAM, 0);
//    if (sock < 0) error("socket");
//    server.sin_family = AF_INET;
//    hp = gethostbyname(argv[1]);
//    if (hp==0) error("Unknown host");

//    bcopy((char *)hp->h_addr, 
//         (char *)&server.sin_addr,
//          hp->h_length);
//    server.sin_port = htons(atoi(argv[2]));
//    length=sizeof(struct sockaddr_in);

//    //variable for storing 
//    struct timeval tv, timeout;
//    double max = INT_MIN * 1.0, min = INT_MAX * 1.0, total;
//    int numLostPackets = 0, numRecvdPackets = 0;
//    for(int i = 1; i < 11; i++)
//    {
//        gettimeofday(&tv, NULL);
//        double milliseconds = (tv.tv_sec) * 1000.0 + (tv.tv_usec) / 1000.0;
//        sprintf(buffer, "%s%d%s%f", "PING ", i, " ", milliseconds);
//        n = sendto(sock, buffer, sizeof(buffer), 0, (const struct sockaddr *)&server, length);
//        if (n < 0) perror("Error sending to socket");
       
//        timeout.tv_sec = 0;
//        timeout.tv_usec = 1000;
//        if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
//        {
//            perror("Error");
//        }

//        n = read(sock, buffer, sizeof(buffer));

//        if(n < 0)
//        {
//            printf("Request timed out \n");
//        }
//        else
//        {
//            struct timeval recvdtime;
//            gettimeofday(&recvdtime, NULL);
//            double recvdmilliseconds = (recvdtime.tv_sec) * 1000.0 + (recvdtime.tv_usec) / 1000.0;
//            numRecvdPackets++;
//            char output[1024];
//            int chtoi[1024];
//            int seqNum = 0;
//            char *token;

//            token = strtok(buffer, " ");
//            printf("%s%s%s%s", token, " received from ", argv[1], " : seq#=");
//            token = strtok(NULL, " ");
//            printf("%s time=", token);
//            token = strtok(NULL, " ");

//            double servMilliseconds = strtod(token, NULL);
//            double diffMilliseconds = recvdmilliseconds - servMilliseconds;

//            total += diffMilliseconds;
//            if(diffMilliseconds > max)
//            {
//                max = diffMilliseconds;
//            }
//            else if(diffMilliseconds < min)
//            {
//                min = diffMilliseconds;
//            }
           
//            printf("%.3f ms\n", diffMilliseconds);
//            memset(buffer, 0, 1024);
//        } 
//    }
//    if(numRecvdPackets == 0)
//    {
//        printf("--- ping statistics --- 10 packets transmitted, 0 received, 100%% packet loss rtt min/avg/max = 0.0 0.0 0.0 ms");
//    }
//    int percent = (10 - numRecvdPackets) * 10;
//    printf("--- ping statistics --- %d packets transmitted, %d received, %d%% packet loss rtt min/avg/max = %.3f %.3f %.3f ms", 10, numRecvdPackets, percent, min, total / numRecvdPackets, max);
//    close(sock);
//    return 0;
// }

// void error(const char *msg)
// {
//     perror(msg);
//     exit(0);
// }