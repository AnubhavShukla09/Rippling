#include<iostream>
#include<vector>
#include<map>
#include<unordered_set>
#include<unordered_map>
using namespace std;
class Excel {
private:
   map<string, string> sheet; // O(N) space
   // Time Complexity: O(L) where L = length of token
   bool isNumber(string expr) {
       if(expr.empty()) return false;
       if(expr[0]=='-' && expr.size()==1) return false;
       int start = (expr[0]=='-') ? 1 : 0;
       for(int i = start; i < expr.size(); i++) {
           if(!isdigit(expr[i])) return false;
       }
       return true;
   }
   // Time Complexity: O(1) for number
   // O(V + E) if recursive call happens
   long long evaluateToken(string token,
                           unordered_map<string, long long>& cache,
                           unordered_set<string>& vis) {
       if(isNumber(token)) return stoll(token);
       if(sheet.count(token)) return evaluateCell(token, cache, vis);
       return 0;
   }
   // Time Complexity: O(V + E)
   // V = number of cells, E = number of dependencies
   long long evaluateCell(string cell,
                          unordered_map<string, long long>& cache,
                          unordered_set<string>& vis) {
       if(cache.count(cell)) return cache[cell]; // O(1)
       if(vis.count(cell)) throw runtime_error("Cycle"); // O(1)
       if(!sheet.count(cell)) return 0; // O(log N)
       vis.insert(cell); // O(1)
       string expr = sheet[cell];
       string token = "";
       long long result = 0;
       int sign = 1;
       int n = expr.size();
       if(expr[0] != '=') {
           result = stoll(expr); // O(L)
       }
       else {
           // O(L) parsing
           for(int i = 1; i < n; i++) {
               if(expr[i] == '+' || expr[i] == '-') {
                   if(token != "")
                       result += (evaluateToken(token, cache, vis) * sign);
                   token = "";
                   sign = (expr[i] == '+') ? 1 : -1;
               }
               else {
                   token += expr[i];
               }
           }
       }
       // O(1) / O(V+E) depending on token
       if(token != "")
           result += (evaluateToken(token, cache, vis) * sign);
       vis.erase(cell); // O(1)
       return cache[cell] = result; // O(1)
   }
public:
   // Time Complexity: O(log N)
   void set(string cell, string expr) {
       sheet[cell] = expr;
   }
   // Time Complexity: O(log N)
   void reset(string cell) {
       sheet.erase(cell);
   }
   // Time Complexity: O(V + E)
   // Each cell evaluated once due to cache
   void print() {
       unordered_map<string, long long> cache;
       for(auto &c: sheet) { // O(V)
           try {
               unordered_set<string> vis;
               long long val = evaluateCell(c.first, cache, vis);
               cout << "Cell : " << c.first
                    << " Raw : " << c.second
                    << " Actual : " << val << endl;
           } catch (const exception &e) {
               cout << "Cell : " << c.first
                    << " Raw : " << c.second
                    << " ERROR : " << e.what() << endl;
           }
       }
   }
};

