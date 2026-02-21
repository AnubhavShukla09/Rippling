#include <bits/stdc++.h> // includes all standard libraries
using namespace std; // use std namespace
using Timestamp = long long; // alias for time stored as epoch seconds

class Driver { // class representing a single driver
private:
    long long rate_per_hour_cents; // hourly rate stored in cents
    multimap<Timestamp, Timestamp> deliveries; // sorted deliveries allowing duplicate start times -> start -> end
    long long total_accrued_cents; // total accrued earnings
//==================================PART 2==============================================================
    Timestamp paid_upto_time; // boundary till which payment already done to avoid double pay

public:
    Driver(double usd_hourly_rate = 0) { // constructor initializes driver state
        rate_per_hour_cents = llround(usd_hourly_rate * 100); // convert dollars to cents
        total_accrued_cents = 0; // initially no earnings
        paid_upto_time = 0; // initially nothing paid
    }
    void recordDelivery(Timestamp start, Timestamp end) { // Time: O(log n)
        if (start >= end) throw runtime_error("Invalid interval"); // validate delivery interval
        deliveries.emplace(start, end); // insert delivery while allowing duplicate start times
        long long duration = end - start; // compute duration in seconds
        total_accrued_cents += (duration * rate_per_hour_cents) / 3600; // update accrued cost
    }
    long long getTotalAccruedCents() const { // Time: O(1)
        return total_accrued_cents; // return stored accrued earnings
    }
//==================================PART 2==============================================================
    long long payUpTo(Timestamp pay_time) { // Time: O(log n + k) where k = overlapping intervals
        long long payable_seconds = 0; // accumulator for payable second
        auto it = deliveries.lower_bound(paid_upto_time); // find first delivery starting after unpaid boundary
        if (it != deliveries.begin()) --it; // check previous interval which may overlap boundary
        for (; it != deliveries.end(); ++it) { // iterate relevant deliveries
            Timestamp start = it->first; // extract start time
            Timestamp end = it->second; // extract end time
            if (start >= pay_time) break; // stop if beyond payment boundary
            Timestamp effective_start = max(start, paid_upto_time); // unpaid portion start
            Timestamp effective_end = min(end, pay_time); // payable portion end
            if (effective_start < effective_end) // if valid unpaid portion exists
                payable_seconds += (effective_end - effective_start); // add payable duration
        }
        paid_upto_time = max(paid_upto_time, pay_time); // move payment boundary forward
        return (payable_seconds * rate_per_hour_cents) / 3600; // convert payable seconds to cents
    }
};
class PayrollSystem { // payroll aggregator managing multiple drivers
private:
    unordered_map<int, Driver> drivers; // driver registry storing Driver objects directly
//==================================PART 2==============================================================
    long long total_paid_cents; // total amount paid globally
public:
    PayrollSystem() { total_paid_cents = 0; } // constructor initializes state
    void Add_driver(int id, double rate) { // Time: O(1)
        if (drivers.count(id)) throw runtime_error("Driver exists"); // prevent duplicate driver
        drivers.emplace(id, rate); // insert driver directly
    }
    void Record_delivery(int id, Timestamp start, Timestamp end) { // Time: O(log n)
        if (!drivers.count(id)) throw runtime_error("Driver not found"); // validate driver
        drivers[id].recordDelivery(start, end); // delegate recording to driver
    }
    double Get_Total_Cost() { // Time: O(d) where d = number of drivers
        long long total = 0; // accumulator
        for (auto &p : drivers) total += p.second.getTotalAccruedCents(); // sum accrued cost
        return total / 100.0; // return in dollars
    }
//==================================PART 2==============================================================
    double Pay_Up_To(Timestamp pay_time) { // Time: O(total intervals across drivers)
        long long paid_now = 0; // accumulator
        for (auto &p : drivers) paid_now += p.second.payUpTo(pay_time); // pay each driver
        total_paid_cents += paid_now; // update global payment
        return paid_now / 100.0; // return in dollars
    }
    double Total_Cost_Unpaid() { // Time: O(d)
        long long total = 0; // accumulator
        for (auto &p : drivers) total += p.second.getTotalAccruedCents(); // compute total accrued
        return (total - total_paid_cents) / 100.0; // unpaid amount
    }
};
int main() { // program entry point
    PayrollSystem ps; // create payroll system

    ps.Add_driver(1, 20.0); // add driver 1 with $20/hour
    ps.Add_driver(2, 30.0); // add driver 2 with $30/hour

    ps.Record_delivery(1, 100, 200); // delivery 1 for driver 1
    ps.Record_delivery(1, 100, 300); // another delivery with same start time (handled correctly)
    ps.Record_delivery(2, 0, 7200); // 2 hour delivery for driver 2

    cout << "Total Cost Accrued: $" << ps.Get_Total_Cost() << endl; // print accrued
    cout << "Paid Now: $" << ps.Pay_Up_To(250) << endl; // pay up to time boundary
    cout << "Unpaid Cost: $" << ps.Total_Cost_Unpaid() << endl; // print remaining unpaid

    return 0; // exit program
}
