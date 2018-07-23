#ifndef server_h
#define server_h

#include "Handler.h"
#include "Observer.h"
#include <boost/asio.hpp>

class Server {
  using io_service = boost::asio::io_service;
  using tcp = boost::asio::ip::tcp;
public:
  Server(io_service&, const tcp::endpoint&, const std::shared_ptr<Handler>&);
  ~Server();
private:
  void do_accept();

  std::shared_ptr<Handler> handler_;
  tcp::acceptor acceptor_;
  tcp::socket socket_;
};

#endif