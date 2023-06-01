##include "ast.h"

namespace monkey {
namespace parser{

std::string Program::TokenLiteral() const {
    if (Statements.empty()) {
        return "";
    }
    return Statements[0]->TokenLiteral();
}

std::string Program::String() const {
    std::string out;
    for (const auto& stmt : Statements) {
        out += stmt->String();
    }
    return out;
}

std::string Identifier::TokenLiteral() const {
    return Token.Literal;
}

std::string Identifier::String() const {
    return Value;
}

std::string LetStatement::TokenLiteral() const {
    return Token.Literal;
}

std::string LetStatement::String() const {
    std::string out;
    out += TokenLiteral() + " ";
    out += Name->String();
    out += " = ";
    if (Value != nullptr) {
        out += Value->String();
    }
    out += ";";
    return out;
}

} // namespace parser
} // namespace monkey