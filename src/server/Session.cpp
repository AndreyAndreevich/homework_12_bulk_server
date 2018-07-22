#include "Session.h"
#include <iostream>

Session::Session(tcp::socket socket, const std::shared_ptr<Handler>& handler, 
  const std::shared_ptr<uint>& count) 
  : socket_(std::move(socket))
  , handler_(handler)
  , session_count_(count)
{
  (*session_count_)++;
}

Session::~Session() {
  (*session_count_)--;
  if (*session_count_ == 0) {
    handler_->stop();
  }
}

void Session::start() {
  do_read();
}

void Session::do_read() {
  auto self(shared_from_this());
  boost::asio::async_read_until(socket_, response_,  '\n',
    [this, self](boost::system::error_code ec, std::size_t /*length*/)
    {
      if (!ec)
      {
        std::istream is(&response_);
        std::string line;
        while (std::getline(is, line)) {
          handler_->addCommand(line);
        }
        do_read();
      }
    });
}