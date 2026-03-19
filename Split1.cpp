#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;


class Solution {
public:


   /*
   Time Complexity: O(T + N log N)
   T = number of transactions
   N = number of people with non-zero balance


   Note:
   - This is a greedy approach
   - Does NOT guarantee minimum number of transactions
   */
   int minTransfers(vector<vector<int>>& transactions) {


       unordered_map<int, int> netBalance;


       // Step 1: Compute net balances
       for (auto& txn : transactions) {
           int from = txn[0];
           int to = txn[1];
           int amount = txn[2];


           netBalance[from] -= amount;
           netBalance[to] += amount;
       }


       // Max heap for creditors (positive balances)
       priority_queue<int> creditors;


       // Max heap for debtors (store absolute values)
       priority_queue<int> debtors;


       // Step 2: Separate balances
       for (auto& entry : netBalance) {
           int balance = entry.second;


           if (balance > 0) {
               creditors.push(balance);
           }
           else if (balance < 0) {
               debtors.push(-balance); // store positive value
           }
       }


       int transactionsCount = 0;


       // Step 3: Greedy settlement
       while (!creditors.empty() && !debtors.empty()) {


           int credit = creditors.top(); creditors.pop();
           int debt = debtors.top(); debtors.pop();


           int settledAmount = min(credit, debt);


           credit -= settledAmount;
           debt -= settledAmount;


           transactionsCount++;


           // Push remaining balances back
           if (credit > 0) creditors.push(credit);
           if (debt > 0) debtors.push(debt);
       }


       return transactionsCount;
   }
};

