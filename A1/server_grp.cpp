#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
// #include <winsock2.h>
// #include <ws2tcpip.h>
#include <arpa/inet.h>
#include <algorithm>

#define PORT 12345
#define BUFFER_SIZE 1024

// We are using the following lines of code to store usernames and passwords, mapping them
std::unordered_map<std::string, std::string> users; 
std::unordered_map<int, std::string> client_usernames; 
std::unordered_map<std::string, int> username_to_socket; 
std::unordered_map<std::string, std::vector<std::string>> groups; 
std::vector<int> clients; 
std::mutex clients_mutex; 

// using the following fun to load users from text file provided in github repository
void load_users() {
    std::ifstream file("users.txt");
    if (!file) {
        std::cerr << "Error: Could not open users.txt!" << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t delimiter_pos = line.find(':');
        if (delimiter_pos != std::string::npos) {
            std::string username = line.substr(0, delimiter_pos);
            std::string password = line.substr(delimiter_pos + 1);
            users[username] = password;
        } else {
            std::cerr << "Warning: Invalid line in users.txt (missing colon): " << line << std::endl;
        }
    }
}


bool authenticate_user(int client_socket) {
    char buffer[BUFFER_SIZE];
    send(client_socket, "Enter username: ", 17, 0);
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    std::string username = buffer;

    send(client_socket, "Enter password: ", 17, 0);
    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    std::string password = buffer;

    if (users.find(username) != users.end() && users[username] == password) {
        client_usernames[client_socket] = username;
        username_to_socket[username] = client_socket;
        send(client_socket, "Welcome to the server!", 23, 0);
        return true;
    }
    send(client_socket, "Authentication failed", 22, 0);
    return false;
}

// Next few functions will be used to send, broadcast messages, in the group or privately  
void broadcast_message(const std::string& message, int sender_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int client : clients) {
        if (client != sender_socket) {
            send(client, message.c_str(), message.size(), 0);
        }
    }
}


void send_private_message(const std::string& recipient, const std::string& message, int sender_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    if (username_to_socket.find(recipient) != username_to_socket.end()) {
        int recipient_socket = username_to_socket[recipient];
        std::string private_message = "[" + client_usernames[sender_socket] + "] " + message;
        send(recipient_socket, private_message.c_str(), private_message.size(), 0);
    } else {
        std::string error_message = "User " + recipient + " not found.";
        send(sender_socket, error_message.c_str(), error_message.size(), 0);
    }
}


void send_group_message(const std::string& group_name, const std::string& message, int sender_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    if (groups.find(group_name) != groups.end()) {
        std::string sender_username = client_usernames[sender_socket];
        std::string group_message = "[Group " + group_name + " from " + sender_username + "] " + message;

        for (const std::string& member : groups[group_name]) {
            if (username_to_socket.find(member) != username_to_socket.end()) {
                int member_socket = username_to_socket[member];
                send(member_socket, group_message.c_str(), group_message.size(), 0);
            }
        }
    } else {
        std::string error_message = "Group " + group_name + " not found.";
        send(sender_socket, error_message.c_str(), error_message.size(), 0);
    }
}


void create_group(const std::string& group_name, const std::string& username) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    if (groups.find(group_name) == groups.end()) {
        groups[group_name] = {username}; // Create group with the creator as the first member
        std::string success_message = "Group " + group_name + " created successfully.";
        send(username_to_socket[username], success_message.c_str(), success_message.size(), 0);
    } else {
        std::string error_message = "Group " + group_name + " already exists.";
        send(username_to_socket[username], error_message.c_str(), error_message.size(), 0);
    }
}


void join_group(const std::string& group_name, const std::string& username) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    if (groups.find(group_name) != groups.end()) {
        auto& members = groups[group_name];
        if (std::find(members.begin(), members.end(), username) == members.end()) {
            members.push_back(username); // Add user to the group
            std::string success_message = "You have joined group " + group_name + ".";
            send(username_to_socket[username], success_message.c_str(), success_message.size(), 0);
        } else {
            std::string error_message = "You are already a member of group " + group_name + ".";
            send(username_to_socket[username], error_message.c_str(), error_message.size(), 0);
        }
    } else {
        std::string error_message = "Group " + group_name + " not found.";
        send(username_to_socket[username], error_message.c_str(), error_message.size(), 0);
    }
}


