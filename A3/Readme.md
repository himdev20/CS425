# TCP Three-Way Handshake using Raw Sockets

This project simulates the *TCP three-way handshake* using *raw sockets* in C++. It involves creating custom TCP packets for each stage of the connection setup: *SYN, **SYN-ACK, and **ACK*.

The implementation is divided into two separate programs: *client* and *server*, each responsible for crafting and parsing packets manually without relying on the OS TCP stack.

---

##  Key Concepts Implemented

1. *Raw Sockets:*
   - Used to manually create and send TCP packets without the kernel’s help.
   - Allows full control over header fields.

2. *Manual TCP Header Construction:*
   - Crafted packets with TCP flags like SYN, ACK, FIN, etc.
   - Proper handling of sequence and acknowledgment numbers.

3. *Packet Filtering and Parsing:*
   - Extracted and interpreted TCP/IP headers using struct iphdr and struct tcphdr.

4. *Simulated Three-Way Handshake Flow:*
   - *SYN (client → server)*
   - *SYN-ACK (server → client)*
   - *ACK (client → server)*

---

##  Features

###  Server Code Highlights:
- Listens for incoming raw TCP packets.
- Detects SYN packets and responds with a SYN-ACK.
- Prints TCP flag values and sequence numbers for debugging.
- Waits for the final ACK to complete the handshake.

###  Client Code Highlights:
- Sends a manually crafted SYN packet to the server.
- Listens for SYN-ACK response and validates the acknowledgment.
- Sends final ACK to complete the handshake.

---

## Client-Side Function Descriptions

###  1. checksum(unsigned short *ptr, int nbytes):

This function calculates the checksum required for the TCP header and pseudo-header, which is essential for ensuring data integrity in IP packets. It iterates over the given data two bytes at a time, adds up their values, folds any overflow back into the sum, and returns the one's complement of the final sum as the checksum.



### 2. send_tcp_packet(int sock, const char *src_ip, const char *dst_ip, uint16_t src_port, uint16_t dst_port, uint32_t seq, uint32_t ack_seq, bool syn, bool ack):

This function builds and sends a TCP packet with specified flags (SYN and/or ACK) using a raw socket. It manually constructs the IP and TCP headers, fills necessary fields like sequence numbers and flags, calculates the checksum, and sends the packet to the destination. It supports sending different types of packets (SYN, ACK, etc.) based on the flags provided.

### 3. receive_and_process(int sock, const char *src_ip, const char *dst_ip, uint16_t src_port, uint16_t dst_port, uint32_t client_final_seq):

This function listens for incoming packets from the server, specifically waiting for a SYN-ACK response. Upon receiving and validating the SYN-ACK, it extracts the server's sequence and acknowledgment numbers, and then sends a final ACK packet to complete the three-way handshake.

### 4. main():

This function oversees the entire three-way handshake process. It starts by creating a raw socket with SOCK_RAW and IPPROTO_TCP, sets the necessary socket options to include IP headers, and defines the source and destination information. It first sends a SYN packet to initiate the connection, waits for the server's SYN-ACK response, and finally calls receive_and_process() to complete the handshake by sending the final ACK packet.




Each function helps simulate the three steps of the TCP connection: SYN → SYN-ACK → ACK, giving full manual control over the packet exchange.

----

##  How to Compile and Run

### 1. *Compile both programs:*
bash
g++ server.cpp -o server
g++ client.cpp -o client


### 2. *Run as root (raw sockets need root privileges):*

#### Open two terminals:

*Terminal 1 (Run the Server):*
bash
sudo ./server


*Terminal 2 (Run the Client):*
bash
sudo ./client


---

##  Expected Output

### Server Terminal:

[+] Server listening on port 12345...
[+] TCP Flags:  SYN: 1 ACK: 0 FIN: 0 RST: 0 PSH: 0 SEQ: 200
[+] Received SYN from 127.0.0.1
[+] Sent SYN-ACK
[+] TCP Flags:  SYN: 0 ACK: 1 FIN: 0 RST: 0 PSH: 0 SEQ: 600
[+] Received ACK, handshake complete.


### Client Terminal:

[+] Client starting handshake...
[+] Sent packet: SEQ=200 ACK=0 SYN=1 ACK flag=0
[+] Received SYN-ACK from server.
    Server SEQ: 400
    Server ACK: 201
[+] Sent packet: SEQ=600 ACK=401 SYN=0 ACK flag=1
[+] Final ACK sent. Handshake complete.


---



## *Individual Contributions*:
- Himanshu Mahale (230476) :
   design, implementation, research

- Vineet Nagrale(231158) : readme, testing

---


##  Sources 

- Linux Raw Sockets Programming: https://man7.org/linux/man-pages/man7/raw.7.html

- TCP/IP Protocol Suite (RFC 793): https://datatracker.ietf.org/doc/html/rfc793

- Berkeley Sockets API Documentation: https://man7.org/linux/man-pages/man2/socket.2.html

- Packet Crafting with Raw Sockets in C: https://www.binarytides.com/raw-sockets-c-code-linux/

- Low-Level Network Programming in C++: https://www.geeksforgeeks.org/socket-programming-in-cpp/

- IP and TCP Header Structures: https://www.tldp.org/LDP/tcpip/tcpip.html

- Wireshark for Packet Analysis: https://www.wireshark.org/docs/wsug_html_chunked/

- Python Socket Programming HOWTO: https://docs.python.org/3/howto/sockets.html

---

##  Declaration :

We declare that we have not indulged in plagiarism.

---