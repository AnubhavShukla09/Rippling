#include<unordered_map>
#include<unordered_set>
#include<iostream>
#include<optional>
using namespace std;
class KeyValueStore {
private:
   unordered_map<string, string> store; // main key-value store (persistent data)
   unordered_map<string, string> txstore; // temporary store for updates inside a transaction
   unordered_set<string> deletedKeys; // keys marked for deletion inside transaction
   bool inTransaction = false; // indicates whether we are currently inside a transaction
public:
   // Time Complexity: O(1)
   optional<string> get(string key) {
       if(inTransaction) { // if a transaction is active
           if(deletedKeys.count(key)) // if key deleted in transaction, treat as non-existent
               return nullopt;
           if(txstore.count(key)) // if key updated in transaction, return updated value
               return txstore[key];
           if(store.count(key)) // outside transaction, read directly from store
               return store[key];
           return nullopt; // key not found
       }
       if(store.count(key)) // outside transaction, read directly from store
           return store[key];
       return nullopt; // key not found
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
           return;
       }
       inTransaction = false; // exit transaction mode
       txstore.clear(); // discard all transaction updates
       deletedKeys.clear(); // discard all transaction deletions
   }
};


int main() {
   KeyValueStore kv;


   // helper lambda to print optional
   auto print = [](optional<string> val) {
       if(val) cout << *val << endl;
       else cout << "NULL" << endl;
   };


   cout << "---- Basic SET/GET ----" << endl;
   kv.set("A", "10");


   cout << "A = ";
   print(kv.get("A")); // expect 10


   cout << "\n---- Transaction SET ----" << endl;
   kv.begin();
   kv.set("A", "20");


   cout << "A inside transaction = ";
   print(kv.get("A")); // expect 20


   kv.commit();


   cout << "A after commit = ";
   print(kv.get("A")); // expect 20


   cout << "\n---- Transaction DELETE + ROLLBACK ----" << endl;
   kv.begin();
   kv.deleteKey("A");


   cout << "A after delete (txn) = ";
   print(kv.get("A")); // expect NULL


   kv.rollback();


   cout << "A after rollback = ";
   print(kv.get("A")); // expect 20


   cout << "\n---- New Key in Transaction ----" << endl;
   kv.begin();
   kv.set("B", "50");


   cout << "B inside txn = ";
   print(kv.get("B")); // expect 50


   kv.commit();


   cout << "B after commit = ";
   print(kv.get("B")); // expect 50


   cout << "\n---- Delete and Commit ----" << endl;
   kv.begin();
   kv.deleteKey("B");
   kv.commit();


   cout << "B after delete commit = ";
   print(kv.get("B")); // expect NULL


   cout << "\n---- Non-existent Key ----" << endl;
   cout << "C = ";
   print(kv.get("C")); // expect NULL


   return 0;
}

