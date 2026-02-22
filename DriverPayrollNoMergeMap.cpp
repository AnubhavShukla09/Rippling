#include <iostream> // for input/output
#include <unordered_map> // for unordered_map
#include <map> // for map
#include <vector> // for vector
#include <algorithm> // for sort, max, min
#include <stdexcept> // for runtime_error
#include <cmath> // for llround
using namespace std; // use std namespace
using Timestamp = long long; // alias for time stored as epoch seconds
class Driver { // class representing a single driver
private:
    long long rate_per_hour_cents; // hourly rate stored in cents
    map<Timestamp, Timestamp> deliveries; // sorted deliveries -> start -> end (unique start times only)
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
        deliveries[start] = end; // insert or overwrite delivery with same start time
        long long duration = end - start; // compute duration in seconds
        total_accrued_cents += (duration * rate_per_hour_cents) / 3600; // update accrued cost
    }
    long long getTotalAccruedCents() const { // Time: O(1)
        return total_accrued_cents; // return stored accrued earnings
    }
    const map<Timestamp, Timestamp>& getDeliveries() const { // Time: O(1)
        return deliveries; // expose read-only access to deliveries (needed for analytics)
    }

//==================================PART 2==============================================================
    long long payUpTo(Timestamp pay_time) { // Time: O(log n + k) where k = overlapping intervals
        if (pay_time <= paid_upto_time) return 0; // nothing new to pay
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
        auto it = drivers.find(id); // find driver
        if (it == drivers.end()) throw runtime_error("Driver not found"); // validate driver
        it->second.recordDelivery(start, end); // delegate recording to driver
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

//================================================PART 3=====================================
// Function to compute the maximum number of active drivers in last 24 hours
// Time Complexity: O(N log N) where N = total deliveries across all drivers
    int get_max_active_drivers_in_last_24_hours(Timestamp currentTime) {
        Timestamp window_start = currentTime - 24 * 3600; // compute start of 24-hour window (24 hours = 86400 seconds)

        vector<pair<Timestamp,int>> events; // stores events: {time, +1 for start, -1 for end}

        for (auto &p : drivers) { // iterate over all drivers in the system
            const Driver &d = p.second; // extract reference to Driver object

            for (auto &del : d.getDeliveries()) { // iterate over all deliveries of this driver
                Timestamp s = max(del.first, window_start); // clip delivery start to window start
                Timestamp e = min(del.second, currentTime); // clip delivery end to current time

                if (s < e) { // check if delivery overlaps with the 24-hour window
                    events.push_back({s, +1}); // driver becomes active at time s
                    events.push_back({e, -1}); // driver becomes inactive at time e
                }
            }
        }

        sort(events.begin(), events.end(), // sort events by time for sweep line processing
             [](auto &a, auto &b){ // custom comparator lambda
                 if (a.first == b.first) return a.second < b.second; // if same time, process end(-1) before start(+1)
                 return a.first < b.first; // otherwise sort by timestamp
             });

        int active = 0, maxActive = 0; // active = current active drivers, maxActive = peak count

        for (auto &e : events) { // iterate through events in chronological order
            active += e.second; // update active count (+1 start, -1 end)
            maxActive = max(maxActive, active); // update maximum active drivers seen so far
        }

        return maxActive; // return peak number of active drivers in last 24 hours
    }
};

int main() { // program entry point
    PayrollSystem ps; // create payroll system

    ps.Add_driver(1, 20.0); // add driver 1 with $20/hour
    ps.Add_driver(2, 30.0); // add driver 2 with $30/hour

    ps.Record_delivery(1, 100, 200); // delivery 1 for driver 1
    ps.Record_delivery(1, 100, 300); // another delivery with same start time (overwrites previous)
    ps.Record_delivery(2, 0, 7200); // 2 hour delivery for driver 2

    cout << "Total Cost Accrued: $" << ps.Get_Total_Cost() << endl; // print accrued
    cout << "Paid Now: $" << ps.Pay_Up_To(250) << endl; // pay up to time boundary
    cout << "Unpaid Cost: $" << ps.Total_Cost_Unpaid() << endl; // print remaining unpaid

    cout << "Max Active Drivers (24h): "
         << ps.get_max_active_drivers_in_last_24_hours(10000) << endl; // print analytics

    return 0; // exit program
}
