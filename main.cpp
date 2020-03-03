//
//  main.cpp
//  Interpreter
//
//  Created by Паршиков Мирон on 08.04.2018.
//  Copyright © 2018 Паршиков Мирон. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <stack>
using namespace std;

enum type_of_lex {
    LEX_KEYWORD,
    LEX_DEL,
    LEX_IDENT,
    LEX_NUMB,
    LEX_STR_CONST,
    LEX_READ,
    LEX_WRITE,
    LEX_ADDRESS,
    LEX_ASSIGN,
    LEX_JUMP_NEQ,
    LEX_JUMP_ABSOLUTE,
    LEX_LABEL,
};

class lex{
    type_of_lex type;
    int number;
    
public:
    lex (type_of_lex t, int v = 0){
        type = t;
        number = v;
    }
    type_of_lex get_type () {return type;}
    int get_value () {return number;}
    friend ostream& operator<<(ostream &os, const lex& a);
};

class ident{
    string name;
    bool declare;
    bool value_declare;
    string type;
    int int_value;
    int label_value;
    string str_value;
    
public:
    ident(string value){
        value_declare = false;
        declare = false;
        name = value;
        type = "";
    }
    void type_declararion(string value) {type = value;}
    string get_name() {return name;}
    string get_type() {return type;}
    int get_int_value() {return  int_value;}
    string get_str_value() {return str_value;}
    int get_label() {return label_value;}
    bool declaration () {return declare;}
    bool value_declaration () {return value_declare;}
    void value_declaration (int a) {int_value = a;}
    void value_declaration (bool value) {value_declare = value;}
    void value_declaration (string value) {str_value = value;}
    void label_declare(int a) {label_value = a;}
    void declaration(bool value) { declare = value;}
    friend ostream& operator<<(ostream &os, const ident& a);
};

class syntatic_analyzer;

class executer;

class lex_analyzer {
    friend class syntatic_analyzer;
    friend class executer;
    enum state {H, ID, NUMB, STR_CONST, DEL, ANOTHER_ONE, NEQ};
    state CS;
    string buf;
    char c;
    int i, j, d;
    static const vector <string> TW;
    static const vector <string> TD;
    static vector <string> STR_CONSTS;
    static vector<ident> IDENTS;
    static int find (const string& s, const vector<string>& TAB) {
        for (int i = 0; i < TAB.size(); i++) {
            if (s == TAB[i]) {
                return i;
            }
        }
        return -1;
    }
    void find_or_add (const string& s, vector<string>& TAB) {
        int i;
        for (i = 0; i < TAB.size(); i++) {
            if (s == TAB[i]) {
                all_lex.push_back(lex(LEX_STR_CONST, i));
                return;
            }
        }
        TAB.push_back(s);
        all_lex.push_back(lex(LEX_STR_CONST, i));
        return;
    }
    void find_or_add (const string& s, vector<ident>& TAB) {
        int i;
        for (i = 0; i < TAB.size(); i++) {
            if (s == TAB[i].get_name()) {
                all_lex.push_back(lex(LEX_IDENT, i));
                return;
            }
        }
        TAB.push_back(s);
        all_lex.push_back(lex(LEX_IDENT, i));
        return;
    }
    
    friend string value (lex &lex);

public:
    vector<lex> all_lex;
    lex_analyzer (){
        CS = H;
        i = 0;
    }
    void analysis (const string & s){
        while (i < s.length()+1) {
            c = s[i];
            switch (CS) {
                case H:
                    if (c == ' ' || c == '\n' || c == '\t' || c == '\0') {
                        i++;
                    }
                    else if (c == '"'){
                        buf += c;
                        all_lex.push_back(lex(LEX_DEL, 19));
                        buf.clear();
                        i++;
                        CS = STR_CONST;
                    }
                    else if (isalpha(c)){
                        buf.clear();
                        buf += c;
                        CS = ID;
                        i++;
                    }
                    else if (isdigit(c)){
                        d = c - '0';
                        CS = NUMB;
                        i++;
                    }
                    else if (c == '{' || c == '}'){
                        buf.clear();
                        buf += c;
                        all_lex.push_back(lex(LEX_DEL, (j = find(buf, TD))));
                        i++;
                    }
                    else if (c == '=' || c == '<' || c == '>'){
                        buf.clear();
                        buf += c;
                        CS = ANOTHER_ONE;
                        i++;
                    }
                    else if (c == '!'){
                        buf.clear();
                        buf += c;
                        CS = NEQ;
                        i++;
                    }
                    else{
                        buf.clear();
                        buf += c;
                        CS = DEL;
                    }
                    break;
                case ID:
                    if (isalpha(c) || isdigit(c)) {
                        buf += c;
                        i++;
                    }
                    else if ((j = find(buf, TW)) != -1){
                        all_lex.push_back(lex(LEX_KEYWORD, j));
                        CS = H;
                    }
                    else{
                        find_or_add(buf, IDENTS);
                        CS = H;
                    }
                    break;
                case NUMB:
                    if (isdigit(c)) {
                        d = d * 10 + (c - '0');
                        i++;
                    }
                    else{
                        all_lex.push_back(lex(LEX_NUMB, d));
                        CS = H;
                    }
                    break;
                case ANOTHER_ONE:
                    if (c == '=') {
                        buf += c;
                        all_lex.push_back(lex(LEX_DEL, (j = find(buf, TD))));
                        i++;
                    }
                    else{
                        all_lex.push_back(lex(LEX_DEL, (j = find(buf, TD))));
                    }
                    CS = H;
                    break;
                case STR_CONST:
                    if (i == s.length() && c!='"') {
                        cout << i << " " << c << endl;
                        cout << "YOU'VE FORGOTTEN '\"' AFTER YOUR STRING CONST" << endl;
                        exit(0);
                    }
                    if (c == '"') {
                        find_or_add(buf, STR_CONSTS);
                        all_lex.push_back(lex(LEX_DEL, 19));
                        CS = H;
                        i++;
                        break;
                    }
                    buf += c;
                    i++;
                    break;
                case NEQ:
                    if (c == '=') {
                        buf += c;
                        all_lex.push_back(lex(LEX_DEL, (j = find(buf, TD))));
                        i++;
                    }
                    CS = H;
                    break;
                case DEL:
                    all_lex.push_back(lex(LEX_DEL, (j = find(buf, TD))));
                    CS = H;
                    i++;
                    break;
            }
        }
    }
};


