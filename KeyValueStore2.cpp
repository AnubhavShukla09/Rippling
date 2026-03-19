#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <optional>
using namespace std;
class KeyValueStore {
private:
   unordered_map<string, string> store; // main persistent store
   vector<unordered_map<string, string>> txStores; // stack of transaction updates
   vector<unordered_set<string>> deletedKeys; // stack of transaction deletions
public:
   // Time Complexity: O(T) where T = number of transactions
   optional<string> get(const string& key) {
       // traverse from latest transaction → oldest
       for(int i = txStores.size() - 1; i >= 0; i--) {
           if(deletedKeys[i].count(key)) // if deleted in this layer
               return nullopt;
           auto it = txStores[i].find(key); // check updates in this layer
           if(it != txStores[i].end())
               return it->second;
       }
       if(store.count(key)) // fallback to base store
           return store[key];
       return nullopt;
   }
   // Time Complexity: O(1)
   void set(const string& key, const string& value) {
       if(!txStores.empty()) { // inside transaction
           int top = txStores.size() - 1;
           deletedKeys[top].erase(key); // undo delete
           txStores[top][key] = value; // update in current layer
       } else {
           store[key] = value; // directly update base store
       }
   }
   // Time Complexity: O(1)
   void deleteKey(const string& key) {
       if(!txStores.empty()) {
           int top = txStores.size() - 1;
           txStores[top].erase(key); // remove pending update
           deletedKeys[top].insert(key); // mark deleted
       } else {
           store.erase(key);
       }
   }
   // Time Complexity: O(1)
   void begin() {
       txStores.push_back({}); // push new transaction layer
       deletedKeys.push_back({});
   }
   // Time Complexity: O(N)
   void commit() {
       if(txStores.empty()) {
           cout << "Nothing to commit\n";
           return;
       }
       int top = txStores.size() - 1;
       if(txStores.size() == 1) {
           // commit to main store
           for(auto &entry : txStores[top])
               store[entry.first] = entry.second;
           for(auto &key : deletedKeys[top])
               store.erase(key);
       } else {
           // merge into parent layer
           int parent = top - 1;
           for(auto &entry : txStores[top]) {
               deletedKeys[parent].erase(entry.first); // undo delete in parent
               txStores[parent][entry.first] = entry.second; // update parent
           }
           for(auto &key : deletedKeys[top]) {
               txStores[parent].erase(key); // remove pending update
               deletedKeys[parent].insert(key); // mark deleted in parent
           }
       }
       txStores.pop_back(); // remove current layer
       deletedKeys.pop_back();
   }
   // Time Complexity: O(1)
   void rollback() {
       if(txStores.empty()) {
           cout << "Nothing to rollback\n";
           return;
       }
       txStores.pop_back(); // discard current layer
       deletedKeys.pop_back();
   }
};

