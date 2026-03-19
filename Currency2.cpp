#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
using namespace std;


class CurrencyConverter {
public:
   /*
   Time Complexity: O(E log V)
   Space Complexity: O(V + E)
   V = number of currencies
   E = number of conversion rates
   */
   double getMaxConversionRate(
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
       // Edge case
       if (!graph.count(from) || !graph.count(to)) return -1.0;
       // Step 2: Max Heap (rate, currency)
       priority_queue<pair<double, string>> pq;
       unordered_map<string, double> bestRate;
       pq.push({1.0, from});
       bestRate[from] = 1.0;
       while (!pq.empty()) {
           auto [currRate, currCurrency] = pq.top();
           pq.pop();
           // If reached target → best possible due to max heap
           if (currCurrency == to) return currRate;
           // Skip outdated entries
           if (currRate < bestRate[currCurrency]) continue;
           for (auto& neighbor : graph[currCurrency]) {
               string nextCurrency = neighbor.first;
               double rate = neighbor.second;
               double newRate = currRate * rate;
               // Relaxation step (maximize instead of minimize)
               if (!bestRate.count(nextCurrency) || newRate > bestRate[nextCurrency]) {
                   bestRate[nextCurrency] = newRate;
                   pq.push({newRate, nextCurrency});
               }
           }
       }
       return -1.0;
   }
};

