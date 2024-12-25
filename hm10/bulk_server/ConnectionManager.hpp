#ifndef _CONNECTION_MANAGER_HPP
#define _CONNECTION_MANAGER_HPP

#include <set>
#include "TcpConnection.hpp"


class connection_manager
{
public:
  connection_manager(const connection_manager&) = delete;
  connection_manager& operator=(const connection_manager&) = delete;

  /// Construct a connection manager.
  connection_manager();

  /// Add the specified connection to the manager and start it.
  void start(TcpConnection::pointer c);

  /// Stop the specified connection.
  void stop(TcpConnection::pointer c);

  /// Stop all connections.
  void stop_all();

private:
  /// The managed connections.
  std::set<TcpConnection::pointer> connections_;
};

#endif