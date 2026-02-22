#include <bits/stdc++.h> // includes all standard libraries
using namespace std; // use std namespace
using Timestamp=long long; // alias for time stored as epoch seconds

class Driver{ // class representing a single driver
private:
    long long rate_per_hour_cents; // hourly rate stored in cents to avoid floating errors
//==================================PART 1==============================================================
    map<Timestamp,Timestamp> intervals; // merged non-overlapping intervals stored as start->end (sorted, unique start)
    long long total_work_seconds; // total unique worked seconds after merging intervals
//==================================PART 2==============================================================
    Timestamp paid_upto_time; // boundary till which payment already done to avoid double pay
public:
    Driver(double usd_hourly_rate=0){ // constructor initializes driver state
        rate_per_hour_cents=llround(usd_hourly_rate*100); // convert hourly rate from dollars to cents
        total_work_seconds=0; // initially no work recorded
        paid_upto_time=0; // initially nothing paid
    }
    //==================================PART 1==============================================================
    // Time: O(log n + k) where k = number of intervals merged
    void recordDelivery(Timestamp start,Timestamp end){ // records a delivery and merges overlapping intervals
        if(start>=end)throw runtime_error("Invalid interval"); // validate interval
        Timestamp new_start=start,new_end=end; // initialize new interval to insert
        auto it=intervals.lower_bound(start); // first interval with start >= given start
        if(it!=intervals.begin()){ // check previous interval for overlap
            auto prev_it=prev(it);
            if(prev_it->second>=start)it=prev_it; // move to overlapping interval
        }
        while(it!=intervals.end() && it->first<=new_end){ // merge overlapping intervals
            if(it->second<new_start){++it;continue;} // skip if no overlap
            new_start=min(new_start,it->first); // expand merged start
            new_end=max(new_end,it->second); // expand merged end
            total_work_seconds-=(it->second-it->first); // remove old duration
            it=intervals.erase(it); // erase and move iterator
        }
        intervals[new_start]=new_end; // insert merged interval
        total_work_seconds+=(new_end-new_start); // add merged duration
    }
    //==================================PART 1==============================================================
    // Time: O(1)
    long long getTotalAccruedCents()const{ // returns total accrued cost
        return(total_work_seconds*rate_per_hour_cents)/3600; // convert seconds to cents
    }
    //==================================PART 2==============================================================
    // Time: O(n intervals)
    long long payUpTo(Timestamp pay_time){ // pays all unpaid work up to given time boundary
        long long payable_seconds=0; // accumulator for unpaid seconds  
        for(auto &p:intervals){ // iterate merged intervals
            Timestamp start=p.first,end=p.second;
            if(end<=paid_upto_time)continue; // already paid
            if(start>=pay_time)break; // beyond payment boundary
            Timestamp effective_start=max(start,paid_upto_time); // unpaid start
            Timestamp effective_end=min(end,pay_time); // payable end
            if(effective_start<effective_end)
                payable_seconds+=(effective_end-effective_start);
        }
        paid_upto_time=max(paid_upto_time,pay_time); // move payment boundary forward
        return(payable_seconds*rate_per_hour_cents)/3600; // convert seconds to cents
    }
};
class PayrollSystem{ // payroll aggregator managing multiple drivers
private:
    unordered_map<int,Driver> drivers; // registry mapping driver id to Driver object
//==================================PART 2==============================================================
    long long total_paid_cents; // total amount paid across all drivers
public:
    PayrollSystem(){total_paid_cents=0;} // constructor initializes global state
    //==================================PART 1==============================================================
    // Time: O(1)
    void Add_driver(int id,double rate){ // adds new driver
        if(drivers.count(id))throw runtime_error("Driver exists");
        drivers.emplace(id,rate);
    }
    //==================================PART 1==============================================================
    // Time: O(log n)
    void Record_delivery(int id,Timestamp start,Timestamp end){ // records delivery for a driver
        if(!drivers.count(id))throw runtime_error("Driver not found");
        drivers[id].recordDelivery(start,end);
    }
    //==================================PART 1==============================================================
    // Time: O(d)
    double Get_Total_Cost(){ // returns total accrued cost across all drivers
        long long total=0;
        for(auto &p:drivers)total+=p.second.getTotalAccruedCents();
        return total/100.0;
    }
    //==================================PART 2==============================================================
    // Time: O(total intervals across drivers)
    double Pay_Up_To(Timestamp pay_time){ // pays all drivers up to given time
        long long paid_now=0;
        for(auto &p:drivers)paid_now+=p.second.payUpTo(pay_time);
        total_paid_cents+=paid_now;
        return paid_now/100.0;
    }
    // Time: O(d)
    double Total_Cost_Unpaid(){ // returns remaining unpaid cost
        long long total=0;
        for(auto &p:drivers)total+=p.second.getTotalAccruedCents();
        return(total-total_paid_cents)/100.0;
    }
};
int main(){ // program entry point
    PayrollSystem ps;

    ps.Add_driver(1,20.0);

    ps.Record_delivery(1,0,3600); // [0,3600]
    ps.Record_delivery(1,1800,5400); // merges to [0,5400]

    cout<<"Total Cost Accrued: $"<<ps.Get_Total_Cost()<<endl;
    cout<<"Paid Now: $"<<ps.Pay_Up_To(4000)<<endl;
    cout<<"Unpaid Cost: $"<<ps.Total_Cost_Unpaid()<<endl;

    return 0;
}
