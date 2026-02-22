#include <bits/stdc++.h> // includes all standard libraries
using namespace std; // use std namespace
//===================================================== EXPENSE MODEL ============================================================
// class representing expense record
class Expense {
private:
    string expense_id; // unique id
    string trip_id; // trip id
    string expense_type; // Food, Travel, Entertainment
    string vendor_type; // restaurant, taxi etc
    double amount; // amount in USD
public:
    Expense(string id,string trip,string type,string vendor,double amt){ // constructor
        expense_id=id; trip_id=trip; expense_type=type; vendor_type=vendor; amount=amt; // assign values
    }
    string getId() const { return expense_id; } // Time: O(1)
    string getTripId() const { return trip_id; } // Time: O(1)
    string getExpenseType() const { return expense_type; } // Time: O(1)
    string getVendorType() const { return vendor_type; } // Time: O(1)
    double getAmount() const { return amount; } // Time: O(1)
};
//===================================================== VIOLATION MODEL ==========================================================
// class representing violation result
class Violation {
private:
    string entity; // EXPENSE / TRIP / SYSTEM
    string id; // entity id
    string message; // reason
public:
    Violation(string e,string i,string m){ entity=e; id=i; message=m; } // constructor
    void print() const { // Time: O(1)
        cout<<entity<<" "<<id<<" -> "<<message<<"\n"; // print violation
    }
};
//===================================================== SIMPLE UNIFIED RULE INTERFACE ============================================
// single rule interface supporting all rule types
class Rule {
public:
    virtual vector<Violation> evaluateExpense(const Expense& e){ return {}; } // per-expense rules
    virtual vector<Violation> evaluateAll(const vector<Expense>& expenses){ return {}; } // total rules
    virtual vector<Violation> evaluateTrip(const string& tripId,const vector<Expense>& tripExpenses){ return {}; } // trip rules
    virtual ~Rule(){}
};
//===================================================== PER-EXPENSE RULES ========================================================
// rule: entertainment not allowed
class BanExpenseTypeRule: public Rule{
    string banned;
public:
    BanExpenseTypeRule(string t){ banned=t; } // constructor
    vector<Violation> evaluateExpense(const Expense& e) override { // Time: O(1)
        if(e.getExpenseType()==banned) // check type
            return {Violation("EXPENSE",e.getId(),"Expense type not allowed")}; // violation
        return {};
    }
};
// rule: vendor amount limit (restaurant etc)
class VendorLimitRule: public Rule{
    string vendor; double limit;
public:
    VendorLimitRule(string v,double l){ vendor=v; limit=l; } // constructor
    vector<Violation> evaluateExpense(const Expense& e) override { // Time: O(1)
        if(e.getVendorType()==vendor && e.getAmount()>limit) // check condition
            return {Violation("EXPENSE",e.getId(),"Vendor expense exceeded")}; // violation
        return {};
    }
};
//===================================================== TOTAL AGGREGATE RULE =====================================================
// rule: total expense limit
class TotalExpenseRule: public Rule{
    double limit;
public:
    TotalExpenseRule(double l){ limit=l; } // constructor

    vector<Violation> evaluateAll(const vector<Expense>& expenses) override { // Time: O(N)
        double total=0; // sum
        for(auto &e:expenses) total+=e.getAmount(); // compute total

        if(total>limit)
            return {Violation("SYSTEM","ALL","Total expense exceeded")}; // violation

        return {};
    }
};
//===================================================== TRIP AGGREGATE RULE ======================================================
// rule: per-trip total limit
class TripTotalLimitRule: public Rule{
    double limit;
public:
    TripTotalLimitRule(double l){ limit=l; } // constructor

    vector<Violation> evaluateTrip(const string& tripId,const vector<Expense>& tripExpenses) override { // Time: O(T)
        double total=0; // trip sum
        for(auto &e:tripExpenses) total+=e.getAmount(); // compute

        if(total>limit)
            return {Violation("TRIP",tripId,"Trip total exceeded")}; // violation

        return {};
    }
};
//===================================================== RULE ENGINE ==============================================================\
// evaluates expense + total + trip rules
// Time Complexity: O(R*N)
vector<Violation> evaluateRules(vector<Rule*>& rules,vector<Expense>& expenses){
    vector<Violation> result; // store violations
    //===================== total rules =====================
    for(auto r:rules){ // run aggregate rules
        auto v=r->evaluateAll(expenses); // evaluate
        result.insert(result.end(),v.begin(),v.end());
    }
    //===================== per-expense rules =====================
    for(auto &e:expenses){ // iterate expenses
        for(auto r:rules){
            auto v=r->evaluateExpense(e); // evaluate
            result.insert(result.end(),v.begin(),v.end());
        }
    }
    //===================== group by trip =====================
    unordered_map<string,vector<Expense>> tripGroups; // trip -> expenses
    for(auto &e:expenses) tripGroups[e.getTripId()].push_back(e); // grouping
    //===================== trip rules =====================
    for(auto &[tripId,tripExpenses]:tripGroups){ // iterate trips
        for(auto r:rules){
            auto v=r->evaluateTrip(tripId,tripExpenses); // evaluate
            result.insert(result.end(),v.begin(),v.end());
        }
    }
    return result; // return all violations
}
//===================================================== DEMO =====================================================================
int main(){
    vector<Expense> expenses; // expense list
    expenses.push_back(Expense("1","T1","Food","restaurant",100)); // vendor + trip violation
    expenses.push_back(Expense("2","T1","Entertainment","club",80)); // banned type
    expenses.push_back(Expense("3","T2","Travel","taxi",50)); // valid
    vector<Rule*> rules; // rules list
    rules.push_back(new BanExpenseTypeRule("Entertainment")); // block entertainment
    rules.push_back(new VendorLimitRule("restaurant",45)); // restaurant limit
    rules.push_back(new TotalExpenseRule(175)); // total limit
    rules.push_back(new TripTotalLimitRule(120)); // trip limit
    auto res=evaluateRules(rules,expenses); // run engine
    for(auto &v:res) v.print(); // print violations
    return 0; // exit
}
