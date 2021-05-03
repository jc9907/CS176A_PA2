/*
 * udpclient.c - A simple UDP client
 * usage: PingClient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <netinet/in.h>
#define BUFSIZE 1024

#ifndef SOL_IP
#define SOL_IP IPPROTO_IP
#endif
/*
 * error - wrapper for perror
 */
void error(char *msg)
{
  perror(msg);
  exit(0);
}

// Define the Packet Constants
// ping packet size
#define PING_PKT_S 64

// Automatic port number
#define PORT_NO 0

// Automatic port number
#define PING_SLEEP_RATE 1000000

// Gives the timeout delay for receiving packets
// in seconds
#define RECV_TIMEOUT 1

// Define the Ping Loop
int pingloop = 1;

struct icmphdr
{
  u_int8_t type; /* message type */
  u_int8_t code; /* type sub-code */
  u_int16_t checksum;
  union
  {
    struct
    {
      u_int16_t id;
      u_int16_t sequence;
    } echo;            /* echo datagram */
    u_int32_t gateway; /* gateway address */
    struct
    {
      u_int16_t __unused;
      u_int16_t mtu;
    } frag; /* path mtu discovery */
  } un;
};

struct ping_pkt
{
  struct icmphdr hdr;
  char msg[PING_PKT_S - sizeof(struct icmphdr)];
};

// make a ping request
void send_ping(int ping_sockfd, struct sockaddr_in *ping_addr, char *rev_host)
{
  int ttl_val = 64, msg_count = 0, i, addr_len, flag = 1, msg_received_count = 0;

  struct ping_pkt pckt;
  struct sockaddr_in r_addr;
  struct timespec time_start, time_end, tfs, tfe;
  long double rtt_msec = 0, total_msec = 0;
  struct timeval tv_out;
  tv_out.tv_sec = RECV_TIMEOUT;
  tv_out.tv_usec = 0;

  clock_gettime(CLOCK_MONOTONIC, &tfs);

  // set socket options at ip to TTL and value to 64,
  // change to what you want by setting ttl_val
  if (setsockopt(ping_sockfd, SOL_IP, IP_TTL, &ttl_val, sizeof(ttl_val)) != 0)
  {
    printf("\nSetting socket options to TTL failed!\n");
    return;
  }

  else
  {
    printf("\nSocket set to TTL..\n");
  }

  // setting timeout of recv setting
  setsockopt(ping_sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv_out, sizeof tv_out);

  // send icmp packet in an infinite loop
  while (pingloop)
  {
    // flag is whether packet was sent or not
    flag = 1;

    //filling packet
    bzero(&pckt, sizeof(pckt));

    pckt.hdr.type = ICMP_ECHO;
    pckt.hdr.un.echo.id = getpid();

    for (i = 0; i < sizeof(pckt.msg) - 1; i++)
      pckt.msg[i] = i + '0';

    pckt.msg[i] = 0;
    pckt.hdr.un.echo.sequence = msg_count++;

    usleep(PING_SLEEP_RATE);

    //send packet
    clock_gettime(CLOCK_MONOTONIC, &time_start);
    if (sendto(ping_sockfd, &pckt, sizeof(pckt), 0,
               (struct sockaddr *)ping_addr,
               sizeof(*ping_addr)) <= 0)
    {
      printf("\nPacket Sending Failed!\n");
      flag = 0;
    }

    //receive packet
    addr_len = sizeof(r_addr);

    if (recvfrom(ping_sockfd, &pckt, sizeof(pckt), 0,
                 (struct sockaddr *)&r_addr, &addr_len) <= 0 &&
        msg_count > 1)
    {
      printf("\nPacket receive failed!\n");
    }

    else
    {
      clock_gettime(CLOCK_MONOTONIC, &time_end);

      double timeElapsed = ((double)(time_end.tv_nsec - time_start.tv_nsec)) / 1000000.0;
      rtt_msec = (time_end.tv_sec - time_start.tv_sec) * 1000.0 + timeElapsed;

      // if packet was not sent, don't receive
      if (flag)
      {
        if (!(pckt.hdr.type == 69 && pckt.hdr.code == 0))
        {
          printf("Error..Packet received with ICMP type % d code % d\n ", pckt.hdr.type, pckt.hdr.code);
        }
        else
        {
          printf("%d bytes from %s msg_seq = % d ttl = % d rtt = % Lf ms.\n ", PING_PKT_S, rev_host, msg_count, ttl_val, rtt_msec);

          msg_received_count++;
        }
      }
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &tfe);
  double timeElapsed = ((double)(tfe.tv_nsec - tfs.tv_nsec)) /
                       1000000.0;

  total_msec = (tfe.tv_sec - tfs.tv_sec) * 1000.0 + timeElapsed;

  printf("\n=== ping statistics===\n");
  printf("\n%d packets sent, %d packets received, %f percent packet loss.Total time : % Lf ms.\n\n ", msg_count, msg_received_count,
         ((msg_count - msg_received_count) / msg_count) * 100.0, total_msec);
}

int main(int argc, char **argv)
{
  int sockfd, portno, n;
  int serverlen;
  struct sockaddr_in serveraddr;
  struct hostent *server;
  char *hostname;
  char buf[BUFSIZE];

  /* check command line arguments */
  if (argc != 3)
  {
    fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
    exit(0);
  }
  hostname = argv[1];
  portno = atoi(argv[2]);

  /* socket: create the socket */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  /* gethostbyname: get the server's DNS entry */
  server = gethostbyname(hostname);
  if (server == NULL)
  {
    fprintf(stderr, "ERROR, no such host as %s\n", hostname);
    exit(0);
  }

  /* build the server's Internet address */
  bzero((char *)&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
        (char *)&serveraddr.sin_addr.s_addr, server->h_length);
  serveraddr.sin_port = htons(portno);

  // /* get a message from the user */
  // bzero(buf, BUFSIZE);
  // printf("Please enter msg: ");
  // fgets(buf, BUFSIZE, stdin);

  // /* send the message to the server */
  // serverlen = sizeof(serveraddr);
  // n = sendto(sockfd, buf, strlen(buf), 0, &serveraddr, serverlen);
  // if (n < 0)
  //   error("ERROR in sendto");

  // /* print the server's reply */
  // n = recvfrom(sockfd, buf, strlen(buf), 0, &serveraddr, &serverlen);
  // if (n < 0)
  //   error("ERROR in recvfrom");
  // printf("Echo from server: %s", buf);
  // return 0;

  send_ping(sockfd, &serveraddr, argv[1]);
}