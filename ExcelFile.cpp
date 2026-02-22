//=========================================================================FIRST PART======================================================================================
#include <iostream>     // cout
#include <unordered_map> // unordered_map
#include <string>       // string, stoll
#include <stdexcept>    // runtime_error
#include <cctype>       // isalpha, isdigit
using namespace std; // use std namespace
class Cell { // class representing a single cell
private:
    string raw_value; // original value like "=5+9"
    long long computed_value; // evaluated numeric result
public:
    Cell() { raw_value = ""; computed_value = 0; } // constructor initializes empty cell
    void setRaw(string val) { raw_value = val; } // Time: O(1) -> store raw value
    string getRaw() const { return raw_value; } // Time: O(1) -> return raw value
    void setComputed(long long val) { computed_value = val; } // Time: O(1) -> store computed value
    long long getComputed() const { return computed_value; } // Time: O(1) -> return computed value
};
class Spreadsheet { // spreadsheet container managing all cells
private:
    unordered_map<string, Cell> cells; // mapping cell name -> Cell object
    long long evaluate(string expr) { // Time: O(n) where n = length of expression
        if (expr.empty()) throw runtime_error("Empty expression"); // validation check
        long long result = 0; // stores final result
        long long sign = 1; // current sign (+1 or -1)
        string num = ""; // buffer to build number
        for (char c : expr) { // iterate each character
            if (c == '+' || c == '-') { // if operator encountered
                if (!num.empty()) { result += sign * stoll(num); num = ""; } // process previous number
                sign = (c == '+') ? 1 : -1; // update sign
            }
            else num.push_back(c); // build number
        }
        if (!num.empty()) result += sign * stoll(num); // process last number
        return result; // return evaluated result
    }
public:
    void set(string cell, string value) { // Time: O(n) -> length of the input expression string being processed
        if (value.empty()) throw runtime_error("Value cannot be empty"); // validation check
        cells[cell].setRaw(value); // store raw value
        long long computed; // variable to store result
        if (value[0] == '=') { // if expression
            if (value.size() == 1) throw runtime_error("Invalid expression"); // "=" only check
            computed = evaluate(value.substr(1)); // evaluate without '='
        }
        else computed = stoll(value); // numeric value
        cells[cell].setComputed(computed); // store computed value
    }
    void reset(string cell) { cells.erase(cell); } // Time: O(1) -> remove cell
    void print() { // Time: O(c) where c = number of cells
        cout << "----- Spreadsheet -----\n"; // header
        for (auto &p : cells) cout << p.first << " | raw: " << p.second.getRaw() << " | computed: " << p.second.getComputed() << "\n"; // print cells
    }
};
int main() { // program entry point
    Spreadsheet sheet; // create spreadsheet object
    sheet.set("A1", "=5+9+-8"); // expression -> 6
    sheet.set("A2", "5"); // numeric value
    sheet.print(); // print cells
    return 0; // exit program
}

//=====================================================PART 2=====================================================================
#include <bits/stdc++.h> // includes all standard libraries
using namespace std; // use std namespace
class Cell { // class representing a single cell
private:
    string raw_value; // original value
    long long computed_value; // evaluated result
public:
    Cell() { raw_value = ""; computed_value = 0; } // constructor
    void setRaw(string val) { raw_value = val; } // Time: O(1)
    string getRaw() const { return raw_value; } // Time: O(1)
    void setComputed(long long val) { computed_value = val; } // Time: O(1)
    long long getComputed() const { return computed_value; } // Time: O(1)
};
class Spreadsheet { // spreadsheet container
private:
    unordered_map<string, Cell> cells; // cell name -> Cell mapping
//=====================================================PART 2=====================================================================
    bool isCellReference(string token) { // checks if token like A1
        if (token.empty()) return false; // invalid if empty
        int i = 0; // index pointer
        while (i < token.size() && isalpha(token[i])) i++; // parse letters
        if (i == 0 || i == token.size()) return false; // must have letters + digits
        while (i < token.size()) if (!isdigit(token[i++])) return false; // check digits
        return true; // valid cell reference
    }
//=====================================================PART 2=====================================================================
    long long getCellValue(string cell) { // return stored computed value
        if (!cells.count(cell)) return 0; // return 0 if cell not present
        return cells[cell].getComputed(); // return stored value
    }
    long long evaluate(string expr) { // Time: O(n)
        if (expr.empty()) throw runtime_error("Empty expression"); // validation check
        long long result = 0; // final result
        long long sign = 1; // current sign
        string token = ""; // number or cell reference
        for (char c : expr) { // iterate characters
            if (c == '+' || c == '-') { // operator encountered
                if (!token.empty()) { // process token
                    if (isCellReference(token)) result += sign * getCellValue(token); // resolve cell reference
                    else result += sign * stoll(token); // numeric value
                    token = ""; // reset token
                }
                sign = (c == '+') ? 1 : -1; // update sign
            }
            else token.push_back(c); // build token
        }
        if (!token.empty()) { // process last token
            if (isCellReference(token)) result += sign * getCellValue(token); // resolve cell reference
            else result += sign * stoll(token); // numeric value
        }
        return result; // return evaluated result
    }
public:
    void set(string cell, string value) { // Time: O(n)
        if (value.empty()) throw runtime_error("Value cannot be empty"); // validation check
        cells[cell].setRaw(value); // store raw value
        long long computed; // store result
        if (value[0] == '=') { // expression
            if (value.size() == 1) throw runtime_error("Invalid expression"); // "=" only check
            computed = evaluate(value.substr(1)); // evaluate expression
        }
        else computed = stoll(value); // numeric value
        cells[cell].setComputed(computed); // store computed value
    }
    void reset(string cell) { cells.erase(cell); } // Time: O(1)
    void print() { // Time: O(c)
        cout << "----- Spreadsheet -----\n"; // header
        for (auto &p : cells) cout << p.first << " | raw: " << p.second.getRaw() << " | computed: " << p.second.getComputed() << "\n"; // print cells
    }
};
int main() { // program entry point
    Spreadsheet sheet; // create spreadsheet
    sheet.set("A1", "=5+9+-8"); // 6
    sheet.set("A2", "5"); // 5
//============================== PART 2 DEMO =======================================================
    sheet.set("A3", "=A1+10"); // 16
    sheet.set("A4", "=A3-A2"); // 11
//============================== PART 2 DEMO =======================================================
    sheet.print(); // print cells
    return 0; // exit
}

//======================================================PART 3=============================================================

/* 
We model the spreadsheet as a dependency graph, where each cell points to the cells that depend on it. 
When a cell’s value changes, we update its value and use BFS (topological propagation) to recompute all 
dependent cells in the correct order. This ensures parent cells are updated before their children and avoids 
repeated recomputation. The approach also scales well and can be extended to detect cyclic dependencies.
*/


