#include<vector>
#include<unordered_map>
#include<iostream>
using namespace std;


// -------------------- Interface --------------------
class HandType {
public:
   virtual bool matches(const vector<int>& hand) = 0;
   virtual int rank() const = 0; // higher = stronger
   virtual ~HandType() {}
};


// -------------------- Helper --------------------
unordered_map<int, int> getFreq(const vector<int>& hand) {
   unordered_map<int, int> freq;
   for (int c : hand) freq[c]++;
   return freq;
}


// -------------------- Concrete Hand Types --------------------


class FourOfAKind : public HandType {
public:
   bool matches(const vector<int>& hand) override {
       auto freq = getFreq(hand);
       return freq.size() == 1;
   }
   int rank() const override { return 5; }
};


class TwoPair : public HandType {
public:
   bool matches(const vector<int>& hand) override {
       auto freq = getFreq(hand);
       if (freq.size() != 2) return false;


       for (auto& p : freq) {
           if (p.second != 2) return false;
       }
       return true;
   }
   int rank() const override { return 4; }
};


class ThreeOfAKind : public HandType {
public:
   bool matches(const vector<int>& hand) override {
       auto freq = getFreq(hand);
       if (freq.size() != 2) return false;


       for (auto& p : freq) {
           if (p.second == 3) return true;
       }
       return false;
   }
   int rank() const override { return 3; }
};


class OnePair : public HandType {
public:
   bool matches(const vector<int>& hand) override {
       auto freq = getFreq(hand);
       return freq.size() == 3;
   }
   int rank() const override { return 2; }
};


class HighCard : public HandType {
public:
   bool matches(const vector<int>& hand) override {
       auto freq = getFreq(hand);
       return freq.size() == 4;
   }
   int rank() const override { return 1; }
};


// -------------------- Evaluator --------------------


class HandEvaluator {
private:
   vector<HandType*> rules;


public:
   HandEvaluator() {
       // Order matters only for matching, rank decides strength
       rules.push_back(new FourOfAKind());
       rules.push_back(new TwoPair());
       rules.push_back(new ThreeOfAKind());
       rules.push_back(new OnePair());
       rules.push_back(new HighCard());
   }


   ~HandEvaluator() {
       for (auto rule : rules) delete rule;
   }


   int getRank(const vector<int>& hand) {
       for (auto rule : rules) {
           if (rule->matches(hand)) {
               return rule->rank();
           }
       }
       return 0;
   }


   int tieBreaker(const vector<int>& h1, const vector<int>& h2) {
       // compare right → left
       for (int i = 3; i >= 0; i--) {
           if (h1[i] > h2[i]) return 1;
           if (h1[i] < h2[i]) return -1;
       }
       return 0;
   }


   string evaluate(const vector<int>& h1, const vector<int>& h2) {
       int r1 = getRank(h1);
       int r2 = getRank(h2);


       if (r1 > r2) return "HAND_1";
       if (r2 > r1) return "HAND_2";


       int t = tieBreaker(h1, h2);
       if (t > 0) return "HAND_1";
       if (t < 0) return "HAND_2";


       return "TIE";
   }
};


// -------------------- Utility --------------------


vector<int> parse(string s) {
   vector<int> hand;
   for (char c : s) {
       hand.push_back(c - '0');
   }
   return hand;
}


// -------------------- Main --------------------


int main() {
   HandEvaluator evaluator;


   vector<int> h1 = parse("2332"); // Two Pair
   vector<int> h2 = parse("9998"); // Three of a kind


   cout << evaluator.evaluate(h1, h2) << endl;


   return 0;
}

