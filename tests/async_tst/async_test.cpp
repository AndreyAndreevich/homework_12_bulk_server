#define BOOST_TEST_LOG_LEVEL test_suite
#define BOOST_TEST_MODULE test_main
#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "async.h"
#include "Storage.h"
#include "Writers.h"
#include "EventLoop.h"

#include <thread>
#include <chrono>

using Commands = std::vector<std::string>;

void clear(EventCode code) {
    bool isClear = false;
    auto lmd = [&isClear](){
        Storage::Instance().clear();
        isClear = true;
    };
    EventLoop::Instance().send(code,lmd);
    while(!isClear) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  
    }
}

BOOST_AUTO_TEST_SUITE(test_async)

    BOOST_AUTO_TEST_CASE(test_connect) {
        std::size_t bulk = 5;
        void* h;
        BOOST_CHECK_NO_THROW(h = async::connect(bulk));
        BOOST_CHECK_THROW(async::connect(-1),std::exception);
        BOOST_CHECK_THROW(async::connect(0),std::exception);
        clear(EventCode::NOW);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(test_receive) {
        auto h1 = async::connect(2);
        BOOST_CHECK_NO_THROW(async::receive(h1,"1", 1));
        BOOST_CHECK_THROW(async::receive(nullptr,"1", 1),std::exception);
        BOOST_CHECK_THROW(async::receive(h1,"1", 2),std::exception);
        BOOST_CHECK_THROW(async::receive(h1,"11", 1),std::exception);
        BOOST_CHECK_THROW(async::receive(h1,"11", -1),std::exception);
        int i = 5;
        auto h2 = reinterpret_cast<void*>(&i);
        async::receive(h2,"1", 1);
        auto lmd = [](){
            BOOST_CHECK_EQUAL(EventLoop::Instance().getException(),"map::at");
        };
        EventLoop::Instance().send(EventCode::ASYNC,lmd);
        clear(EventCode::ASYNC);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(test_disconnect) {
        auto h = async::connect(2);
        BOOST_CHECK_NO_THROW(async::receive(h,"1", 1));
        BOOST_CHECK_NO_THROW(async::disconnect(h));
        BOOST_CHECK_THROW(async::disconnect(nullptr),std::exception);
        async::disconnect(h);
        auto lmd = [](){
            BOOST_CHECK_EQUAL(EventLoop::Instance().getException(),"map::at");
        };
        EventLoop::Instance().send(EventCode::ASYNC,lmd);
        clear(EventCode::ASYNC);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(test_write) {
        std::stringstream console_stream;
        auto h = async::connect(2);
        int id = *static_cast<int*>(h);
        auto lmd1 = [id,&console_stream](){
            std::shared_ptr<Handler> handler;
            std::vector<std::shared_ptr<Observer>> writers;
            std::string buffer;
            std::tie(handler,writers,buffer) = Storage::Instance().getHandler(id);
            BOOST_CHECK_EQUAL(writers.size(),2);
            auto console_writer = dynamic_cast<ConsoleWriter*>(writers[0].get());
            auto file_writer = dynamic_cast<FileWriter*>(writers[1].get());
            console_writer->setOutStream(console_stream);
        };
        EventLoop::Instance().send(EventCode::NOW,lmd1);
        BOOST_CHECK_NO_THROW(async::receive(h,"cmd", 3));
        auto lmd2 = [id](){
            auto buffer = std::get<2>(Storage::Instance().getHandler(id));
            BOOST_CHECK_EQUAL(buffer,"cmd");
        };
        EventLoop::Instance().send(EventCode::ASYNC,lmd2);
        
        BOOST_CHECK_NO_THROW(async::receive(h,"1\n", 2));
        auto lmd3 = [id](){
            auto buffer = std::get<2>(Storage::Instance().getHandler(id));
            BOOST_CHECK_EQUAL(buffer,"");
        };
        EventLoop::Instance().send(EventCode::ASYNC,lmd3);

        BOOST_CHECK_NO_THROW(async::receive(h,"cmd2\n", 5));
        EventLoop::Instance().send(EventCode::ASYNC,lmd3);

        auto lmd4 = [id,&console_stream](){
            auto writers = std::get<1>(Storage::Instance().getHandler(id));
            auto file_writer = dynamic_cast<FileWriter*>(writers[1].get());
            std::ifstream file{file_writer->getName()};
            std::stringstream file_stream;
            file_stream << file.rdbuf();
            file.close();
            std::remove(file_writer->getName().c_str());
            BOOST_CHECK_EQUAL(console_stream.str(), "bulk: cmd1, cmd2\n");
            BOOST_CHECK_EQUAL(file_stream.str(), "bulk: cmd1, cmd2");
        };
        EventLoop::Instance().send(EventCode::ASYNC,lmd4);
        clear(EventCode::ASYNC);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(tusk)
    {
        std::stringstream console_stream1;
        std::stringstream console_stream2;
        std::shared_ptr<Handler> handler;
        std::vector<std::shared_ptr<Observer>> writers;
        std::string buffer;
        auto file_names1 = std::make_shared<std::vector<std::string>>();
        auto file_names2 = std::make_shared<std::vector<std::string>>();

        std::size_t bulk = 5;
        auto h1 = async::connect(bulk);
        auto h2 = async::connect(bulk);
        int id1 = *static_cast<int*>(h1);
        int id2 = *static_cast<int*>(h2);

        std::tie(handler,writers,buffer) = Storage::Instance().getHandler(id1);
        auto console_writer1 = dynamic_cast<ConsoleWriter*>(writers[0].get());
        auto file_writer1 = dynamic_cast<FileWriter*>(writers[1].get());
        console_writer1->setOutStream(console_stream1);
        file_writer1->setNameList(file_names1);

        std::tie(handler,writers,buffer) = Storage::Instance().getHandler(id2);
        auto console_writer2 = dynamic_cast<ConsoleWriter*>(writers[0].get());
        auto file_writer2 = dynamic_cast<FileWriter*>(writers[1].get());
        console_writer2->setOutStream(console_stream2);
        file_writer2->setNameList(file_names2);
        
        async::receive(h1, "1", 1);
        auto lmd1 = [id1](){
            auto buffer = std::get<2>(Storage::Instance().getHandler(id1));
            BOOST_CHECK_EQUAL(buffer,"1");
        };
        EventLoop::Instance().send(EventCode::ASYNC,lmd1);

        async::receive(h2, "1\n", 2);
        auto lmd2 = [id2](){
            auto buffer = std::get<2>(Storage::Instance().getHandler(id2));
            BOOST_CHECK_EQUAL(buffer,"");
        };
        EventLoop::Instance().send(EventCode::ASYNC,lmd2);

        async::receive(h1, "\n2\n3\n4\n5\n6\n{\na\n", 15);
        auto lmd3 = [id1,&file_names1,&console_stream1](){
            std::stringstream file_stream;
            auto buffer = std::get<2>(Storage::Instance().getHandler(id1));
            BOOST_CHECK_EQUAL(buffer,"");
            BOOST_CHECK_EQUAL(file_names1->size(),2);
            std::ifstream file{file_names1->at(0)};
            file_stream << file.rdbuf();
            file.close();
            std::remove(file_names1->at(0).c_str());

            BOOST_CHECK_EQUAL(console_stream1.str(), "bulk: 1, 2, 3, 4, 5\nbulk: 6\n");
            BOOST_CHECK_EQUAL(file_stream.str(), "bulk: 1, 2, 3, 4, 5");
            console_stream1.str("");
            file_stream.str("");

            file.open(file_names1->at(1));
            file_stream << file.rdbuf();
            file.close();
            std::remove(file_names1->at(1).c_str());
            BOOST_CHECK_EQUAL(file_stream.str(), "bulk: 6");
            file_names1->clear();
        };
        EventLoop::Instance().send(EventCode::ASYNC,lmd3);

        async::receive(h1, "b\nc\nd\n}\n89\n", 11);
        auto lmd4 = [id1,&file_names1,&console_stream1](){
            std::stringstream file_stream;
            auto buffer = std::get<2>(Storage::Instance().getHandler(id1));
            BOOST_CHECK_EQUAL(buffer,"");
            BOOST_CHECK_EQUAL(file_names1->size(),1);

            std::ifstream file{file_names1->at(0)};
            file_stream << file.rdbuf();
            file.close();
            std::remove(file_names1->at(0).c_str());

            BOOST_CHECK_EQUAL(console_stream1.str(), "bulk: a, b, c, d\n");
            BOOST_CHECK_EQUAL(file_stream.str(), "bulk: a, b, c, d");
            console_stream1.str("");
            file_names1->clear();
        };
        EventLoop::Instance().send(EventCode::ASYNC,lmd4);
        async::disconnect(h1);
         EventLoop::Instance().send(EventCode::ASYNC,[](){
            BOOST_CHECK_EQUAL(1,1);
        });
        async::disconnect(h2);
         EventLoop::Instance().send(EventCode::ASYNC,[](){
            BOOST_CHECK_EQUAL(1,1);
        });
        auto lmd5 = [&file_names1,&file_names2,&console_stream1,&console_stream2](){
            std::stringstream file_stream;
            BOOST_CHECK_EQUAL(file_names1->size(),1);
            std::ifstream file{file_names1->at(0)};
            file_stream << file.rdbuf();
            file.close();
            std::remove(file_names1->at(0).c_str());
            BOOST_CHECK_EQUAL(console_stream1.str(), "bulk: 89\n");
            BOOST_CHECK_EQUAL(file_stream.str(), "bulk: 89");
            console_stream1.str("");
            file_stream.str("");
            file_names1->clear();

            BOOST_CHECK_EQUAL(file_names2->size(),1);
            file.open(file_names2->at(0));
            file_stream << file.rdbuf();
            file.close();
            std::remove(file_names2->at(0).c_str());
            BOOST_CHECK_EQUAL(console_stream2.str(), "bulk: 1\n");
            BOOST_CHECK_EQUAL(file_stream.str(), "bulk: 1");
        };
        EventLoop::Instance().send(EventCode::ASYNC,lmd5);
        clear(EventCode::ASYNC);
    }

BOOST_AUTO_TEST_SUITE_END()