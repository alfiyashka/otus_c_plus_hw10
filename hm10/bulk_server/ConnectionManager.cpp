#include "ConnectionManager.hpp"

connection_manager::connection_manager()
{
}

void connection_manager::start(TcpConnection::pointer c)
{
  connections_.insert(c);
  c->start();
}

void connection_manager::stop(TcpConnection::pointer c)
{
  connections_.erase(c);
  c->stop();
}

void connection_manager::stop_all()
{
  for (auto c: connections_)
    c->stop();
  connections_.clear();
}

