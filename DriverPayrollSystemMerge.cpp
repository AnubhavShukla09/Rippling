#include <bits/stdc++.h> // includes all standard libraries
using namespace std; // use std namespace
using Timestamp=long long; // alias for time stored as epoch seconds
class Driver{ // class representing a single driver
private:
    long long rate_per_hour_cents; // hourly rate stored in cents to avoid floating errors
    map<Timestamp,Timestamp> intervals; // merged non-overlapping intervals stored as start->end
    long long total_work_seconds; // total unique worked seconds after merging intervals
    Timestamp paid_upto_time; // boundary till which payment already done to avoid double pay
public:
    Driver(double usd_hourly_rate=0){ // constructor initializes driver state
        rate_per_hour_cents=llround(usd_hourly_rate*100); // convert hourly rate from dollars to cents
        total_work_seconds=0; // initially no work recorded
        paid_upto_time=0; // initially nothing paid
    }
    // Time: O(log n + k) where k = number of intervals merged
    void recordDelivery(Timestamp start,Timestamp end){ // records a delivery and merges overlapping intervals
        if(start>=end)throw runtime_error("Invalid interval"); // validate interval
        Timestamp new_start=start,new_end=end; // initialize new interval to insert
        auto it=intervals.lower_bound(start); // find first interval with start >= given start
        if(it!=intervals.begin()){ // check previous interval for overlap
            auto prev_it=prev(it); // get previous interval
            if(prev_it->second>=start)it=prev_it; // move iterator to overlapping interval if exists
        }
        vector<Timestamp> erase_keys; // store keys of intervals to remove after merging
        while(it!=intervals.end() && it->first<=new_end){ // merge all overlapping intervals
            if(it->second<new_start){++it;continue;} // skip if no overlap
            new_start=min(new_start,it->first); // expand merged start
            new_end=max(new_end,it->second); // expand merged end
            total_work_seconds-=(it->second-it->first); // remove old interval duration from total work
            erase_keys.push_back(it->first); // mark interval for deletion
            ++it; // move iterator forward
        }
        for(auto s:erase_keys)intervals.erase(s); // remove all merged intervals
        intervals[new_start]=new_end; // insert merged interval
        total_work_seconds+=(new_end-new_start); // add merged duration to total work
    }
    // Time: O(1)
    long long getTotalAccruedCents()const{ // returns total accrued cost
        return(total_work_seconds*rate_per_hour_cents)/3600; // convert seconds to cents
    }
    // Time: O(n intervals)
    long long payUpTo(Timestamp pay_time){ // pays all unpaid work up to given time boundary
        long long payable_seconds=0; // accumulator for unpaid seconds  
        for(auto &p:intervals){ // iterate through merged intervals
            Timestamp start=p.first,end=p.second; // extract interval   
            if(end<=paid_upto_time)continue; // skip already paid portion
            if(start>=pay_time)break; // stop if interval starts beyond payment boundary  
            Timestamp effective_start=max(start,paid_upto_time); // compute unpaid start portion
            Timestamp effective_end=min(end,pay_time); // compute payable end portion        
            if(effective_start<effective_end) // check valid payable region
                payable_seconds+=(effective_end-effective_start); // add unpaid duration
        }     
        paid_upto_time=max(paid_upto_time,pay_time); // move payment boundary forward    
        return(payable_seconds*rate_per_hour_cents)/3600; // convert payable seconds to cents
    }
};

class PayrollSystem{ // payroll aggregator managing multiple drivers
private:
    unordered_map<int,Driver> drivers; // registry mapping driver id to Driver object
    long long total_paid_cents; // total amount paid across all drivers
public:
    PayrollSystem(){total_paid_cents=0;} // constructor initializes global state
    // Time: O(1)
    void Add_driver(int id,double rate){ // adds new driver
        if(drivers.count(id))throw runtime_error("Driver exists"); // prevent duplicate driver id
        drivers.emplace(id,rate); // insert driver object directly into map
    }
    // Time: O(log n)
    void Record_delivery(int id,Timestamp start,Timestamp end){ // records delivery for a driver
        if(!drivers.count(id))throw runtime_error("Driver not found"); // validate driver exists
        drivers[id].recordDelivery(start,end); // delegate recording to driver object
    }
    // Time: O(d) where d = number of drivers
    double Get_Total_Cost(){ // returns total accrued cost across all drivers
        long long total=0; // accumulator
        for(auto &p:drivers)total+=p.second.getTotalAccruedCents(); // sum cost from each driver
        return total/100.0; // convert cents to dollars
    }
    // Time: O(total intervals across drivers)
    double Pay_Up_To(Timestamp pay_time){ // pays all drivers up to given time
        long long paid_now=0; // accumulator
        for(auto &p:drivers)paid_now+=p.second.payUpTo(pay_time); // pay each driver
        total_paid_cents+=paid_now; // update global paid amount
        return paid_now/100.0; // return payment in dollars
    }
    // Time: O(d)
    double Total_Cost_Unpaid(){ // returns remaining unpaid cost
        long long total=0; // accumulator
        for(auto &p:drivers)total+=p.second.getTotalAccruedCents(); // compute total accrued
        return(total-total_paid_cents)/100.0; // unpaid = accrued - paid
    }
};
int main(){ // program entry point
    PayrollSystem ps; // create payroll system instance
    
    ps.Add_driver(1,20.0); // add driver with $20/hour
    
    ps.Record_delivery(1,0,3600); // record first delivery [0,3600]
    ps.Record_delivery(1,1800,5400); // overlapping delivery merges to [0,5400]
    
    cout<<"Total Cost Accrued: $"<<ps.Get_Total_Cost()<<endl; // print total accrued cost
    cout<<"Paid Now: $"<<ps.Pay_Up_To(4000)<<endl; // pay up to time 4000
    cout<<"Unpaid Cost: $"<<ps.Total_Cost_Unpaid()<<endl; // print remaining unpaid
    
    return 0; // exit program
}
