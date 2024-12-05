#include "Connected_Client.h"
#include <iostream>
#include <string>
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::string;
void Connected_Client::recv_data(string& arg_recv_buffer, size_t const& arg_recv_buffer_size) {
  arg_recv_buffer.resize(arg_recv_buffer_size);
  recv_ret = recv(communicate_socket, arg_recv_buffer.data(), arg_recv_buffer_size, 0);
  if (recv_ret <= 0) {
    std::cerr << "receive failed\n";
    connect_status = false;
  } else {
    arg_recv_buffer.resize(recv_ret);
    cout << "server has received:" << arg_recv_buffer << endl;
  }
}
void Connected_Client::send_data(string const& arg_send_buffer) {
  size_t temp_buffer_size = arg_send_buffer.size();
  send_ret                = send(communicate_socket, arg_send_buffer.c_str(), temp_buffer_size, 0);
  if (send_ret > 0) {
    cout << "server has send:" << arg_send_buffer << endl;
  } else {
    cerr << "send failed\n";
    connect_status = false;
  }
}
void Connected_Client::send_data_by_getline(string& arg_send_buffer) {
  getline(cin, arg_send_buffer);
  if (arg_send_buffer.size() >= send_buffer_size - 1) {
    cerr << "the send buffer is too long\n";
    return;
  }
  send_data(arg_send_buffer);
}

Connected_Client::~Connected_Client() {
  closesocket(communicate_socket);
}
