#include <bits/stdc++.h> // includes all standard libraries
using namespace std; // use std namespace
// Enum representing different access types
enum class AccessType { READ, WRITE, ADMIN }; // strongly typed access permissions
// Utility function to convert AccessType to string (for printing/debugging)
string accessTypeToString(AccessType type) { // Time: O(1)
    if (type == AccessType::READ) return "READ"; // check READ
    if (type == AccessType::WRITE) return "WRITE"; // check WRITE
    return "ADMIN"; // otherwise ADMIN
}
//============================== ACCESS MANAGEMENT SYSTEM =============================================
class EmployeeAccessManagementSystem { // main system managing employee-resource access
private:
    unordered_map<int, unordered_map<int, unordered_set<AccessType>>> access_store; //employee_id -> resource_id -> set of access types
public:
    void grant_access(int employee_id, int resource_id, AccessType access_type) {
        access_store[employee_id][resource_id].insert(access_type); // grant permission
    }
    void revoke_access(int employee_id, int resource_id, optional<AccessType> access_type) {
        // check if employee exists
        if (!access_store.count(employee_id)) return; // nothing to revoke
        // check if resource exists
        if (!access_store[employee_id].count(resource_id)) return; // nothing to revoke
        // if no access type provided → remove all access for resource
        if (!access_type.has_value()) { // revoke all access
            access_store[employee_id].erase(resource_id); // remove resource entry
        }
        else { // revoke specific access type
            access_store[employee_id][resource_id].erase(access_type.value()); // remove specific permission
            // if no access types remain, remove resource entry
            if (access_store[employee_id][resource_id].empty())
                access_store[employee_id].erase(resource_id); // cleanup empty resource
        }
        // if employee has no resources left, remove employee entry
        if (access_store[employee_id].empty())
            access_store.erase(employee_id); // cleanup empty employee
    }
    // Return all access types an employee has for a resource
    // Time: O(k) where k = number of access types (max 3)
    vector<AccessType> retrieve_access(int employee_id, int resource_id) {
        vector<AccessType> result; // store access types
        // check employee exists
        if (!access_store.count(employee_id)) return result; // return empty list
        // check resource exists
        if (!access_store[employee_id].count(resource_id)) return result; // return empty list
        // copy access types from set to vector
        for (auto &type : access_store[employee_id][resource_id])
            result.push_back(type); // add to result
        return result; // return all access types
    }
    // Return all resources an employee can access
    // Time: O(r) where r = number of resources for employee
    vector<int> retrieve_resources(int employee_id) {
        vector<int> result; // store resource ids
        // check employee exists
        if (!access_store.count(employee_id)) return result; // return empty list
        // collect all resource ids
        for (auto &p : access_store[employee_id])
            result.push_back(p.first); // add resource id
        return result; // return resources
    }
};

//======================================= DRIVER CODE ==================================================

int main() { // program entry point
    EmployeeAccessManagementSystem system; // create access management system
    system.grant_access(1, 101, AccessType::READ); // grant READ to employee 1 on resource 101
    system.grant_access(1, 101, AccessType::WRITE); // grant WRITE to employee 1 on resource 101
    system.grant_access(1, 102, AccessType::ADMIN); // grant ADMIN to employee 1 on resource 102
    system.grant_access(2, 101, AccessType::READ); // grant READ to employee 2 on resource 101
    // retrieve access for employee 1 on resource 101
    auto access = system.retrieve_access(1, 101); // get access list
    cout << "Employee 1 Access on Resource 101: "; // print label
    for (auto &a : access) cout << accessTypeToString(a) << " "; // print access types
    cout << endl; // newline
    // retrieve resources for employee 1
    auto resources = system.retrieve_resources(1); // get resources
    cout << "Employee 1 Resources: "; // print label
    for (auto &r : resources) cout << r << " "; // print resource ids
    cout << endl; // newline
    // revoke specific access
    system.revoke_access(1, 101, AccessType::WRITE); // revoke WRITE from employee 1 on resource 101
    // revoke all access
    system.revoke_access(1, 102, nullopt); // revoke all access on resource 102
    return 0; // exit program
}
