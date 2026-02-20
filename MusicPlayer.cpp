#include <bits/stdc++.h>
using namespace std;
// ========================== SONG ENTITY ==========================
class Song{
private:
    int songId; // unique identifier for song
    string title; // song title
public:
    Song(int id,string name){ // Time: O(1)
        songId=id; // assign id
        title=name; // assign title
    }
    int getId() const{ return songId; } // getter for id
    string getTitle() const{ return title; } // getter for title
};
// ========================== MUSIC PLAYER SERVICE ==========================
class MusicPlayerService{
private:
    int nextSongId; // auto-increment id starting from 1
    unordered_map<int,Song*> songs; // songId -> Song object
    // ------------------ PART 1 : UNIQUE USER ANALYTICS ------------------
    unordered_map<int,unordered_set<int>> songToUsers; // songId -> unique users
    set<pair<int,int>> sortedSongs; // (uniqueCount, songId) sorted ascending
    // ------------------ PART 2 : RECENTLY PLAYED ------------------
    unordered_map<int,list<int>> userRecentList; // userId -> recent songs
    unordered_map<int,unordered_map<int,list<int>::iterator>> userRecentMap; // userId -> iterator map
    // ------------------ PART 3 : STARRED SONGS ------------------
    unordered_map<int,unordered_set<int>> userStarredSongs; // userId -> starred songs
public:
    MusicPlayerService(){ // Time: O(1)
        nextSongId=1; // initialize id counter
    }
    int add_song(string title){ // Time: O(log S)
        int id=nextSongId++; // generate id
        songs[id]=new Song(id,title); // create Song object
        songToUsers[id]={}; // initialize analytics set
        sortedSongs.insert({0,id}); // insert with 0 unique count
        return id; // return id
    }
    void play_song(int userId,int songId){ // Time: O(log S)
        if(!songs.count(songId)){ // validate song
            cout<<"Invalid songId"<<endl; // error
            return; // exit
        }
        // ---------- PART 1 : ANALYTICS ----------
        auto &users=songToUsers[songId]; // reference unique user set
        if(users.find(userId)==users.end()){ // if new unique user
            int oldCount=users.size(); // get old count
            sortedSongs.erase({oldCount,songId}); // remove old pair
            users.insert(userId); // insert user
            int newCount=users.size(); // updated count
            sortedSongs.insert({newCount,songId}); // insert updated pair
        }
        // ---------- PART 2 : RECENTLY PLAYED ----------
        auto &recentList=userRecentList[userId]; // get recent list
        auto &recentMap=userRecentMap[userId]; // get iterator map
        if(recentMap.find(songId)!=recentMap.end()) // if already present
            recentList.erase(recentMap[songId]); // remove old position
        recentList.push_front(songId); // insert at front
        recentMap[songId]=recentList.begin(); // update iterator
    }
    void print_analytics(int k){ // Time: O(k)
        cout<<"Top "<<k<<" Songs by unique users:"<<endl; // heading
        int count=0; // counter
        for(auto it=sortedSongs.rbegin();it!=sortedSongs.rend()&&count<k;++it){ // iterate descending
            cout<<songs[it->second]->getTitle()<<" | Unique Users: "<<it->first<<endl; // print
            count++; // increment
        }
    }
 // ---------- PART 2 : RECENTLY PLAYED ----------
    vector<int> getLastThreeSongs(int userId){ // Time: O(1)
        vector<int> result; // result container
        if(userRecentList.find(userId)==userRecentList.end()) // if no history
            return result; // return empty
        int count=0; // counter
        for(int songId:userRecentList[userId]){ // iterate recent list
            if(count==3) break; // stop after 3
            result.push_back(songId); // add id
            count++; // increment
        }
        return result; // return vector
    }
    // ---------- PART 3 : STAR / UNSTAR ----------
    void star_song(int userId,int songId){ // Time: O(1)
        if(!songs.count(songId)){ // validate song
            cout<<"Invalid songId"<<endl; // error
            return;
        }
        userStarredSongs[userId].insert(songId); // add song to starred set
    }
    void unstar_song(int userId,int songId){ // Time: O(1)
        if(userStarredSongs.count(userId)) // if user exists
            userStarredSongs[userId].erase(songId); // remove from starred set
    }
    vector<int> getLastNFavouriteSongs(int userId,int k){ // Time: O(N)
        vector<int> result; // store result
        if(userRecentList.find(userId)==userRecentList.end()) // if no history
            return result; // return empty
        int count=0; // counter
        for(int songId:userRecentList[userId]){ // iterate recent list
            if(count==k) break; // stop at k
            if(userStarredSongs[userId].count(songId)){ // if song is starred
                result.push_back(songId); // add to result
                count++; // increment
            }
        }
        return result; // return favourite songs
    }
};
// ========================== MAIN ==========================

int main(){ // Time: O(n log S)
    MusicPlayerService service; // create service
    int s1=service.add_song("Song A");
    int s2=service.add_song("Song B");
    int s3=service.add_song("Song C");
    service.play_song(1,s1);
    service.play_song(1,s2);
    service.play_song(1,s3);
    service.play_song(2,s1);
    service.play_song(1,s2); // replay
    service.star_song(1,s1); // star
    service.star_song(1,s3); // star
    service.print_analytics(3);
    vector<int> fav=service.getLastNFavouriteSongs(1,2);
    cout<<"Last 2 Favourite Songs for user 1: ";
    for(int id:fav) cout<<id<<" ";
    cout<<endl;
    return 0;
}
