# Server Program README

## Features

### Implemented Features:
- **User Authentication**: Username and password-based login system with a file-based user database (`users.txt`).
- **Private Messaging**: Allows users to send private messages to each other.
- **Group Messaging**: Users can create, join, leave, and send messages to groups.
- **Broadcast Messaging**: Users can broadcast messages to all connected clients.
- **Error Handling**: Invalid commands or errors are returned to clients with appropriate messages.

### Not Implemented:
- **Encrypted Password Storage**: Passwords are stored in plain text rather than being hashed or encrypted.
- **Advanced Group Permissions**: No access control or permission management for group members.
- **User Status Management** : There’s no feature to track user status (online/offline).
- **Scalability Features**: No distributed or load-balanced setup for handling large numbers of clients.

## Design Decisions

- **Multithreaded Design**: The server uses multiple threads to handle client connections simultaneously. Each new client connection spawns a new thread, ensuring the server can manage multiple users concurrently without blocking.
  
- **Reason for Threading**: Creating new threads for each connection avoids performance bottlenecks. Creating new processes would have been more resource-intensive.

- **Thread Synchronization**: Synchronization between threads is achieved using `std::mutex` to protect shared data structures, such as the list of clients and groups, from concurrent modifications that could lead to race conditions.

- **Efficient Data Access**: `std::unordered_map` is used for storing client details and group memberships due to its constant-time complexity for lookups, making it an efficient choice for managing shared data.

- **Authentication**: Authentication is implemented through a simple username-password mechanism using a text file for storage. This approach was chosen for simplicity and ease of implementation.

- **Command Parsing**: A custom command parser using `std::istringstream` was implemented to handle client requests efficiently. This was preferred over regular expressions to avoid unnecessary overhead.

- **Group Management**: Groups are managed using an `unordered_map` of strings (group names) to vectors (member usernames). This allows efficient lookup and management of group members.

## Implementation

### **High-Level Idea of Important Functions**:
- The server begins by setting up the necessary environment and loading user credentials from the `users.txt` file using the `load_users()` function. This creates a map of usernames and passwords to authenticate users.
- The server creates a listening socket and starts listening for incoming connections from clients.
- When a client tries to connect, the server accepts the connection and spawns a new thread using std::thread. This thread is responsible for handling the client's interaction with the server.
- The client is first prompted for a username and password for authentication. This is managed by the `authenticate_user()` function, which verifies the credentials by checking them against the previously loaded users map.
- If the authentication is successful, the server associates the client’s socket with their username, allowing them to send and receive messages under that identity.
- After successful authentication, the server listens for commands from the client. Commands can be broadcast messages, private messages, or group-related actions (such as creating or joining a group).
- For a broadcast message (`/broadcast`), `the broadcast_message()` function sends the message to all connected clients except the sender.
- For a private message (`/msg`),` the send_private_message()` function locates the recipient using the username and sends the message directly to that user’s socket.
- If the client issues a command involving groups (e.g., `/group create`, `/group join`, `/group leave, or /group msg`), the server checks if the group exists and performs the requested action. Functions like `create_group()`,` join_group()`, and `leave_group()` manage group memberships and ensure that users can interact within the specified groups.
- In case of invalid commands or errors, appropriate error messages are sent back to the client, helping them correct the issue or understand what went wrong.
- Once a client disconnects or terminates their session (e.g., by closing the connection), the server cleans up by removing the client’s information (username, socket) from the various data structures, ensuring that there are no leftover references to the disconnected client.
- The entire system works by continuously managing multiple client threads, ensuring each client can interact with the server in a secure, thread-safe manner, while the server is capable of handling new incoming connections and processing messages.
---


## Testing
The server was tested using both functional and stress testing strategies:
### **Types of Testing**:
 **Correctness Testing**:
  
- Verified user authentication with correct and incorrect credentials to ensure proper validation.
- Tested broadcasting functionality by sending messages to all connected clients and confirming reception.
- Ensured private messaging worked by sending messages to specific users and checking if they were received.
- Validated group functionalities like creating, joining, leaving groups, and sending group messages.
- Checked error handling by testing invalid commands, group names, and user names.



---

## Challenges

### **Challenges Faced**:
- Thread Synchronization: Managing access to shared data structures like clients, groups, and `username_to_socket` caused race conditions initially. Using mutex locks resolved these issues, ensuring thread safety during concurrent access.

- Command Parsing: Initially, command handling was not well-organized, leading to confusing behavior for certain commands. A cleaner command parsing logic was implemented to streamline handling multiple commands.

- Error Handling: Properly managing error messages and ensuring correct error responses (e.g., user not found or group not found) took time to refine, ensuring all edge cases were covered.

---

## Restrictions in  Server

1. **Maximum Clients**: The server can handle up to 50 clients concurrently. This limit is based on system resources and thread management.
2. **Maximum Groups**: The server can handle up to 100 groups.
3. **Group Size Limit**: Each group can have a maximum of 50 members.
4. **Message Size**: The maximum message size that can be transferred in one go is 1024 bytes.

---

## Challenges :

We faced some challenges while working on the assignment. Initially we didn't knew from where to start as it was not touched upon in classes in technical details. We faced error in parsing the string given in users.txt file. We also faced issues while brainstorming for ways to send messages to clients collectively, or in group or personally.

---

### **Individual Contributions**:
- **Himanshu Mahale (230476) **:
   design, implementation, research
- **Vineet Nagrale(231158) **: readme, testing.

---


## Sources

https://devdocs.io/cpp/
https://github.com/themisvaltinos/TCP-Client-Server
https://github.com/nnnyt/chat
https://www.reddit.com/r/cpp_questions/comments/i1mgda/create_a_chat_application/
https://www.lldcoding.com/design-lld-a-real-time-chat-system-with-support-for-millions-of-concurrent-users-machine-coding
https://www.tutorialspoint.com/cplusplus/cpp_socket_programming.htm
https://stackoverflow.com/questions/71572056/multithreaded-server-c-socket-programming
https://www.geeksforgeeks.org/socket-programming-in-cpp/ 

---

## Declaration

We declare that we have not indulged in plagiarism

---
 






## Instruction for Run

1. Prerequisites
Windows OS (uses Winsock).

Compiler: Use a C++ compiler like g++ with Winsock support.

users.txt File:

Create a file named users.txt in the server directory.

