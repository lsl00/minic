#include "ast.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <fstream>
#include <iostream>
#include <llvm/Support/raw_ostream.h>
#include <string>

std::string ReadFile(const char *FilePath) {
  std::ifstream Input(FilePath);
  if (!Input.is_open()) {
    return "";
  }
  return std::string(std::istreambuf_iterator<char>(Input),
                     std::istreambuf_iterator<char>());
}
using namespace ast;
int main(int argc, char **argv) {
  if (argc == 1) {
    printf("This program take one input file and output an exectuable.");
    return 1;
  }

  std::string src = ReadFile(argv[1]);

  Lexer lexer(src);
  std::cout << lexer.src << std::endl;
  // while (true) {
  //   Token t = lexer.NextToken();
  //   if (t.isKind(TokenType::Eof))
  //     break;
  //   std::cout << t << std::endl;
  // }
  // return 0;

  Parser parser(lexer);
  try {
    Program program = parser.ParseProgram();
    TreePrinter Printer;
    Printer.accept(program);

  } catch (string s) {
    cout << "Exception : " << s;
  }
  return 0;
}