#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "Handler.h"
#include "Writers.h"

using Commands = std::vector<std::string>;

BOOST_AUTO_TEST_SUITE(test_handler)

    BOOST_AUTO_TEST_CASE(print_commands)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>(2);
        auto consoleWriter = std::make_shared<ConsoleWriter>(out_stream);
        auto fileWriter = std::make_shared<FileWriter>();
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");

        std::ifstream file{fileWriter->getName()};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(delete_writer)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>(1);
        {
            auto fileWriter = std::make_shared<FileWriter>();
            fileWriter->subscribe(handler);
        }
        auto consoleWriter = std::make_shared<ConsoleWriter>(out_stream);
        consoleWriter->subscribe(handler);
        
        handler->addCommand("cmd1");
        handler->stop();

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1\n");
    }



    BOOST_AUTO_TEST_CASE(delete_handler)
    {
        std::stringstream out_stream1;
        std::stringstream out_stream2;

        auto console_writer1 = std::make_shared<ConsoleWriter>(out_stream1);
        auto console_writer2 = std::make_shared<ConsoleWriter>(out_stream2);           
        {
            auto handler = std::make_shared<Handler>(1);
            console_writer1->subscribe(handler);
            console_writer2->subscribe(handler);
            handler->addCommand("cmd1");
            handler->addCommand("cmd2");
        }
        BOOST_CHECK_EQUAL(out_stream1.str(),"bulk: cmd1\nbulk: cmd2\n");
        BOOST_CHECK_EQUAL(out_stream2.str(),"bulk: cmd1\nbulk: cmd2\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(large_string)
    {
        Handler handler(4);
        std::string command("123456789012345678901234567890123456789012345678901");
        assert(command.size() == 51);
        BOOST_CHECK_THROW(handler.addCommand(command),std::exception);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(error_set_N)
    {
        BOOST_CHECK_THROW(Handler handler(-1),std::exception);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(empty_string)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>(2);
        auto consoleWriter = std::make_shared<ConsoleWriter>(out_stream);
        auto fileWriter = std::make_shared<FileWriter>();
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);
        
        handler->addCommand("");
        handler->addCommand("");
        handler->stop();


        std::fstream file;
        file.open(fileWriter->getName());
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: , \n");
        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: , \n"); 
    }

BOOST_AUTO_TEST_SUITE_END()
