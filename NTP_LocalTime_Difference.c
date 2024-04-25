#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define NTP_SERVER "216.239.35.0"
#define NTP_PORT 123
#define NTP_PACKET_SIZE 48
#define NTP_TIMESTAMP_DELTA 2208988800ull

typedef struct {
    uint8_t li_vn_mode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    uint32_t root_delay;
    uint32_t root_dispersion;
    uint32_t reference_id;
    uint32_t ref_ts_sec;
    uint32_t ref_ts_frac;
    uint32_t orig_ts_sec;
    uint32_t orig_ts_frac;
    uint32_t recv_ts_sec;
    uint32_t recv_ts_frac;
    uint32_t trans_ts_sec;
    uint32_t trans_ts_frac;
} ntp_packet;

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void get_ntp_time(struct timeval *ntp_time) {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    ntp_packet packet;

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(NTP_PORT);
    if (inet_pton(AF_INET, NTP_SERVER, &serv_addr.sin_addr) != 1) {
        fprintf(stderr, "ERROR converting address: %s\n", NTP_SERVER);
        exit(EXIT_FAILURE);
    }

    memset(&packet, 0, sizeof(packet));
    packet.li_vn_mode = (0x03 << 6) | (0x03 << 3) | 0x03;

    if (sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR sending packet");

    n = recv(sockfd, &packet, sizeof(packet), 0);
    if (n < 0)
        error("ERROR receiving response");

    close(sockfd);
    time_t current_time = (ntohl(packet.trans_ts_sec) - NTP_TIMESTAMP_DELTA);
    ntp_time->tv_sec = current_time;
    ntp_time->tv_usec = 0;
}

int main() {
    struct timeval ntp_time;
    struct tm *local_time;
    time_t current_time;
    long time_diff;

    // Get NTP time
    get_ntp_time(&ntp_time);

    // Get local time
    current_time = time(NULL);
    local_time = localtime(&current_time);

    // Calculate time difference
    time_diff = (ntp_time.tv_sec - current_time);

    // Print NTP time
    printf("NTP time: %s", ctime(&ntp_time.tv_sec));

    // Print local time
    printf("Local time: %s", asctime(local_time));

    // Print time difference
    printf("Time difference (NTP - Local): %ld seconds\n", time_diff);

    return 0;
}

