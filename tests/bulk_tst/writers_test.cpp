#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "Writers.h"

using Commands = std::vector<std::string>;


BOOST_AUTO_TEST_SUITE(test_writers)

    BOOST_AUTO_TEST_CASE(print_console)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        ConsoleWriter writer(out_stream);
        auto commands = std::make_shared<Commands>(Commands{"cmd1", "cmd2"});
        BOOST_CHECK_NO_THROW(writer.update(commands));
        BOOST_CHECK_NO_THROW(writer.print());
        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2\n");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(print_file)
    {
        FileWriter writer;
        auto commands = std::make_shared<Commands>(Commands{"cmd1"});
        BOOST_CHECK_NO_THROW(writer.update(commands));
        BOOST_CHECK_NO_THROW(writer.print());
        std::ifstream file{writer.getName()};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();
        std::remove(writer.getName().c_str());
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(current_time)
    {
        FileWriter writer;
        auto commands = std::make_shared<Commands>(Commands{"cmd1"});
        writer.update(commands);
        BOOST_CHECK_EQUAL(std::time(nullptr),writer.getTime());
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(delete_commands_console)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);
        ConsoleWriter writer(out_stream);
        {
            auto commands = std::make_shared<Commands>(Commands{"cmd1", "cmd2"});
            writer.update(commands);
        }
        BOOST_CHECK_THROW(writer.print(),std::exception);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(delete_commands_file)
    {
        FileWriter writer;
        std::shared_ptr<Commands> commands;
        BOOST_CHECK_THROW(writer.update(commands),std::exception);
        BOOST_CHECK_THROW(writer.print(),std::exception);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(error_open_file)
    {
        FileWriter file_writer;
        auto commands = std::make_shared<Commands>(Commands{"cmd1","cmd2"});
        file_writer.update(commands);
        BOOST_CHECK_THROW(file_writer.print(),std::exception);
    }

BOOST_AUTO_TEST_SUITE_END()
