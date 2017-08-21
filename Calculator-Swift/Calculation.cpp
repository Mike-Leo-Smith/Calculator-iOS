//
//  Calculation.cpp
//  Calculator-Swift
//
//  Created by Mike Smith on 20/08/2017.
//  Copyright © 2017 Mike Smith. All rights reserved.
//

#include "Calculation.hpp"

#include <string>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stack>
#include <vector>

class Exception : public std::exception {
private:
    std::string _message;

public:
    explicit Exception(const std::string &type = "UNKNOWN", const std::string &info = "")
            : _message("[ " + type + " ]\t" + info) {
    }
    
    const char *what() const noexcept override {
        return _message.c_str();
    }
    
    Exception(const Exception &) = default;
    Exception(Exception &&) = default;
    Exception &operator=(const Exception &) = default;
    Exception &operator=(Exception &&) = default;
    ~Exception() override = default;
};

class Lexer {
protected:
    static bool is_identifier_prefix_char(char c) {
        return (c == '_' || (isalpha(c) != 0));
    }
    
    static bool is_identifier_char(char c) {
        return (c == '_' || (isalnum(c) != 0));
    }
    
    static bool is_number_prefix_char(char c) {
        return (c == '.' || (isdigit(c) != 0));
    }
    
    static bool is_number_char(char c) {
        return (c == '.' || (isalnum(c) != 0));
    }
    
    static bool is_operator_char(char c) {
        return (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' ||
                c == ')');
    }
    
    static bool is_blank_char(char c) {
        return (c == '\n' || c == '\r' || (isblank(c) != 0));
    }

public:
    static bool is_operator(const std::string &token) {
        return (std::set<std::string>({ "+", "-", "*", "/", "=", "(", ")" })
                .count(token) != 0);
    }
    
    static bool is_identifier(const std::string &token) {
        auto iter = token.cbegin();
        if (iter == token.cend()) {
            return false;
        }
        if (!is_identifier_prefix_char(*(iter++))) {
            return false;
        }
        while (iter != token.cend()) {
            if (!is_identifier_char(*(iter++))) {
                return false;
            }
        }
        return true;
    }
    
    static bool is_number(const std::string &token) {
        if (!is_number_prefix_char(token[0])) {
            return false;
        }
        
        double val;
        std::string unexpected;
        std::stringstream buffer;
        
        buffer << token;
        buffer >> val >> unexpected;
        return unexpected.empty();
    }
    
    static std::vector<std::string> scan(const std::string &buffer) {
        enum State {
            SCANNING_IDENTIFIER,
            SCANNING_NUMBER,
            SCANNING_OPERATOR,
            SCANNING_BLANK,
            SCANNING_UNEXPECTED
        };
        
        std::string scanning;
        std::vector<std::string> tokens;
        State state = SCANNING_BLANK;
        
        for (auto ch : buffer) {
            // Finite-state automaton.
            switch (state) {
                case SCANNING_BLANK: {
                    if (is_identifier_prefix_char(ch)) {
                        state = SCANNING_IDENTIFIER;
                        scanning.push_back(ch);
                    } else if (is_number_prefix_char(ch)) {
                        state = SCANNING_NUMBER;
                        scanning.push_back(ch);
                    } else if (is_operator_char(ch)) {
                        state = SCANNING_OPERATOR;
                        scanning.push_back(ch);
                    } else if (!is_blank_char(ch)) {
                        state = SCANNING_UNEXPECTED;
                        scanning.push_back(ch);
                    }
                    break;
                }
                case SCANNING_IDENTIFIER: {
                    if (is_identifier_char(ch)) {
                        scanning.push_back(ch);
                    } else if (is_blank_char(ch)) {
                        tokens.push_back(scanning);
                        scanning.clear();
                        state = SCANNING_BLANK;
                    } else if (is_operator_char(ch)) {
                        tokens.push_back(scanning);
                        scanning.clear();
                        scanning.push_back(ch);
                        state = SCANNING_OPERATOR;
                    } else {
                        state = SCANNING_UNEXPECTED;
                        scanning.push_back(ch);
                    }
                    break;
                }
                case SCANNING_NUMBER: {
                    if (is_number_char(ch)) {
                        scanning.push_back(ch);
                    } else if (is_operator_char(ch)) {
                        tokens.push_back(scanning);
                        scanning.clear();
                        scanning.push_back(ch);
                        state = SCANNING_OPERATOR;
                    } else if (is_blank_char(ch)) {
                        tokens.push_back(scanning);
                        scanning.clear();
                        state = SCANNING_BLANK;
                    } else {
                        state = SCANNING_UNEXPECTED;
                        scanning.push_back(ch);
                    }
                    break;
                }
                case SCANNING_OPERATOR: {
                    if (is_operator_char(ch)) {
                        if (!is_operator(scanning + ch)) {
                            tokens.push_back(scanning);
                            scanning.clear();
                        }
                        scanning.push_back(ch);
                    } else if (is_number_prefix_char(ch)) {
                        tokens.push_back(scanning);
                        scanning.clear();
                        scanning.push_back(ch);
                        state = SCANNING_NUMBER;
                    } else if (is_identifier_prefix_char(ch)) {
                        tokens.push_back(scanning);
                        scanning.clear();
                        scanning.push_back(ch);
                        state = SCANNING_IDENTIFIER;
                    } else if (is_blank_char(ch)) {
                        tokens.push_back(scanning);
                        scanning.clear();
                        state = SCANNING_BLANK;
                    } else {
                        state = SCANNING_UNEXPECTED;
                        scanning.push_back(ch);
                    }
                    break;
                }
                case SCANNING_UNEXPECTED: {
                    throw Exception(
                            "ERROR",
                            "Unexpected character received... Last token being scanned: " +
                                    scanning);
                }
            }
        }
        if (!scanning.empty()) {
            tokens.push_back(scanning);
        }
        
        return tokens;
    }
};

