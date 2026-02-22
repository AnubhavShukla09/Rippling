//==============================================PART 1=============================================
#include <iostream>      // cout, endl
#include <vector>        // vector
#include <queue>         // queue (BFS)
#include <algorithm>     // max, sort
#include <numeric>       // iota
using namespace std; // use std namespace
// ========================== EMPLOYEE ENTITY ==========================
class Employee{
private:
    int id; // unique employee id
    vector<int> children; // list of direct reportees (adjacency list)
public:
    Employee(int empId){ id = empId; } // constructor assigns id // Time: O(1)
    int getId() const{ return id; } // getter for id // Time: O(1)
    vector<int>& getChildren(){ return children; } // getter for children reference // Time: O(1)
    void addChild(int childId){ children.push_back(childId); } // add reportee // Time: O(1)
};
// ========================== ORG HIERARCHY SERVICE ==========================
class OrgHierarchyService{
private:
    int n; // number of employees
    vector<Employee*> employees; // employee storage (1-indexed)
    vector<int> level; // stores reporting layer of each employee
public:
    OrgHierarchyService(int size){ // constructor initializes employees // Time: O(n)
        n = size; // assign number of employees
        employees.resize(n+1); // allocate space (1-indexed)
        level.resize(n+1,0); // initialize all levels to 0
        for(int i=1;i<=n;i++) employees[i] = new Employee(i); // create employee objects
    }
    void buildHierarchy(vector<int>& manager, vector<int>& reportee){ // builds tree // Time: O(n)
        int m = manager.size(); // number of relations
        for(int i=0;i<m;i++){ // iterate all relations
            int parent = manager[i]; // manager node
            int child = reportee[i]; // reportee node
            employees[parent]->addChild(child); // add edge parent -> child
        }
    }
    // ------------------ PART 1 : COMPUTE TREE HEIGHT ------------------
    int computeHeight(){ // BFS to compute max reporting layer // Time: O(n)
        queue<int> q; // queue for BFS traversal
        q.push(1); // start from CEO
        level[1] = 1; // CEO has level 1
        int maxLevel = 1; // track maximum level found
        while(!q.empty()){ // process nodes level by level
            int curr = q.front(); // get current employee
            q.pop(); // remove from queue
            for(int child : employees[curr]->getChildren()){ // visit all direct reportees
                level[child] = level[curr] + 1; // assign child level
                maxLevel = max(maxLevel, level[child]); // update maximum level
                q.push(child); // push child into queue
            }
        }
        return maxLevel; // return tree height
    }
};
// ========================== MAIN ==========================
int main(){ // driver code // Time: O(n)
    int n = 7; // total employees
    vector<int> manager = {1,1,2,2,4,4}; // manager array
    vector<int> reportee = {2,3,4,5,6,7}; // reportee array
    OrgHierarchyService service(n); // create service object
    service.buildHierarchy(manager, reportee); // build org tree
    int height = service.computeHeight(); // compute tree height
    cout<<"Original Height: "<<height<<endl; // print result
    return 0; // program ends
}

