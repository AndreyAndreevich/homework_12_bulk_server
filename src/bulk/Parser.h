#ifndef parser_h
#define parser_h

#include <string>

struct BlockParser {
  int blocks_count = 0;
  
  enum Block {
    StartBlock,
    CancelBlock,
    Command,
    Empty
  };

  Block parsing(const std::string& line);
};

int start_parsing(int argc, char *argv[], uint n);

#endif
