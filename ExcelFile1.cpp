#include<iostream>                     // for input-output operations
#include<unordered_map>                // for unordered_map
using namespace std;
class Cell {
public:
   string raw;                        // raw expression entered by user (e.g., "10", "=9+10")
   long long computed;                // computed numeric value of the expression
   Cell() {}                          // default constructor required for unordered_map
   Cell(string raw, long long computed) {   // parameterized constructor
       this->raw = raw;               // store raw expression
       this->computed = computed;     // store computed value
   }
};
class Excel {
private:
   unordered_map<string, Cell> sheet; // map from cell name (A1,B2) → Cell object
   // Time Complexity: O(L) where L = length of expression
   long long eval(string expr) {
       if(expr[0] != '=') return stoll(expr); // if not a formula, convert string directly to number
       long long result = 0;                  // store final computed value
       string token = "";                     // temporary token for parsing numbers
       for(int i = 1; i < expr.size(); i++) { // iterate over expression skipping '='
           if(expr[i] == '+') {               // '+' indicates end of current number token
               if(token != "") result += stoll(token); // convert token to number and add to result
               token = "";                    // reset token for next number
           }
           else {
               token += expr[i];              // keep building the current number token
           }
       }
       if(token != "") result += stoll(token); // add the last token after loop
       return result;                          // return evaluated result
   }
public:
   // Time Complexity: O(L) where L = length of formula
   // cell can be A1, B2. value can be "10", "1", "=9+10", "=-1+-10+2"
   void set(string cell, string expr) {
       long long computedValue = eval(expr);  // compute the formula value
       sheet[cell] = Cell(expr, computedValue); // store raw expression and computed value in sheet
   }
   // Time Complexity: O(1)
   // reset the cell
   void reset(string cell) {
       sheet.erase(cell);                     // remove cell from the sheet
   }
   // Time Complexity: O(N) where N = number of cells stored
   // print all the cells along with their raw and computed values
   void print() {
       for(auto &entry : sheet) {             // iterate through all stored cells
           cout << "Cell : " << entry.first   // print cell name
                << " has raw value : " << entry.second.raw   // print raw expression
                << " actual value : " << entry.second.computed // print computed numeric value
                << endl;
       }
   }
};


int main() {
   Excel excel;                                      // create Excel objec
   cout << "----- Setting Basic Values -----" << endl;
   excel.set("A1","10");                             // A1 = 10
   excel.set("B1","5");                               // B1 = 5
   excel.set("C1","1");                               // C1 = 1
   excel.print();                                     // print current sheet
   cout << endl;
   cout << "----- Setting Formula Cells -----" << endl;
   excel.set("A2","=9+10");                           // A2 = 19
   excel.set("B2","=-1+-10+2");                       // B2 = -9
   excel.set("C2","=5+5+5");                          // C2 = 15
   excel.print();
   cout << endl;
   cout << "----- Updating Existing Cell -----" << endl;
   excel.set("A1","20");                              // update A1
   excel.print();
   cout << endl;
   cout << "----- Reset Cell -----" << endl;
   excel.reset("B1");                                 // remove B1
   excel.print();
   cout << endl;
   cout << "----- Adding More Expressions -----" << endl;
   excel.set("D1","=100+200+300");                    // D1 = 600
   excel.set("E1","=-5+3+-2");                        // E1 = -4
   excel.print();
   cout << endl;
   cout << "----- Reset Multiple Cells -----" << endl;
   excel.reset("A2");
   excel.reset("C1");
   excel.print();
   return 0;
}

