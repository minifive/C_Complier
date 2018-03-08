#ifndef _TOKEN_H
#define _TOKEN_H
enum Associativity   //结合性
{
	NONE,
	LEFT,
	RIGHT
};

class Token {
public: 
    string name;  
    string type;
	bool operator<(const Token& r) const
	{
		return name < r.name;
	}
	bool operator==(const Token&r)const {
		return name == r.name;
	}
};

class TerminalToken : public Token{//终结符
public:
	
	Associativity associativity;
	int precedence;
};

class NonterminalToken : public Token {//非终结符

};

#endif //_TOKEN_H
