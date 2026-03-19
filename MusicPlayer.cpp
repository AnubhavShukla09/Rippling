#include<map>
#include<string>
#include<unordered_set>
#include<unordered_map>
#include<set>
#include<iostream>
#include<list>
using namespace std;
class SongAnalytics {
private:
   int nextId = 1; // auto-incrementing song ID
   unordered_map<int, string> songIdToName; // song_id -> song name
   unordered_map<int, unordered_set<int>> songToUser; // song_id -> set of users who played the song (unique listeners)
   map<int, set<pair<string, int>>, greater<int>> leaderboard;
   // score -> set of (song_name, song_id)
   // map keeps scores sorted descending
   // set ensures songs with same score sorted lexicographically
   unordered_map<int, list<int>> userRecentSongList;
   // user_id -> list of recently played songs (most recent at front)
   unordered_map<int, unordered_map<int, list<int>::iterator>> userRecentSongMap;
   // user_id -> (song_id -> iterator to its position in list)
   // allows O(1) deletion from the list
public:
   // Time Complexity: O(log N)
   // N = number of songs
   int add_song(string name) {
       int id = nextId++; // assign new auto-incremented song ID
       songIdToName[id] = name; // store mapping from ID → name
       songToUser[id] = {}; // initialize empty set of listeners
       leaderboard[0].insert({name, id}); // insert song into leaderboard with score 0
       return id; // return assigned song ID
   }
   /*
   Records a play event for a song by a user.
   Each user counted once per song (unique plays).
   */
   // Time Complexity: O(log N)
   // due to leaderboard updates
   void play_song(int song_id, int user_id) {
       if(!songIdToName.count(song_id)) { // validate song exists
           cout<<"Error: Song ID "<<song_id <<" does not exist.";
           return;
       }
       // check if this user already listened to the song
       if(!songToUser[song_id].count(user_id)) {
           int curr_score = songToUser[song_id].size(); // current unique listener count
           string name = songIdToName[song_id]; // fetch song name
           leaderboard[curr_score].erase({name, song_id}); // remove song from current score bucket
           if(leaderboard[curr_score].size()==0) // if no songs left at this score
               leaderboard.erase(curr_score); // remove score bucket entirely
           songToUser[song_id].insert(user_id); // add user as unique listener
           int new_score = curr_score + 1; // increment listener count
           leaderboard[new_score].insert({name, song_id}); // move song into new score bucket
       }
       // update user's recent songs list
       auto &recentSongs = userRecentSongList[user_id]; // reference to user's recent list
       auto &recentSongsMap = userRecentSongMap[user_id]; // reference to map for fast lookup
       if(recentSongsMap.count(song_id)) // if song already in recent list
           recentSongs.erase(recentSongsMap[song_id]); // remove old position
       recentSongs.push_front(song_id); // insert song at front (most recent)
       recentSongsMap[song_id] = recentSongs.begin(); // store iterator to new position
       if(recentSongs.size()>3) { // keep only last 3 songs
           int lastSong = recentSongs.back(); // identify least recent song
           recentSongs.pop_back(); // remove it from list
           recentSongsMap.erase(lastSong); // remove from lookup map
       }
   }
   /*
   Prints songs sorted by unique listeners descending.
   Tie-break: lexicographic order of song name.
   */
   // Time Complexity: O(N)
   // N = number of songs
   void print_analytics() {
       for(auto &entry: leaderboard) { // iterate score buckets (already sorted descending)
           int score = entry.first; // number of unique listeners
           for(auto &song: entry.second) { // iterate songs with this score
               cout<<song.first << " (" << score << " unique listeners)" << endl;
           }
       }
       return;
   }
   // Return top K songs based on unique listeners
   // Time Complexity: O(K)
   void get_top_k_songs(int k) {
       if(k <= 0) {
           cout << "Invalid value of K" << endl;
           return;
       }
       cout << "Top " << k << " songs are:" << endl;
       int count = 0;
       for(auto &entry : leaderboard) { // scores in descending order
           int score = entry.first;
           for(auto &song : entry.second) { // lexicographically sorted
               cout << song.first << " (" << score << " unique listeners)" << endl;
               count++;
               if(count == k) return; // stop once K songs are printed
           }
       }
       if(count == 0) {
           cout << "No songs available" << endl;
       }
   }
   // Return last 3 unique songs played by a user
   // Time Complexity: O(1)
   void get_last_three_songs(int user_id) {
       if(!userRecentSongList.count(user_id)) { // user never played any song
           cout<<"User has not listened to any song"<<endl;
           return;
       }
       cout<<"Top 3 songs for user: "<<user_id<<" are: "<<endl;
       for(int song_id: userRecentSongList[user_id]) { // iterate recent songs list
           cout<<songIdToName[song_id]<<endl; // print song name
       }
       return;
   }
};


int main() {
   SongAnalytics sa;
   cout << "---- Adding Songs ----" << endl;
   int s1 = sa.add_song("ShapeOfYou");       // add song 1
   int s2 = sa.add_song("BlindingLights");   // add song 2
   int s3 = sa.add_song("Believer");         // add song 3
   int s4 = sa.add_song("Closer");           // add song 4
   cout << "Songs added with IDs: "
        << s1 << " " << s2 << " " << s3 << " " << s4 << endl;
   cout << "\n---- Recording Plays ----" << endl;
   sa.play_song(s1, 101); // user 101 plays song 1
   sa.play_song(s1, 102); // user 102 plays song 1
   sa.play_song(s2, 101); // user 101 plays song 2
   sa.play_song(s3, 101); // user 101 plays song 3
   sa.play_song(s3, 102); // user 102 plays song 3
   sa.play_song(s3, 103); // user 103 plays song 3
   sa.play_song(s4, 104); // user 104 plays song 4
   // duplicate play (should not increase unique count)
   sa.play_song(s1, 101);
   cout << "\n---- Leaderboard ----" << endl;
   sa.print_analytics(); // display leaderboard
   cout << "\n---- Top 2 Songs ----" << endl;
   sa.get_top_k_songs(2); // test top K
   cout << "\n---- User Recent Songs ----" << endl;
   sa.get_last_three_songs(101); // user 101
   sa.get_last_three_songs(102); // user 102
   sa.get_last_three_songs(200); // user with no songs
   cout << "\n---- More Plays (testing recency) ----" << endl;
   sa.play_song(s4, 101); // user 101 plays song 4
   sa.play_song(s2, 101); // user 101 plays song 2
   sa.get_last_three_songs(101); // updated recency
   return 0;
}