string value (lex &lex){
    string s;
    if(lex.get_type() == LEX_DEL) {s = lex_analyzer::TD[lex.get_value()];}
    if (lex.get_type() == LEX_KEYWORD) {s = lex_analyzer::TW[lex.get_value()];}
    if (lex.get_type() == LEX_IDENT) {s = lex_analyzer::IDENTS[lex.get_value()].get_name();}
    if (lex.get_type() == LEX_STR_CONST) {s = lex_analyzer::STR_CONSTS[lex.get_value()];}
    if (lex.get_type() == LEX_JUMP_NEQ){s = "F!";}
    if (lex.get_type() == LEX_JUMP_ABSOLUTE){s = "!";}
    if (lex.get_type() == LEX_NUMB) {s = to_string(lex.get_value());}
    if (lex.get_type() == LEX_ASSIGN) {s = "LEX_ASSIGN";}
    if (lex.get_type() == LEX_ADDRESS) {s = "LEX_ADDRESS ";s += to_string(lex.get_value());}
    if (lex.get_type() == LEX_WRITE) {s = "write";}
    if (lex.get_type() == LEX_READ) {s = "read";}
    if (lex.get_type() == LEX_LABEL) {s = "LEX_LABEL ";s += to_string(lex.get_value());}
    return s;
}



