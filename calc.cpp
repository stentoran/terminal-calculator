#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <cctype>
#include <cmath>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <assert.h>
#include <optional>

const std::string validStrings = "+-*x/^()";

int precedence(const std::string& s)
{
    if (s.compare("^") == 0)
    {
        return 3;
    }
    else if (s.compare("x") == 0 || s.compare("/") == 0)
    {
        return 2;
    }
    else if (s.compare("+") == 0 || s.compare("-") == 0)
    {
        return 1;
    }
    return 0;
}

bool isLeftAssociative(const std::string& s)
{
    if (s.compare("^") == 0)
    {
        return false;
    }
    return true;
}

bool stodworks(const std::string& s)
{
    bool result = true;
    try
    {
        size_t pos;
        if (std::stod(s, &pos) && pos == s.length())
        {
            return true;
        }
        throw std::invalid_argument("Invalid argument provided");
    }
    catch (const std::invalid_argument& e)
    {
        result = false;
    }
    return result;
}

std::optional<double> evaluateRPN(const std::vector<std::string>& s, bool error = false)
{
    std::stack<double> nums;
    for (const std::string& c: s)
    {
        if (stodworks(c))
        {
            nums.push(std::stod(c));
        }
        else
        {
            // need this block to reject stuff like '/2-3'
            double a = 1;
            if (!nums.empty())
            {
                a = nums.top();
                nums.pop();
            }
            else
            {
                std::cerr << "Invalid ordering of equation" << std::endl;
                return error;
            }
            double b = 1;
            if (!nums.empty())
            {
                b = nums.top();
                nums.pop();
            }
            else
            {
                std::cerr << "Invalid ordering of equation" << std::endl;
                return error;
            }

            if (c[0] == '+')
            {
                nums.push(a+b);
            }
            else if (c[0] == '-')
            {
                nums.push(b-a);
            }
            else if (c[0] == 'x' || c[0] == '*')
            {
                nums.push(a*b);
            }
            else if (c[0] == '/')
            {
                nums.push(b/a);
            }
            else if (c[0] == '^')
            {
                nums.push(pow(b, a));
            }
            else {
                nums.push(1);
            }
        }
    }
    return nums.top();
}

// Shunting yard algorithm
std::vector<std::string> convertToRPN(const std::vector<std::string>& s)
{
    std::vector<std::string> nums;
    std::stack<std::string> ops;
    std::queue<std::string> out;
    // do multichar later
    for (std::string c: s)
    {
        if (stodworks(c))
        {
            nums.push_back(c);
            out.push(c);
        }
        else
        {
            // o1
            if (
                    !ops.empty() &&
                    c.compare(")") != 0 &&
                    c.compare("(") != 0 &&
                    (
                        precedence(c) < precedence(ops.top()) ||
                        (
                            precedence(c) == precedence(ops.top()) &&
                            isLeftAssociative(c)
                        )
                    )
               )
            {
                nums.push_back(ops.top());
                ops.pop();
                ops.push(c);
            }
            else if (c.compare(")") == 0)
            {
                while (!ops.empty() && ops.top().compare("(") != 0)
                {

                    nums.push_back(ops.top());
                    ops.pop();

                }
                if (ops.empty())
                {
                    std::cerr << "Parenthesis misaligned" << std::endl;
                    return std::vector<std::string> {};
                }
                if (ops.top().compare("(") == 0)
                {
                     ops.pop();
                }
            }
            else // functions and "(" go on the operator stack
            {
                ops.push(c);
            }

        }
    }
    while (!ops.empty())
    {
        nums.push_back(ops.top());
        ops.pop();
    }
    return nums;
}

void test_convertToRPN()
{
    std::vector<std::string> x = {"3", "4", "+"};
    assert(convertToRPN(std::vector<std::string> {"3", "+", "4"}) == x);
    x = {"5", "2", "3", "x", "+"};
    assert(convertToRPN(std::vector<std::string> {"5", "+", "2", "x", "3"}) == x);
    x = {"1", "2", "+", "4", "x"};
    assert(convertToRPN(std::vector<std::string> {"(", "1", "+", "2", ")", "x", "4"}) == x);
    x = {"2", "3", "2", "^", "^"};
    assert(convertToRPN(std::vector<std::string> {"2", "^", "3", "^", "2"}) == x);
    x = {"3", "4", "+", "5", "2", "-", "2", "^", "x"};
    assert(convertToRPN(std::vector<std::string> {"(", "3", "+", "4", ")", "x", "(", "5", "-", "2", ")", "^", "2"}) == x);
}

void test_evaluateRPN()
{
    assert(evaluateRPN(std::vector<std::string> {"3", "4", "+", "2", "*"}) == 14);
    assert(evaluateRPN(std::vector<std::string> {"5", "1", "2", "+", "4", "*", "+", "3", "-"}) == 14);
    assert(evaluateRPN(std::vector<std::string> {"2", "3", "4", "*", "+"}) == 14);
    assert(evaluateRPN(std::vector<std::string> {"7", "2", "3", "+", "-"}) == 2);
    assert(evaluateRPN(std::vector<std::string> {"10", "2", "/", "3", "4", "+", "x"}) == 35);
    assert(evaluateRPN(std::vector<std::string> {"10", "2", "/", "3", "4", "+", "^"}) == 78125);
    assert(evaluateRPN(std::vector<std::string> {"2", "10", "/", "3", "4", "+", "^"}) == pow(0.2, 7));
}

