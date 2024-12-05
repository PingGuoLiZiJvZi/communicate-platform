#pragma once
#include <atomic>
#include <cstddef>
#include <string>
#include<fstream>
static size_t const recv_buffer_size = 200;
static size_t const send_buffer_size = 224;
static size_t const name_size        = send_buffer_size - recv_buffer_size - 1;
#include <winsock.h>
class Connected_Client {
 public:
  std::atomic<bool> connect_status = false;
  sockaddr_in client_addr{};
  std::string name;
  SOCKET communicate_socket{};
  std::string recv_buffer;
  std::string send_buffer;
  std::ifstream record_reader;
  int send_ret                                                         = 0;
  int recv_ret                                                         = 0;
  Connected_Client()                                                   = default;
  Connected_Client(Connected_Client const& connected_client)           = delete;
  Connected_Client operator=(Connected_Client const& connected_client) = delete;
  
  Connected_Client& operator=(Connected_Client&& other) noexcept {
    if (this != &other) {
      connect_status = false;
      client_addr    = other.client_addr;
      name           = std::move(other.name);
      closesocket(communicate_socket);
    
      communicate_socket       = other.communicate_socket;
    
      recv_buffer              = std::move(other.recv_buffer);
      send_buffer              = std::move(other.send_buffer);
      send_ret                 = other.send_ret;
      recv_ret                 = other.recv_ret;
      other.communicate_socket = INVALID_SOCKET;
    
    }
    return *this;
  }
  Connected_Client(Connected_Client&& other) noexcept
      : connect_status(false),
        client_addr(other.client_addr),
        name(std::move(other.name)),
        communicate_socket(other.communicate_socket),
    
        recv_buffer(std::move(other.recv_buffer)),
        send_buffer(std::move(other.send_buffer)),
        send_ret(other.send_ret),
        recv_ret(other.recv_ret) {
    closesocket(communicate_socket);

    other.communicate_socket = INVALID_SOCKET;
  }
  void get_name();  
  void recv_data(std::string& arg_recv_buffer, size_t const& arg_recv_buffer_size);
  void send_data(const std::string& arg_send_buffer);
  void send_data_by_getline(std::string& arg_send_buffer);
  void write_communicate_record();
  void read_communicate_record();
  ~Connected_Client();
};