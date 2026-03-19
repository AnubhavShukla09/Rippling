#include <iostream>
#include <vector>
#include <queue>
using namespace std;
/*
-------------------------------------------------------
PART 1: Find height of org tree
Time Complexity: O(N)
Space Complexity: O(N)
-------------------------------------------------------
*/
int getOrgHeight(int n, vector<int>& managers, vector<int>& reportees) {
   vector<vector<int>> tree(n + 1);                     // adjacency list for tree
   for (int i = 0; i < managers.size(); i++) {
       tree[managers[i]].push_back(reportees[i]);       // build tree: manager -> reportee
   }
   queue<pair<int, int>> q;                             // {node, level}
   q.push({1, 1});                                      // CEO = 1 at level 1
   int maxHeight = 0;                                   // store max depth
   while (!q.empty()) {
       auto curr = q.front(); q.pop();                  // pop current node
       int node = curr.first;                           // current employee
       int level = curr.second;                         // current level
       maxHeight = max(maxHeight, level);               // update max height
       for (int child : tree[node]) {
           q.push({child, level + 1});                  // push children with incremented level
       }
   }
   return maxHeight;                                    // return final height
}
/*
-------------------------------------------------------
PART 2: Minimize CEO direct reports while keeping height ≤ h
Time Complexity: O(N)
Space Complexity: O(N)
-------------------------------------------------------
*/
int dfs(int node, vector<vector<int>>& tree, int h, int& cuts) {
   int maxChildHeight = 0;                              // track max height among children
   for (int child : tree[node]) {
       int childHeight = dfs(child, tree, h, cuts);     // recursively compute child height
       maxChildHeight = max(maxChildHeight, childHeight); // update max height
   }
   if (maxChildHeight == h - 1) {                       // if adding this node makes height = h
       cuts++;                                          // we cut here (attach to CEO)
       return 0;                                        // reset height since detached subtree
   }
   return maxChildHeight + 1;                           // return height including current node
}
int minimizeCEOReports(int n, vector<int>& managers, vector<int>& reportees, int h) {
   vector<vector<int>> tree(n + 1);                     // adjacency list
   for (int i = 0; i < managers.size(); i++) {
       tree[managers[i]].push_back(reportees[i]);       // build tree
   }
   int cuts = 0;                                        // number of nodes reattached to CEO
   dfs(1, tree, h, cuts);                               // run DFS from CEO
   return cuts;                                         // minimum extra direct reports to CEO
}
/*
-------------------------------------------------------
MAIN FUNCTION (TESTING)
-------------------------------------------------------
*/
int main() {


   int n = 7;


   vector<int> managers = {1, 1, 2, 2, 3, 3};            // managers
   vector<int> reportees = {2, 3, 4, 5, 6, 7};           // reportees


   cout << "Org Height: "
        << getOrgHeight(n, managers, reportees) << endl; // Part 1


   int h = 2;


   cout << "Minimum nodes reporting to CEO after restructuring: "
        << minimizeCEOReports(n, managers, reportees, h) << endl; // Part 2


   return 0;
}