bool isOperator(char c)
{
    if (c == '+') return true;
    if (c == '-') return true;
    if (c == 'x') return true;
    if (c == '*') return true;
    if (c == '/') return true;
    if (c == '(') return true;
    if (c == ')') return true;
    if (c == '^') return true;
    return false;
}

std::vector<std::string> fixParenthesis(const std::vector<std::string>& sv)
{
    std::vector<std::string> newVec;
    for (const std::string& s : sv)
    {
        unsigned int i = 0;
        while (i<s.length())
        {
            if (isOperator(s[i]))
            {
                newVec.push_back(s.substr(i, 1));
                i++;
            }
            else if (isdigit(s[i]))
            {
                unsigned int start = i;
                while (i < s.length() && ( isdigit(s[i]) || s[i] == '.' ) ) // floats
                {
                    i++;
                }
                newVec.push_back(s.substr(start, i-start));
            }
        }
    }
    return newVec;
}

bool validInput(const std::vector<std::string>& input)
{
    for (std::string s: input)
    {
        if (s.empty())
        {
            return false;
        }
        if (stodworks(s))
        {
            // is number, can move to next string
        }
        else if (validStrings.find(s) != std::string::npos)
        {
            // is operator, can move to next string
        }
        else
        {
            // bad input, should automatically return false
            return false;
        }
    }
    // every string is valid
    return true;
}
void test_validInput()
{
    assert(validInput(std::vector<std::string> {"2", "+", "3"}) == true);
    assert(validInput(std::vector<std::string> {"-", "5", "+", "3"}) == true);
    assert(validInput(std::vector<std::string> {"3.14", "+", "2.72"}) == true);
    assert(validInput(std::vector<std::string> {"2", "a", "+", "3"}) == false);
    assert(validInput(std::vector<std::string> {" "}) == false);
    assert(validInput(std::vector<std::string> {""}) == false);
    assert(validInput(std::vector<std::string> {"5", "$", "+", "2"}) == false);
    assert(validInput(std::vector<std::string> {"3.1.4", "+", "2"}) == false);
}

std::vector<std::string> vectorize(const std::vector<std::string>& sv)
{
    std::vector<std::string> newVec;
    for (const std::string& s : sv)
    {
        unsigned int i = 0;
        while (i<s.length())
        {
            if (isdigit(s[i]))
            {
                unsigned int start = i;
                while (i < s.length() && ( isdigit(s[i]) || s[i] == '.' ) ) // floats
                {
                    i++;
                }
                newVec.push_back(s.substr(start, i-start));
            }
            else
            {
                newVec.push_back(s.substr(i, 1));
                i++;
            }
        }
    }
    return newVec;
}

std::optional<double> evaluateString(std::string s, bool error = false)
{
    std::vector<std::string> svec;

    if (svec.empty())
    {
        std::istringstream iss(s);
        std::string word;
        while (iss >> word)
        {
            svec.push_back(word);
        }
    }

    svec = vectorize(svec);

    if (!validInput(svec))
    {
        std::cout << "Invalid input \n";
        return error;
    }

    svec = fixParenthesis(svec);

    std::vector<std::string> rpn = convertToRPN(svec);
    if (rpn.empty())
    {
        return error;
    }
    std::optional<double>  answer = evaluateRPN(rpn);
    if (!answer)
    {
        return false;
    }
    return answer.value();
}

void test_evaluateString()
{
    assert(evaluateString("3+2") == 5);
    assert(evaluateString("a") == false);
    assert(evaluateString("x3+4") == false);
    assert(evaluateString("^6") == false);
    assert(evaluateString("4^x6") == false);
    assert(evaluateString("(6))-1") == false);
}

void test_all()
{
    test_validInput();
    test_convertToRPN();
    test_evaluateRPN();
    test_evaluateString();
}

int main(int argc, char* argv[])
{
    //test_all();

    std::string s;
    std::vector<std::string> svec;

    if (argc == 1)
    {
        std::cout << "Enter an equation: \n";
        std::getline(std::cin, s);
    }
    else if (argc == 2)
    {
        s = argv[1];
    }
    else
    {
        std::cout << "Too many arguements. Usage: ./calc '(4x8)^2' \n";
    }

    std::replace(s.begin(), s.end(), '*', 'x');

    if (svec.empty())
    {
        std::istringstream iss(s);
        std::string word;
        while (iss >> word)
        {
            svec.push_back(word);
        }
    }

    svec = vectorize(svec);

    if (!validInput(svec))
    {
        std::cout << "Invalid input \n";
        return 1;
    }

    svec = fixParenthesis(svec);

    std::vector<std::string> rpn = convertToRPN(svec);
    if (rpn.empty()) return 1;
    std::optional<double>  answer = evaluateRPN(rpn);
    std::cout << "answer: " << answer.value()<< '\n';
    return 0;
}
