#include <vector>
#include <unordered_map>
#include <climits>
using namespace std;
class Solution {
private:
   /*
   Time Complexity: O(N!)
   N = number of people with non-zero balance
   Explanation:
   - At each step, we try settling current debt with all possible opposite balances
   - In worst case → factorial branching
   */
   int settleDebts(int currentIndex, vector<int>& balances) {
       // Skip settled (zero) balances
       while (currentIndex < balances.size() && balances[currentIndex] == 0) {
           currentIndex++;
       }
       // All debts settled
       if (currentIndex == balances.size()) return 0;
       int minTransactions = INT_MAX;
       // Try settling current person with others
       for (int next = currentIndex + 1; next < balances.size(); next++) {
           // Only settle with opposite sign (creditor vs debtor)
           if (balances[currentIndex] * balances[next] < 0) {
               // Settle currentIndex with next
               balances[next] += balances[currentIndex];
               // Recurse for remaining
               int transactions = 1 + settleDebts(currentIndex + 1, balances);
               minTransactions = min(minTransactions, transactions);
               // Backtrack
               balances[next] -= balances[currentIndex];
               // Optimization:
               // If perfect settlement (becomes zero), no need to try further
               if (balances[next] + balances[currentIndex] == 0) break;
           }
       }
       return minTransactions;
   }
public:
   /*
   Time Complexity: O(T + N!)
   T = number of transactions
   N = number of unique people with non-zero balance
   Steps:
   1. Compute net balance → O(T)
   2. Filter non-zero balances → O(N)
   3. DFS/backtracking → O(N!)
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
       // Step 2: Keep only non-zero balances
       vector<int> balances;
       for (auto& entry : netBalance) {
           if (entry.second != 0) {
               balances.push_back(entry.second);
           }
       }
       // Step 3: Backtracking to minimize transactions
       return settleDebts(0, balances);
   }
};

