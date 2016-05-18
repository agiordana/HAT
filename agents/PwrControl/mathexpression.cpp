/*
 *  namelist.cpp
 *  mngagent
 *
 *  Created by Attilio Giordana on 1/13/12.
 *  Copyright 2012 Università del Piemonte Orientale. All rights reserved.
 *
 */

using namespace std;

#include "mathexpression.h"
#define EPSILON 0.000000000001

bool MathExpression::showExpression() {
    for(unsigned i=0; i<size(); i++) cout<<(*this)[i]<<", ";
    cout<<endl;
    return true;
}

bool MathExpression::init(string names, char delim) {
	string tmp = names;
	size_t found;
    
	while ((found = tmp.find(delim)) != string::npos) {
		push_back(tmp.substr(0,found));
		tmp = tmp.substr(found+1);
	}
    
	if(tmp != "") push_back(tmp);
    
	return size() > 0;
}

bool MathExpression::member(string name) {
    
	for(unsigned i=0; i<size(); i++) {
		if(name == (*this)[i]) 
            return true;
    }
    
	return false;
}

int MathExpression::find(string name) {
	
    for(unsigned i=0; i<size(); i++) {
		if(name == (*this)[i]) {
			return (int)i;
		}
    }
    
	return -1;
}

bool MathExpression::parse(string exp) {
    vector<string> tokens;
    vector<string> stack;
    tokenize(exp, tokens);
    for(unsigned i=0; i< tokens.size(); i++) {
        if(tokens[i]=="(") stack.push_back(tokens[i]);
        else if(isOperator(tokens[i]) && stack.empty()) stack.push_back(tokens[i]);
        else if(isOperator(tokens[i]) && !stack.empty()) {
            while (!stack.empty() && stack.back()!="(" && precedence(stack.back(), tokens[i]) >=0) {
                push_back(stack.back());
                stack.pop_back();
            }
            stack.push_back(tokens[i]);
        }
        else if(tokens[i]==")") {
            while (!stack.empty()&&stack.back()!="(") {
                push_back(stack.back());
                stack.pop_back();
            }
            if (!stack.empty()) {
                stack.pop_back();
            }
        }
        else push_back(tokens[i]);
    }
    if (!stack.empty()) {
        while (!stack.empty()&&stack.back()!="(") {
            push_back(stack.back());
            stack.pop_back();
        }
    }
    return stack.empty();
}

int MathExpression::precedence(string s1, string s2) {
    if (s1=="^") {
        if(s2=="^") return 0;
        else return 1;
    }
    else if(s1 == "*" || s1 == "/") {
        if(s2 == "^") return -1;
        else if(s2 == "*" || s2 == "/") return 0;
        else return 1;
    }
    else if(s1=="+" || s1=="-") {
        if (s2=="^"||s2=="*"||s2=="/") return -1;
        else if(s2=="+"||s2=="-") return 0;
        else return 1;
    }
    else if(s1=="<" || s1==">"||s1=="=="||s1==">="||s1=="<=") {
        if(s2=="<" || s2==">"||s2=="=="||s2==">="||s2=="<=") return 0;
        else if(s2=="||" ||s2=="&&") return 1;
        else return -1;
    }
    return -1;
}

bool MathExpression::isMathOperator(string s) {
    if (s=="+" || s=="-" || s=="*" || s=="/" || s=="^")
        return true;
    else return false;
}

bool MathExpression::isBoolOperator(string s) {
    if (s=="<" || s==">" || s=="==" || s==">=" || s=="<=" || s=="&&" || s=="||")
        return true;
    else return false;
}

bool MathExpression::isDelim(char c) {
    return (c==' ' || c=='+' || c=='-' || c=='*' || 
            c=='/' || c=='^' || c=='<' || c=='=' ||
            c=='&' || c=='|' || c==' ');
}

bool MathExpression::isSymbol(string s) {
    for (unsigned i=0; i<s.size(); i++) {
        if(!(s[i]=='0'||s[i]=='1'||s[i]=='2'||s[i]=='3'||s[i]=='4'||s[i]=='5'||s[i]=='6'||
             s[i]=='7'||s[i]=='8'||s[i]=='9'||s[i]=='.')) return true;
    }
    return false;
}

