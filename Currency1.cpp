#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <unordered_set>
using namespace std;


class CurrencyConverter {
public:
   /*
   Time Complexity:
   - BFS traversal: O(V + E)
   => Overall: O(V + E)
   Space Complexity:
   - Graph storage: O(V + E)
   => Overall: O(V + E)
   where:
   V = number of currencies (nodes)
   E = number of conversion rates (edges)
   */
   double getConversionRate(
       vector<vector<string>>& rates,
       string from,
       string to
   ) {
       // Step 1: Build graph
       unordered_map<string, vector<pair<string, double>>> graph;
       for (auto& rate : rates) {
           string src = rate[0];
           string dest = rate[1];
           double value = stod(rate[2]);
           graph[src].push_back({dest, value});
           graph[dest].push_back({src, 1.0 / value});
       }
       // Edge case: currency not present
       if (!graph.count(from) || !graph.count(to)) return -1.0;
       // Step 2: BFS
       queue<pair<string, double>> q;   // {currency, accumulated rate}
       unordered_set<string> visited;
       q.push({from, 1.0});
       visited.insert(from);
       while (!q.empty()) {
           auto curr = q.front();
           q.pop();
           string currCurrency = curr.first;
           double currRate = curr.second;
           // Found target
           if (currCurrency == to) {
               return currRate;
           }
           // Explore neighbors
           for (auto& neighbor : graph[currCurrency]) {
               string nextCurrency = neighbor.first;
               double rate = neighbor.second;
               if (!visited.count(nextCurrency)) {
                   visited.insert(nextCurrency);
                   q.push({nextCurrency, currRate * rate});
               }
           }
       }
       // No valid conversion path
       return -1.0;
   }
};

