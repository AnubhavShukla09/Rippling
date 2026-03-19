#include<iostream>
#include<vector>
#include<map>
#include<unordered_map>
using namespace std;
class Excel {
private:
    map<string, string> sheet;
    // O(L)
    bool isNumber(string expr) {
        if(expr.empty()) return false;
        int start = (expr[0]=='-') ? 1 : 0;
        for(int i = start; i < expr.size(); i++) {
            if(!isdigit(expr[i])) return false;
        }
        return true;
    }
    // O(1) for number, else recursive
    long long evaluateToken(string token,
                            unordered_map<string, long long>& cache) {
        if(isNumber(token)) return stoll(token);
        if(sheet.count(token)) return evaluateCell(token, cache);
        return 0;
    }
    // O(V + E)
    long long evaluateCell(string cell,
                           unordered_map<string, long long>& cache) {
        if(cache.count(cell)) return cache[cell];
        if(!sheet.count(cell)) return 0;
        string expr = sheet[cell];
        string token = "";
        long long result = 0;
        int sign = 1;
        int n = expr.size();
        if(expr[0] != '=') {
            result = stoll(expr);
        }
        else {
            for(int i = 1; i < n; i++) {
                if(expr[i] == '+' || expr[i] == '-') {
                    if(token != "")
                        result += evaluateToken(token, cache) * sign;
                    token = "";
                    sign = (expr[i] == '+') ? 1 : -1;
                }
                else {
                    token += expr[i];
                }
            }
        }
        if(token != "")
            result += evaluateToken(token, cache) * sign;
        return cache[cell] = result;
    }
public:
    // O(log N)
    void set(string cell, string expr) {
        sheet[cell] = expr;
    }
    // O(log N)
    void reset(string cell) {
        sheet.erase(cell);
    }
    // O(V + E)
    void print() {
        unordered_map<string, long long> cache;

        for(auto &c: sheet) {
            long long val = evaluateCell(c.first, cache);

            cout << "Cell : " << c.first
                 << " Raw : " << c.second
                 << " Actual : " << val << endl;
        }
    }
};

int main() {
    Excel excel;
    cout << "---- Basic Expressions ----" << endl;
    excel.set("A1", "=9+-3+4");      // 10
    excel.set("A2", "=10-5+2");      // 7
    cout << "\n---- Cell References ----" << endl;
    excel.set("B1", "=A1+A2");       // 17
    excel.set("B2", "=B1-A1+5");     // 12
    cout << "\n---- Chain Dependency ----" << endl;
    excel.set("C1", "=B2+B1+A1");    // 12 + 17 + 10 = 39
    cout << "\n---- Negative Handling ----" << endl;
    excel.set("D1", "=-10+5-3");     // -8
    cout << "\n---- Missing Cell ----" << endl;
    excel.set("E1", "=Z1+5");        // Z1 → 0 → result = 5
    cout << "\n---- Final Output ----" << endl;
    excel.print();
    return 0;
}
