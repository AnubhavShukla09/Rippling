//===============================================PART 1====================================================
#include <iostream>        // input/output
#include <vector>          // vector
#include <unordered_map>   // unordered_map
#include <unordered_set>   // unordered_set
#include <list>            // list
#include <string>          // string
#include <algorithm>       // algorithms
using namespace std; // use std namespace
// vote representation
enum VoteType { // represents vote state
    NONE = 0, // no vote
    UP = 1, // upvote
    DOWN = -1 // downvote
};
//============================== USER DATA ======================================
// stores per-user vote state and flip history
class UserData {
public:
    unordered_map<int, VoteType> articleVote; // articleId -> latest vote by user
    list<int> flipOrder; // stores flipped articles in recency order (most recent at front)
    unordered_set<int> flipSet; // ensures unique flipped articles
    UserData() {} // default constructor
    // record vote flip while keeping only last 3 unique articles
    void recordFlip(int articleId) {   // Time: O(1)
        if (flipSet.count(articleId)) { // if already present in history
            flipOrder.remove(articleId); // remove old occurrence (max size 3 -> constant)
        } else if (flipOrder.size() == 3) { // if already storing 3 articles
            int last = flipOrder.back(); // get least recent article
            flipOrder.pop_back(); // remove least recent
            flipSet.erase(last); // remove from set
        }
        flipOrder.push_front(articleId); // add as most recent
        flipSet.insert(articleId); // mark present
    }
    vector<int> getLast3Flips() { // return last 3 flipped articles (most recent first)
        return vector<int>(flipOrder.begin(), flipOrder.end()); // convert list to vector
    }
};
//============================== PART 1 : VOTING SYSTEM ======================================
// supports article creation, voting, vote flip tracking
class VotingSystem {
private:
    int nextArticleId; // auto increment article id
    unordered_map<int, string> articleName; // articleId -> article name (only for existence tracking)
    unordered_map<int, UserData> users; // userId -> user data
    void processVote(int userId, int articleId, VoteType newVote) { // Time: O(1)
        UserData &user = users[userId]; // fetch or create user
        VoteType prevVote = NONE; // default previous vote
        if (user.articleVote.count(articleId)) // check if user voted before
            prevVote = user.articleVote[articleId]; // get previous vote
        // detect vote flip (UP->DOWN or DOWN->UP)
        if ((prevVote == UP && newVote == DOWN) || (prevVote == DOWN && newVote == UP))
            user.recordFlip(articleId); // store flip event
        user.articleVote[articleId] = newVote; // store latest vote
    }
public:
    VotingSystem() { // constructor
        nextArticleId = 1; // article ids start from 1
    }
    int addArticle(const string &name) {
        int id = nextArticleId++; // assign id
        articleName[id] = name; // store article
        return id; // return created id
    } // Time: O(1)
    void upvote(int userId, int articleId) {
        processVote(userId, articleId, UP); // delegate vote logic
    } // Time: O(1)
    void downvote(int userId, int articleId) {
        processVote(userId, articleId, DOWN); // delegate vote logic
    } // Time: O(1)
    vector<int> last3Flips(int userId) { // Time: O(1)
        if (!users.count(userId)) return {}; // if user not found return empty
        return users[userId].getLast3Flips(); // fetch flip history
    }
};

//============================== DRIVER CODE ======================================
int main() { // program entry point
    VotingSystem vs; // create system
    int a1 = vs.addArticle("AI"); // add article AI -> id 1
    int a2 = vs.addArticle("ML"); // add article ML -> id 2
    vs.upvote(1, a1); // user 1 upvotes article 1
    vs.downvote(1, a1); // vote flip recorded
    vs.upvote(1, a2); // user 1 upvotes article 2
    vs.downvote(1, a2); // vote flip recorded
    vs.upvote(1, a1); // flip again -> becomes most recent
    vector<int> flips = vs.last3Flips(1); // fetch last 3 flips
    cout << "Last 3 flipped articles for user 1: "; // print header
    for (int id : flips) cout << id << " "; // print ids
    cout << endl;
    return 0; // exit program
}


//==========================================================PART 2======================================================================
#include <iostream>        // input/output
#include <vector>          // vector
#include <unordered_map>   // unordered_map
#include <unordered_set>   // unordered_set
#include <list>            // list
#include <string>          // string
#include <algorithm>       // algorithms
#include <set>             // set for ranking (PART 2)
using namespace std; // use std namespace
// vote representation
enum VoteType { // represents vote state
    NONE = 0, // no vote
    UP = 1, // upvote
    DOWN = -1 // downvote
};

