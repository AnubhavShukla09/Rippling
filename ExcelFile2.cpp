#include<iostream>                     // for input-output operations
#include<unordered_map>                // for unordered_map
#include<cctype>                       // for isdigit
using namespace std;

class Cell {
public:
   string raw;                        // raw expression entered by user (e.g., "10", "=9+10", "=A1+5")
   long long computed;                // computed numeric value of the expression
   Cell() {}                          // default constructor required for unordered_map
   Cell(string raw, long long computed) { this->raw = raw; this->computed = computed; } // constructor to initialize raw and computed
};

class Excel {
private:
   unordered_map<string, Cell> sheet; // map from cell name (A1,B2) → Cell object
   bool isNumber(string token) {      // Time Complexity: O(L) where L = token length
       if(token=="") return false;    // empty token is not a number
       int start = 0;                 // start index for checking digits
       if(token[0]=='-') start = 1;   // allow negative numbers
       for(int i = start; i < token.size(); i++) if(!isdigit(token[i])) return false; // ensure all characters are digits
       return true;                   // token is a valid number
   }
   long long resolveToken(string token) { // Time Complexity: O(1)
       if(isNumber(token)) return stoll(token); // if token is numeric convert to integer
       auto it = sheet.find(token);  // search if token is a cell reference
       if(it != sheet.end()) return it->second.computed; // return computed value of referenced cell
       return 0;                     // treat missing cell as value 0
   }
   // Time Complexity: O(L) where L = length of expression
   long long eval(string expr) {
       if(expr[0] != '=') return stoll(expr); // if not a formula, convert string directly to number
       long long result = 0;                  // store final computed value
       string token = "";                     // temporary token for parsing numbers or cell references
       for(int i = 1; i < expr.size(); i++) { // iterate over expression skipping '='
           if(expr[i] == '+') {               // '+' indicates end of current token
               if(token != "") result += resolveToken(token); // resolve token (number or cell reference) and add to result
               token = "";                    // reset token for next value
           } else token += expr[i];           // keep building the current token
       }
       if(token != "") result += resolveToken(token); // add the last token after loop
       return result;                          // return evaluated result
   }
public:
   // Time Complexity: O(L) where L = length of formula
   void set(string cell, string expr) {       // set a cell with raw value or formula
       long long computedValue = eval(expr);  // compute the formula value
       sheet[cell] = Cell(expr, computedValue); // store raw expression and computed value in sheet
   }
   // Time Complexity: O(1)
   void reset(string cell) {                  // reset the cell
       sheet.erase(cell);                     // remove cell from the sheet
   }
   // Time Complexity: O(N) where N = number of cells stored
   void print() {                             // print all cells with raw and computed values
       for(auto &entry : sheet) cout << "Cell : " << entry.first << " has raw value : " << entry.second.raw << " actual value : " << entry.second.computed << endl; // print each stored cell
   }
};


int main() {                                   // testing Excel functionality
   Excel excel;                               // create Excel object
   cout<<"----- Setting Basic Values -----"<<endl; // test plain numeric values
   excel.set("A1","10");                      // set A1 = 10
   excel.set("B1","5");                       // set B1 = 5
   excel.set("C1","1");                       // set C1 = 1
   excel.print();                             // print current sheet
   cout<<endl;
   cout<<"----- Setting Formula Values -----"<<endl; // test numeric formulas
   excel.set("A2","=9+10");                   // A2 = 19
   excel.set("B2","=-1+-10+2");               // B2 = -9
   excel.set("C2","=5+5+5");                  // C2 = 15
   excel.print();                             // print updated sheet
   cout<<endl;
   cout<<"----- Testing Cell References -----"<<endl; // test formulas with cell references
   excel.set("D1","=A1+5");                   // D1 = A1 + 5 → 15
   excel.set("E1","=A1+B1+10");               // E1 = A1 + B1 + 10 → 25
   excel.print();                             // print sheet including references
   cout<<endl;
   cout<<"----- Updating Cell Used In Formula -----"<<endl; // change base cell
   excel.set("A1","20");                      // update A1
   excel.print();                             // note: dependent cells do not auto-update
   cout<<endl;
   cout<<"----- Reset Cell -----"<<endl;       // test reset functionality
   excel.reset("B1");                         // remove B1
   excel.print();                             // print remaining cells
   return 0;                                  // end program
}

