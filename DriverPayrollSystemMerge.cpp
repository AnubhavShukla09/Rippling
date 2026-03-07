#include<map>
#include<string>
#include<unordered_set>
#include<set>
#include<iostream>
#include<list>
using namespace std;
class SongAnalytics {
   private:
       int nextId = 1;
       unordered_map<int, string>songIdToName;
       unordered_map<int, unordered_set<int>>songToUser;
       map<int, set<pair<string, int>>, greater<int>>leaderboard; //map {score->song name, id in asc order}
       unordered_map<int, list<int>>userRecentSongList; //list of recent songs in order
       unordered_map<int, unordered_map<int, list<int>::iterator>>userRecentSongMap; //map of song_id to address in the list
   public:
       //Adds a song to the system, assigns it a unique auto-incrementing ID starting from 1, and returns the assigned ID.
       int add_song(string name) { //O(log n)
           int id = nextId++;
           songIdToName[id] = name;
           songToUser[id] = {};
           leaderboard[0].insert({name, id}); //O(log n)
           return id;
       }
       /*
       Records a play event for a song by a user.
       If song_id does not exist, output: Error: Song ID <song_id> does not exist. (replace <song_id> with the invalid ID).
       Each user is counted once per song, even if they play it multiple times. (unique plays)
       */
       void play_song(int song_id, int user_id) { //O(log n)
           if(!songIdToName.count(song_id)) {
               cout<<"Error: Song ID "<<song_id <<" does not exist.";
               return;
           }
           if(!songToUser[song_id].count(user_id)) {
               int curr_score = songToUser[song_id].size();
               string name = songIdToName[song_id];
               leaderboard[curr_score].erase({name, song_id}); //O(log n)
               if(leaderboard[curr_score].size()==0)leaderboard.erase(curr_score);
               songToUser[song_id].insert(user_id);
               int new_score = curr_score+1;
               leaderboard[new_score].insert({name, song_id}); //O(log n)
           }
           auto &recentSongs = userRecentSongList[user_id];
           auto &recentSongsMap = userRecentSongMap[user_id];
           if(recentSongsMap.count(song_id))
               recentSongs.erase(recentSongsMap[song_id]);
           recentSongs.push_front(song_id);
           recentSongsMap[song_id] = recentSongs.begin();
           if(recentSongs.size()>3) {
               int lastSong = recentSongs.back();
               recentSongs.pop_back();
               recentSongsMap.erase(lastSong);
           }
       }
       /*
       Prints a summary of all songs sorted by the number of unique listeners in descending order.
       If two songs have the same number of unique listeners, sort them lexicographically by name in ascending order.
       Each line in the output should follow the format: <song_name> (<count> unique listeners)
       */
       void print_analytics() { // O(n)
           for(auto &entry: leaderboard) { // O(n)
               int score = entry.first;
               for(auto &song: entry.second) {
                   cout<<song.first << " (" << score << " unique listeners)" << endl;
               }
           }
           return;
       }
       // get last 3 unique songs played by a given user
       void get_last_three_songs(int user_id) { //O(1)
           if(!userRecentSongList.count(user_id)) {
               cout<<"User has not listened to any song"<<endl;
               return;
           }
           cout<<"Top 3 songs for user: "<<user_id<<" are: "<<endl;
           for(int song_id: userRecentSongList[user_id]) {
               cout<<songIdToName[song_id]<<endl;
           }
           return;
       }
};
int main() {


}

