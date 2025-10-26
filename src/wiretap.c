/* wiretap.c - a simple packet analysis tool */

#include <arpa/inet.h>
#include <errno.h>
#include "format.h"
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

enum exit_codes {
    SUCCESS,
    ERR_OPERATIONAL,
    ERR_USAGE
};

int main(int argc, char** argv) {
    int sockfd, status;
    size_t if_name_length;

    char eth_frame_buffer[ETH_FRAME_LEN];
    memset(eth_frame_buffer, 0, ETH_FRAME_LEN);

    // Detect invalid usage.
    if (argc != 2) {
        fprintf(stderr, "Usage: wiretap interface\n");
        return ERR_USAGE;
    }

    // It is assumed that elements of argv are null-terminated.
    if_name_length = strlen(*(argv + 1)) + 1;

    // Detect interface names longer than 16 bytes.
    if (if_name_length > IF_NAMESIZE) {
        fprintf(stderr, "Interface names must be at most 15 characters long.\n");
        return ERR_USAGE;
    }

    /* Create a raw socket (SOCK_RAW) of address family packet (AF_PACKET) that
       accepts all ethernet traffic (ETH_P_ALL, which for some reason has to be
       converted to network byte order).*/
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        perror("socket");
        return ERR_OPERATIONAL;
    }

    // Bind the created socket to the chosen network interface.
    if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, *(argv + 1), if_name_length) == -1) {
        perror("setsockopt bind device");
        if (close(sockfd) == -1) {
            perror("close");
        }
        return ERR_OPERATIONAL;
    }

    // Set the network interface to promiscuous mode using ioctl().
    
    struct ifreq req;
    memset(&req, 0, sizeof(req));
    strncpy(req.ifr_ifrn.ifrn_name, *(argv + 1), IF_NAMESIZE);
    
    // Get interface flags.
    if (ioctl(sockfd, SIOCGIFFLAGS, &req) == -1) {
        perror("ioctl");
        if (close(sockfd) == -1) {
            perror("close");
        }
        return ERR_OPERATIONAL;
    }

    // Set interface flags. The IFF_PROMISC flag is set to turn on promiscuous mode.
    req.ifr_ifru.ifru_flags |= IFF_PROMISC;
    if (ioctl(sockfd, SIOCSIFFLAGS, &req) == -1) {
        perror("ioctl");
        if (close(sockfd) == -1) {
            perror("close");
        }
        return ERR_OPERATIONAL;
    }

    /* Begin analysing the traffic going through the chosen network interface.
       In other words, begin wiretapping :3 */

    uint16_t ethertype;

    struct ethhdr* eth_frame_header;
    struct iphdr* ipv4_header;
    struct ip6_hdr* ipv6_header;
    struct timespec time;

    uint8_t timestamp[ISO_8601_TIMESTRLEN];
    uint8_t mac_src_addrstr_buffer[MAC_ADDRSTRLEN], mac_dest_addrstr_buffer[MAC_ADDRSTRLEN];
    uint8_t ip_src_addrstr[INET6_ADDRSTRLEN], ip_dest_addrstr[INET6_ADDRSTRLEN];

    while (1) {
        if ((status = recv(sockfd, eth_frame_buffer, ETH_FRAME_LEN, 0)) == -1) {
            perror("recv");
            // TODO: actually handle the error.
        }
        
        // Get the current time since Unix epoch.
        if (clock_gettime(CLOCK_REALTIME, &time) == -1) {
            perror("clock");
            // TODO: actually handle the error.
        }

        time_to_iso8601(&time, timestamp);
        fprintf(stdout, "%s: ", timestamp);

        // Extract ethernet frame header.
        eth_frame_header = (struct ethhdr*)eth_frame_buffer;

        // Convert the source and destination MAC addresses to strings.
        mac_ntop(eth_frame_header->h_source, mac_src_addrstr_buffer);
        mac_ntop(eth_frame_header->h_dest, mac_dest_addrstr_buffer);

        fprintf(stdout, "MAC %s > %s: ", mac_src_addrstr_buffer, mac_dest_addrstr_buffer);
        
        // Only handle ethernet frames with Ethertype IPv4 or IPv6.
        switch (htons(eth_frame_header->h_proto)) {
            case ETHERTYPE_IP:
                ipv4_header = (struct iphdr*)(eth_frame_buffer + ETH_HLEN);

                // Convert the source and destination IPv4 addresses to strings.
                inet_ntop(AF_INET, &(ipv4_header->saddr), (char*)ip_src_addrstr, INET_ADDRSTRLEN);
                inet_ntop(AF_INET, &(ipv4_header->daddr), (char*)ip_dest_addrstr, INET_ADDRSTRLEN);

                fprintf(stdout, "IPv4 %s > %s, ", ip_src_addrstr, ip_dest_addrstr);
                break;
            case ETHERTYPE_IPV6:
                ipv6_header = (struct ip6_hdr*)(eth_frame_buffer + ETH_HLEN);

                // Convert the source and destination IPv4 addresses to strings.
                inet_ntop(AF_INET6, &(ipv6_header->ip6_src), (char*)ip_src_addrstr, INET6_ADDRSTRLEN);
                inet_ntop(AF_INET6, &(ipv6_header->ip6_dst), (char*)ip_dest_addrstr, INET6_ADDRSTRLEN);

                fprintf(stdout, "IPv6 %s > %s, ", ip_src_addrstr, ip_dest_addrstr);
                break;
            default:
                // An unknown Ethertype was encountered...
                ethertype = htons(eth_frame_header->h_proto);
                fprintf(stdout, "????, ethertype %#0hx, ", ethertype);
        }

        fprintf(stdout, "length %d\n", status);
    }

    // Close the socket.
    if (close(sockfd) == -1) {
        perror("close");
        return ERR_OPERATIONAL;
    }

    return SUCCESS;
}