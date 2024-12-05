#include "Server.h"

#include "Connected_Client.h"

#include <algorithm>
#include <cstddef>
#include <ctime>
#include <filesystem>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

#include <windows.h>
#include <winsock.h>
using std::cerr;
using std::cout;
using std::string;
using std::vector;
Server::Server() : thread_pool(thread_amount) {
  local_addr.sin_addr.s_addr = INADDR_ANY;
  local_addr.sin_port        = htons(PORT);
  local_addr.sin_family      = AF_INET;
  if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
    cerr << "Failed to initialize Winsock\n";
    exit(1);
  }
  listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  ret           = bind(listen_socket, (struct sockaddr*)(&local_addr), sizeof(sockaddr));
  if (ret != 0) {
    cerr << "failed to bind\n";
    exit(1);
  }
  ret = listen(listen_socket, listen_amount);
  if (ret != 0) {
    cerr << "failed to listen\n";
    exit(1);
  }
  connected_client_vector.resize(static_cast<int>(thread_amount / 3 * 2));
  record_writer.open(get_time_now(), std::ios::app);
  if (!record_writer.is_open()) {
    cerr << "failed to open communicate record\n";
    exit(1);
  }
}
string Server::get_time_now() {
  std::time_t now     = std::time(nullptr);
  std::tm* local_time = std::localtime(&now);
  std::ostringstream oss;
  oss << std::put_time(local_time, "%Y-%m-%d_%H-%M-%S");
  return R"(D:\platform_record\communicate_record\)" + oss.str() + ".txt";
}
auto Server::waiting_for_empty_client() {
  auto iter = connected_client_vector.end();
  while (iter == connected_client_vector.end()) {
    iter = std::ranges::find_if_not(connected_client_vector, &Connected_Client::connect_status);
  }
  iter->connect_status = true;
  return iter;
}
void Server::server_accept(vector<Connected_Client>::iterator iter) {
  (*iter).communicate_socket = accept(listen_socket, (sockaddr*)(&(*iter).client_addr), &addrlen);
  if ((*iter).communicate_socket == -1) {
    std::cerr << "failed to accept\n";
    (*iter).connect_status = false;
    return;
  }
  cout << "IP:" << ntohl((*iter).client_addr.sin_addr.S_un.S_addr) << " Port:" << ntohs((*iter).client_addr.sin_port)
       << " has connected\n";
}
void Server::keep_communicating(vector<Connected_Client>::iterator iter) {
  while (true) {
    iter->recv_data(iter->recv_buffer, recv_buffer_size);
    if (!iter->connect_status) {
      broad_cast(iter->name + ":has disconnected\n");
      break;
    }
    iter->send_buffer = iter->name + ":" + iter->recv_buffer;
    broad_cast(iter->send_buffer);
    if (!iter->connect_status) {
      broad_cast(iter->name + ":has disconnected\n");
      break;
    }
  }
}
void ::Server::broad_cast(string const& str) {
  for (auto& connected_client : connected_client_vector) {
    if (connected_client.connect_status) {
      connected_client.send_data(str);
    }
  }
  write_communicate_record(str);
}
void Server::connecting(vector<Connected_Client>::iterator iter) {
  iter->recv_data(iter->name, name_size);
  if (iter->name == "GET /checkAliveServer?d") {
    iter->connect_status = false;
    return;
  }
  if (!iter->connect_status) {
    broad_cast("unknown client has disconnected");
    return;
  }
  broad_cast(iter->name + ":has connected");
  keep_communicating(iter);
}
void Server::connect_process() {
  auto iter = waiting_for_empty_client();
  server_accept(iter);
  thread_pool.add_task([this](vector<Connected_Client>::iterator iter) { connecting(iter); }, iter);
}
void Server::read_communicate_record(vector<Connected_Client>::iterator iter, string const& file) {
  iter->record_reader.open(file);
  if (!iter->record_reader.is_open()) {
    cerr << "failed to open\n";
    return;
  }
  if (!iter->record_reader.eof()) {
    getline(iter->record_reader, iter->send_buffer);
    iter->send_data(iter->send_buffer);
  }
}
void Server::write_communicate_record(string const& str) {
  record_writer << str;
}