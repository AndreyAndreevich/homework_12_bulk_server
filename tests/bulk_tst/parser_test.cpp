//#define BOOST_TEST_MODULE test_parser
#include <boost/test/unit_test.hpp>
#include <boost/mpl/assert.hpp>

#include "Parser.h"

BOOST_AUTO_TEST_SUITE(test_parser)

    BOOST_AUTO_TEST_CASE(start_parser)
    {
        char* argv[2];
        argv[1] = "55";
        BOOST_CHECK_THROW(start_parsing(0,argv,1),std::exception);
        BOOST_CHECK_THROW(start_parsing(1,argv,1),std::exception);
        BOOST_CHECK_EQUAL(start_parsing(2,argv,1),55);
        BOOST_CHECK_EQUAL(start_parsing(10,argv,1),55);
        argv[1] = "asd";
        BOOST_CHECK_THROW(start_parsing(2,argv,1),std::exception);
        argv[1] = "0";
        BOOST_CHECK_THROW(start_parsing(2,argv,1),std::exception);
        argv[1] = "-1";
        BOOST_CHECK_THROW(start_parsing(2,argv,1),std::exception);
        argv[1] = "asd4";
        BOOST_CHECK_THROW(start_parsing(2,argv,1),std::exception);
        argv[1] = "4fd";
        BOOST_CHECK_THROW(start_parsing(10,argv,1),std::exception);
        argv[1] = "4";
        BOOST_CHECK_EQUAL(start_parsing(10,argv,1),4);
    }

////////////////////////////////////////////////////////////////////////////////////////////////

    BOOST_AUTO_TEST_CASE(block_parser)
    {
        auto parser = std::make_unique<BlockParser>();
        BOOST_CHECK_EQUAL(parser->parsing("cmd"),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing(""),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing(" "),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing("{"),BlockParser::StartBlock);
        BOOST_CHECK_EQUAL(parser->parsing("cmd"),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing("{"),BlockParser::Empty);
        BOOST_CHECK_EQUAL(parser->parsing("cmd"),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing("}"),BlockParser::Empty);
        BOOST_CHECK_EQUAL(parser->parsing("}"),BlockParser::CancelBlock);
        BOOST_CHECK_EQUAL(parser->parsing("}"),BlockParser::Command);

        BOOST_CHECK_EQUAL(parser->parsing("{cmd"),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing("cmd{"),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing("{cmd}"),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing("{}"),BlockParser::Command);
        BOOST_CHECK_EQUAL(parser->parsing("}{"),BlockParser::Command);
    }

BOOST_AUTO_TEST_SUITE_END()