bool MathExpression::specialSymbol(string s) {
    if (s=="ON"||s=="OFF"||s=="UP"||s=="DOWN"||s=="TRUE"||s=="FALSE") {
        return true;
    }
    else return false;
}

int MathExpression::tokenize(string s, vector<string>& tokens) {
    string sym = "";
    unsigned j=0;
    bool flag = false;
    tokens.clear();
    for (j=0; j<s.size()-1; j++) {
        switch(s[j]) {
            case ' ':
                if(sym != "") tokens.push_back(sym);
                sym = "";
                flag = false;
                break;
            case '+':
            case '-':
            case '/':
            case '*':
            case '^':
            case '(':
            case ')':
                if(sym != "") tokens.push_back(sym);
                sym = "";
                sym+= s[j];
                if(sym!="") tokens.push_back(sym);
                sym = "";
                break;
            case '<':
            case '>':
            case '&':
            case '|':
            case '=':
                if (!flag) {
                    if(sym!="") tokens.push_back(sym);
                    sym = "";
                }
                sym+=s[j];
                if (!isDelim(s[j+1])) {
                    if(sym!="") tokens.push_back(sym);
                    sym = "";
                }
                else flag = true;
                break;
            default:
                sym += s[j];
        }
    }
    if(!isDelim(s[j])&&s[j]!=')') sym += s[j];
    else {
        if(s[j]==')') {
            tokens.push_back(sym);
            sym = "";
            sym += s[j];
        }
    }
    if(sym != "") tokens.push_back(sym);
    
    return (int)tokens.size();
}

bool MathExpression::fillTable(map<string, double>& tab) {
    for(unsigned i=0; i<size(); i++) {
        if(!isOperator((*this)[i])) {
            if (isSymbol((*this)[i])) {
                if(!specialSymbol((*this)[i])) tab[(*this)[i]] = 0.0;
            }
            else {
                tab[(*this)[i]] = hsrv::a2double((*this)[i]);
            }
        }
    }
    return true;
}

double MathExpression::evaluate(std::map<std::string, double>& symtab) {
    double a, b, x;
    unsigned i;
    vector<double> stack;
    for (i=0; i<size(); i++) {
        if (isOperator((*this)[i]) && stack.size()>1) {
            a = stack.back();
            stack.pop_back();
            b = stack.back();
            stack.pop_back();
            x = evalOP((*this)[i], b, a);
            stack.push_back(x);
        }
        else stack.push_back(symtab[(*this)[i]]);
    }
    return (stack.empty()? 0.0 : stack.back());
}

double MathExpression::evalOP(string op, double a, double b) {
    if (op=="+") return a+b;
    if (op=="-") return a-b;
    else if(op=="*") return a*b;
    else if(op=="/") return (b != 0.0? a/b : 0);
    else if(op=="^") return pow(a,b);
    else if(op=="<") return (a<b? 1.0 : 0.0);
    else if(op=="<=") return (a<=b? 1.0 : 0.0);
    else if(op==">") return (a>b? 1.0 : 0.0);
    else if(op==">=") return (a>=b? 1.0 : 0.0);
    else if(op=="==") return (a+EPSILON>b&&a<b+EPSILON? 1.0 : 0.0);
    else if(op=="&&") return (a>0&&b>0? 1.0 : 0.0);
    else if(op=="||") return (a>0||b>0? 1.0 : 0.0);
    return 0;
}

string MathExpression::eval(std::map<std::string, double>& symtab) {
    double res;
    if(size()==0) return "";
    if(size()==1&&specialSymbol((*this)[0])) return (*this)[0];
    res = evaluate(symtab);
    return hsrv::double2a(res);
}

bool MathExpression::match(std::map<std::string, double>& symtab) {
    double res = evaluate(symtab);
    return res>0;
}

bool MathExpression::showMap(std::map<std::string, double> &symtab) {
    for (map<string,double>::iterator it=symtab.begin(); it!=symtab.end(); it++) {
        cout<<it->first<<" "<<it->second<<endl;
    }
    return true; 
}