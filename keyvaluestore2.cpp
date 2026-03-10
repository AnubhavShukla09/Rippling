#include<unordered_map>
#include<unordered_set>
#include<vector>
#include<iostream>
using namespace std;


class KeyValueStore {
private:
   unordered_map<string, string> store; // base key-value store
   vector<unordered_map<string,string>> txstores; // stack of transaction stores (each layer stores updates)
   vector<unordered_set<string>> deletedKeysStack; // stack of deleted keys per transaction layer
public:
   // Time Complexity: O(T) where T = number of active transactions
   string get(string key) {
       // search from most recent transaction to oldest
       for(int i = txstores.size()-1; i >= 0; i--) {
           if(deletedKeysStack[i].count(key)) // if key deleted in this transaction layer
               return "";
           if(txstores[i].count(key)) // if key updated in this transaction layer
               return txstores[i][key];
       }
       if(store.count(key)) // fallback to base store
           return store[key];
       return ""; // key does not exist
   }
   // Time Complexity: O(1)
   void set(string key, string value) {
       if(!txstores.empty()) { // if inside a transaction
           deletedKeysStack.back().erase(key); // undo delete if key was deleted earlier in this transaction
           txstores.back()[key] = value; // update key in current transaction layer
       } else {
           store[key] = value; // update directly in base store
       }
   }
   // Time Complexity: O(1)
   void deleteKey(string key) {
       if(!txstores.empty()) { // if inside transaction
           txstores.back().erase(key); // remove from updates in current transaction
           deletedKeysStack.back().insert(key); // mark as deleted in this transaction layer
       } else {
           store.erase(key); // delete directly from base store
       }
   }
   // Time Complexity: O(1)
   void begin() {
       txstores.push_back({}); // push new empty transaction layer
       deletedKeysStack.push_back({}); // push empty deleted-key set for this transaction
   }
   // Time Complexity: O(N) where N = number of modified keys in the current transaction
   void commit() {
       if(txstores.empty()) { // no active transaction
           cout<<"Nothing to commit"<<endl;
           return;
       }
       auto currTx = txstores.back(); // get current transaction updates
       auto currDeleted = deletedKeysStack.back(); // get keys deleted in this transaction
       txstores.pop_back(); // remove current transaction layer
       deletedKeysStack.pop_back(); // remove its deleted-key set
       if(txstores.empty()) { // if this was the outermost transaction
           for(auto &entry : currTx)
               store[entry.first] = entry.second; // apply updates to base store
           for(auto &key : currDeleted)
               store.erase(key); // apply deletions to base store
       } else { // merge changes into parent transaction
           auto &parentTx = txstores.back(); // reference to parent transaction updates
           auto &parentDeleted = deletedKeysStack.back(); // reference to parent deleted keys
           for(auto &entry : currTx) {
               parentDeleted.erase(entry.first); // undo delete in parent if needed
               parentTx[entry.first] = entry.second; // propagate update to parent transaction
           }
           for(auto &key : currDeleted) {
               parentTx.erase(key); // remove key from parent updates
               parentDeleted.insert(key); // mark key deleted in parent transaction
           }
       }
   }
   // Time Complexity: O(1)
   void rollback() {
       if(txstores.empty()) { // no active transaction
           cout<<"Not in transaction"<<endl;
           return;
       }
       txstores.pop_back(); // discard current transaction updates
       deletedKeysStack.pop_back(); // discard its deleted keys
   }
};

