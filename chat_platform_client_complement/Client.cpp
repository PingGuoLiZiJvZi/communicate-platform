#include "Client.h"

#include <iostream>
#include <string>
#include <thread>

#include <winsock.h>
using std::cerr;
using std::cin;
using std::cout;
using std::string;
Client::Client() {
  // cout << "please enter your user name\n";
  // cin.getline(name, name_size);
  local_addr.sin_addr.s_addr = inet_addr(SERVER_IP.data());
  local_addr.sin_port        = htons(PORT);
  local_addr.sin_family      = AF_INET;
  if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
    cerr << "Failed to initialize Winsock\n";
    exit(1);
  }
  communicate_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  cout << "please input you name\n";
  getline(cin, name);
  if (name.size() >= name_size - 1) {
    cout << "name is less than 22 words,please input again\n";
    getline(cin, name);
  }
}
void Client::client_connect() {
  ret = connect(communicate_socket, (sockaddr*)(&local_addr), sizeof(sockaddr));
  if (ret == -1) {
    cerr << "failed to connect\n";
    exit(1);
  }
  cout << "has connected to the server\n";
}
void Client::recv_data(string& arg_recv_buffer, size_t arg_recv_buffer_size) {
  arg_recv_buffer.resize(arg_recv_buffer_size);
  recv_ret = recv(communicate_socket, arg_recv_buffer.data(), arg_recv_buffer_size, 0);
  if (recv_ret <= 0) {
    std::cerr << "receive failed\n";
    exit(0);
  } else {
    arg_recv_buffer.resize(recv_ret);
  }
}
void Client::send_data(string& arg_send_buffer) {
  size_t temp_buffer_size = arg_send_buffer.size();
  send_ret                = send(communicate_socket, arg_send_buffer.c_str(), temp_buffer_size, 0);
  if (send_ret <= 0) {
    cerr << "send failed\n";
    exit(0);
  }
}
void Client::send_data_by_getline(string& arg_send_buffer, size_t arg_send_buffer_size) {
  Sleep(200);
  getline(cin, arg_send_buffer);
  while (arg_send_buffer.size() >= arg_send_buffer_size - 1) {
    cout << "input is too long,please try again\n";
    getline(cin, arg_send_buffer);
  }
  cout << "\033[A\033[K";
  send_data(arg_send_buffer);
}
Client::~Client() {
  closesocket(communicate_socket);
  WSACleanup();
}
void Client::communicate_to_server() {
  send_data_by_getline(send_buffer, send_buffer_size);
}
void Client::connect_process() {
  Sleep(200);
  send_data(name);
  cout << "you have successfully connected\n";
  std::thread input_thread([this]() {
    while (true) {
      send_data_by_getline(send_buffer, send_buffer_size);
    }
  });
  while (true) {
    recv_data(recv_buffer, recv_buffer_size);
    cout << recv_buffer << '\n';
  }
  input_thread.join();
}