#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <signal.h>
#include <utility>

#include "../include/cmd.parse/CommandParser.hpp"
using namespace std;
using namespace boost::asio;

#include "TcpConnection.hpp"
#include "ConnectionManager.hpp"

#include "AsyncTcpServer.hpp"

void handle_stop(boost::system::error_code const &, // error,
                 const int signal,                               // signal_number,
                 AsyncTcpServer &server)
{
  std::cout << "Shutting down. Signal number is " <<  signal << std::endl;
  server.stop();
}

int main(int argc, const char *argv[])
{
  if (argc <= 2)
  {
    std::cerr << "The required parameters(bulk size, port number) of bulk server are not defined!" << std::endl;
    return -1;
  }
  try
  {
    const auto inputParameterBulkSize = argv[1];
    const int bulkSize = std::atoi(inputParameterBulkSize);

    const auto inputParameterPort = argv[2];
    const int port = std::atoi(inputParameterPort);

    CommandParser parser(bulkSize);
    parser.init();

    io_service service;
    boost::asio::signal_set m_signals(service);
    m_signals.add(SIGINT);
    m_signals.add(SIGTERM);
    m_signals.add(SIGQUIT);

    AsyncTcpServer server(service, port, parser);

    m_signals.async_wait([&server](boost::system::error_code const &error, int signal_number)
                         { handle_stop(error, signal_number, server); });
    service.run();
  }
  catch (std::exception &e)
  {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
