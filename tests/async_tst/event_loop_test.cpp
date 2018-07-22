#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "EventLoop.h"
#include "Storage.h"

#include <thread>
#include <chrono>

using Commands = std::vector<std::string>;

BOOST_AUTO_TEST_SUITE(test_event_loop)

    BOOST_AUTO_TEST_CASE(start_test) {
        auto id = 0;
        auto lmd = [&id](){id = 1;};
        EventLoop::Instance().send(EventCode::NOW,lmd);
        BOOST_CHECK_EQUAL(id,1);
        id = 0;
        EventLoop::Instance().send(EventCode::ASYNC,lmd);
        BOOST_CHECK_EQUAL(id,0);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        BOOST_CHECK_EQUAL(id,1);   
    }

////////////////////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()