//=================================================PART 2=================================================
#include <iostream>      // cout, endl
#include <vector>        // vector
#include <queue>         // queue (BFS)
#include <algorithm>     // max, sort
#include <numeric>       // iota
using namespace std; // use std namespace
// ========================== EMPLOYEE ENTITY ==========================
class Employee{
private:
    int id; // unique employee id
    vector<int> children; // list of direct reportees (adjacency list)
public:
    Employee(int empId){ id = empId; } // constructor assigns id // Time: O(1)
    int getId() const{ return id; } // getter for id // Time: O(1)
    vector<int>& getChildren(){ return children; } // getter for children reference // Time: O(1)
    void addChild(int childId){ children.push_back(childId); } // add reportee // Time: O(1)
};
// ========================== ORG HIERARCHY SERVICE ==========================
class OrgHierarchyService{
private:
    int n; // number of employees
    vector<Employee*> employees; // employee storage (1-indexed)
    vector<int> level; // stores reporting layer of each employee
    // ------------------ PART 2 ADDITIONS ------------------
    vector<int> parent; // parent of each employee (for upward traversal)
    vector<bool> removed; // marks nodes whose subtree already moved
    int moveCount; // count of subtree moves
// ------------------PART 2 HELPER : MARK SUBTREE AS REMOVED ------------------
    void markSubtree(int node){ // marks whole subtree handled // Time: O(size of subtree)
        removed[node] = true; // mark current node

        for(int child : employees[node]->getChildren()) // visit children
            if(!removed[child]) markSubtree(child); // recursively mark
    }
public:
    OrgHierarchyService(int size){ // constructor initializes employees // Time: O(n)
        n = size; // assign number of employees
        employees.resize(n+1); // allocate space (1-indexed)
        level.resize(n+1,0); // initialize all levels
        parent.resize(n+1,0); // initialize parent array
        removed.resize(n+1,false); // initialize removed flags
        for(int i=1;i<=n;i++) employees[i] = new Employee(i); // create employee objects
    }
    void buildHierarchy(vector<int>& manager, vector<int>& reportee){ // builds tree // Time: O(n)
        int m = manager.size(); // number of relations
        for(int i=0;i<m;i++){ // iterate all relations
            int p = manager[i]; // manager node
            int c = reportee[i]; // reportee node
            employees[p]->addChild(c); // add edge parent -> child
            parent[c] = p; // store parent pointer (needed for Part 2)
        }
        parent[1] = 0; // CEO has no parent
    }
    // ------------------ PART 1 : COMPUTE TREE HEIGHT ------------------
    int computeHeight(){ // BFS to compute max reporting layer // Time: O(n)
        queue<int> q; // queue for BFS traversal
        q.push(1); // start from CEO
        level[1] = 1; // CEO has level 1
        int maxLevel = 1; // track maximum level found
        while(!q.empty()){ // process nodes level by level
            int curr = q.front(); // get current employee
            q.pop(); // remove from queue
            for(int child : employees[curr]->getChildren()){ // visit all direct reportees
                level[child] = level[curr] + 1; // assign child level
                maxLevel = max(maxLevel, level[child]); // update maximum level
                q.push(child); // push child into queue
            }
        }
        return maxLevel; // return tree height
    }
// ------------------ PART 2 : MINIMUM MOVES (GREEDY) ------------------
    int minimumMoves(int h){ // greedy solution // Time: O(n log n)
        moveCount = 0; // reset move counter
        vector<int> nodes(n); // store all employee ids
        for(int i = 0; i < n; i++) {
          nodes[i] = i+1;
        }
        // process deepest employees first
        sort(nodes.begin(), nodes.end(),
            [&](int a,int b){ return level[a] > level[b]; }); // sort by level descending
        for(int node : nodes){ // iterate nodes
            if(removed[node]) continue; // skip if subtree already moved
            if(level[node] <= h) break; // remaining nodes already valid
            int curr = node; // start from violating node
            // move up to ancestor at level h
            while(level[curr] > h) curr = parent[curr]; // climb to level h
            moveCount++; // perform move operation
            markSubtree(curr); // mark entire subtree fixed
        }
        return moveCount; // return minimum moves
    }
};
// ========================== MAIN ==========================
int main(){ // driver code // Time: O(n log n)
    int n = 7; // total employees
    vector<int> manager = {1,1,2,2,4,4}; // manager array
    vector<int> reportee = {2,3,4,5,6,7}; // reportee array
    int h = 3; // allowed maximum reporting layer
    OrgHierarchyService service(n); // create service object
    service.buildHierarchy(manager, reportee); // build org tree
    // ---------- PART 1 ----------
    int height = service.computeHeight(); // compute tree height
    cout<<"Original Height: "<<height<<endl; // print result
    // ---------- PART 2 ----------
    int moves = service.minimumMoves(h); // compute minimum moves
    cout<<"Minimum Moves Required: "<<moves<<endl; // print result
    return 0; // program ends
}
