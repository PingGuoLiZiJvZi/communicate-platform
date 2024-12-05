#pragma once
#include <cstddef>
#include <string>
#include <winsock.h>
size_t const recv_buffer_size = 224;
size_t const send_buffer_size = 200;
size_t const name_size        = recv_buffer_size - send_buffer_size - 1;
int const PORT                = 9430;
std::string const SERVER_IP   = "10.203.200.172";
int const waiting_time=200;
class Client {
 private:
  int ret      = 0;
  int send_ret = 0;
  int recv_ret = 0;
  WSADATA wsadata;
  sockaddr_in local_addr, client_addr;
  SOCKET communicate_socket;
  int addrlen = sizeof(sockaddr_in);
  std::string send_buffer, recv_buffer, name;

 public:
  Client();
  void client_connect();
  void recv_data(std::string& arg_recv_buffer, size_t arg_recv_buffer_size);
  void send_data(std::string& arg_send_buffer);
  void send_data_by_getline(std::string& arg_send_buffer, size_t send_buffer_size);
  void communicate_to_server();
  Client(Client const& client)            = delete;
  Client& operator=(Client const& client) = delete;
  Client(Client&& client)                 = delete;
  Client& operator=(Client&& client)      = delete;  //移动构造先删了，有必要再写
  ~Client();
  [[noreturn]] void connect_process();
};