class syntatic_analyzer {
    enum state {DEF, VAR, VAR_DEF, CONST, NUMBER, STR_CONST};
    state CS;
    int i, j;
    string type, top;
    stack<string> stack_for_symbols, stack_for_types;
    void check_def (vector<lex> all_lex, int &i){
        CS = DEF;
        auto it = all_lex.begin()+i;
        if (value(*it) != "boolean" && value(*it) != "int" && value(*it) != "string")  {
            return;
        }
        while (!(value(*it) == "if" || value(*it) == "while" || value(*it) == "case" || value(*it) == "read" || value(*it) == "write" ||value(*it) == "}")) {
            switch (CS) {
                case DEF:
                    if (value(*it) == "boolean" || value(*it) == "int" || value(*it) == "string") {
                        CS = VAR;
                        type = value(*it);
                        it++;
                    }
                    else{
                        return;
                    }
                    break;
                case VAR:
                    if (it->get_type() != LEX_IDENT) {
                        cout << "\nDEFINITION ERROR : SHOULD BE IDENT" << endl;
                        exit(0);
                    }
                    if (lex_analyzer::IDENTS[it->get_value()].declaration() == false) {
                        lex_analyzer::IDENTS[it->get_value()].declaration(true);
                        lex_analyzer::IDENTS[it->get_value()].type_declararion(type);
                    }
                    else{
                        cout << "\nVARIABLE " << lex_analyzer::IDENTS[it->get_value()] << " ALREADY BEEN DECLARED" << endl;
                        exit(0);
                    }
                    CS = VAR_DEF;
                    it++;
                    break;
                case VAR_DEF:
                    if (value(*it) == "=") {
                        it--;
                        if (lex_analyzer::IDENTS[it->get_value()].value_declaration() == false) {
                            lex_analyzer::IDENTS[it->get_value()].value_declaration(true);
                            poliz.push_back(lex(LEX_ADDRESS, it->get_value()));
                            it++;
                        }
                        CS = CONST;
                        it++;
                        break;
                    }
                    if (value(*it) == ",") {
                        CS = VAR;
                        it++;
                        break;
                    }
                    if (value(*it) == ";") {
                        CS = DEF;
                        it++;
                        break;
                    }
                    cout << "\nDEF ERROR : SHOULD BE ',' OR ';' OR '='" << endl;
                    exit(0);
                case CONST:
                    if (type == "string") {
                        if (value(*it) == "\"") {
                            CS = STR_CONST;
                            it++;
                            break;
                        }
                        else{
                            cout << "\nERROR OF TYPE DECLARATION : SHOULD BE string" << endl;
                            exit(0);
                        }
                    }
                    if (type == "int") {
                        if (value(*it) == "+" || value(*it) == "-") {
                            CS = NUMBER;
                            it++;
                            break;
                        }
                        if (it->get_type() == LEX_NUMB) {
                            CS = NUMBER;
                            i--;
                        }
                        else{
                            cout << "\nERROR OF TYPE DECLARATION : SHOULD BE int" << endl;
                            exit(0);
                        }
                    }
                    if (type == "boolean") {
                        if (value(*it) == "true" || value(*it) == "false") {
                            CS = VAR_DEF;
                            poliz.push_back(*it);
                            poliz.push_back(lex(LEX_ASSIGN));
                            it++;
                            break;
                        }
                        else{
                            cout << "\nERROR OF TYPE DECLARATION : SHOULD BE boolean" << endl;
                            exit(0);
                        }
                    }
                    break;
                case STR_CONST:
                    if (it->get_type() == LEX_STR_CONST) {
                        poliz.push_back(*it);
                        poliz.push_back(lex(LEX_ASSIGN));
                        it++;
                        break;
                        
                    }
                    if (value(*it) == "\"") {
                        CS = VAR_DEF;
                        it++;
                        break;
                    }
                    cout << "DEF ERROR : SHOULD BE '\"'" << endl;
                    exit(0);
                case NUMBER:
                    if (it->get_type() == LEX_NUMB) {
                        poliz.push_back(*it);
                        poliz.push_back(lex(LEX_ASSIGN));
                        CS = VAR_DEF;
                        it++;
                        break;
                    }
                    cout << "DEF ERROR : SHOULD BE NUMBER" << endl;
                    exit(0);
                default:
                    break;
            }
            i++;
        }
    }
    void check_for_too_much (vector<lex> all_lex, int &i){
        if (i > all_lex.size() - 1) {
            cout << "\nDISBALANCE OF BRACKETS" << endl;
            exit(0);
        }
    }
    void check_operation(){
        string type1, type2, op;
        type1 = stack_for_types.top();
        stack_for_types.pop();
        op = stack_for_types.top();
        stack_for_types.pop();
        type2 = stack_for_types.top();
        stack_for_types.pop();
        if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
            if (type1 == type2 && type1 == "int") {
                stack_for_types.push("int");
                return;
            }
            if (op == "+" && type1 == type2 && type1 == "string") {
                stack_for_types.push("string");
                return;
            }
            else{
                cout << "\nwrong types in operation" << endl;
                exit(0);
            }
        }
        if (op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=") {
            if (type1 == type2) {
                stack_for_types.push("boolean");
                return;
            }
            else{
                cout << "\nwrong types in operation" << endl;
                exit(0);
            }
        }
        if (op == "or" || op == "and") {
            if (type1 == type2 && type1 == "boolean") {
                stack_for_types.push("boolean");
                return;
            }
            else{
                cout << "\nwrong types in operation" << endl;
                exit(0);
            }
        }
    }
    void check_not_operation(){
        if (stack_for_types.top() == "boolean") {
            return;
        }
        else{
            cout << "\nwrong type in 'not' operation" << endl;
            exit(0);
        }
    }
    void check_expresiion (vector<lex> all_lex, int &i){
        first(all_lex, i);
        if (value(all_lex[i]) == "=") {
            poliz[poliz.size()-1] = lex(LEX_ADDRESS, all_lex[i-1].get_value());
            while (value(all_lex[i]) == "=") {
                if (stack_for_symbols.empty() != true) {
                    while (stack_for_symbols.top() == "=" || stack_for_symbols.top() == "or" || stack_for_symbols.top() == "and" || stack_for_symbols.top() == "<" || stack_for_symbols.top() == ">" || stack_for_symbols.top() == "<=" || stack_for_symbols.top() == ">=" || stack_for_symbols.top() == "==" || stack_for_symbols.top() == "!=" || stack_for_symbols.top() == "*" || stack_for_symbols.top() == "/" || stack_for_symbols.top() == "%" || stack_for_symbols.top() == "not" || stack_for_symbols.top() == "+" || stack_for_symbols.top() == "-") {
                        if (stack_for_symbols.top() == "=" || stack_for_symbols.top() == "<" || stack_for_symbols.top() == ">" || stack_for_symbols.top() == "<=" || stack_for_symbols.top() == ">=" || stack_for_symbols.top() == "==" || stack_for_symbols.top() == "!=" || stack_for_symbols.top() == "*" || stack_for_symbols.top() == "/" || stack_for_symbols.top() == "%" || stack_for_symbols.top() == "+" || stack_for_symbols.top() == "-") {
                            poliz.push_back(lex(LEX_DEL,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TD)));
                        }
                        else{
                            poliz.push_back(lex(LEX_KEYWORD,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TW)));
                        }
                        stack_for_symbols.pop();
                        if (stack_for_symbols.empty() == true) {
                            break;
                        }
                    }
                }
                stack_for_types.push(value(all_lex[i]));
                i++;
                first(all_lex, i);
                check_operation();
            }
            while (stack_for_symbols.empty() != true) {
                if (stack_for_symbols.top() == "=" || stack_for_symbols.top() == "<" || stack_for_symbols.top() == ">" || stack_for_symbols.top() == "<=" || stack_for_symbols.top() == ">=" || stack_for_symbols.top() == "==" || stack_for_symbols.top() == "!=" || stack_for_symbols.top() == "*" || stack_for_symbols.top() == "/" || stack_for_symbols.top() == "%" || stack_for_symbols.top() == "+" || stack_for_symbols.top() == "-") {
                    poliz.push_back(lex(LEX_DEL,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TD)));
                }
                else{
                    poliz.push_back(lex(LEX_KEYWORD,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TW)));
                }
                stack_for_symbols.pop();
            }
            poliz.push_back(lex(LEX_ASSIGN));
        }
        while (stack_for_symbols.empty() != true) {
            if (stack_for_symbols.top() == "=" || stack_for_symbols.top() == "<" || stack_for_symbols.top() == ">" || stack_for_symbols.top() == "<=" || stack_for_symbols.top() == ">=" || stack_for_symbols.top() == "==" || stack_for_symbols.top() == "!=" || stack_for_symbols.top() == "*" || stack_for_symbols.top() == "/" || stack_for_symbols.top() == "%" || stack_for_symbols.top() == "+" || stack_for_symbols.top() == "-") {
                poliz.push_back(lex(LEX_DEL,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TD)));
            }
            else{
                poliz.push_back(lex(LEX_KEYWORD,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TW)));
            }
            stack_for_symbols.pop();
        }
        return;
    }
    void first (vector<lex> all_lex, int &i){
        second(all_lex, i);
        while (value(all_lex[i]) == "or") {
            if (stack_for_symbols.empty() != true) {
                while (stack_for_symbols.top() == "or" || stack_for_symbols.top() == "and" || stack_for_symbols.top() == "<" || stack_for_symbols.top() == ">" || stack_for_symbols.top() == "<=" || stack_for_symbols.top() == ">=" || stack_for_symbols.top() == "==" || stack_for_symbols.top() == "!=" || stack_for_symbols.top() == "*" || stack_for_symbols.top() == "/" || stack_for_symbols.top() == "%" || stack_for_symbols.top() == "not") {
                    if (stack_for_symbols.top() == "=" || stack_for_symbols.top() == "<" || stack_for_symbols.top() == ">" || stack_for_symbols.top() == "<=" || stack_for_symbols.top() == ">=" || stack_for_symbols.top() == "==" || stack_for_symbols.top() == "!=" || stack_for_symbols.top() == "*" || stack_for_symbols.top() == "/" || stack_for_symbols.top() == "%") {
                        poliz.push_back(lex(LEX_DEL,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TD)));
                    }
                    else{
                        poliz.push_back(lex(LEX_KEYWORD,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TW)));
                    }
                    stack_for_symbols.pop();
                    if (stack_for_symbols.empty() == true) {
                        break;
                    }
                }
            }
            stack_for_symbols.push(value(all_lex[i]));
            stack_for_types.push(value(all_lex[i]));
            i++;
            second(all_lex, i);
            check_operation();
        }
        return;
    }
    void second (vector<lex> all_lex, int &i){
        third(all_lex, i);
        while (value(all_lex[i]) == "and") {
            if (stack_for_symbols.empty() != true) {
                while (stack_for_symbols.top() == "and" || stack_for_symbols.top() == "<" || stack_for_symbols.top() == ">" || stack_for_symbols.top() == "<=" || stack_for_symbols.top() == ">=" || stack_for_symbols.top() == "==" || stack_for_symbols.top() == "!=" || stack_for_symbols.top() == "*" || stack_for_symbols.top() == "/" || stack_for_symbols.top() == "%" || stack_for_symbols.top() == "not") {
                    if (stack_for_symbols.top() == "<" || stack_for_symbols.top() == ">" || stack_for_symbols.top() == "<=" || stack_for_symbols.top() == ">=" || stack_for_symbols.top() == "==" || stack_for_symbols.top() == "!=" || stack_for_symbols.top() == "*" || stack_for_symbols.top() == "/" || stack_for_symbols.top() == "%") {
                        poliz.push_back(lex(LEX_DEL,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TD)));
                    }
                    else{
                        poliz.push_back(lex(LEX_KEYWORD,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TW)));
                    }
                    stack_for_symbols.pop();
                    if (stack_for_symbols.empty() == true) {
                        break;
                    }
                }
            }
            stack_for_symbols.push(value(all_lex[i]));
            stack_for_types.push(value(all_lex[i]));
            i++;
            third(all_lex, i);
            check_operation();
        }
        return;
    }
    void third (vector<lex> all_lex, int &i){
        forth(all_lex, i);
        while (value(all_lex[i]) == "<" || value(all_lex[i]) == ">" || value(all_lex[i]) == "<=" || value(all_lex[i]) == ">=" || value(all_lex[i]) == "==" || value(all_lex[i]) == "!=") {
            if (stack_for_symbols.empty() != true) {
                while (stack_for_symbols.top() == "<" || stack_for_symbols.top() == ">" || stack_for_symbols.top() == "<=" || stack_for_symbols.top() == ">=" || stack_for_symbols.top() == "==" || stack_for_symbols.top() == "!=" || stack_for_symbols.top() == "*" || stack_for_symbols.top() == "/" || stack_for_symbols.top() == "%" || stack_for_symbols.top() == "not") {
                    if (stack_for_symbols.top() == "<" || stack_for_symbols.top() == ">" || stack_for_symbols.top() == "<=" || stack_for_symbols.top() == ">=" || stack_for_symbols.top() == "==" || stack_for_symbols.top() == "!=" || stack_for_symbols.top() == "*" || stack_for_symbols.top() == "/" || stack_for_symbols.top() == "%") {
                        poliz.push_back(lex(LEX_DEL,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TD)));
                    }
                    else{
                        poliz.push_back(lex(LEX_KEYWORD,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TW)));
                    }
                    stack_for_symbols.pop();
                    if (stack_for_symbols.empty() == true) {
                        break;
                    }
                }
            }
            stack_for_symbols.push(value(all_lex[i]));
            stack_for_types.push(value(all_lex[i]));
            i++;
            forth(all_lex, i);
            check_operation();
        }
        return;
    }
    void forth (vector<lex> all_lex, int &i){
        fifth(all_lex, i);
        while (value(all_lex[i]) == "+" || value(all_lex[i]) == "-") {
            if (stack_for_symbols.empty() != true) {
                while (stack_for_symbols.top() == "+" || stack_for_symbols.top() == "-" || stack_for_symbols.top() == "*" || stack_for_symbols.top() == "/" || stack_for_symbols.top() == "%" || stack_for_symbols.top() == "not") {
                    if (stack_for_symbols.top() == "*" || stack_for_symbols.top() == "/" || stack_for_symbols.top() == "%" || stack_for_symbols.top() == "+" || stack_for_symbols.top() == "-") {
                        poliz.push_back(lex(LEX_DEL,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TD)));
                    }
                    else{
                        poliz.push_back(lex(LEX_KEYWORD,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TW)));
                    }
                    stack_for_symbols.pop();
                    if (stack_for_symbols.empty() == true) {
                        break;
                    }
                }
            }
            stack_for_symbols.push(value(all_lex[i]));
            stack_for_types.push(value(all_lex[i]));
            i++;
            fifth(all_lex, i);
            check_operation();
        }
        return;
    }
    void fifth (vector<lex> all_lex, int &i){
        six(all_lex, i);
        while (value(all_lex[i]) == "*" || value(all_lex[i]) == "/" || value(all_lex[i]) == "%") {
            if (stack_for_symbols.empty() != true) {
                while (stack_for_symbols.top() == "*" || stack_for_symbols.top() == "/" || stack_for_symbols.top() == "%" || stack_for_symbols.top() == "not") {
                    if (stack_for_symbols.top() == "*" || stack_for_symbols.top() == "/" || stack_for_symbols.top() == "%") {
                        poliz.push_back(lex(LEX_DEL,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TD)));
                    }
                    else{
                        poliz.push_back(lex(LEX_KEYWORD,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TW)));
                    }
                    stack_for_symbols.pop();
                    if (stack_for_symbols.empty() == true) {
                        break;
                    }
                }
            }
            stack_for_symbols.push(value(all_lex[i]));
            stack_for_types.push(value(all_lex[i]));
            i++;
            six(all_lex, i);
            check_operation();
        }
        return;
    }
    void six (vector<lex> all_lex, int &i){
        if (value(all_lex[i]) == "not") {
            if (stack_for_symbols.empty() != true) {
                while (stack_for_symbols.top() == "not") {
                    poliz.push_back(lex(LEX_KEYWORD,  j = lex_analyzer::find(stack_for_symbols.top(), lex_analyzer::TW)));
                    stack_for_symbols.pop();
                    if (stack_for_symbols.empty() == true) {
                        break;
                    }
                }
            }
            stack_for_symbols.push(value(all_lex[i]));
            i++;
            six(all_lex, i);
            check_not_operation();
            return;
        }
        if (value(all_lex[i]) == "(") {
            i++;
            stack_for_symbols.push("(");
            first(all_lex, i);
            if (value(all_lex[i]) != ")") {
                cout << "\nEXPRESSION ERROR : YOU'VE FORGOTTEN \")\"" << endl;
                exit(0);
            }
            i++;
            if (stack_for_symbols.empty() != true) {
                while ((top = stack_for_symbols.top()) != "(") {
                    poliz.push_back(lex(LEX_DEL,  j = lex_analyzer::find(top, lex_analyzer::TD)));
                    stack_for_symbols.pop();
                    if (stack_for_symbols.empty() == true) {
                        break;
                    }
                }
            }
            if (stack_for_symbols.empty() != true) {
                stack_for_symbols.pop();
            }
            return;
        }
        if (value(all_lex[i]) == "\"") {
            i++;
        }
        if (all_lex[i].get_type() == LEX_IDENT || all_lex[i].get_type() == LEX_NUMB || all_lex[i].get_type() == LEX_STR_CONST || value(all_lex[i]) == "true" || value(all_lex[i]) == "false") {
            if (all_lex[i].get_type() == LEX_IDENT) {
                if (lex_analyzer::IDENTS[all_lex[i].get_value()].declaration() == true){
                    poliz.push_back(all_lex[i]);
                    stack_for_types.push(lex_analyzer::IDENTS[all_lex[i].get_value()].get_type());
                }
                else{
                    cout << "\nVARIABLE " << lex_analyzer::IDENTS[all_lex[i].get_value()].get_name() << " HAVEN'T BEEN DECLARED YET" << endl;
                    exit(0);
                }
            }
            else{
                poliz.push_back(all_lex[i]);
                if (all_lex[i].get_type() == LEX_NUMB) {
                    stack_for_types.push("int");
                }else
                if (all_lex[i].get_type() == LEX_STR_CONST) {
                    stack_for_types.push("string");
                    i++;
                }
                else{
                    stack_for_types.push("boolean");
                }
            }
            i++;
            return;
        }
        cout << "\nEXPRESSION ERROR : SHOULD BE SOME VALUE OR IDENT" << endl;
        return;
    }
    void check_operations (vector<lex> all_lex, int &i){
        check_for_too_much(all_lex, i);
        check_def(all_lex, i);
        auto it = all_lex.begin()+i;
        int label_1, label_2;
        if (value(*it) == "{") {
            i++;
            it++;
            while (value(*it) != "}") {
                check_operations(all_lex, i);
                check_for_too_much(all_lex, i);
                it = all_lex.begin()+i;
            }
            i++;
            return;
        }
        if (value(*it) == "if") {
            i++;
            it++;
            if (value(*it) == "(") {
                i++;
                check_expresiion(all_lex, i);
                if (stack_for_types.top() != "boolean") {
                    cout << "\nTHE CONDITION MUST BE A LOGICAL EXPRESSION" << endl;
                    exit(0);
                }
                label_1 = (int)poliz.size();
                poliz.push_back(lex(LEX_LABEL, -1));
                poliz.push_back(lex(LEX_JUMP_NEQ));
                check_for_too_much(all_lex, i);
                it = all_lex.begin()+i;
            }
            else {
                cout << "\nIF ERROR : YOU'VE FORGOTTEN \"(\"" << endl;
                exit(0);
            }
            if (value(*it) != ")") {
                cout << "\nIF ERROR : YOU'VE FORGOTTEN \")\"" << endl;
                exit(0);
            }
            i++;
            check_operations(all_lex, i);
            label_2 = (int)poliz.size();
            poliz.push_back(lex(LEX_LABEL, -1));
            poliz.push_back(lex(LEX_JUMP_ABSOLUTE));
            poliz[label_1] = lex(LEX_LABEL, (int)poliz.size());
            check_for_too_much(all_lex, i);
            it = all_lex.begin()+i;
            if (value(*it) == "else") {
                i++;
                check_operations(all_lex, i);
                poliz[label_2] = lex(LEX_LABEL, (int)poliz.size());
            }
            else{
                poliz[label_2] = lex(LEX_LABEL, (int)poliz.size());
                
            }
            return;
        }
        if (value(*it) == "do") {
            i++;
            label_1 = (int)poliz.size();
            check_operations(all_lex, i);
            it = all_lex.begin()+i;
            if (value(*it) == "while") {
                i++;
                it++;
                if (value(*it) == "(") {
                    i++;
                    check_expresiion(all_lex, i);
                    if (stack_for_types.top() != "boolean") {
                        cout << "\nTHE CONDITION MUST BE A LOGICAL EXPRESSION" << endl;
                        exit(0);
                    }
                    label_2 = (int)poliz.size();
                    poliz.push_back(lex(LEX_LABEL, -1));
                    poliz.push_back(lex(LEX_JUMP_NEQ));
                    poliz.push_back(lex(LEX_LABEL , label_1));
                    poliz.push_back(lex(LEX_JUMP_ABSOLUTE));
                    poliz[label_2] = lex(LEX_LABEL, (int)poliz.size());
                    it = all_lex.begin()+i;
                    if (value(*it) != ")") {
                        cout << "\nBRACKETS DISBALANCE" << endl;
                        exit(0);
                    }
                    i++;
                    it++;
                    if (value(*it) != ";") {
                        cout << "\nDO_WHILE ERROR : YOU'VE FORGOTTEN \";\""<< endl;
                        exit(0);
                    }
                    i++;
                }
                else{
                    cout << "\nDO_WHILE ERROR\n";
                    exit(0);
                }
            }
            else{
                cout << "\nDO_WHILE ERROR\n";
                exit(0);
            }
            return;
            
        }
        if (value(*it) == "while") {
            i++;
            it++;
            if (value(*it) == "(") {
                i++;
                label_1 = (int)poliz.size();
                check_expresiion(all_lex, i);
                if (stack_for_types.top() != "boolean") {
                    cout << "\nTHE CONDITION MUST BE A LOGICAL EXPRESSION" << endl;
                    exit(0);
                }
                label_2 = (int)poliz.size();
                poliz.push_back(lex(LEX_LABEL, -1));
                poliz.push_back(lex(LEX_JUMP_NEQ));
                check_for_too_much(all_lex, i);
                it = all_lex.begin()+i;
            }
            else{
                cout << "\nWHILE ERROR : YOU'VE FORGOTTEN \"(\"" << endl;
                exit(0);
            }
            if (value(*it) != ")") {
                cout << "\nWHILE ERROR : BRACKETS DISBALANCE" << endl;
                exit(0);
            }
            i++;
            check_operations(all_lex, i);
            poliz.push_back(lex(LEX_LABEL, label_1));
            poliz.push_back(lex(LEX_JUMP_ABSOLUTE));
            poliz[label_2] = lex(LEX_LABEL, (int)poliz.size());
            cout << endl;
            return;
        }
        if (value(*it) == "read") {
            it++;
            i++;
            if (value(*it) == "(") {
                i++;
                it++;
                if (it->get_type() != LEX_IDENT) {
                    cout << "\nREAD ERROR : SHOULD BE IDENT";
                    exit(0);
                }
                if (lex_analyzer::IDENTS[it->get_value()].declaration() == false) {
                    cout << "\nREAD ERROR : NOT DECLARED IDENT" << endl;
                    exit(0);
                }
                if (lex_analyzer::IDENTS[it->get_value()].get_type() == "boolean") {
                    cout << "\nREAD ERROR : CAN'T READ boolean TYPE" << endl;
                    exit(0);
                }
                poliz.push_back(lex(LEX_ADDRESS, it->get_value()));
                poliz.push_back(lex(LEX_READ));
            }
            else{
                cout << "\nREAD ERROR : YOU'VE FORGOTTEN \"(\"" << endl;
                exit(0);
            }
            it++;
            i++;
            if (value(*it) != ")") {
                cout << "\nBRACKETS DISBALANCE" << endl;
                exit(0);
            }
            it++;
            i++;
            if (value(*it) != ";") {
                cout << "\nREAD ERROR : YOU'VE FORGOTTEN \";\""<< endl;
                exit(0);
            }
            i++;
            return;
        }
        if (value(*it) == "write") {
            it++;
            i++;
            if (value(*it) == "(") {
                i++;
                check_expresiion(all_lex, i);
                check_for_too_much(all_lex, i);
                it = all_lex.begin()+i;
                if (value(*it) == ",") {
                    poliz.push_back(lex(LEX_WRITE));
                    while (value(*it) != ")") {
                        if (value(*it) == ",") {
                            i++;
                        }
                        check_expresiion(all_lex, i);
                        poliz.push_back(lex(LEX_WRITE));
                        check_for_too_much(all_lex, i);
                        it = all_lex.begin()+i;
                    }
                    i++;
                    it++;
                    check_for_too_much(all_lex, i);
                }
                else {
                    if(value(*it) == ")"){
                        i++;
                        it++;
                    }
                    else {
                        cout << "\nWRITE ERROR : YOU'VE FORGOTTEN \")\"" << endl;
                        exit(0);
                    }
                    poliz.push_back(lex(LEX_WRITE));
                }
                if (value(*it) != ";") {
                    cout << "\nWRITE ERROR : YOU'VE FORGOTTEN \";\"" << endl;
                    exit(0);
                }
                i++;
            }
            else{
                cout << "\nWRITE ERROR : YOU'VE FORGOTTEN \"(\"" << endl;
                exit(0);
            }
            return;
        }
        if (value(*it) == "}") {
            return;
        }
        if (value(*it) == "goto") {
            i++;
            it++;
            if (lex_analyzer::IDENTS[it->get_value()].declaration() == false) {
                lex_analyzer::IDENTS[it->get_value()].type_declararion("label");
                poliz.push_back(lex(LEX_IDENT, it->get_value()));
                poliz.push_back(lex(LEX_JUMP_ABSOLUTE));
                i++;
                it++;
                if (value(*it) == ";") {
                    i++;
                    return;
                }
                else{
                    cout << "\nGOTO ERROR : YOU'VE FORGOTTEN \";\"" << endl;
                    exit(0);
                }
            }
            if (lex_analyzer::IDENTS[it->get_value()].get_type() == "label") {
                poliz.push_back(lex(LEX_LABEL, lex_analyzer::IDENTS[it->get_value()].get_label()));
                poliz.push_back(lex(LEX_JUMP_ABSOLUTE));
                i++;
                it++;
                if (value(*it) == ";") {
                    i++;
                    return;
                }
                else{
                    cout << "\nGOTO ERROR : YOU'VE FORGOTTEN \";\"" << endl;
                    exit(0);
                }
            }
            else{
                cout << "\nCAN'T GO TO THAT LABEL" << endl;
                exit(0);
            }
        }
        if (it->get_type() == LEX_IDENT && (i+1 < all_lex.size()) && value(*(it+1)) == ":") {
            if (lex_analyzer::IDENTS[it->get_value()].declaration() == false) {
                lex_analyzer::IDENTS[it->get_value()].declaration(true);
                lex_analyzer::IDENTS[it->get_value()].type_declararion("label");
                i+=2;
                lex_analyzer::IDENTS[it->get_value()].label_declare((int)poliz.size());
                for (auto jit = poliz.begin(); jit != poliz.end(); jit++) {
                    if (value(*it) == value(*jit)) {
                        *jit = lex(LEX_LABEL,lex_analyzer::IDENTS[it->get_value()].get_label());
                    }
                }
                check_operations(all_lex, i);
            }
            else{
                cout << "\nLABEL " <<lex_analyzer::IDENTS[it->get_value()] << " ALREADY BEEN DECLARED" << endl;
                exit(0);
            }
        }
        else{
            check_expresiion(all_lex, i);
            if (value(all_lex[i]) != ";") {
                cout << "\nEXPRESSION ERROR : YOU'VE FORGOTTEN \";\"" << endl;
                exit(0);
            }
            i++;
        }
    }

