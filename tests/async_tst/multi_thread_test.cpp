#define BOOST_TEST_LOG_LEVEL test_suite
#define BOOST_TEST_MODULE test_main
#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "async.h"
#include "Storage.h"
#include "Writers.h"
#include "EventLoop.h"
#include <thread>
#include <iostream>

using Commands = std::vector<std::string>;

BOOST_AUTO_TEST_SUITE(test_multi_thread)

    BOOST_AUTO_TEST_CASE(tusk)
    {
        std::mutex mtx;
        std::condition_variable c_v;
        int N_finish = 0;
        int N_threads = 10;
        int N_repeat = 1000;
        std::vector<std::thread> v;
        v.reserve(N_threads);
        std::vector<std::shared_ptr<std::stringstream>> v_stream;
        v_stream.reserve(N_threads);

        for(auto i = 0; i < N_threads; i++) {
            v.emplace_back(std::thread([N_repeat,&v_stream,&N_finish,&mtx,&c_v](){
                auto file_names = std::make_shared<std::vector<std::string>>();
                std::size_t bulk = 5;
                auto h = async::connect(bulk);
                int id = *static_cast<int*>(h);
                auto lmd1 = [id,file_names,&v_stream](){
                    std::shared_ptr<Handler> handler;
                    std::vector<std::shared_ptr<Observer>> writers;
                    std::string buffer;
                    std::tie(handler,writers,buffer) = Storage::Instance().getHandler(id);
                    auto console_writer = dynamic_cast<ConsoleWriter*>(writers[0].get());
                    auto file_writer = dynamic_cast<FileWriter*>(writers[1].get());
                    auto console_stream = std::make_shared<std::stringstream>();
                    console_writer->setOutStream(*console_stream);
                    v_stream.push_back(console_stream);
                    file_writer->setNameList(file_names);
                };
                EventLoop::Instance().send(EventCode::NOW,lmd1);
                for (auto j = 0; j < N_repeat; j++) {            
                    async::receive(h, "1", 1);
                    async::receive(h, "\n2\n3\n4\n5\n", 9);
                }
                bool f = false;
                EventLoop::Instance().send(EventCode::ASYNC,[&f](){
                    f = true;
                });               
                auto lmd2 = [file_names,N_repeat]() {
                    BOOST_CHECK_EQUAL(file_names->size(), N_repeat);
                    for (const auto& file_name : *file_names) {
                        std::stringstream file_stream;
                        std::ifstream file{file_name};
                        file_stream << file.rdbuf();
                        file.close();
                        BOOST_CHECK_EQUAL(file_stream.str(), "bulk: 1, 2, 3, 4, 5");
                        std::remove(file_name.c_str());
                    }
                };
                EventLoop::Instance().send(EventCode::ASYNC,lmd2);
                async::disconnect(h);
                EventLoop::Instance().send(EventCode::ASYNC,[&N_finish,&c_v](){
                    N_finish++;
                    c_v.notify_one();
                });
            }));   
        }
        std::unique_lock<std::mutex> lk(mtx);
        c_v.wait(lk,[&N_finish,N_threads] {
            return N_finish == N_threads;
        });
        std::for_each(v.begin(),v.end(), [](auto& t) {
          t.join();
        });
    }

BOOST_AUTO_TEST_SUITE_END()