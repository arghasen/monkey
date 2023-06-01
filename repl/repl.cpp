#include "../lexer/lexer.hpp"

#include <iostream>
#include <version.hpp>

constexpr auto PROMPT = ">> ";

int main(){
    std::cout << "Hello, Monkey! version : " << VERSION << std::endl;
    std::cout << "Feel free to type in commands" << std::endl;
    while (1)
    {
        std::cout << PROMPT;
        std::string input;
        std::getline(std::cin, input);
        if (input == "exit")
        {
            break;
        }
        std::cout << input << std::endl;
        monkey::lexer::Lexer l(input);
        for (auto tok = l.NextToken(); tok.Type != monkey::lexer::EOFILE; tok = l.NextToken())
        {
            std::cout << tok << std::endl;
        }
    }
    return 0;
}
