
#include "AsyncTcpServer.hpp"

AsyncTcpServer::AsyncTcpServer(io_service &service, const int port, CommandParser &parser)
    : m_cmdParser(parser),
      m_service(service),
      m_acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), port))
{
    m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    m_cmdParser.init();
    do_accept();
}
AsyncTcpServer::~AsyncTcpServer()
{
    stop();
}

void AsyncTcpServer::stop()
{
    m_cmdParser.stop();
    m_connection_manager.stop_all();
    m_acceptor.close();
}

void AsyncTcpServer::do_accept()
{
    m_acceptor.async_accept(
        [this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket)
        {
            // Check whether the server was stopped by a signal before this
            // completion handler had a chance to run.
            if (!m_acceptor.is_open())
            {
                return;
            }

            if (!ec)
            {
                m_connection_manager.start(TcpConnection::create(std::move(socket), m_cmdParser));
            }

            do_accept();
        });
}
