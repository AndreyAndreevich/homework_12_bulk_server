#ifndef session_h
#define session_h

#include "Handler.h"
#include <boost/asio.hpp>

class Session : public std::enable_shared_from_this<Session> {
  using tcp = boost::asio::ip::tcp;
public:
  Session(tcp::socket, const std::shared_ptr<Handler>&, const std::shared_ptr<uint>&);
  ~Session();
  void start();
private:
  void do_read();

  tcp::socket socket_;
  std::shared_ptr<Handler> handler_;
  boost::asio::streambuf response_;
  std::shared_ptr<uint> session_count_;
};

#endif