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
      if (!ec)
      {
        std::istream is(&response_);
        std::ostream out(&buffer_);
        std::string line;
        while (std::getline(is, line)) {
          auto block = parser_.parsing(line);
          if (block == BlockParser::StartBlock) {
            sign_ = "}\n";
          } else if (block == BlockParser::CancelBlock) {
            sign_ = "\n";
            handler_->stop();
            handler_->accumulate();
            std::istream is(&buffer_);
            std::string line;  
            while (std::getline(is, line)) {
              handler_->addCommand(line);
            }
            handler_->stop();
          } else if (block == BlockParser::Command) {
            if (parser_.is_block) {
              out << line << '\n';
            } else {
              handler_->addCommand(line);
            }
          }
        }
        do_read();
      }
    });
}