class VariableTable {
private:
    std::map<std::string, double> _table;

public:
    void reset() {
        _table.clear();
    }
    
    void set(const std::string &identifier, double val) {
        _table[identifier] = val;
    }
    
    double get(const std::string &identifier) const {
        return (_table.count(identifier) != 0) ? _table.at(identifier) : 0;
    }
};

class Expression {
public:
    Expression() = default;
    Expression(const Expression &) = default;
    Expression(Expression &&) = default;
    Expression &operator=(const Expression &) = default;
    Expression &operator=(Expression &&) = default;
    virtual ~Expression() = default;
    virtual double calculate(const VariableTable &var_table) const = 0;
};

class ConstantExpression : public Expression {
private:
    double _val;

public:
    explicit ConstantExpression(double val = 0) : _val(val) {}
    
    ConstantExpression(const ConstantExpression &) = default;
    ConstantExpression(ConstantExpression &&) = default;
    ConstantExpression &operator=(const ConstantExpression &) = default;
    ConstantExpression &operator=(ConstantExpression &&) = default;
    ~ConstantExpression() override = default;
    
    double calculate(const VariableTable &var_table) const override {
        return _val;
    }
};

class BinaryExpression : public Expression {
private:
    std::string _operator;
    Expression *_lhs;
    Expression *_rhs;

public:
    BinaryExpression(std::string op, Expression *lhs, Expression *rhs)
            : _operator(std::move(op)), _lhs(lhs), _rhs(rhs) {
    }
    
    BinaryExpression(const BinaryExpression &) = default;
    BinaryExpression(BinaryExpression &&) = default;
    BinaryExpression &operator=(const BinaryExpression &) = default;
    BinaryExpression &operator=(BinaryExpression &&) = default;
    
    ~BinaryExpression() override {
        delete _lhs, delete _rhs;
    }
    
    double calculate(const VariableTable &var_table) const override {
        double lhs = _lhs->calculate(var_table);
        double rhs = _rhs->calculate(var_table);
        
        if (_operator == "+") { return lhs + rhs; }
        if (_operator == "-") { return lhs - rhs; }
        if (_operator == "*") { return lhs * rhs; }
        if (_operator == "/") {
            if (rhs == 0) { throw Exception("ERROR", "Divisors cannot be zero."); }
            return lhs / rhs;
        }
        throw Exception("ERROR", "Unexpected operator: " + _operator);
    }
};

class IdentifierExpression : public Expression {
private:
    std::string _identifier;

public:
    explicit IdentifierExpression(std::string identifier) : _identifier(std::move(identifier)) {}
    
    IdentifierExpression(const IdentifierExpression &) = default;
    IdentifierExpression(IdentifierExpression &&) = default;
    IdentifierExpression &operator=(const IdentifierExpression &) = default;
    IdentifierExpression &operator=(IdentifierExpression &&) = default;
    ~IdentifierExpression() override = default;
    
    double calculate(const VariableTable &var_table) const override {
        return var_table.get(_identifier);
    }
};

class Parser {
protected:
    static int get_precedence(const std::string &op) {
        if (op == "+") { return 10; }
        if (op == "*" || op == "/") { return 20; }
        if (op == "-") { return 30; }
        return 0;
    }
    
