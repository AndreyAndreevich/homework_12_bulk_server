#include "Session.h"
#include <iostream>

uint Session::session_count_ = 0;

Session::Session(tcp::socket socket, const std::shared_ptr<Handler>& handler) 
  : socket_(std::move(socket))
  , handler_(handler)
{
  session_count_++;
}

Session::~Session() {
  session_count_--;
  if (session_count_ == 0) {
    handler_->stop();
  }
}

void Session::start() {
  do_read();
}

void Session::do_read() {
  auto self(shared_from_this());
  boost::asio::async_read_until(socket_, response_,  sign_,
    [this, self](boost::system::error_code ec, std::size_t /*length*/)
    {
      std::istream is(&response_);
      std::ostream out(&buffer_);
      std::string line;
      if (!ec)
      {
        while (std::getline(is, line)) {
          auto block = parser_.parsing(line);
          if (block == BlockParser::StartBlock) {
            sign_ = "\n}\n";
          } else if (block == BlockParser::CancelBlock) {
            sign_ = "\n";
            std::cout << "size = " << buffer_.size() << '\n';
            std::istream is(&buffer_);
            std::string line;   
            while (std::getline(is, line)) {
              std::cout << "buffer " << line << '\n';
              handler_->addCommand(line);
            }
          } else if (block == BlockParser::Command) {
            if (parser_.blocks_count == 0) {
              handler_->addCommand(line);
            } else {
              out << line << '\n';
            }
          }
        }
        do_read();
      } else {

      }
    });
}