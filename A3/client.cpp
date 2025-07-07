#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>

// This code implements a TCP client that performs a 3-way handshake with a server using raw sockets.
// It sends a SYN packet, waits for a SYN-ACK response, and then sends an ACK packet to complete the handshake.
struct pseudo_header {
    uint32_t source_address;
    uint32_t dest_address;
    uint8_t placeholder;
    uint8_t protocol;
    uint16_t tcp_length;
};

// This function makes a checksum for the TCP packet. It takes a pointer to the packet and its length in bytes as arguments.
// It returns the checksum as an unsigned short integer.
unsigned short checksum(unsigned short *ptr, int nbytes) {
    register long sum = 0;
    unsigned short oddbyte;
    register short answer;

    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }

    if (nbytes == 1) {
        oddbyte = 0;
        *((unsigned char *)&oddbyte) = *(unsigned char *)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = (short)~sum;

    return answer;
}
// This function sends a TCP packet. It takes the socket file descriptor, source and destination IP addresses, source and destination ports,
// sequence and acknowledgment numbers, and SYN and ACK flags as arguments. It constructs the TCP packet and sends it to the destination.
void send_tcp_packet(int sock, const char *src_ip, const char *dst_ip, uint16_t src_port, uint16_t dst_port,
                     uint32_t seq, uint32_t ack_seq, bool syn, bool ack) {
    char datagram[4096];
    memset(datagram, 0, 4096);

    struct iphdr *ip = (struct iphdr *)datagram;
    struct tcphdr *tcp = (struct tcphdr *)(datagram + sizeof(struct iphdr));
    struct sockaddr_in dest;

    // These are the TCP header fields
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
    ip->id = htons(54321);
    ip->frag_off = 0;
    ip->ttl = 255;
    ip->protocol = IPPROTO_TCP;
    ip->check = 0;
    ip->saddr = inet_addr(src_ip);
    ip->daddr = inet_addr(dst_ip);

    // These are the TCP header fields
    tcp->source = htons(src_port);
    tcp->dest = htons(dst_port);
    tcp->seq = htonl(seq);
    tcp->ack_seq = htonl(ack_seq);
    tcp->doff = 5;
    tcp->syn = syn ? 1 : 0;
    tcp->ack = ack ? 1 : 0;
    tcp->window = htons(8192);
    tcp->check = 0;
    tcp->urg_ptr = 0;

    // This is the pseudo header for TCP checksum calculation
    // The pseudo header is used to calculate the checksum for TCP packets.
    struct pseudo_header psh;
    psh.source_address = inet_addr(src_ip);
    psh.dest_address = inet_addr(dst_ip);
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(sizeof(struct tcphdr));
    // The pseudo header is used to calculate the checksum for TCP packets.
    // The checksum is calculated using the pseudo header and the TCP header.
    char pseudo_packet[sizeof(struct pseudo_header) + sizeof(struct tcphdr)];
    memcpy(pseudo_packet, &psh, sizeof(struct pseudo_header));
    memcpy(pseudo_packet + sizeof(struct pseudo_header), tcp, sizeof(struct tcphdr));
    // The checksum is calculated using the pseudo header and the TCP header.
    tcp->check = checksum((unsigned short *)pseudo_packet, sizeof(pseudo_packet));
    // This is the destination address and port for the packet.
    dest.sin_family = AF_INET;
    dest.sin_port = htons(dst_port);
    dest.sin_addr.s_addr = inet_addr(dst_ip);

    if (sendto(sock, datagram, ip->tot_len, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        perror("sendto() failed");
    } else {
        std::cout << "[+] Sent packet: SEQ=" << seq << " ACK=" << ack_seq
                  << " SYN=" << syn << " ACK flag=" << ack << std::endl;
    }
}
// This function performs the TCP 3-way handshake. It creates a raw socket, sends a SYN packet with SEQ=200, waits for a SYN-ACK response,
// and then sends an ACK packet with SEQ=600 and ACK=401. It uses the send_tcp_packet function to send the packets.
void client_handshake() {
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int one = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt() failed");
        exit(EXIT_FAILURE);
    }

    const char *src_ip = "127.0.0.1";
    const char *dst_ip = "127.0.0.1";
    uint16_t src_port = 54321;
    uint16_t dst_port = 12345;

    // This is the initial sequence number for the client. It can be any value, but it should be unique for each connection.
    uint32_t client_seq = 200; 
    // I had tried to take a random number for the initial sequence number, but it was not working and due to lack of time, I couldn't debug it and work on it, this was purely experimental

//     srand(time(0));  // Seed random number generator
// uint32_t client_seq = rand() % 100000 + 1000;  // Random initial sequence number
 // Required by server
 
    send_tcp_packet(sock, src_ip, dst_ip, src_port, dst_port, client_seq, 0, true, false);

    // Step 2: Wait for SYN-ACK and parse it
    uint32_t server_seq = 0;
    uint32_t server_ack = 0;

    char buffer[65536];
    while (true) {
        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len < 0) continue;

        struct iphdr *ip = (struct iphdr *)buffer;
        if (ip->protocol != IPPROTO_TCP) continue;

        struct tcphdr *tcp = (struct tcphdr *)(buffer + ip->ihl * 4);

        // Check if it's from server and destined to us
        if (ntohs(tcp->source) == dst_port &&
            ntohs(tcp->dest) == src_port &&
            tcp->syn == 1 && tcp->ack == 1) {

            server_seq = ntohl(tcp->seq);
            server_ack = ntohl(tcp->ack_seq);
            // this is the server's sequence number and acknowledgment number
            // The server's sequence number is the value of the SEQ field in the SYN-ACK packet.
            std::cout << "[+] Received SYN-ACK from server." << std::endl;
            std::cout << "    Server SEQ: " << server_seq << std::endl;
            std::cout << "    Server ACK: " << server_ack << std::endl;
            break;
        }
    }

    // Step 3: Send ACK with SEQ=600, ACK=401
    uint32_t client_final_seq = 600;  // Client's sequence number
    uint32_t client_ack_seq = server_seq + 1;  // Server's ACK + 1

    // This is the final ACK packet sent to the server to complete the handshake.
    // The sequence number is set to 600 and the acknowledgment number is set to server_seq + 1.
    send_tcp_packet(sock, src_ip, dst_ip, src_port, dst_port, client_final_seq, client_ack_seq, false, true);
    std::cout << "[+] Final ACK sent. Handshake complete." << std::endl;

    close(sock);
}
// This is the main function that starts the client handshake process. It calls the client_handshake function to perform the 3-way handshake.
// It prints a message indicating that the client is starting the handshake process.
int main() {
    std::cout << "[+] Client starting handshake..." << std::endl;
    client_handshake();
    return 0;
}