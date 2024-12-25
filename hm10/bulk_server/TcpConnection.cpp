#include "TcpConnection.hpp"


TcpConnection::TcpConnection(ip::tcp::socket socket,
              CommandParser &cmdParser)
    : m_socket(std::move(socket)), m_cmdParser(cmdParser) {}


TcpConnection::pointer TcpConnection::create(ip::tcp::socket socket, CommandParser &cmdParser)
{
    return pointer(new TcpConnection(std::move(socket), cmdParser));
}

ip::tcp::socket & TcpConnection::socket()
{
    return m_socket;
}

void TcpConnection::start()
{
    m_socket.async_read_some(
        boost::asio::buffer(inputData, max_length),
        boost::bind(&TcpConnection::handle_read,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));

    /* m_socket.async_write_some(
         boost::asio::buffer("Work started", max_length),
         boost::bind(&TcpConnection::handle_write,
                     this,
                     boost::asio::placeholders::error,
                     boost::asio::placeholders::bytes_transferred));*/
}


void TcpConnection::parseInputData(const char *input)
{
    this->m_cmdParser.parse(std::string(input));
}

void TcpConnection::handle_read(const boost::system::error_code &err, size_t bytes_transferred)
{
    if (!err)
    {
        std::cout << "Accepted following data: " << inputData << std::endl;
        m_thread = std::thread(&TcpConnection::parseInputData, this, inputData);
    }
    else
    {
        std::cerr << "error: " << err.message() << std::endl;
        m_socket.close();
    }
}

void TcpConnection::handle_write(const boost::system::error_code &err, size_t bytes_transferred)
{
    if (!err)
    {
        std::cout << "Server perform parse cmd work" << std::endl;
    }
    else
    {
        std::cerr << "error: " << err.message() << std::endl;
        m_socket.close();
    }
}

void TcpConnection::stop()
{
    if (m_thread.joinable())
        m_thread.join();

    m_cmdParser.endJob();
    m_cmdParser.stop();
    m_socket.close();
}

TcpConnection::~TcpConnection()
{
    stop();
}