//============================== USER DATA ======================================
// stores per-user vote state and flip history
class UserData {
public:
    unordered_map<int, VoteType> articleVote; // articleId -> latest vote by user
    list<int> flipOrder; // stores flipped articles in recency order (most recent at front)
    unordered_set<int> flipSet; // ensures unique flipped articles
    //================ PART 2 =================
    int lastVotedArticleId; // last article user voted on
    UserData() { lastVotedArticleId = -1; } // default constructor
    // record vote flip while keeping only last 3 unique articles
    void recordFlip(int articleId) {   // Time: O(1)
        if (flipSet.count(articleId)) { // if already present in history
            flipOrder.remove(articleId); // remove old occurrence (max size 3 -> constant)
        } 
        else if (flipOrder.size() == 3) { // if already storing 3 articles
            int last = flipOrder.back(); // get least recent article
            flipOrder.pop_back(); // remove least recent
            flipSet.erase(last); // remove from set
        }
        flipOrder.push_front(articleId); // add as most recent
        flipSet.insert(articleId); // mark present
    }
    vector<int> getLast3Flips() { // return last 3 flipped articles (most recent first) // Time: O(1)
        return vector<int>(flipOrder.begin(), flipOrder.end()); // convert list to vector
    }
};
//============================== PART 1 + PART 2 : VOTING SYSTEM ======================================
// supports article creation, voting, vote flip tracking + optimized ranking
class VotingSystem {
private:
    int nextArticleId; // auto increment article id
    unordered_map<int, string> articleName; // articleId -> article name
    unordered_map<int, UserData> users; // userId -> user data
    //================ PART 2 =================
    unordered_map<int,int> articleScore; // articleId -> score (upvotes - downvotes)
    set<pair<int,int>> ranking; // (score, articleId) sorted ascending -> iterate reverse for descending
    //================ INTERNAL VOTE PROCESSOR =================
    void processVote(int userId, int articleId, VoteType newVote) { // Time: O(log n)
        UserData &user = users[userId]; // fetch or create user
        VoteType prevVote = NONE; // default previous vote
        if (user.articleVote.count(articleId))
            prevVote = user.articleVote[articleId]; // get previous vote
        //================ PART 2 : REMOVE OLD SCORE FROM RANKING =================
        if (prevVote != NONE) { // if article had previous vote
            int oldScore = articleScore[articleId]; // fetch current score
            ranking.erase({oldScore, articleId}); // remove old ranking entry -> O(log n)
            if (prevVote == UP) oldScore--; // remove previous upvote effect
            else if (prevVote == DOWN) oldScore++; // remove previous downvote effect
            articleScore[articleId] = oldScore; // update score
        }
        //================ PART 1 : DETECT FLIP =================
        if ((prevVote == UP && newVote == DOWN) || (prevVote == DOWN && newVote == UP))
            user.recordFlip(articleId); // store flip event
        //================ PART 2 : APPLY NEW SCORE =================
        int updatedScore = articleScore[articleId]; // current score
        if (newVote == UP) updatedScore++; // apply upvote
        else if (newVote == DOWN) updatedScore--; // apply downvote
        articleScore[articleId] = updatedScore; // update score
        ranking.insert({updatedScore, articleId}); // insert new ranking -> O(log n)
        user.articleVote[articleId] = newVote; // store latest vote
        //================ PART 2 =================
        user.lastVotedArticleId = articleId; // update last voted article
    }
public:
    VotingSystem() { nextArticleId = 1; } // constructor
    int addArticle(const string &name) { // Time: O(log n)
        int id = nextArticleId++; // assign id
        articleName[id] = name; // store article
        //================ PART 2 =================
        articleScore[id] = 0; // initialize score
        ranking.insert({0, id}); // add to ranking with score 0

        return id; // return created id
    }
    void upvote(int userId, int articleId) { // Time: O(log n)
        processVote(userId, articleId, UP);
    }
    void downvote(int userId, int articleId) { // Time: O(log n)
        processVote(userId, articleId, DOWN);
    }
    vector<int> last3Flips(int userId) { // Time: O(1)
        if (!users.count(userId)) return {};
        return users[userId].getLast3Flips();
    }
    //============================== PART 2 APIS ======================================
    // return articles sorted by score descending
    vector<pair<int,int>> getArticlesByScore() { // Time: O(n)
        vector<pair<int,int>> res;
        // reverse iterate because set stores ascending
        for (auto it = ranking.rbegin(); it != ranking.rend(); ++it)
            res.push_back({it->second, it->first}); // {articleId, score}
        return res;
    }
    // return last article user voted on
    int lastVotedArticle(int userId) { // Time: O(1)
        if (!users.count(userId)) return -1;
        return users[userId].lastVotedArticleId;
    }
};
//============================== DRIVER CODE ======================================
int main() { // program entry point
    VotingSystem vs; // create system
    int a1 = vs.addArticle("AI"); // add article AI -> id 1
    int a2 = vs.addArticle("ML"); // add article ML -> id 2
    vs.upvote(1, a1); // user 1 upvotes article 1
    vs.downvote(1, a1); // vote flip recorded
    vs.upvote(1, a2); // user 1 upvotes article 2
    vs.downvote(1, a2); // vote flip recorded
    vs.upvote(1, a1); // flip again -> becomes most recent
    cout << "Last 3 flipped articles for user 1: ";
    for (int id : vs.last3Flips(1)) cout << id << " ";
    cout << endl;
    //================ PART 2 TEST =================
    cout << "Articles by score (desc): ";
    for (auto &p : vs.getArticlesByScore())
        cout << "(id=" << p.first << ", score=" << p.second << ") ";
    cout << endl;
    cout << "User 1 last voted article: " << vs.lastVotedArticle(1) << endl;
    return 0; // exit program
}
