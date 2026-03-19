#include<iostream>
#include<vector>
#include<unordered_map>
using namespace std;


enum class ViolationType {Expense, Trip};


class Expense {
public:
   string expenseId;
   string tripId;
   string expenseType;
   double amt; // CHANGED: string → double
   string sellerName;
   string sellerType;


   // Time Complexity: O(1)
   Expense(string expenseId, string tripId, string expenseType, double amt, string sellerName, string sellerType) {
       this->expenseId = expenseId;
       this->amt = amt;
       this->expenseType = expenseType;
       this->sellerType = sellerType;
       this->sellerName = sellerName;
       this->tripId = tripId;
   }
};


class Violation {
public:
   string ruleName;
   string expenseId;
   string tripId;
   string message;
   ViolationType violationType;


   // Time Complexity: O(1)
   Violation(string ruleName, string expenseId, string tripId, string message, ViolationType violationType) {
       this->ruleName = ruleName;
       this->expenseId = expenseId;
       this->tripId = tripId;
       this->message = message;
       this->violationType = violationType;
   }
};


class Rule {
public:
   // Time Complexity: O(N) where N = number of expenses
   virtual vector<Violation> evaluate(const vector<Expense>& expenses) = 0;


   // Time Complexity: O(1)
   virtual string getRuleName() = 0;


   virtual ~Rule(){}
};


class ExpenseLimitRule: public Rule {
private:
   double limit;
public:
   // Time Complexity: O(1)
   ExpenseLimitRule(double limit) {
       this->limit = limit;
   }


   // Time Complexity: O(N)
   vector<Violation> evaluate(const vector<Expense>& expenses) override {
       vector<Violation> violations;
       for(auto &expense: expenses) {
           if(expense.amt > limit) {
               violations.push_back(Violation(getRuleName(), expense.expenseId, expense.tripId,
                                             "Expense exceeds limit", ViolationType::Expense));
           }
       }
       return violations;
   }


   // Time Complexity: O(1)
   string getRuleName() override { return "ExpenseLimitRule"; }
};


class ExpenseTypeLimitRule: public Rule {
private:
   string type;
   double limit;
public:
   // Time Complexity: O(1)
   ExpenseTypeLimitRule(string type, double limit) {
       this->type = type;
       this->limit = limit;
   }


   // Time Complexity: O(N)
   vector<Violation> evaluate(const vector<Expense>& expenses) override {
       vector<Violation> violations;
       for(auto &expense: expenses) {
           if(expense.amt > limit && expense.expenseType == type) {
               violations.push_back(Violation(getRuleName(), expense.expenseId, expense.tripId,
                                             "Expense exceeds limit for expense type: " + type,
                                             ViolationType::Expense));
           }
       }
       return violations;
   }


   // Time Complexity: O(1)
   string getRuleName() override { return "ExpenseTypeLimitRule"; }
};


class SellerTypeLimitRule: public Rule {
private:
   string type;
   double limit;
public: 
   // Time Complexity: O(1)
   SellerTypeLimitRule(string type, double limit) {
       this->type = type;
       this->limit = limit;
   }


   // Time Complexity: O(N)
   vector<Violation> evaluate(const vector<Expense>& expenses) override {
       vector<Violation> violations;
       for(auto &expense: expenses) {
           if(expense.amt > limit && expense.sellerType == type) {
               violations.push_back(Violation(getRuleName(), expense.expenseId, expense.tripId,
                                             "Expense exceeds limit for seller type: " + type,
                                             ViolationType::Expense));
           }
       }
       return violations;
   }


   // Time Complexity: O(1)
   string getRuleName() override { return "SellerTypeLimitRule"; }
};


class BannedSellerRule: public Rule {
private:
   string bannedSeller;
public:
   // Time Complexity: O(1)
   BannedSellerRule(string bannedSeller) {
       this->bannedSeller = bannedSeller;
   }