void leave_group(const std::string& group_name, const std::string& username) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    if (groups.find(group_name) != groups.end()) {
        auto& members = groups[group_name];
        auto it = std::find(members.begin(), members.end(), username);
        if (it != members.end()) {
            members.erase(it); // Remove user from the group
            std::string success_message = "You have left group " + group_name + ".";
            send(username_to_socket[username], success_message.c_str(), success_message.size(), 0);
        } else {
            std::string error_message = "You are not a member of group " + group_name + ".";
            send(username_to_socket[username], error_message.c_str(), error_message.size(), 0);
        }
    } else {
        std::string error_message = "Group " + group_name + " not found.";
        send(username_to_socket[username], error_message.c_str(), error_message.size(), 0);
    }
}

// We will use following function to handle clients

void handle_client(int client_socket) {
    if (!authenticate_user(client_socket)) {
        close(client_socket);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(client_socket);
    }

    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            break;
        }

        std::string message(buffer);
        std::istringstream iss(message);
        std::string command;
        iss >> command;

        if (command == "/broadcast") {
            std::string broadcast_msg;
            std::getline(iss, broadcast_msg);
            broadcast_msg = client_usernames[client_socket] + ": " + broadcast_msg.substr(1); 
            broadcast_message(broadcast_msg, client_socket);
        } else if (command == "/msg") {
            std::string recipient, private_message;
            iss >> recipient;
            std::getline(iss, private_message);
            private_message = private_message.substr(1); 
            send_private_message(recipient, private_message, client_socket);
        } else if (command == "/group") {
            std::string subcommand, group_name;
            iss >> subcommand;
            if (subcommand == "create" || subcommand == "create_group") {
                iss >> group_name;
                create_group(group_name, client_usernames[client_socket]);
            } else if (subcommand == "join" || subcommand == "join_group") {
                iss >> group_name;
                join_group(group_name, client_usernames[client_socket]);
            } else if (subcommand == "leave" || subcommand == "leave_group") {
                iss >> group_name;
                leave_group(group_name, client_usernames[client_socket]);
            } else if (subcommand == "msg" || subcommand == "group_msg") {
                iss >> group_name;
                std::string group_message;
                std::getline(iss, group_message);
                group_message = group_message.substr(1); 
                send_group_message(group_name, group_message, client_socket);
            } else {
                std::string error_message = "Invalid group command.";
                send(client_socket, error_message.c_str(), error_message.size(), 0);
            }
        } else if (command == "/create_group") {
            std::string group_name;
            iss >> group_name;
            create_group(group_name, client_usernames[client_socket]);
        } else if (command == "/join_group") {
            std::string group_name;
            iss >> group_name;
            join_group(group_name, client_usernames[client_socket]);
        } else if (command == "/group_msg") {
            std::string group_name, group_message;
            iss >> group_name;
            std::getline(iss, group_message);
            group_message = group_message.substr(1); 
            send_group_message(group_name, group_message, client_socket);
        } else if (command == "/leave_group") {
            std::string group_name;
            iss >> group_name;
            leave_group(group_name, client_usernames[client_socket]);
        } else {
            std::string error_message = "Invalid command.";
            send(client_socket, error_message.c_str(), error_message.size(), 0);
        }
    }

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
        username_to_socket.erase(client_usernames[client_socket]);
        client_usernames.erase(client_socket);
    }
    close(client_socket);
}

int main() {
    // WSADATA wsaData;
    // WSAStartup(MAKEWORD(2, 2), &wsaData);

    load_users();

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    listen(server_socket, 5);
    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_len);
        if (client_socket >= 0) {
            std::thread(handle_client, client_socket).detach();
        }
    }
    // WSACleanup();
    return 0;
}