#include<iostream>
#include<map>
#include<unordered_map>
#include<vector>
#include<cmath>
using namespace std;
using TimeStamp = long long;
class Payroll {
private:
   unordered_map<int, long long> driverRate;   // driverId -> rate (cents/hour)
   map<TimeStamp, long long> deliveries;       // endTime -> aggregated cost
   long long total_cost_cents = 0;
   long long paid_till_now_cents = 0;
public:
   // Time Complexity: O(1) average, O(N) worst-case (hash collisions)
   void addDriver(int driverId, double rate) {
       if(driverRate.count(driverId)) return;  // O(1)
       driverRate[driverId] = llround(rate * 100); // O(1)
   }
   // Time Complexity: O(log D)
   // D = number of unique delivery end timestamps (size of map)
   void addOrder(int driverId, TimeStamp start, TimeStamp end) {
       if(end <= start) return;                     // O(1)
       if(!driverRate.count(driverId)) return;      // O(1)
       TimeStamp duration = end - start;            // O(1)
       long long rate = driverRate[driverId];       // O(1)
       // O(1) (safe multiplication using __int128)
       long long delivery_cost_cents = ((__int128)duration * rate) / 3600;
       deliveries[end] += delivery_cost_cents;      // O(log D)
       total_cost_cents += delivery_cost_cents;     // O(1)
   }
   // Time Complexity: O(1)
   void getTotalCost() {
       cout << "The total cost till now is : "
            << total_cost_cents / 100.0 << endl;
   }
   // Time Complexity: O(K log D)
   // K = number of timestamps <= t (number of map entries processed)
   // D = total number of timestamps in map
   void payUpTo(TimeStamp t) {
       auto cutoff = deliveries.upper_bound(t); // O(log D)
       auto it = deliveries.begin();
       long long paid_now_cents = 0;
       while(it != cutoff) {                   // runs K times
           paid_now_cents += it->second;       // O(1)
           it = deliveries.erase(it);          // O(log D) per erase
       }
       paid_till_now_cents += paid_now_cents;  // O(1)
   }
   // Time Complexity: O(1)
   void totalUnpaidCost() {
       cout << "The total cost yet to be paid is : "
            << (total_cost_cents - paid_till_now_cents) / 100.0 << endl;
   }
};
int main() {
   Payroll payroll;


   cout << "----- Adding Drivers -----" << endl;
   payroll.addDriver(1, 20.0);   // $20/hour
   payroll.addDriver(2, 15.5);   // $15.5/hour
   payroll.addDriver(1, 25.0);   // duplicate (should be ignored)


   cout << endl;


   cout << "----- Recording Orders -----" << endl;
   payroll.addOrder(1, 0, 3600);       // 1 hr → $20
   payroll.addOrder(2, 0, 1800);       // 0.5 hr → $7.75
   payroll.addOrder(1, 4000, 11200);   // 2 hrs → $40
   payroll.addOrder(3, 0, 1000);       // invalid driver (ignored)
   payroll.addOrder(1, 5000, 4000);    // invalid time (ignored)


   cout << endl;


   cout << "----- Total Cost -----" << endl;
   payroll.getTotalCost();  // expected ≈ 67.75


   cout << endl;


   cout << "----- Pay up to time 3000 -----" << endl;
   payroll.payUpTo(3000);   // pays delivery ending at 1800


   cout << "Unpaid after partial payment:" << endl;
   payroll.totalUnpaidCost();


   cout << endl;


   cout << "----- Pay up to time 5000 -----" << endl;
   payroll.payUpTo(5000);   // pays delivery ending at 3600


   cout << "Unpaid after second payment:" << endl;
   payroll.totalUnpaidCost();


   cout << endl;


   cout << "----- Final Payment -----" << endl;
   payroll.payUpTo(20000);  // pays remaining deliveries


   cout << "Unpaid after final payment:" << endl;
   payroll.totalUnpaidCost();  // should be 0


   cout << endl;


   return 0;
}

