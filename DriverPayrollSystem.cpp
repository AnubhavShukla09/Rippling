#include<iostream>
#include<map>
#include <cmath>
using namespace std;
using Timestamp = long long;
class PayrollSystem {
   private:
       unordered_map<int, long long>drivers; //driver_id->rate in cents
       map<Timestamp, vector<long long>>deliveries; //endTime->list of cost
       long long total_cost_cents = 0;
       long long paid_till_now_cents = 0;
   public:
       /*
       The given driver will not already be in the system
       The hourly rate applies per delivery, and a driver can be paid for simultaneous deliveries
       */
       void add_driver(int driver_id, float usd_hourly_rate) {
           if(drivers.count(driver_id)) {
               cout<<"Driver already added to the system"<< endl;
               return;
           }
           drivers.emplace(driver_id, llround(usd_hourly_rate*100));
           cout<<"Driver added successfully to the system"<< endl;
       }
       /*
       Discuss the time format you choose
       Times require minimum one-second precision
       The given driver will already be in the system
       All deliveries will be recorded immediately after the delivery is completed
       */
       void record_delivery(int driver_id, Timestamp start, Timestamp end) {
           if(!drivers.count(driver_id)) {
               cout<<"This driver is not added to the system"<< endl;
               return;
           }
           long long hourly_rate_in_cents = drivers[driver_id];
           long long cost_cents = ((end-start)*hourly_rate_in_cents)/3600;
           deliveries[end].push_back(cost_cents);
           total_cost_cents+=cost_cents;
           cout<<"Delivery recorded"<< endl;
           return;
       }
       /*
       Return the total, aggregated cost of all drivers' deliveries recorded in the system
       For example, return 135.30 if one driver is in the system and has a total cost of 100.30 USD and another driver is in the system and has a total cost of 35.00 USD.
       This will be used for a live dashboard
       Do not worry about exact formatting
       */
       void get_total_cost() {
           cout<< "The total cost of all the deliveries is" << total_cost_cents/100.0<<endl;
           return;
       }
       // Pay all drivers for recorded deliveries which ended up to and including the given time
       void pay_up_to(Timestamp paytime) {
           auto cutoff = deliveries.upper_bound(paytime);
           long long paid_now = 0;
           auto it = deliveries.begin();
           while(it!=cutoff) {
               for(auto &cost:it->second) {
                   paid_now+=cost;
               }
               it = deliveries.erase(it);
           }
           paid_till_now_cents+=paid_now;
           cout<<"Paid up to time: " << paytime << " amount: " << paid_now/100.0 << endl;
       }


       // Return the total, aggregated cost of all drivers' deliveries which have not been paid
       void get_total_cost_unpaid() {
           cout<<"The total unpaid cost is " << (total_cost_cents - paid_till_now_cents)/100.0 <<endl;
       }
};
int main() {


}

