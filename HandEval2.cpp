#include<vector>
#include<unordered_map>
#include<iostream>
#include<algorithm>
using namespace std;


// -------------------- Helper --------------------
unordered_map<int, int> getFreq(const vector<int>& hand) {
   unordered_map<int, int> freq;                    // map: card → frequency
   for (int c : hand) freq[c]++;                    // count occurrences
   return freq;                                     // return frequency map
}


// -------------------- Interface --------------------
class HandType {
public:
   string type_id;                                  // unique id for type


   HandType(string id) : type_id(id) {}             // constructor


   virtual bool matches(const vector<int>& hand) = 0;   // check if hand matches this type


   virtual vector<int> getKey(const vector<int>& hand) = 0; // tie-break key


   virtual ~HandType() {}                           // virtual destructor
};


// -------------------- Concrete Hand Types --------------------


class FourOfAKind : public HandType {
public:
   FourOfAKind() : HandType("FOUR") {}              // type id = FOUR


   bool matches(const vector<int>& hand) override {
       auto freq = getFreq(hand);                   // get frequencies
       for (auto &p : freq) {
           if (p.second == 4) return true;          // check if any card appears 4 times
       }
       return false;                                // otherwise false
   }


   vector<int> getKey(const vector<int>& hand) override {
       auto freq = getFreq(hand);                   // get frequencies
       int four = 0, kicker = 0;                    // variables for key


       for (auto &p : freq) {
           if (p.second == 4) four = p.first;       // value of four cards
           else kicker = p.first;                   // remaining card
       }


       return {four, kicker};                       // key: [four, kicker]
   }
};


class ThreeOfAKind : public HandType {
public:
   ThreeOfAKind() : HandType("THREE") {}            // type id = THREE


   bool matches(const vector<int>& hand) override {
       auto freq = getFreq(hand);                   // get frequencies
       for (auto &p : freq) {
           if (p.second == 3) return true;          // check for triplet
       }
       return false;                                // otherwise false
   }


   vector<int> getKey(const vector<int>& hand) override {
       auto freq = getFreq(hand);                   // get frequencies
       int trip = 0;                                // store trip value
       vector<int> kickers;                         // remaining cards


       for (auto &p : freq) {
           if (p.second == 3) trip = p.first;       // trip value
           else kickers.push_back(p.first);         // add kicker
       }


       sort(kickers.rbegin(), kickers.rend());      // sort descending


       vector<int> key = {trip};                    // start key with trip
       key.insert(key.end(), kickers.begin(), kickers.end()); // append kickers
       return key;                                  // return key
   }
};


class TwoPair : public HandType {
public:
   TwoPair() : HandType("TWO_PAIR") {}              // type id = TWO_PAIR


   bool matches(const vector<int>& hand) override {
       auto freq = getFreq(hand);                   // get frequencies
       int pairs = 0;                               // count pairs


       for (auto &p : freq) {
           if (p.second == 2) pairs++;              // increment if pair found
       }


       return pairs == 2;                           // must have exactly 2 pairs
   }


   vector<int> getKey(const vector<int>& hand) override {
       auto freq = getFreq(hand);                   // get frequencies
       vector<int> pairs;                           // store pair values
       int kicker = 0;                              // store remaining card


       for (auto &p : freq) {
           if (p.second == 2) pairs.push_back(p.first); // collect pairs
           else kicker = p.first;                   // remaining card
       }


       sort(pairs.rbegin(), pairs.rend());          // sort pairs descending


       return {pairs[0], pairs[1], kicker};         // key: [high_pair, low_pair, kicker]
   }
};


class OnePair : public HandType {
public:
   OnePair() : HandType("PAIR") {}                  // type id = PAIR


   bool matches(const vector<int>& hand) override {
       auto freq = getFreq(hand);                   // get frequencies
       for (auto &p : freq) {
           if (p.second == 2) return true;          // check for one pair
       }
       return false;                                // otherwise false
   }


