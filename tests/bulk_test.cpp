#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "Handler.h"
#include "Writers.h"

using Commands = std::vector<std::string>;


BOOST_AUTO_TEST_SUITE(test_bulk)

    BOOST_AUTO_TEST_CASE(example_1)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>(3);
        auto consoleWriter = std::shared_ptr<ConsoleWriter>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<FileWriter>(new FileWriter());
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);

        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("cmd3");

        std::ifstream file{fileWriter->getName()};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2, cmd3\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2, cmd3");

        out_buffer.str("");
        string_stream.str("");

        handler->addCommand("cmd4");
        handler->addCommand("cmd5");
        handler->stop();

        file.open(fileWriter->getName());
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd4, cmd5\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd4, cmd5");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(example_2)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>(3);
        auto consoleWriter = std::shared_ptr<ConsoleWriter>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<FileWriter>(new FileWriter());
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);
        
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("cmd3");

        std::ifstream file{fileWriter->getName()};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2, cmd3\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2, cmd3");

        out_buffer.str("");
        string_stream.str("");

        handler->addCommand("{");
        handler->addCommand("cmd4");
        handler->addCommand("cmd5");
        handler->addCommand("cmd6");
        handler->addCommand("cmd7");
        handler->addCommand("}");

        file.open(fileWriter->getName());
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd4, cmd5, cmd6, cmd7\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd4, cmd5, cmd6, cmd7");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(example_3)
    {
        std::stringbuf out_buffer;
        std::ostream out_stream(&out_buffer);

        auto handler = std::make_shared<Handler>(1);
        auto consoleWriter = std::shared_ptr<ConsoleWriter>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<FileWriter>(new FileWriter());
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);

        handler->addCommand("{");
        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("{");
        handler->addCommand("cmd3");
        handler->addCommand("cmd4");
        handler->addCommand("}");
        handler->addCommand("cmd5");
        handler->addCommand("cmd6");
        handler->addCommand("}");

        std::ifstream file{fileWriter->getName()};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();
        std::remove(fileWriter->getName().c_str());

        BOOST_CHECK_EQUAL(out_buffer.str(),"bulk: cmd1, cmd2, cmd3, cmd4, cmd5, cmd6\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"bulk: cmd1, cmd2, cmd3, cmd4, cmd5, cmd6");
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(example_4)
    {
        std::stringstream out_stream;

        auto handler = std::make_shared<Handler>(4);
        auto consoleWriter = std::shared_ptr<ConsoleWriter>(new ConsoleWriter(out_stream));
        auto fileWriter = std::shared_ptr<FileWriter>(new FileWriter());
        consoleWriter->subscribe(handler);
        fileWriter->subscribe(handler);

        handler->addCommand("cmd1");
        handler->addCommand("cmd2");
        handler->addCommand("cmd3");
        handler->addCommand("{");
        std::remove(fileWriter->getName().c_str());
        handler->addCommand("cmd4");
        handler->addCommand("cmd5");
        handler->addCommand("cmd6");
        handler->addCommand("cmd7");
        handler->stop();

        std::ifstream file{fileWriter->getName()};
        std::stringstream string_stream;
        string_stream << file.rdbuf();
        file.close();

        BOOST_CHECK_EQUAL(out_stream.str(),"bulk: cmd1, cmd2, cmd3\n");
        BOOST_CHECK_EQUAL(string_stream.str(),"");
    }

BOOST_AUTO_TEST_SUITE_END()