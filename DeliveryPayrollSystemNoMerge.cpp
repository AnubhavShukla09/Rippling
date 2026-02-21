#include <bits/stdc++.h> // includes all standard libraries
using namespace std; // use std namespace
using Timestamp = long long; // alias for time stored as epoch seconds
class Driver { // class representing a single driver
private:
    long long rate_per_hour_cents; // hourly rate stored in cents to avoid floating errors
    set<pair<Timestamp, Timestamp>> deliveries; // sorted set storing deliveries as {start,end}
    long long total_accrued_cents; // total earned amount (accrued but not necessarily paid)
//=========================PART 2================================================================
    Timestamp paid_upto_time; // boundary till which payment is already done to avoid double pay
public:
    Driver(double usd_hourly_rate = 0) { // constructor initializes driver state (default needed for map)
        rate_per_hour_cents = llround(usd_hourly_rate * 100); // convert hourly rate from dollars to cents
        total_accrued_cents = 0; // initially no earnings
        paid_upto_time = 0; // initially nothing is paid
    }
    void recordDelivery(Timestamp start, Timestamp end) { // Time: O(log n) inserts delivery in sorted set
        if (start >= end) throw runtime_error("Invalid interval"); // validate that delivery interval is valid
        deliveries.insert({start, end}); // insert delivery into sorted set automatically ordered by start time
        long long duration = end - start; // compute delivery duration in seconds
        total_accrued_cents += (duration * rate_per_hour_cents) / 3600; // update accrued cost based on hourly rate
    }
    long long getTotalAccruedCents() const { // Time: O(1) returns total accrued cost
        return total_accrued_cents; // return stored total earnings
    }
//=========================PART 2================================================================
    long long payUpTo(Timestamp pay_time) { // Time: O(log n + k) where k = intervals overlapping payment boundary
        long long payable_seconds = 0; // store total seconds to pay in this call
        auto it = deliveries.lower_bound({paid_upto_time, LLONG_MIN}); // find first interval starting at or after unpaid boundary
        if (it != deliveries.begin()) --it; // move one step back to check interval overlapping boundary
        for (; it != deliveries.end(); ++it) { // iterate through relevant deliveries
            auto &[start, end] = *it; // extract start and end from pair
            if (start >= pay_time) break; // stop if delivery starts after payment boundary
            Timestamp effective_start = max(start, paid_upto_time); // compute unpaid start portion
            Timestamp effective_end = min(end, pay_time); // compute payable end portion
            if (effective_start < effective_end) payable_seconds += (effective_end - effective_start); // add payable duration
        }
        paid_upto_time = max(paid_upto_time, pay_time); // update payment boundary to prevent re-paying
        return (payable_seconds * rate_per_hour_cents) / 3600; // convert payable seconds to cents and return
    }
};
class PayrollSystem { // main payroll aggregator class
private:
    unordered_map<int, Driver> drivers; // registry mapping driver id to driver object directly (no pointers)
    long long total_paid_cents; // total amount paid across all drivers

public:
    PayrollSystem() { // constructor initializes global payment state
        total_paid_cents = 0; // initially nothing paid
    }
    void Add_driver(int id, double rate) { // Time: O(1) adds new driver
        if (drivers.count(id)) throw runtime_error("Driver exists"); // ensure driver id not duplicated
        drivers.emplace(id, Driver(rate)); // insert new driver object directly
    }
    void Record_delivery(int id, Timestamp start, Timestamp end) { // Time: O(log n) records delivery for driver
        if (!drivers.count(id)) throw runtime_error("Driver not found"); // validate driver exists
        drivers[id].recordDelivery(start, end); // delegate recording to driver object
    }
    double Get_Total_Cost() { // Time: O(d) where d = number of drivers
        long long total = 0; // accumulator for total accrued cost
        for (auto &p : drivers) total += p.second.getTotalAccruedCents(); // sum accrued cost of each driver
        return total / 100.0; // convert cents to dollars and return
    }
//=========================PART 2================================================================
    double Pay_Up_To(Timestamp pay_time) { // Time: O(total intervals across drivers)
        long long paid_now = 0; // accumulator for amount paid in this call
        for (auto &p : drivers) paid_now += p.second.payUpTo(pay_time); // pay each driver up to given time
        total_paid_cents += paid_now; // update global paid amount
        return paid_now / 100.0; // return payment in dollars
    }
    double Total_Cost_Unpaid() { // Time: O(d) computes remaining unpaid cost
        long long total = 0; // accumulator for total accrued
        for (auto &p : drivers) total += p.second.getTotalAccruedCents(); // compute total accrued
        return (total - total_paid_cents) / 100.0; // unpaid = accrued - paid
    }
};

int main() { // program entry point
    PayrollSystem ps; // create payroll system instance
    ps.Add_driver(1, 20.0); // add driver 1 with $20/hour
    ps.Add_driver(2, 30.0); // add driver 2 with $30/hour
    ps.Record_delivery(1, 0, 3600); // record 1 hour delivery for driver 1
    ps.Record_delivery(1, 1800, 5400); // record overlapping delivery for driver 1 (paid independently)
    ps.Record_delivery(2, 0, 7200); // record 2 hour delivery for driver 2
    cout << "Total Cost Accrued: $" << ps.Get_Total_Cost() << endl; // print total accrued cost
    cout << "Paid Now: $" << ps.Pay_Up_To(4000) << endl; // pay all drivers up to time 4000
    cout << "Unpaid Cost: $" << ps.Total_Cost_Unpaid() << endl; // print remaining unpaid cost
    return 0; // exit program
}
