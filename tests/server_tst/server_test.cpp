#define BOOST_TEST_LOG_LEVEL test_suite
#define BOOST_TEST_MODULE test_main
#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "Server.h"
#include "Writers.h"

using namespace boost::asio;

std::mutex mtx;
std::condition_variable cv;
bool is_ready = false;

class TestHandler : public Handler 
{
  uint count = 0;
  uint stop_count;
public:
  TestHandler(int n,uint stop_count_) : Handler(n) , stop_count(stop_count_) {}
  void stop() override {
    Handler::stop();
    count++;
    if (count == stop_count) {
      std::unique_lock<std::mutex> lk(mtx);
      is_ready = false;
      cv.notify_one();
    }
  }
};

auto start_server(io_service& service, std::stringstream& out, uint count) {
    auto thread = std::make_unique<std::thread>([&service,&out,count](){
        ip::tcp::endpoint ep(ip::tcp::v4(),9000);
        auto handler = std::make_shared<TestHandler>(3,count);
        auto consoleWriter = std::make_shared<ConsoleWriter>(out);
        consoleWriter->subscribe(handler);
        Server server(service, ep, handler); 
        {
          std::unique_lock<std::mutex> lk(mtx);
          is_ready = true;
          cv.notify_one();
        }
        service.run();
    });
    {
      std::unique_lock<std::mutex> lk(mtx);
      cv.wait(lk,[]() {
          return is_ready;
      });
    }
    return std::move(thread);
}

BOOST_AUTO_TEST_SUITE(server_test)
    
    BOOST_AUTO_TEST_CASE(one_server)
    {
        std::stringstream out;
        io_service service;
        auto server_thread = start_server(service,out,3);
        {
            ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 9000);
            ip::tcp::socket sock(service);
            sock.connect(ep);
            std::string command = "cmd1\n"
                                  "cmd2\n"
                                  "{\n"
                                  "cmd3\n"
                                  "cmd4\n"
                                  "}\n"
                                  "cmd5\n";
            write(sock, buffer(command, command.size()));
        }
        {
          std::unique_lock<std::mutex> lk(mtx);
          cv.wait(lk,[]() {
              return !is_ready;
          });
        }
        std::string result = "bulk: cmd1, cmd2\n"
                             "bulk: cmd3, cmd4\n"
                             "bulk: cmd5\n";
        service.stop();
        server_thread->join();
        BOOST_CHECK_EQUAL(out.str(),result);
    }

///////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(two_servers) 
    {
        std::stringstream out;
        io_service service;
        auto server_thread = start_server(service,out,3);
        {
            ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 9000);
            ip::tcp::socket sock1(service);
            ip::tcp::socket sock2(service);
            sock1.connect(ep);
            sock2.connect(ep);
            std::string command = "cmd1\n"
                                  "cmd2\n";
            write(sock1, buffer(command, command.size()));
            command = "cmd3\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            write(sock2, buffer(command, command.size()));
            command = "cmd4\n";
            write(sock1, buffer(command, command.size()));
            command = "{\n"
                      "cmd5\n";
            write(sock2, buffer(command, command.size()));
            command = "cmd6\n"
                      "cmd7\n"
                      "cmd8\n";
            write(sock1, buffer(command, command.size()));
            command = "cmd9\n"
                      "}\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            write(sock2, buffer(command, command.size()));
            command = "cmd10\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            write(sock1, buffer(command, command.size()));
            command = "cmd11\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            write(sock2, buffer(command, command.size()));
            command = "cmd12\n"
                      "cmd13\n";
            write(sock1, buffer(command, command.size()));
        }
        {
          std::unique_lock<std::mutex> lk(mtx);
          cv.wait(lk,[]() {
              return !is_ready;
          });
        }
        std::string result = "bulk: cmd1, cmd2, cmd3\n"
                             "bulk: cmd4, cmd6, cmd7\n"
                             "bulk: cmd8\n"
                             "bulk: cmd5, cmd9\n"
                             "bulk: cmd10, cmd11, cmd12\n"
                             "bulk: cmd13\n";
        service.stop();
        server_thread->join();
        BOOST_CHECK_EQUAL(out.str(),result);
    }

BOOST_AUTO_TEST_SUITE_END()
