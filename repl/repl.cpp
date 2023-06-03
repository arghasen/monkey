#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include "eval/evaluator.hpp"

#include <iostream>
#include <version.hpp>

constexpr auto PROMPT = ">> ";
const auto MonkeyFace = R"(
            __,__
   .--.  .-"     "-.  .--.
  / .. \/  .-. .-.  \/ .. \
 | |  '|  /   Y   \  |'  | |
 | \   \  \ 0 | 0 /  /   / |
  \ '- ,\.-"""""""-./, -' /
   ''-' /_   ^ ^   _\ '-''
       |  \._   _./  |
       \   \ '~' /   /
        '._ '-=-' _.'
           '-----'
)";

void printParserErrors(const std::vector<std::string> &errors) {
  std::cout << MonkeyFace << std::endl;
  std::cout << "Woops! We ran into some monkey business here!" << std::endl;
  std::cout << " parser errors:" << std::endl;
  for (const auto &err : errors) {
    std::cout << "\t" << err << std::endl;
  }
}
int main() {
  std::cout << "Hello, Monkey! version : " << VERSION << std::endl;
  std::cout << "Feel free to type in commands" << std::endl;
  auto env = std::make_shared<monkey::evaluator::EnvironmentImpl>();
  while (1) {
    std::cout << PROMPT;
    std::string input;
    std::getline(std::cin, input);
    if (input == "exit") {
      break;
    }
    std::cout << "Input: " << input << std::endl;
    monkey::lexer::Lexer l(input);
    monkey::parser::Parser p(&l);
    auto program = p.parseProgram();
    if(p.getErrors().size() != 0){
      printParserErrors(p.getErrors());
      continue;
    }
    std::cout << "Parsed: " <<program->to_string() << std::endl;

    auto evaluated = monkey::evaluator::Evaluator().eval(program.get(), env);
    if (evaluated != nullptr) {
      std::cout <<"Evaluated: " << evaluated->to_string() << std::endl;
    }
  }
  return 0;
}
