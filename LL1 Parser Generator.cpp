#include <iostream>
#include <vector>
#include <stack>
using namespace std;

struct Rule {
    string lhs;
    vector<string> rhs;
    Rule(string l, vector<string> r) : lhs(l), rhs(r) {}
};

class LL1Parser {
    vector<Rule> rules;
    string start;
    stack<string> stk;

    bool is_terminal(const string& s) {
        return !isupper(s[0]) && s != "ε";
    }

    vector<string> first_of(const string& sym) {
        vector<string> result;
        if(is_terminal(sym) || sym == "ε") {
            result.push_back(sym);
            return result;
        }

        for(auto& rule : rules) {
            if(rule.lhs == sym) {
                if(rule.rhs[0] == "ε") {
                    result.push_back("ε");
                } else {
                    auto first = first_of(rule.rhs[0]);
                    result.insert(result.end(), first.begin(), first.end());
                }
            }
        }
        return result;
    }

public:
    void add_rule(string lhs, vector<string> rhs) {
        rules.emplace_back(lhs, rhs);
    }

    void set_start(string s) { start = s; }

    void parse(vector<string> input) {
        stk.push("$");
        stk.push(start);
        input.push_back("$");

        cout << "Parsing Steps:\n";
        cout << "Stack\t\tInput\t\tAction\n";

        size_t pos = 0;
        while(!stk.empty()) {
            // Print current state
            print_state(input, pos);

            string top = stk.top();
            string curr = input[pos];

            if(top == curr) {
                cout << "Match '" << top << "'\n";
                stk.pop();
                pos++;
            }
            else if(is_terminal(top)) {
                error("Expected " + top + " but found " + curr);
                return;
            }
            else {
                bool applied = false;
                for(auto& rule : rules) {
                    if(rule.lhs == top && 
                      (rule.rhs[0] == "ε" || contains(first_of(rule.rhs[0]), curr))) {
                        apply_rule(rule);
                        applied = true;
                        break;
                    }
                }
                if(!applied) {
                    error("No rule for " + top + " on " + curr);
                    return;
                }
            }
        }
        cout << "Parse successful!\n";
    }

private:
    bool contains(const vector<string>& vec, const string& val) {
        for(auto& s : vec) if(s == val) return true;
        return false;
    }

    void apply_rule(const Rule& rule) {
        cout << "Apply " << rule.lhs << " -> ";
        for(auto& s : rule.rhs) cout << s << " ";
        cout << "\n";

        stk.pop();
        if(rule.rhs[0] != "ε") {
            for(int i = rule.rhs.size()-1; i >= 0; i--) {
                stk.push(rule.rhs[i]);
            }
        }
    }

    void print_state(const vector<string>& input, size_t pos) {
        // Print stack
        stack<string> temp = stk;
        vector<string> stack_elems;
        while(!temp.empty()) {
            stack_elems.push_back(temp.top());
            temp.pop();
        }
        for(int i = stack_elems.size()-1; i >= 0; i--) {
            cout << stack_elems[i] << " ";
        }
        cout << "\t\t";

        // Print remaining input
        for(size_t i = pos; i < input.size(); i++) {
            cout << input[i] << " ";
        }
        cout << "\t\t";
    }

    void error(const string& msg) {
        cout << "ERROR: " << msg << "\n";
    }
};

int main() {
    LL1Parser parser;
    
    //Grammar rules for Arithmetic Expressions
    parser.set_start("E");
    parser.add_rule("E", {"T", "E'"});
    parser.add_rule("E'", {"+", "T", "E'"});
    parser.add_rule("E'", {"ε"});
    parser.add_rule("T", {"F", "T'"});
    parser.add_rule("T'", {"*", "F", "T'"});
    parser.add_rule("T'", {"ε"});
    parser.add_rule("F", {"(", "E", ")"});
    parser.add_rule("F", {"id"});
    
    //Accepting Inputs
    // vector<string> input = {"id", "+", "id", "*", "id"};
    // vector<string> input = {"id"};
    // vector<string> input = {"id", "+", "id"};
    // vector<string> input = {"id", "+", "id", "*", "id"};
    // vector<string> input = {"(", "id", ")", "*", "id"};
    vector<string> input = {"(", "id", "+", "id", ")"};
    parser.parse(input);

    //input = {"+", "id"};
    //input = {"id", "*"};
    input = {"id", "+", "*", "id"};
    //input = {"(", "id", "+", "id"};
    //input = {"id", "id"};
    parser.parse(input);
    
    return 0;
}