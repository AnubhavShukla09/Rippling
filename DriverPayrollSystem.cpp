#include <bits/stdc++.h>
using namespace std;
class Driver{
private:
    long rate_per_hour_cents; // hourly rate stored in cents
    map<long,long> intervals; // merged non-overlapping intervals, start->end map
    long total_work_seconds; // total unique worked seconds
    //Part 2
    long paid_upto_time; // boundary till which payment is cleared (Part 2)
public:
    Driver(long usd_hourly_rate){
        rate_per_hour_cents=usd_hourly_rate*100; // convert dollars to cents
        total_work_seconds=0; // initially no work
        paid_upto_time=0; // initially nothing paid
    }
    void addOrder(long start,long end){ // Time: O(log n + k)
        if(start>=end)throw runtime_error("Invalid interval"); // validate input
        long new_start=start,new_end=end; // initialize merge interval
        auto it=intervals.lower_bound(start); // find first possible overlap
        if(it!=intervals.begin()){auto prev_it=prev(it);if(prev_it->second>=start)it=prev_it;} // check previous interval
        vector<long> eraseKeys; // store intervals to delete
        while(it!=intervals.end()&&it->first<=new_end){ // merge overlapping intervals
            if(it->second<new_start){++it;continue;} // skip if no overlap
            new_start=min(new_start,it->first); // expand start
            new_end=max(new_end,it->second); // expand end
            total_work_seconds-=(it->second-it->first); // remove old duration
            eraseKeys.push_back(it->first); // mark for erase
            ++it; // move iterator
        }
        for(long s:eraseKeys)intervals.erase(s); // erase merged intervals
        intervals[new_start]=new_end; // insert merged interval
        total_work_seconds+=(new_end-new_start); // add merged duration
    }
    long getTotalCost()const{ // Time: O(1)
        return(total_work_seconds*rate_per_hour_cents)/3600; // convert seconds to cents
    }
    // ------------------ PART 2 : Payment Logic Starts Here ------------------

    long payUpTo(long pay_time){ // Time: O(n)
        long payable_seconds=0; // accumulate unpaid seconds
        for(auto &p:intervals){long start=p.first,end=p.second; // iterate intervals
            if(end<=paid_upto_time)continue; // already paid
            if(start>=pay_time)break; // beyond boundary
            long effective_start=max(start,paid_upto_time); // adjust start
            long effective_end=min(end,pay_time); // adjust end
            if(effective_start<effective_end)payable_seconds+=(effective_end-effective_start); // add payable
        }
        paid_upto_time=max(paid_upto_time,pay_time); // move payment boundary
        return(payable_seconds*rate_per_hour_cents)/3600; // return amount paid
    }
};
class DeliverySystem{
private:
    unordered_map<int,Driver*> drivers; // driver registry
    long total_paid_cents; // total amount cleared
public:
    DeliverySystem(){total_paid_cents=0;} // simple constructor
    void addDriver(int driverId,long usd_hourly_rate){ // Time: O(1)
        if(drivers.count(driverId))throw runtime_error("Driver exists"); // avoid duplicate
        drivers[driverId]=new Driver(usd_hourly_rate); // create driver
    }
    void addOrder(int driverId,long start,long end){ // Time: O(log n + k)
        if(!drivers.count(driverId))throw runtime_error("Driver not found"); // validate driver
        drivers[driverId]->addOrder(start,end); // delegate to driver
    }
    long getTotalCost(){ // Time: O(d)
        long total=0; // accumulator
        for(auto &p:drivers)total+=p.second->getTotalCost(); // sum driver costs
        return total; // return total cents
    }
    // ------------------ PART 2 : System Payment APIs ------------------
    long Pay_Up_To(long pay_time){ // Time: O(total intervals)
        long paid_now=0; // accumulate payment
        for(auto &p:drivers)paid_now+=p.second->payUpTo(pay_time); // pay each driver
        total_paid_cents+=paid_now; // update global paid
        return paid_now; // return amount cleared
    }
    long Total_Cost_Unpaid(){ // Time: O(d)
        return getTotalCost()-total_paid_cents; // unpaid = accrued - cleared
    }
};
int main(){
    DeliverySystem ds; // create system
    ds.addDriver(1,20); // add driver
    ds.addOrder(1,0,3600); // 1 hour
    ds.addOrder(1,1800,5400); // overlapping interval
    cout<<"Total Cost: "<<ds.getTotalCost()<<" cents"<<endl; // print total
    cout<<"Pay upto 4000: "<<ds.Pay_Up_To(4000)<<" cents"<<endl; // partial pay
    cout<<"Unpaid: "<<ds.Total_Cost_Unpaid()<<" cents"<<endl; // remaining unpaid
    return 0; // end
}
