#pragma once
#include "Connected_Client.h"
#include "Thread_Pool.h"

#include <fstream>
#include <string>
#include <vector>

#include <winsock.h>
int const PORT          = 9430;
int const thread_amount = 12;
int const listen_amount = 64;

class Server {
 private:
  std::mutex mtx;
  Thread_Pool thread_pool;
  int ret = 0;
  WSADATA wsadata;
  sockaddr_in local_addr;
  SOCKET listen_socket;
  int addrlen      = sizeof(sockaddr_in);
  bool name_status = true;
  std::vector<Connected_Client> connected_client_vector;
  std::ofstream record_writer;

 public:
  Server();
  Server(Server const& server)           = delete;
  Server(Server&& server)                = delete;
  Server operator=(Server const& server) = delete;
  Server& operator=(Server&& server) = delete;  //移动构造先删了，有需要再写，其他类都如此（我是懒狗
  void server_accept(std::vector<Connected_Client>::iterator iter);  //连接管理 等待连接一个客户端
  void pool_server_accept();                                         //连接管理
  ~Server() {
    closesocket(listen_socket);
    record_writer.close();
  }
  void broad_cast(std::string const& str);  //广播
  void keep_communicating(std::vector<Connected_Client>::iterator iter);
  auto waiting_for_empty_client();  //找到一个空缺的客户端
  void connecting(std::vector<Connected_Client>::iterator iter);
  void connect_process();
  static std::string get_time_now();
  void write_communicate_record(std::string const& str);
  void read_communicate_record(std::vector<Connected_Client>::iterator iter,std::string const& file);
  void special_command(std::string const& command);
  void show_file(std::string const& category);
};