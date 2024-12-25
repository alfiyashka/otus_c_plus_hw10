#ifndef __ASYNC_TCP_SERVER__
#define __ASYNC_TCP_SERVER__
#include <iostream>
#include <boost/asio.hpp>

#include <signal.h>
#include <utility>

#include "../include/cmd.parse/CommandParser.hpp"
using namespace std;
using namespace boost::asio;

#include "TcpConnection.hpp"
#include "ConnectionManager.hpp"

struct AsyncTcpServer
{
  AsyncTcpServer(io_service &service, const int port, CommandParser &parser);
  ~AsyncTcpServer();
  void stop();
private:
  void do_accept();

  ip::tcp::acceptor m_acceptor;
  io_service &m_service;

  CommandParser &m_cmdParser; // cmd parser

  // The manager for connections.
  connection_manager m_connection_manager;
 
};

#endif