#ifndef session_h
#define session_h

#include "Handler.h"
#include <boost/asio.hpp>

class Session : public std::enable_shared_from_this<Session> {
  using tcp = boost::asio::ip::tcp;
public:
  Session(tcp::socket, const std::shared_ptr<Handler>&);
  ~Session();
  void start();
private:
  void do_read();

  tcp::socket socket_;
  std::shared_ptr<Handler> handler_;
  boost::asio::streambuf response_;
  boost::asio::streambuf buffer_;
  static uint session_count_;
  BlockParser parser_;
  std::string sign_ = "\n";
};

#endif