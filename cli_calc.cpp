#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <cmath>

class Calculator {
private:
int base;

bool verbose;

enum TokenType {
	T_NUM,
	T_OP,
	T_LPAR,
	T_RPAR,
	T_UNKNOWN
};

const std::unordered_map<std::string, int> PRECENDENCE {
	{"+", 1},
	{"-", 1},
	{"*", 2},
	{"/", 2},
	{"^", 3},
};

[[maybe_unused]] const std::unordered_map<TokenType, std::string> TOKEN_NAME {
	{T_NUM, "NUM"},
	{T_OP, "OP"},
	{T_LPAR, "LPAR"},
	{T_RPAR, "RPAR"},
	{T_UNKNOWN, "UNKNOWN"}
};

struct Token {
	std::string value;
	enum TokenType type;
	Token(std::string input_value = "", TokenType input_type = T_UNKNOWN) : value(input_value), type(input_type) {};
	Token(char input_value, TokenType input_type = T_UNKNOWN) : value(std::string() + input_value), type(input_type) {};
};

template <typename T> class Stack {
private:
	int pos;
	std::vector<T> s_vector;
public:
	Stack() : pos(-1) {};
	void push(T n) {
		pos++;
		s_vector.push_back(n);
	}
	T pop() {
		assert(pos >= 0);
		T tmp = s_vector[pos];
		s_vector.erase(s_vector.end());
		pos--;
		return tmp;
	}
	T peek() {
		assert(pos >= 0);
		return s_vector[pos];
	}
	int size() {
		return pos + 1;
	}
};

std::vector<Token> tokenize(std::string source) {
	std::vector<Token> token_vector; 
	int s_size = source.size();
	for (int i = 0; i < s_size; i++) {
		switch (source[i]) {
			case ' ':
			case '\f':
			case '\n':
			case '\r':
			case '\t':
			case '\v':
				break;
			case '+':
			case '-':
			case '*':
			case '/':
			case '^':
				{
				Token tmp(source[i], T_OP);
				token_vector.push_back(tmp);
				}
				break;
			case '(':
			case '[':
				{
				Token tmp(source[i], T_LPAR);
				token_vector.push_back(tmp);
				}
				break;
			case ')':
			case ']':
				{
				Token tmp(source[i], T_RPAR);
				token_vector.push_back(tmp);
				}
				break;
			default:
				if ((source[i] >= '0' && source[i] <= '9') || (source[i] >= 'a' && source[i] <= 'z') || (source[i] >= 'A' && source[i] <= 'Z') || source[i] == '.') {
					Token tmp("", T_NUM);
					if (source[i] == '.') {
						tmp.value.push_back('0');
						tmp.value.push_back('.');
						i++;
					}
					while ((source[i] >= '0' && source[i] <= '9') || (source[i] >= 'a' && source[i] <= 'z') || (source[i] >= 'A' && source[i] <= 'Z') || source[i] == '.') {
						tmp.value.push_back(source[i++]);
					}
					i--;
					token_vector.push_back(tmp);
				} else {
					Token tmp(source[i], T_UNKNOWN);
					token_vector.push_back(tmp);
				}
				break;
		}
	}
	return token_vector;
}

std::vector<Token> toPrefix(const std::vector<Token>& token_vector) {
	std::vector<Token> prefix_vector;
	Stack<Token> stack;
	for (Token token : token_vector) {
		switch (token.type) {
			case T_NUM:
				prefix_vector.push_back(token);
				break;
			case T_OP:
				if (stack.size() > 0 && stack.peek().type != T_LPAR && (PRECENDENCE.at(token.value) < PRECENDENCE.at(stack.peek().value))) {
					while (stack.size() > 0 && stack.peek().type != T_LPAR && (PRECENDENCE.at(token.value) < PRECENDENCE.at(stack.peek().value))) prefix_vector.push_back(stack.pop());
				}
				stack.push(token);
				break;
			case T_LPAR:
				stack.push(token);
				break;
			case T_RPAR:
				while (stack.peek().type != T_LPAR) {
					prefix_vector.push_back(stack.pop());
				}
				stack.pop();
				break;
		}
	}
	while (stack.size() > 0) prefix_vector.push_back(stack.pop());

	return prefix_vector;
}

double calculatePrefix(std::vector<Token> prefix_vector) {
	Stack<double> stack;
	int op_num = 1;
	for (Token token : prefix_vector) {
		switch (token.type) {
			case T_NUM:
				stack.push(base == 10 ? std::stod(token.value, nullptr) : (double)std::stoi(token.value, nullptr, base));
				break;
			case T_OP:
				assert(stack.size() > 0);
				if (stack.size() == 1 && token.value == "-") {
					double a = stack.pop();
					stack.push(-a);
				}
				else {
					double b = stack.pop(), a = stack.pop();
 					switch (token.value[0]) {
						case '+':
							if (verbose) std::cout << op_num << ") " << a << ' ' << token.value << ' ' << b << " = " << a + b << ";\n";
							stack.push(a + b);
							break;
						case '-':
							if (verbose) std::cout << op_num << ") " << a << ' ' << token.value << ' ' << b << " = " << a - b << ";\n";
							stack.push(a - b);
							break;
						case '*':
							if (verbose) std::cout << op_num << ") " << a << ' ' << token.value << ' ' << b << " = " << a * b << ";\n";
							stack.push(a * b);
							break;
						case '/':
							if (verbose) std::cout << op_num << ") " << a << ' ' << token.value << ' ' << b << " = " << a / b << ";\n";
							stack.push(a / b);
							break;
						case '^':
							if (verbose) std::cout << op_num << ") " << a << ' ' << token.value << ' ' << b << " = " << std::pow(a, b) << ";\n";
							stack.push(std::pow(a, b));
							break;
					}
					op_num++;
				}
		}
	}
	return stack.pop();
}

public:
Calculator(int num_base = 10, bool is_verbose = false) {
	assert(num_base > 1 && num_base < 37);
	base = num_base;
	verbose = is_verbose;
};

int getBase() {
	return base;
}

void setBase(int num_base) {
	base = num_base;
}

double calculate(std::string source) {
	return calculatePrefix(toPrefix(tokenize(source)));
}
};

int main(int argc, char* argv[]) {
	int base = 10;
	bool verbose = false;
	for (int i = 1; i < argc; i++) {
		if ((std::string)argv[i] == "-b") {
			if (argc - i > 1) {
				i++;
				for (int j = 0; j < ((std::string)argv[i]).size(); j++) {
					if (!(argv[i][j] >= '0' && argv[i][j] <= '9')) {
						std::cout << "Invalid base\n";
						return -1;
					}
				}
				base = std::stoi(argv[i]);
				if (base < 2 || base > 36) base = 10;
			} else {
				std::cout << "Base not specified\n";
				return -1;
			}
		} else if ((std::string)argv[i] == "-v") {
			verbose = true;
		} else {
			std::cout << "Invalid argument \"" << argv[i] << "\"\n";
			return -1;
		}
	}
	Calculator calculator(base, verbose);

	std::string source(10, true);
	std::cout << "Please enter a mathematical expression: ";
	std::getline(std::cin, source);
	double ans = calculator.calculate(source);
	std::cout << source << " = " << ans << '\n';

	return 0;
}
