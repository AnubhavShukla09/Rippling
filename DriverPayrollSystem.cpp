#include<iostream>
#include<map>
#include<vector>
#include<unordered_map>
#include<cmath>
using namespace std;
using Timestamp = long long; // timestamp stored as seconds since epoch (or any second-precision time)
class PayrollSystem {
private:
   unordered_map<int, long long> drivers; // driver_id -> hourly rate in cents
   map<Timestamp, vector<long long>> deliveries; // endTime -> list of delivery costs finishing at that time
   long long total_cost_cents = 0; // total cost of all deliveries recorded so far
   long long paid_till_now_cents = 0; // total amount already paid to drivers
public:
   /*
   The given driver will not already be in the system
   The hourly rate applies per delivery, and a driver can be paid for simultaneous deliveries
   */
   // Time Complexity: O(1)
   void add_driver(int driver_id, float usd_hourly_rate) {
       if(drivers.count(driver_id)) { // check if driver already exists
           cout<<"Driver already added to the system"<< endl;
           return;
       }
       drivers.emplace(driver_id, llround(usd_hourly_rate*100)); // convert USD/hour to cents/hour and store
       cout<<"Driver added successfully to the system"<< endl;
   }
   /*
   Discuss the time format you choose
   Times require minimum one-second precision
   The given driver will already be in the system
   All deliveries will be recorded immediately after the delivery is completed
   */
   // Time Complexity: O(log N)
   // N = number of unique delivery end timestamps
   void record_delivery(int driver_id, Timestamp start, Timestamp end) {
       if(!drivers.count(driver_id)) { // validate driver exists
           cout<<"This driver is not added to the system"<< endl;
           return;
       }
       long long hourly_rate_in_cents = drivers[driver_id]; // fetch driver's hourly rate
       long long cost_cents = ((end-start)*hourly_rate_in_cents)/3600; // compute cost proportional to duration
       deliveries[end].push_back(cost_cents); // group deliveries by end timestamp
       total_cost_cents += cost_cents; // update running total cost
       cout<<"Delivery recorded"<< endl;
       return;
   }


   /*
   Return the total, aggregated cost of all drivers' deliveries recorded in the system
   For example, return 135.30 if one driver is in the system and has a total cost of 100.30 USD and another driver is in the system and has a total cost of 35.00 USD.
   This will be used for a live dashboard
   Do not worry about exact formatting
   */
   // Time Complexity: O(1)
   void get_total_cost() {
       cout<< "The total cost of all the deliveries is" << total_cost_cents/100.0<<endl; // convert cents → dollars
       return;
   }
   // Pay all drivers for recorded deliveries which ended up to and including the given time
   // Time Complexity: O(K + log N)
   // K = number of deliveries being paid
   // N = number of timestamp buckets
   void pay_up_to(Timestamp paytime) {
       auto cutoff = deliveries.upper_bound(paytime); // iterator to first delivery ending AFTER paytime
       long long paid_now = 0; // amount paid in this operation
       auto it = deliveries.begin(); // start from earliest delivery
       while(it!=cutoff) { // process deliveries whose end time <= paytime
           for(auto &cost:it->second) { // iterate through all deliveries ending at this timestamp
               paid_now += cost; // accumulate payout
           }
           it = deliveries.erase(it); // erase processed bucket and move iterator to next
       }
       paid_till_now_cents += paid_now; // update total paid amount
       cout<<"Paid up to time: " << paytime << " amount: " << paid_now/100.0 << endl; // display payment
   }
   // Return the total, aggregated cost of all drivers' deliveries which have not been paid
   // Time Complexity: O(1)
   void get_total_cost_unpaid() {
       cout<<"The total unpaid cost is "
           << (total_cost_cents - paid_till_now_cents)/100.0 // unpaid = total cost - already paid
           <<endl;
   }
};
int main() {


   PayrollSystem payroll;


   cout << "----- Adding Drivers -----" << endl;


   payroll.add_driver(1, 20.0);   // driver 1 earns $20/hour
   payroll.add_driver(2, 15.5);   // driver 2 earns $15.5/hour


   cout << endl;


   cout << "----- Recording Deliveries -----" << endl;


   payroll.record_delivery(1, 0, 3600);      // 1 hour delivery → driver 1
   payroll.record_delivery(2, 0, 1800);      // 30 minute delivery → driver 2
   payroll.record_delivery(1, 4000, 11200);  // 2 hour delivery → driver 1


   cout << endl;


   cout << "----- Total Cost -----" << endl;


   payroll.get_total_cost(); // display total cost of all deliveries


   cout << endl;


   cout << "----- Pay up to time 5000 -----" << endl;


   payroll.pay_up_to(5000); // pay deliveries that ended by time 5000


   cout << endl;


   cout << "----- Total Unpaid -----" << endl;


   payroll.get_total_cost_unpaid(); // remaining unpaid deliveries


   cout << endl;


   cout << "----- Pay remaining deliveries -----" << endl;


   payroll.pay_up_to(20000); // pay remaining deliveries


   cout << endl;


   cout << "----- Total Unpaid After Final Payment -----" << endl;


   payroll.get_total_cost_unpaid(); // should now be zero


   return 0;
}

