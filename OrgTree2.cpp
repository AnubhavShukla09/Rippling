#include <iostream>
#include <vector>
using namespace std;


/*
-------------------------------------------------------
BUILD TREE
-------------------------------------------------------
*/
vector<vector<int>> buildTree(int n, vector<int>& managers, vector<int>& reportees) {
   vector<vector<int>> tree(n + 1);                          // adjacency list
   for (int i = 0; i < managers.size(); i++) {
       tree[managers[i]].push_back(reportees[i]);            // manager → reportee
   }
   return tree;
}


/*
-------------------------------------------------------
PART 1: Direct Reportees
Time Complexity: O(1)
-------------------------------------------------------
*/
int getDirectReportees(int manager, vector<vector<int>>& tree) {
   return tree[manager].size();                              // number of direct children
}


/*
-------------------------------------------------------
PART 2: Lowest Common Manager (LCA)
Time Complexity: O(N)
-------------------------------------------------------
*/
int findLCA(int node, int emp1, int emp2, vector<vector<int>>& tree) {


   if (node == emp1 || node == emp2) return node;            // if match found


   int foundCount = 0;                                       // count matches in subtree
   int temp = -1;                                            // store candidate


   for (int child : tree[node]) {
       int res = findLCA(child, emp1, emp2, tree);           // DFS into subtree


       if (res != -1) {                                      // if found in subtree
           foundCount++;
           temp = res;
       }
   }


   if (foundCount >= 2) return node;                         // both found in different branches


   return temp;                                              // propagate found node
}


/*
-------------------------------------------------------
PART 3: Minimum Weight Independent Set (Tree DP)
Time Complexity: O(N)
-------------------------------------------------------
*/


pair<long long, long long> dfsMinWeight(int node,
                                       vector<vector<int>>& tree,
                                       vector<int>& weight) {


   long long include = weight[node];                         // include this node
   long long exclude = 0;                                    // exclude this node


   for (int child : tree[node]) {


       auto childDP = dfsMinWeight(child, tree, weight);     // recurse on child


       include += childDP.first;                             // include → child excluded


       exclude += min(childDP.first, childDP.second);        // exclude → take best of child
   }


   return {exclude, include};                                // return both cases
}


long long getMinWeightSet(vector<vector<int>>& tree, vector<int>& weight) {
   auto res = dfsMinWeight(1, tree, weight);                 // assume CEO = 1
   return min(res.first, res.second);                        // choose optimal
}


/*
-------------------------------------------------------
MAIN FUNCTION
-------------------------------------------------------
*/
int main() {


   int n = 7;


   vector<int> managers = {1, 1, 2, 2, 3, 3};                // manager list
   vector<int> reportees = {2, 3, 4, 5, 6, 7};               // reportee list


   vector<vector<int>> tree = buildTree(n, managers, reportees); // build tree


   // PART 1
   cout << "Direct reportees of 1: "
        << getDirectReportees(1, tree) << endl;


   // PART 2
   cout << "Common manager of 4 and 5: "
        << findLCA(1, 4, 5, tree) << endl;


   // PART 3
   vector<int> weight = {0, 5, 3, 6, 2, 4, 7, 1};            // 1-based indexing


   cout << "Minimum Weight Independent Set: "
        << getMinWeightSet(tree, weight) << endl;


   return 0;
}

