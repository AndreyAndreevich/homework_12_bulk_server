#ifndef parser_h
#define parser_h

#include <string>

class BlockParser {
  int blocks_count = 0;
public:

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
