#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <list>
#include <vector>
#include <string>
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
    set<pair<int,string>> sortedSongs;
    // ------------------ PART 2 : RECENTLY PLAYED (LRU STYLE) ------------------
    unordered_map<int,list<int>> userRecentList; // userId -> recent songs
    unordered_map<int,unordered_map<int,list<int>::iterator>> userRecentMap; // userId -> iterator map
    // ------------------ PART 3 : FAVORITES WITH FIXED CAPACITY = 3 ------------------
    static const int FAVORITE_CAPACITY = 3; // fixed capacity
    unordered_map<int,list<int>> userFavoritesList; // userId -> favorite songs order
    unordered_map<int,unordered_map<int,list<int>::iterator>> userFavoritesMap; // userId -> iterator map
public:
    MusicPlayerService(){ // Time: O(1)
        nextSongId=1; // initialize id counter
    }
    // ========================== ADD SONG ==========================
    int add_song(string title){ // Time: O(log S)
        int id=nextSongId++; // generate new id
        songs[id]=new Song(id,title); // create Song object
        songToUsers[id]={}; // initialize analytics set
        sortedSongs.insert({0,title}); // insert with 0 unique listeners
        return id; // return assigned id
    }
    // ========================== PLAY SONG ==========================
    void play_song(int userId,int songId){ // Time: O(log S)
        if(!songs.count(songId)){ // validate song id
            cout<<"Error: Song ID "<<songId<<" does not exist."<<endl;
            return;
        }
        // ---------- PART 1 : UNIQUE LISTENER ANALYTICS ----------
        auto &users=songToUsers[songId]; // reference unique user set
        string title = songs[songId]->getTitle(); // get song title
        if(users.find(userId)==users.end()){ // if new unique user
            int oldCount=users.size(); // old unique count
            sortedSongs.erase({oldCount,title}); // remove old entry
            users.insert(userId); // add user
            int newCount=users.size(); // updated count
            sortedSongs.insert({newCount,title}); // insert updated entry
        }
        // ---------- PART 2 : RECENTLY PLAYED (MOVE TO FRONT) ----------
        auto &recentList=userRecentList[userId]; // get user's list
        auto &recentMap=userRecentMap[userId]; // iterator map
        if(recentMap.find(songId)!=recentMap.end()) // if already present
            recentList.erase(recentMap[songId]); // remove old position
        recentList.push_front(songId); // insert at front (most recent)
        recentMap[songId]=recentList.begin(); // update iterator
    }
    // ========================== PART 1 ==========================
    void print_analytics(){ // Time: O(S)
        for(auto it=sortedSongs.rbegin(); it!=sortedSongs.rend(); ++it){
            cout<<it->second<<" ("<<it->first<<" unique listeners)"<<endl;
        }
    }
    // ========================== PART 2 : GET RECENT SONGS ==========================
    vector<int> getRecentSongs(int userId,int k){ // Time: O(k)
        vector<int> result;
        if(userRecentList.find(userId)==userRecentList.end()) return result; // no history
        int count=0;
        for(int songId:userRecentList[userId]){
            if(count==k) break;
            result.push_back(songId);
            count++;
        }
        return result;
    }
    // ========================== PART 3 : ADD FAVORITE ==========================
    /*
        Adds favorite with capacity = 3.
        If full -> removes oldest.
        LRU style.
    */
    void addFavorite(int userId,int songId){ // Time: O(1)
        if(!songs.count(songId)){
            cout<<"Error: Song ID "<<songId<<" does not exist."<<endl;
            return;
        }
        auto &favList=userFavoritesList[userId];
        auto &favMap=userFavoritesMap[userId];
        if(favMap.count(songId)) return; // already favorite
        if(favList.size()==FAVORITE_CAPACITY){ // remove oldest
            int last=favList.back();
            favMap.erase(last);
            favList.pop_back();
        }
        favList.push_front(songId); // add newest
        favMap[songId]=favList.begin();
    }
    // ========================== REMOVE FAVORITE ==========================
    void removeFavorite(int userId,int songId){ // Time: O(1)
        if(!userFavoritesMap.count(userId)) return;
        auto &favMap=userFavoritesMap[userId];
        auto &favList=userFavoritesList[userId];
        if(!favMap.count(songId)) return;
        favList.erase(favMap[songId]);
        favMap.erase(songId);
    }
    // ========================== LIST FAVORITES ==========================
    vector<int> listFavorites(int userId){ // Time: O(3) = O(1)
        vector<int> result;
        if(!userFavoritesList.count(userId)) return result;
        for(int songId:userFavoritesList[userId])
            result.push_back(songId);
        return result;
    }
};


// ========================== MAIN ==========================
int main(){ // Time: O(N log S)
    MusicPlayerService service;
    int s1=service.add_song("Song A");
    int s2=service.add_song("Song B");
    int s3=service.add_song("Song C");
    service.play_song(1,s1);
    service.play_song(1,s2);
    service.play_song(1,s3);
    service.play_song(2,s1);
    service.play_song(1,s2); // replay
    service.addFavorite(1,s1);
    service.addFavorite(1,s3);
    cout<<"--- Analytics ---"<<endl;
    service.print_analytics();
    cout<<"--- Recent Songs (User 1) ---"<<endl;
    vector<int> recent=service.getRecentSongs(1,3);
    for(int id:recent) cout<<id<<" ";
    cout<<endl;
    cout<<"--- Favorites (User 1) ---"<<endl;
    vector<int> fav=service.listFavorites(1);
    for(int id:fav) cout<<id<<" ";
    cout<<endl;
    return 0;
}
