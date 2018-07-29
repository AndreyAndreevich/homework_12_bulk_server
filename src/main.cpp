#include <iostream>
#include <exception>

#include "Server.h"
#include "Writers.h"

using namespace boost::asio;

int main(int argc, char *argv[])
{
  try {
    io_service service;
    ip::tcp::endpoint ep(ip::tcp::v4(),start_parsing(argc,argv,1));
    auto handler = std::make_shared<Handler>(start_parsing(argc,argv,2));
    auto consoleWriter = std::make_shared<ConsoleWriter>();
    auto fileWriter = std::make_shared<FileWriter>();
    consoleWriter->subscribe(handler);
    fileWriter->subscribe(handler);
    Server server(service, ep, handler);

    service.run();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