public:
    syntatic_analyzer (){
        i = 0;
    }
    vector<lex> poliz;
    void analysis (vector<lex> all_lex){
        if (value(all_lex[i]) != "program") {
            cout << "\nMUST BE program AS A FIRST WORD" << endl;
            return;        }
        i++;
        check_for_too_much(all_lex, i);
        if (value(all_lex[i]) == "{") {
            check_operations(all_lex, i);
            cout << endl;
            for (auto it = poliz.begin(); it != poliz.end(); it++) {
                if (it->get_type() == LEX_IDENT) {
                    if (lex_analyzer::IDENTS[it->get_value()].get_type() == "label" && lex_analyzer::IDENTS[it->get_value()].declaration() == false) {
                        cout << "LABEL " << lex_analyzer::IDENTS[it->get_value()].get_name() << " STILL HASN'T BEEN DECLARED";
                        exit(0);
                    }
                }
                cout << value(*it) << endl;
            }
            
        }
        else {
            cout << "\nMUST BE \"{\" AFTER \"program\"" << endl;
        }
    }
};

struct int_or_string{
    int int_value;
    string str_value;
    bool int_type = false;
    bool str_type = false;
    void int_set(int value){int_value = value; int_type = true; str_type = false;}
    void string_set(string value){str_value = value; str_type = true; int_type = false;}
    int_or_string(){};
};

