#include <iostream>     // for cout, endl
#include <vector>       // for vector
#include <string>       // for string
#include <unordered_map>// for unordered_map
#include <queue>        // for priority_queue
#include <tuple>        // for tuple, get<>
#include <algorithm>    // for min()
using namespace std; // use std namespace
//===================================================== PART 1 : TRANSACTION MODEL ===============================================
// class representing a single transaction <from, to, amount>
class Transaction {
public:
    string from; // payer
    string to; // receiver
    long long amount; // amount transferred
    Transaction(string f, string t, long long a) { // constructor
        from = f; // assign payer
        to = t; // assign receiver
        amount = a; // assign amount
    }
};
//===================================================== PART 1 : SETTLEMENT ENGINE ===============================================
class SettlementService {
private:
    // computes net balance of each person
    // positive -> should receive money
    // negative -> should pay money
    // Time: O(T)
    unordered_map<string, long long> computeNetBalance(vector<Transaction>& transactions) {
        unordered_map<string, long long> balance; // person -> net balance
        for (auto &t : transactions) { // process each transaction
            balance[t.from] -= t.amount; // payer loses money
            balance[t.to] += t.amount; // receiver gains money
        }
        return balance; // return net balances
    }
public:
//===================================================== PART 1 : BASIC SETTLEMENT (NOT OPTIMAL) =================================
// settle balances in simple way (not minimum transactions)
// Time: O(N)
    vector<tuple<string,string,long long>> settleBasic(vector<Transaction>& transactions) {
        auto balance = computeNetBalance(transactions); // compute net balances
        vector<pair<string,long long>> debtors; // people who need to pay
        vector<pair<string,long long>> creditors; // people who should receive
        // separate debtors and creditors
        for (auto &p : balance) {
            if (p.second < 0) debtors.push_back({p.first, -p.second}); // store positive debt
            else if (p.second > 0) creditors.push_back({p.first, p.second}); // store credit
        }
        vector<tuple<string,string,long long>> result; // settlement result
        int i = 0, j = 0; // pointers
        // match debtors with creditors sequentially
        while (i < debtors.size() && j < creditors.size()) {
            long long amount = min(debtors[i].second, creditors[j].second); // settle minimum
            result.push_back({debtors[i].first, creditors[j].first, amount}); // record payment
            debtors[i].second -= amount; // reduce debt
            creditors[j].second -= amount; // reduce credit
            if (debtors[i].second == 0) i++; // move debtor pointer
            if (creditors[j].second == 0) j++; // move creditor pointer
        }
        return result; // return settlements
    }
//===================================================== PART 2 : OPTIMAL SETTLEMENT (GREEDY) ====================================
// minimizes number of transactions using greedy approach
// Time: O(N log N)
    vector<tuple<string,string,long long>> settleOptimal(vector<Transaction>& transactions) {
        auto balance = computeNetBalance(transactions); // compute net balance
        // max heap for creditors (max positive balance)
        priority_queue<pair<long long,string>> maxCredit;
        // max heap for debtors (max debt)
        priority_queue<pair<long long,string>> maxDebit;
        // populate heaps
        for (auto &p : balance) {
            if (p.second > 0) maxCredit.push({p.second, p.first}); // creditor
            else if (p.second < 0) maxDebit.push({-p.second, p.first}); // debtor
        }
        vector<tuple<string,string,long long>> result; // settlement result
        // match highest debtor with highest creditor
        while (!maxCredit.empty() && !maxDebit.empty()) {
            auto [credit, creditor] = maxCredit.top(); // get max creditor
            auto [debit, debtor] = maxDebit.top(); // get max debtor
            maxCredit.pop(); // remove
            maxDebit.pop(); // remove
            long long settleAmount = min(credit, debit); // settle max possible
            result.push_back({debtor, creditor, settleAmount}); // record transaction
            if (credit > settleAmount) // remaining credit
                maxCredit.push({credit - settleAmount, creditor});
            if (debit > settleAmount) // remaining debt
                maxDebit.push({debit - settleAmount, debtor});
        }
        return result; // return optimized settlements
    }
};
//===================================================== DEMO =====================================================================
int main() { // program entry point
    vector<Transaction> transactions; // transaction list
    transactions.push_back(Transaction("A","B",100)); // A paid B
    transactions.push_back(Transaction("B","C",50)); // B paid C
    transactions.push_back(Transaction("C","A",30)); // C paid A
    SettlementService service; // create service
    //=========================== PART 1 DEMO =====================================
    cout << "----- Basic Settlement -----\n"; // header
    auto basic = service.settleBasic(transactions); // compute basic settlement
    for (auto &t : basic) // print
        cout << get<0>(t) << " pays " << get<1>(t) << " : " << get<2>(t) << "\n";
    //=========================== PART 2 DEMO =====================================
    cout << "\n----- Optimal Settlement -----\n"; // header
    auto optimal = service.settleOptimal(transactions); // compute optimal settlement
    for (auto &t : optimal) // print
        cout << get<0>(t) << " pays " << get<1>(t) << " : " << get<2>(t) << "\n";
    return 0; // exit
}