   vector<int> getKey(const vector<int>& hand) override {
       auto freq = getFreq(hand);                   // get frequencies
       int pair = 0;                                // pair value
       vector<int> kickers;                         // remaining cards


       for (auto &p : freq) {
           if (p.second == 2) pair = p.first;       // pair value
           else kickers.push_back(p.first);         // add kicker
       }


       sort(kickers.rbegin(), kickers.rend());      // sort descending


       vector<int> key = {pair};                    // start key with pair
       key.insert(key.end(), kickers.begin(), kickers.end()); // append kickers
       return key;                                  // return key
   }
};


class HighCard : public HandType {
public:
   HighCard() : HandType("HIGH") {}                 // type id = HIGH


   bool matches(const vector<int>& hand) override {
       return true;                                 // fallback type (always matches)
   }


   vector<int> getKey(const vector<int>& hand) override {
       vector<int> sorted = hand;                   // copy hand
       sort(sorted.rbegin(), sorted.rend());        // sort descending
       return sorted;                               // key: sorted cards
   }
};


// -------------------- Evaluator --------------------


class HandEvaluator {
private:
   unordered_map<string, HandType*> registry;       // type_id → HandType mapping


public:
   void add_type(HandType* type) {
       registry[type->type_id] = type;              // register type
   }


   pair<string, vector<int>> evaluateHand(
       const vector<int>& hand,
       vector<string>& order
   ) {
       for (string &type_id : order) {              // iterate in priority order
           if (registry[type_id]->matches(hand)) {  // check match
               return {type_id, registry[type_id]->getKey(hand)}; // return type + key
           }
       }
       return {"HIGH", {}};                         // fallback
   }


   int compareKeys(const vector<int>& k1, const vector<int>& k2) {
       int n = min(k1.size(), k2.size());           // compare up to min length
       for (int i = 0; i < n; i++) {
           if (k1[i] > k2[i]) return 1;             // k1 wins
           if (k1[i] < k2[i]) return -1;            // k2 wins
       }
       return 0;                                    // tie
   }


   string evaluate(
       const vector<int>& h1,
       const vector<int>& h2,
       vector<string> order
   ) {
       auto [t1, k1] = evaluateHand(h1, order);     // evaluate hand1
       auto [t2, k2] = evaluateHand(h2, order);     // evaluate hand2


       unordered_map<string, int> rankMap;          // type → priority index
       for (int i = 0; i < order.size(); i++) {
           rankMap[order[i]] = i;                   // lower index = stronger
       }


       if (rankMap[t1] != rankMap[t2]) {            // compare types
           return rankMap[t1] < rankMap[t2] ? "HAND_1" : "HAND_2"; // decide winner
       }


       int cmp = compareKeys(k1, k2);               // tie-break using keys
       if (cmp > 0) return "HAND_1";                // hand1 wins
       if (cmp < 0) return "HAND_2";                // hand2 wins
       return "TIE";                                // exact tie
   }
};


// -------------------- Utility --------------------


vector<int> parse(string s) {
   vector<int> hand;
   for (char c : s) hand.push_back(c - '0');        // convert char → int
   return hand;
}


// -------------------- Main --------------------


int main() {
   HandEvaluator evaluator;


   evaluator.add_type(new FourOfAKind());           // register FOUR
   evaluator.add_type(new ThreeOfAKind());          // register THREE
   evaluator.add_type(new TwoPair());               // register TWO_PAIR
   evaluator.add_type(new OnePair());               // register PAIR
   evaluator.add_type(new HighCard());              // register HIGH


   vector<string> order = {                         // define ranking
       "FOUR",
       "THREE",
       "TWO_PAIR",
       "PAIR",
       "HIGH"
   };


   vector<int> h1 = parse("8883");                  // hand1
   vector<int> h2 = parse("7779");                  // hand2


   cout << evaluator.evaluate(h1, h2, order) << endl; // compare and print


   return 0;
}