class executer {
public:
    friend string value (lex lex);
    void execute (vector<lex> poliz){
        stack <int_or_string> args;
        int_or_string argument;
        int i, j, boolean_value, label;
        string a, b;
        auto it = poliz.begin();
        while (it != poliz.end()) {
            if (it->get_type() == LEX_NUMB || it->get_type() == LEX_LABEL || it->get_type() == LEX_ADDRESS || value(*it) == "true" || value(*it) == "false") {
                argument.int_set(it->get_value());
                args.push(argument);
                it++;
                continue;
            }
            if (it->get_type() == LEX_STR_CONST) {
                argument.string_set(lex_analyzer::STR_CONSTS[it->get_value()]);
                args.push(argument);
                it++;
                continue;
            }
            if (it->get_type() == LEX_IDENT) {
                if (lex_analyzer::IDENTS[it->get_value()].value_declaration() == true) {
                    if (lex_analyzer::IDENTS[it->get_value()].get_type() == "int") {
                        argument.int_set(lex_analyzer::IDENTS[it->get_value()].get_int_value());
                        args.push(argument);
                    }
                    if (lex_analyzer::IDENTS[it->get_value()].get_type() == "string") {
                        argument.string_set(lex_analyzer::IDENTS[it->get_value()].get_str_value());
                        args.push(argument);
                    }
                    it++;
                    continue;
                }
                else{
                    cout << "VARIABLE " << lex_analyzer::IDENTS[it->get_value()].get_name() << "DOESN'T HAS VALUE";
                    exit(0);
                }
            }
            if (value(*it) == "not") {
                i = args.top().int_value;
                args.pop();
                argument.int_set(!i);
                args.push(argument);
                it++;
                continue;
            }
            if (value(*it) == "or") {
                i = args.top().int_value;
                args.pop();
                j = args.top().int_value;;
                args.pop();
                argument.int_set(i || j);
                args.push(argument);
                it++;
                continue;
            }
            if (value(*it) == "and") {
                i = args.top().int_value;
                args.pop();
                j = args.top().int_value;
                args.pop();
                argument.int_set(i && j);
                args.push(argument);
                it++;
                continue;
            }
            if (it->get_type() == LEX_JUMP_ABSOLUTE) {
                it = poliz.begin() + args.top().int_value;
                args.pop();
                continue;
            }
            if (it->get_type() == LEX_JUMP_NEQ) {
                label = args.top().int_value;
                args.pop();
                boolean_value = args.top().int_value;
                args.pop();
                if (!boolean_value) {
                    it = poliz.begin() + label;
                }
                else{
                    it++;
                }
                continue;
            }
            if (value(*it) == "write") {
                if (args.top().int_type) {
                    cout << args.top().int_value;
                }
                if (args.top().str_type) {
                    cout << args.top().str_value;
                }
                args.pop();
                it++;
                continue;
            }
            if (value(*it) == "read") {
                i = args.top().int_value;
                int int_value;
                string str_value;
                args.pop();
                if (lex_analyzer::IDENTS[i].get_type() == "int") {
                    cout << "INPUT int VALUE for " << lex_analyzer::IDENTS[i].get_name() << endl;
                    cin >> int_value;
                    lex_analyzer::IDENTS[i].value_declaration(int_value);
                    lex_analyzer::IDENTS[i].value_declaration(true);
                    it++;
                    continue;
                }
                if (lex_analyzer::IDENTS[i].get_type() == "boolean") {
                    cout << "CAN'T READ BOOLEAN TYPE" << endl;
                    exit(0);
                }
                if (lex_analyzer::IDENTS[i].get_type() == "string") {
                    cout << "INPUT string VALUE for " << lex_analyzer::IDENTS[i].get_name() << endl;
                    cin >> str_value;
                    lex_analyzer::IDENTS[i].value_declaration(str_value);
                    lex_analyzer::IDENTS[i].value_declaration(true);
                    it++;
                    continue;
                }
            }
            if (value(*it) == "+") {
                if (args.top().int_type) {
                    i = args.top().int_value;
                    args.pop();
                    j = args.top().int_value;
                    args.pop();
                    argument.int_set(i+j);
                    args.push(argument);
                }
                if (args.top().str_type) {
                    a = args.top().str_value;
                    args.pop();
                    b = args.top().str_value;
                    args.pop();
                    argument.string_set(a+b);
                    args.push(argument);
                }
                it++;
                continue;
            }
            if (value(*it) == "-") {
                i = args.top().int_value;
                args.pop();
                j = args.top().int_value;
                args.pop();
                argument.int_set(j-i);
                args.push(argument);
                it++;
                continue;
            }
            if (value(*it) == "*") {
                i = args.top().int_value;
                args.pop();
                j = args.top().int_value;
                args.pop();
                argument.int_set(i*j);
                args.push(argument);
                it++;
                continue;
            }
            if (value(*it) == "/") {
                i = args.top().int_value;
                args.pop();
                if (i != 0) {
                    j = args.top().int_value;
                    args.pop();
                    argument.int_set(j/i);
                    args.push(argument);
                    it++;
                    continue;
                }
                else{
                    cout << "YOU CAN'T DIVIDE BY 0" << endl;
                    exit(0);
                }
            }
            if (value(*it) == "%") {
                i = args.top().int_value;
                args.pop();
                if (i != 0) {
                    j = args.top().int_value;
                    args.pop();
                    argument.int_set(j % i);
                    args.push(argument);
                    it++;
                    continue;
                }
                else{
                    cout << "YOU CAN'T DIVIDE BY 0" << endl;
                    exit(0);
                }
            }
            if (value(*it) == "==") {
                if (args.top().int_type) {
                    i = args.top().int_value;
                    args.pop();
                    j = args.top().int_value;
                    args.pop();
                    argument.int_set(i == j);
                    args.push(argument);
                }
                if (args.top().str_type) {
                    a = args.top().str_value;
                    args.pop();
                    b = args.top().str_value;
                    args.pop();
                    argument.int_set(a == b);
                    args.push(argument);
                }
                it++;
                continue;
            }
            if (value(*it) == "<=") {
                if (args.top().int_type) {
                    i = args.top().int_value;
                    args.pop();
                    j = args.top().int_value;
                    args.pop();
                    argument.int_set(j <= i);
                    args.push(argument);
                }
                if (args.top().str_type) {
                    a = args.top().str_value;
                    args.pop();
                    b = args.top().str_value;
                    args.pop();
                    argument.int_set(b <= a);
                    args.push(argument);
                }
                it++;
                continue;
            }
            if (value(*it) == ">=") {
                if (args.top().int_type) {
                    i = args.top().int_value;
                    args.pop();
                    j = args.top().int_value;
                    args.pop();
                    argument.int_set(j >= i);
                    args.push(argument);
                }
                if (args.top().str_type) {
                    a = args.top().str_value;
                    args.pop();
                    b = args.top().str_value;
                    args.pop();
                    argument.int_set(b >= a);
                    args.push(argument);
                }
                it++;
                continue;
            }
            if (value(*it) == "<") {
                if (args.top().int_type) {
                    i = args.top().int_value;
                    args.pop();
                    j = args.top().int_value;
                    args.pop();
                    argument.int_set(j < i);
                    args.push(argument);
                }
                if (args.top().str_type) {
                    a = args.top().str_value;
                    args.pop();
                    b = args.top().str_value;
                    args.pop();
                    argument.int_set(b < a);
                    args.push(argument);
                }
                it++;
                continue;
            }
            if (value(*it) == ">") {
                if (args.top().int_type) {
                    i = args.top().int_value;
                    args.pop();
                    j = args.top().int_value;
                    args.pop();
                    argument.int_set(j > i);
                    args.push(argument);
                }
                if (args.top().str_type) {
                    a = args.top().str_value;
                    args.pop();
                    b = args.top().str_value;
                    args.pop();
                    argument.int_set(b > a);
                    args.push(argument);
                }
                it++;
                continue;
            }
            if (value(*it) == "!=") {
                if (args.top().int_type) {
                    i = args.top().int_value;
                    args.pop();
                    j = args.top().int_value;
                    args.pop();
                    argument.int_set(j != i);
                    args.push(argument);
                }
                if (args.top().str_type) {
                    a = args.top().str_value;
                    args.pop();
                    b = args.top().str_value;
                    args.pop();
                    argument.int_set(b != a);
                    args.push(argument);
                }
                it++;
                continue;
            }
            if (it->get_type() == LEX_ASSIGN) {
                if (args.top().int_type) {
                    i = args.top().int_value;
                    args.pop();
                    j = args.top().int_value;
                    args.pop();
                    lex_analyzer::IDENTS[j].value_declaration(i);
                    lex_analyzer::IDENTS[j].value_declaration(true);
                }else
                if (args.top().str_type) {
                    a = args.top().str_value;
                    args.pop();
                    j = args.top().int_value;
                    args.pop();
                    lex_analyzer::IDENTS[j].value_declaration(a);
                    lex_analyzer::IDENTS[j].value_declaration(true);
                }
                it++;
                continue;
            }
            it++;
        }
        return;
    }
};

