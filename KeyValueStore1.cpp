#include<unordered_map>
#include<unordered_set>
#include<iostream>
using namespace std;
class KeyValueStore {
private:
   unordered_map<string, string> store; // main key-value store (persistent data)
   unordered_map<string, string> txstore; // temporary store for updates inside a transaction
   unordered_set<string> deletedKeys; // keys marked for deletion inside transaction
   bool inTransaction = false; // indicates whether we are currently inside a transaction
public:
   // Time Complexity: O(1)
   string get(string key) {
       if(inTransaction) { // if a transaction is active
           if(deletedKeys.count(key)) // if key deleted in transaction, treat as non-existent
               return "";
           if(txstore.count(key)) // if key updated in transaction, return updated value
               return txstore[key];
           if(store.count(key)) // otherwise check original store
               return store[key];
           return ""; // key not found anywhere
       }
       if(store.count(key)) // outside transaction, read directly from store
           return store[key];
       return ""; // key not found
   }


   // Time Complexity: O(1)
   void set(string key, string value) {
       if(inTransaction) { // if inside transaction
           deletedKeys.erase(key); // undo deletion if this key was deleted earlier
           txstore[key] = value; // store updated value in transaction store
       } else {
           store[key] = value; // directly update main store if not in transaction
       }
   }


   // Time Complexity: O(1)
   void deleteKey(string key) {
       if(inTransaction) { // if inside transaction
           txstore.erase(key); // remove any pending updates for this key
           deletedKeys.insert(key); // mark this key as deleted in transaction
       }
       else {
           store.erase(key); // directly remove key from main store
       }
   }


   // Time Complexity: O(1)
   void begin() {
       if(inTransaction) { // nested transactions not supported in this implementation
           cout<<"Already in transaction, commit before starting another";
           return;
       }
       inTransaction = true; // start transaction mode
       txstore.clear(); // clear previous transaction updates
       deletedKeys.clear(); // clear deleted key set
   }
   // Time Complexity: O(N) where N = number of keys modified in transaction
   void commit() {
       if(!inTransaction) { // commit without active transaction
           cout<<"Nothing to commit";
           return;
       }
       for(auto &txentry: txstore) // apply all transaction updates to main store -> O(N)
           store[txentry.first] = txentry.second;
       for(auto &deletedKey: deletedKeys) // remove keys that were deleted in transaction
           store.erase(deletedKey);
       inTransaction = false; // exit transaction mode
       txstore.clear(); // clear temporary transaction updates
       deletedKeys.clear(); // clear deleted key markers
   }
   // Time Complexity: O(1)
   void rollback() {
       if(!inTransaction) { // rollback without active transaction
           cout<<"Not in transaction";
       }
       inTransaction = false; // exit transaction mode
       txstore.clear(); // discard all transaction updates
       deletedKeys.clear(); // discard all transaction deletions
   }
};


int main() {
   KeyValueStore kv;


   cout << "---- Basic SET/GET ----" << endl;


   kv.set("A", "10"); // set key A to value 10
   cout << "A = " << kv.get("A") << endl; // retrieve A


   cout << "\n---- Transaction SET ----" << endl;


   kv.begin(); // start transaction
   kv.set("A", "20"); // update A inside transaction


   cout << "A inside transaction = " << kv.get("A") << endl; // should return 20


   kv.commit(); // commit transaction changes


   cout << "A after commit = " << kv.get("A") << endl; // should persist as 20




   cout << "\n---- Transaction DELETE ----" << endl;


   kv.begin(); // start new transaction
   kv.deleteKey("A"); // delete A inside transaction


   cout << "A after delete in transaction = " << kv.get("A") << endl; // should return empty


   kv.rollback(); // discard transaction


   cout << "A after rollback = " << kv.get("A") << endl; // should restore original value




   cout << "\n---- New Key in Transaction ----" << endl;


   kv.begin(); // begin transaction


   kv.set("B", "50"); // create new key B


   cout << "B inside transaction = " << kv.get("B") << endl; // visible inside transaction


   kv.commit(); // commit changes


   cout << "B after commit = " << kv.get("B") << endl; // should persist




   cout << "\n---- Delete and Commit ----" << endl;


   kv.begin(); // start transaction
   kv.deleteKey("B"); // delete B


   kv.commit(); // commit deletion


   cout << "B after delete commit = " << kv.get("B") << endl; // should be empty




   cout << "\n---- Rollback Test ----" << endl;


   kv.begin(); // begin transaction


   kv.set("C", "100"); // add new key C


   cout << "C inside transaction = " << kv.get("C") << endl; // visible inside transaction


   kv.rollback(); // discard transaction


   cout << "C after rollback = " << kv.get("C") << endl; // should not exist




   return 0;
}