   // Time Complexity: O(N)
   vector<Violation> evaluate(const vector<Expense>& expenses) override {
       vector<Violation> violations;
       for(auto &expense: expenses) {
           if(expense.sellerName == bannedSeller) {
               violations.push_back(Violation(getRuleName(), expense.expenseId, expense.tripId,
                                             "Banned seller: " + bannedSeller,
                                             ViolationType::Expense));
           }
       }
       return violations;
   }


   // Time Complexity: O(1)
   string getRuleName() override { return "BannedSellerRule"; }
};


class TripExpenseLimit: public Rule {
private:
   double limit;
public:
   // Time Complexity: O(1)
   TripExpenseLimit(double limit) {
       this->limit = limit;
   }


   // Time Complexity: O(N)
   vector<Violation> evaluate(const vector<Expense>& expenses) override {
       unordered_map<string, double> tripToAmount;


       for(auto &expense: expenses) // O(N)
           tripToAmount[expense.tripId] += expense.amt;


       vector<Violation> violations;


       for(auto &trip: tripToAmount) { // O(T)
           if(trip.second > limit) {
               violations.push_back(Violation(getRuleName(), "", trip.first,
                                             "Trip expense limit exceeded",
                                             ViolationType::Trip));
           }
       }
       return violations;
   }


   // Time Complexity: O(1)
   string getRuleName() override { return "TripExpenseLimit"; }
};


class TripExpenseTypeLimit: public Rule {
private:
   double limit;
   string type;
public:
   // Time Complexity: O(1)
   TripExpenseTypeLimit(double limit, string type) {
       this->limit = limit;
       this->type = type;
   }


   // Time Complexity: O(N)
   vector<Violation> evaluate(const vector<Expense>& expenses) override {
       unordered_map<string, double> tripToAmount;


       for(auto &expense: expenses) // O(N)
           if(expense.expenseType == type)
               tripToAmount[expense.tripId] += expense.amt;


       vector<Violation> violations;


       for(auto &trip: tripToAmount) { // O(T)
           if(trip.second > limit) {
               violations.push_back(Violation(getRuleName(), "", trip.first,
                                             "Trip expense limit exceeded for type: " + type,
                                             ViolationType::Trip));
           }
       }
       return violations;
   }


   // Time Complexity: O(1)
   string getRuleName() override { return "TripExpenseTypeLimit"; }
};


class RuleEngine {
public:
   // Time Complexity: O(R * N)
   // R = number of rules, N = number of expenses
   vector<Violation> evaluateRules(const vector<Rule*>& rules, const vector<Expense>& expenses) {
       vector<Violation> violations;


       for(auto &rule: rules) { // O(R)
           auto vio = rule->evaluate(expenses); // each is O(N)
           violations.insert(violations.end(), vio.begin(), vio.end()); // O(K)
       }


       return violations;
   }
};


int main() {
   vector<Expense> expenses = {
       {"1","T1","Food",100,"ABC Restaurant","restaurant"},
       {"2","T1","Entertainment",80,"XYZ Club","club"},
       {"3","T1","Travel",50,"Uber","taxi"},
       {"4","T2","Food",60,"ABC Restaurant","restaurant"},
       {"5","T2","Food",30,"Cafe Coffee","restaurant"},
       {"6","T2","Travel",200,"Delta Airlines","airline"}
   };


   vector<Rule*> rules;
   rules.push_back(new ExpenseLimitRule(150));
   rules.push_back(new SellerTypeLimitRule("restaurant",45));
   rules.push_back(new BannedSellerRule("XYZ Club"));
   rules.push_back(new TripExpenseLimit(250));
   rules.push_back(new TripExpenseTypeLimit(120, "Food"));


   RuleEngine re;
   vector<Violation> violations = re.evaluateRules(rules, expenses);


   for(auto &violation: violations) {
       if(violation.violationType == ViolationType::Expense) {
           cout << "Expense " << violation.expenseId << " violates " << violation.ruleName << endl;
           cout << violation.message << endl;
       } else {
           cout << "Trip " << violation.tripId << " violates " << violation.ruleName << endl;
           cout << violation.message << endl;
       }
   }


   return 0;
}