int main(int argc, const char * argv[]) {
    FILE* fp;
    char c;
    string s;
    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            lex_analyzer a;
            syntatic_analyzer b;
            executer ex;
            fp = fopen(argv[i], "r");
            while ((c = fgetc(fp)) != EOF) {
                s += c;
            }
            a.analysis(s);
            b.analysis(a.all_lex);
            ex.execute(b.poliz);
            s.clear();
        }
    }
    else{
        while ((c = getchar()) != '.') {
            s += c;
        }
        lex_analyzer a;
        syntatic_analyzer b;
        executer ex;
        a.analysis(s);
        b.analysis(a.all_lex);
        ex.execute(b.poliz);
    }
    cout << endl;
    return 0;
}

const vector <string> lex_analyzer::TW =
{
    "false",
    "true",
    "program",
    "and",
    "or",
    "boolean",
    "int",
    "string",
    "if",
    "else",
    "do",
    "while",
    "goto",
    "case",
    "of",
    "read",
    "write",
    "not"
};

const vector <string> lex_analyzer::TD =
{
    "=", //0
    ",", //1
    ";", //2
    ":", //3
    "*", //4
    "/", //5
    "%", //6
    "+", //7
    "-", //8
    "<", //9
    ">", //10
    "<=", //11
    ">=", //12
    "==", //13
    "!=", //14
    "(", //15
    ")", //16
    "{", //17
    "}", //18
    "\"" //19
};

vector<ident> lex_analyzer::IDENTS = {};

vector<string> lex_analyzer::STR_CONSTS = {};

ostream& operator<<(ostream &os, lex &a){
    os << value(a);
    return os;
}

ostream& operator<<(ostream &os, const ident& a){
    os << a.name;
    return os;
}