    static std::vector<std::string>
    preprocess(const std::vector<std::string> &tokens) {
        std::vector<std::string> processed;
        
        for (const std::string &token : tokens) {
            if (token == "+" || token == "-") {
                simplify_operators(processed, token);
            } else {
                processed.push_back(token);
            }
        }
        
        int open_parentheses = 0;
        for (const std::string &token : processed) {
            if (token == "(") {
                open_parentheses++;
            } else if (token == ")") {
                open_parentheses--;
                if (open_parentheses < 0) {
                    throw Exception("ERROR", "Failed to balance parentheses...");
                }
            }
        }
        while ((open_parentheses--) != 0) { processed.emplace_back(")"); }
        return processed;
    }
    
    // This procedure simplifies the +/- operators.
    // Note that all - operators will be finally reduced to unary operators.
    // Rules:
    // - -            => +
    // <operator> +   => <operator>
    // <expr-head> +  => <expr-head>
    // -              => + -
    // <identifier> - => <identifier> + -
    static void simplify_operators(std::vector<std::string> &processed, const std::string &op) {
        if (processed.empty()) {
            if (op != "+") { processed.push_back(op); }
        } else {
            if (op == "+") {
                if (!Lexer::is_operator(processed.back()) || processed.back() == ")") {
                    processed.emplace_back("+");
                }
            } else if (op == "-") {
                if (processed.back() == "-") {
                    processed.pop_back();
                    simplify_operators(processed, "+");
                } else if (Lexer::is_identifier(processed.back()) ||
                        Lexer::is_number(processed.back())) {
                    simplify_operators(processed, "+");
                    processed.emplace_back("-");
                } else { processed.emplace_back("-"); }
            }
        }
    }

public:
    static Expression *parse(const std::vector<std::string> &tokens) {
        std::stack<Expression *> expr_stack;
        std::stack<std::string> operator_token_stack;
        std::vector<std::string> simplified_tokens = preprocess(tokens);
        
        auto create_binary_expression_with_top = [](std::stack<std::string> &op_stack, std::stack<Expression *> &ex_stack) {
            if (op_stack.empty()) {
                throw Exception("ERROR", "No enough operators...");
            }
            
            std::string top((std::string) op_stack.top());
            op_stack.pop();
            
            if (ex_stack.empty()) {
                throw Exception("ERROR", "No enough operators...");
            }
            
            Expression *lhs;
            Expression *rhs;
            
            rhs = ex_stack.top();
            ex_stack.pop();
            
            if (top == "-") {
                lhs = new ConstantExpression(0);
            } else {
                if (ex_stack.empty()) {
                    throw Exception("ERROR", "No enough operands...");
                }
                lhs = ex_stack.top(), ex_stack.pop();
            }
            
            ex_stack.push(new BinaryExpression(top, lhs, rhs));
        };
        
        for (const std::string &token : simplified_tokens) {
            if (Lexer::is_number(token)) {
                expr_stack.push(new ConstantExpression(std::strtod(token.c_str(), nullptr)));
            } else if (Lexer::is_identifier(token)) {
                expr_stack.push(new IdentifierExpression(token));
            } else if (Lexer::is_operator(token)) {
                if (token == "(") {
                    operator_token_stack.push(token);
                } else if (token == ")") {
                    while (!operator_token_stack.empty()) {
                        if (operator_token_stack.top() == "(") {
                            operator_token_stack.pop();
                            break;
                        }
                        create_binary_expression_with_top(operator_token_stack, expr_stack);
                    }
                } else {
                    int token_precedence = get_precedence(token);
                    
                    while (!operator_token_stack.empty()) {
                        if (operator_token_stack.top() == "(" ||
                                token_precedence > get_precedence(operator_token_stack.top())) {
                            break;
                        }
                        create_binary_expression_with_top(operator_token_stack, expr_stack);
                    }
                    operator_token_stack.push(token);
                }
            } else {
                while (!expr_stack.empty()) {
                    delete expr_stack.top(), expr_stack.pop();
                }
                throw Exception("ERROR", "Unexpected token found: " + token);
            }
        }
        while (!operator_token_stack.empty()) {
            if (operator_token_stack.top() == "(") { break; }
            create_binary_expression_with_top(operator_token_stack, expr_stack);
        }
        if (expr_stack.size() != 1) {
            throw Exception("ERROR", "Failed to create expression tree...");
        }
        return expr_stack.top();
    }
};

const std::string &Calculation::calculate(const std::string &expression) {
    try {
        std::vector<std::string> tokens = Lexer::scan(expression);
        Expression *expr = Parser::parse(tokens);
        _result = std::to_string(expr->calculate(*_var_table));
        
        if (_result.find('.') != std::string::npos) {
            while (_result.back() == '0') {
                _result.pop_back();
            }
            if (_result.back() == '.') {
                _result.pop_back();
            }
        }
        if (_result.empty()) {
            _result = "0";
        }
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        _result = "Error";
    }
    return _result;
}

Calculation::Calculation() {
    _var_table = new VariableTable;
}

Calculation::~Calculation() {
    delete _var_table;
}
