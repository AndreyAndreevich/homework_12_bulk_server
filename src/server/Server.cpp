#include "Server.h"
#include "Session.h"

Server::Server(io_service& service, const tcp::endpoint& ep, const std::shared_ptr<Handler>& handler)
  : acceptor_(service, ep)
  , socket_(service)
  , handler_(handler)
{
  do_accept();
}

Server::~Server() {

}

void Server::do_accept() {
  acceptor_.async_accept(socket_, [this](boost::system::error_code ec) {
    if (!ec) {
      std::make_shared<Session>(std::move(socket_),handler_)->start();
    }
    do_accept();
  });
}