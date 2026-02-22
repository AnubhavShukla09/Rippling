#include <iostream>        // cout, endl
#include <string>          // string
#include <unordered_map>   // unordered_map
#include <unordered_set>   // unordered_set
#include <vector>          // vector
using namespace std; // using standard namespace
class KeyValueStore{ // class definition
private:
    unordered_map<string,string> store; // ================= PART 1: main storage =================
public:
    string get(string key){ // returns value for a key
        if(store.find(key)!=store.end()) return store[key]; // if key exists return value
        return ""; // return empty string if not found
    }
    void set(string key,string value){ // sets value for key
        store[key]=value; // insert or update key
    }
    void deleteKey(string key){ // deletes a key
        store.erase(key); // remove key if exists
    }
};
int main(){ // Time Complexity: get/set/delete O(1) average
    KeyValueStore kv; // create object
    kv.set("a","10"); // set key a
    cout<<kv.get("a")<<endl; // print value
    kv.deleteKey("a"); // delete key
    cout<<kv.get("a")<<endl; // print after deletion
    return 0; // end program
}
//===================================================2nd part==================================================
#include <bits/stdc++.h> // includes all standard headers
using namespace std; // using standard namespace

class KeyValueStore{ // class definition
private:
    unordered_map<string,string> store; // ================= PART 1: main storage =================
    // ================= PART 2: single transaction =================
    unordered_map<string,string> transactionStore; // transaction update layer
    unordered_set<string> deletedKeys; // track deleted keys in transaction
    bool inTransaction=false; // flag for active transaction   
public:
    string get(string key){ // returns value for a key
        // ================= PART 2=================================
        if(inTransaction){ // if transaction active
            if(deletedKeys.count(key)) return ""; // if deleted inside transaction return empty
            if(transactionStore.find(key)!=transactionStore.end()) return transactionStore[key]; // check transaction layer first
        }
        if(store.find(key)!=store.end()) return store[key]; // fallback to main store
        return ""; // if not found
    }
    void set(string key,string value){ // sets value
        // ================= PART 2=================================
        if(inTransaction){ // if inside transaction
            transactionStore[key]=value; // update transaction layer
            deletedKeys.erase(key); // remove from delete set if exists
        }else{
            store[key]=value; // update main store
        }
    }
    void deleteKey(string key){ // deletes key
        // ================= PART 2=================================
        if(inTransaction){ // if inside transaction
            transactionStore.erase(key); // remove from transaction updates
            deletedKeys.insert(key); // mark as deleted
        }else{
            store.erase(key); // remove from main store
        }
    }
    void begin(){ // start transaction
        inTransaction=true; // activate transaction
        transactionStore.clear(); // clear previous transaction data
        deletedKeys.clear(); // clear deleted keys
    }
    void commit(){ // commit transaction
        if(!inTransaction) return; // if no active transaction do nothing
        for(auto &p:transactionStore) store[p.first]=p.second; // merge updates
        for(auto &key:deletedKeys) store.erase(key); // apply deletions
        inTransaction=false; // deactivate transaction
        transactionStore.clear(); // cleanup
        deletedKeys.clear(); // cleanup
    }
    void rollback(){ // rollback transaction
        if(!inTransaction) return; // if no active transaction do nothing
        inTransaction=false; // discard transaction
        transactionStore.clear(); // discard updates
        deletedKeys.clear(); // discard deletes
    }
};
int main(){ // Time Complexity: get/set/delete O(1) avg, commit O(n)
    KeyValueStore kv; // create object
    kv.set("a","10"); // base value
    kv.begin(); // start transaction
    kv.set("a","20"); // update in transaction
    cout<<kv.get("a")<<endl; // should print 20
    kv.rollback(); // rollback
    cout<<kv.get("a")<<endl; // should print 10
    return 0; // end program
}
//===================================================================3rd part================================================================================
#include <bits/stdc++.h> // includes all standard headers
using namespace std; // using standard namespace
class KeyValueStore{ // class definition
private:
    unordered_map<string,string> store; // ================= PART 1: main committed storage =================
    // ================= PART 2 + PART 3: nested transactions =================
    vector<unordered_map<string,string>> txnStack; // stack of transaction update layers
    vector<unordered_set<string>> deleteStack; // stack of delete layers
public:
    string get(string key){ // returns value for key
        for(int i=txnStack.size()-1;i>=0;i--){ // traverse from most recent transaction
            if(deleteStack[i].count(key)) return ""; // if deleted in this layer
            if(txnStack[i].find(key)!=txnStack[i].end()) return txnStack[i][key]; // if found in this layer
        }
        if(store.find(key)!=store.end()) return store[key]; // fallback to main store
        return ""; // if not found
    }
    void set(string key,string value){ // sets value
        if(!txnStack.empty()){ // if inside transaction
            txnStack.back()[key]=value; // update top transaction layer
            deleteStack.back().erase(key); // remove from delete set if exists
        }else{
            store[key]=value; // update main store
        }
    }
    void deleteKey(string key){ // deletes key
        if(!txnStack.empty()){ // if inside transaction
            txnStack.back().erase(key); // remove from update layer
            deleteStack.back().insert(key); // mark deleted
        }else{
            store.erase(key); // remove from main store
        }
    }
    void begin(){ // start new transaction layer
        txnStack.push_back(unordered_map<string,string>()); // push new update layer
        deleteStack.push_back(unordered_set<string>()); // push new delete layer
    }
    void commit(){ // commit top transaction
        if(txnStack.empty()) return; // if no active transaction
        auto &currentMap=txnStack.back(); // reference to top update layer
        auto &currentDelete=deleteStack.back(); // reference to top delete layer
        txnStack.pop_back(); // remove top update layer
        deleteStack.pop_back(); // remove top delete layer
        if(txnStack.empty()){ // if committing to main store
            for(auto &p:currentMap) store[p.first]=p.second; // merge updates
            for(auto &key:currentDelete) store.erase(key); // apply deletions
        }else{
            for(auto &p:currentMap) txnStack.back()[p.first]=p.second; // merge into parent transaction
            for(auto &key:currentDelete){ // propagate deletions
                txnStack.back().erase(key); // remove from parent updates
                deleteStack.back().insert(key); // mark deleted in parent
            }
        }
    }
    void rollback(){ // rollback top transaction
        if(txnStack.empty()) return; // if no active transaction
        txnStack.pop_back(); // discard update layer
        deleteStack.pop_back(); // discard delete layer
    }
};

int main(){ // Time Complexity: get O(depth), set/delete O(1), begin/rollback O(1), commit O(n)
    KeyValueStore kv; // create object
    kv.set("a","1"); // base value
    kv.begin(); // txn1
    kv.set("a","2"); // update in txn1
    kv.begin(); // txn2
    kv.set("a","3"); // update in txn2
    cout<<kv.get("a")<<endl; // should print 3
    kv.rollback(); // rollback txn2
    cout<<kv.get("a")<<endl; // should print 2
    kv.commit(); // commit txn1
    cout<<kv.get("a")<<endl; // should print 2
    return 0; // end program
}
