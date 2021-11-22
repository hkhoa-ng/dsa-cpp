// Datastructures.cc
//
// Student name: Khoa Nguyen
// Student email: khoa.h.nguyen@tuni.fi
// Student number: 50359141

#include "datastructures.hh"
#include <iterator>
#include <numeric>
#include <random>
#include <algorithm>
#include <cmath>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures()
{

}

Datastructures::~Datastructures()
{

}

unsigned int Datastructures::town_count()
{
    return dataset.size();
}

void Datastructures::clear_all()
{
    distance_from_origin.clear();
    return dataset.clear();
}

bool Datastructures::add_town(TownID id, const Name & name, Coord coord, int tax)
{
    if (dataset.find(id) == dataset.end()) {
        int dist = floor(sqrt(pow(coord.x, 2) + pow(coord.y, 2)));
        float total_net_tax = tax;
        struct Town new_town = {id, name, coord, tax, total_net_tax, dist};
        dataset[id] = new_town;
        distance_from_origin.insert({dist, id});
        return true;
    }
    return false;
}

Name Datastructures::get_town_name(TownID id)
{
    if (dataset.find(id) == dataset.end()) {
        return NO_NAME;
    }
    return dataset.find(id)->second._name;
}

Coord Datastructures::get_town_coordinates(TownID id)
{
    if (dataset.find(id) == dataset.end()) {
        return NO_COORD;
    }
    return dataset.find(id)->second._coord;
}

int Datastructures::get_town_tax(TownID id)
{
    if (dataset.find(id) == dataset.end()) {
        return NO_VALUE;
    }
    return dataset.find(id)->second._tax;
}

std::vector<TownID> Datastructures::all_towns()
{
    std::vector<TownID> all_towns_id = {};
    for (auto const &i : dataset) {
        all_towns_id.push_back(i.first);
    }
    return all_towns_id;
}

std::vector<TownID> Datastructures::find_towns(const Name &name)
{
    std::vector<Name> town_names = {};
    for (auto const &i : dataset) {
        if (i.second._name == name) {
            town_names.push_back(i.first);
        }
    }
    return town_names;
}

bool Datastructures::change_town_name(TownID id, const Name &newname)
{
    if (dataset.find(id) != dataset.end()) {
        dataset[id]._name = newname;
        return true;
    }
    return false;
}

std::vector<TownID> Datastructures::towns_alphabetically()
{
    std::multimap<Name, TownID> all_towns_name_ordered;
    for (auto it = dataset.begin(); it != dataset.end(); it++) {
        all_towns_name_ordered.insert({it->second._name, it->first});
    }
    std::vector<TownID> ordered_id;
    for (auto it = all_towns_name_ordered.begin(); it != all_towns_name_ordered.end(); it++) {
        ordered_id.push_back(it->second);
    }
    return ordered_id;
}

std::vector<TownID> Datastructures::towns_distance_increasing()
{
    std::vector<TownID> ordered_id;
    for (auto it = distance_from_origin.begin(); it != distance_from_origin.end(); it++) {
        // O(N)
        ordered_id.push_back(it->second);
    }
    return ordered_id;
}

TownID Datastructures::min_distance()
{
    if (distance_from_origin.empty())  {
        return NO_TOWNID;
    }
    return distance_from_origin.begin()->second;
}

TownID Datastructures::max_distance()
{
    if (distance_from_origin.empty())  {
        return NO_TOWNID;
    }
    return (distance_from_origin.rbegin())->second;
}

bool Datastructures::add_vassalship(TownID vassalid, TownID masterid)
{
    if (dataset.find(vassalid) == dataset.end() || dataset.find(masterid) == dataset.end()) {
        return false;
    }

    // Add vassal ptr to the vassals vector master
    dataset[masterid]._vassal.push_back(&dataset[vassalid]);
    // Update ptr to master in the vassal town
    dataset[vassalid]._master = &dataset[masterid];
    // Add the level of the new vassal into the levels of vassal vector in master town
    dataset[masterid]._levels_of_vassals.push_back(dataset[vassalid]._level);
    // Update vassal's net tax
    dataset[vassalid]._total_net_tax = std::round(0.9*dataset[vassalid]._total_net_tax);

    /* Traverse upward to masters of master, and update level + tax accordingly
    The idea of the total net tax is, when adding a new vassal into a vassal path,
    all the master above the new vassal will have its net tax += new vassal net tax/0.9*0.1
    Or += new vassal net tax/9
    When we get higher and higher, the tax will be += new vassal net tax/(9*10^h),
    with h being the height of the current master in the vassal path*/

    // Every master upward, will have its level += the level of the new vassal + 1
    int level_to_add = dataset[vassalid]._level + 1;
    float vassal_net_tax = dataset[vassalid]._total_net_tax;
    TownID prev_master = NO_TOWNID;
    int height_of_path = 0;
    while (true) {
        // Update the level of master town
        dataset[masterid]._level += level_to_add;
        // Since we are traversing upward, check if the town we are currently in is the
        // original master, or is it master of the original master
        if (prev_master != NO_TOWNID) {
            // Updating the vassal vector of masters of the original master
            auto vassals_of_master = dataset[masterid]._vassal;
            auto prev_master_as_vassal = &dataset[prev_master];
            // Find the position of the previous master in the current master's vassal master
            auto position_of_prev_master = std::distance(vassals_of_master.begin(),
                                                         std::find(vassals_of_master.begin(), vassals_of_master.end(), prev_master_as_vassal));
            // Use this position to update the level of the previous master in the level vector
            dataset[masterid]._levels_of_vassals[position_of_prev_master] += level_to_add;
        }
        // Stop traversing when reaching the top master of vassal path
        if (dataset[masterid]._master == nullptr) {
            // Apply the formula: += vassal net tax/(9*10^h)
            dataset[masterid]._total_net_tax += (vassal_net_tax/(9 * std::pow(10, height_of_path)));
            break;
        }
        // Update total net tax for each master upward
        //                                              total net tax before *0.9                    add that with the new vassal tax
        dataset[masterid]._total_net_tax = 0.9 * ((dataset[masterid]._total_net_tax/0.9) + vassal_net_tax/(9 * std::pow(10, height_of_path)));
        // Assign new IDs for the next loop
        prev_master = masterid;
        masterid = dataset[masterid]._master->_id;
        height_of_path++;
    }
    return true;
}

std::vector<TownID> Datastructures::get_town_vassals(TownID id) {
    std::vector<TownID> results;
    if (dataset.find(id) == dataset.end()) {
        results.push_back(NO_TOWNID);
        return results;
    }
    std::vector<Town*> const& vassals = dataset[id]._vassal;
    for (auto ptr : vassals) {
        results.push_back(ptr->_id);
    }
    return results;
}

std::vector<TownID> Datastructures::taxer_path(TownID id)
{
    std::vector<TownID> results;
    auto town_iter = dataset.find(id) ;
    if (town_iter == dataset.end()) {
        results.push_back(NO_TOWNID);
    } else {
        results.push_back(dataset[id]._id);
        while (dataset[id]._master != nullptr) {
            results.push_back(dataset[id]._master->_id);
            id = dataset[id]._master->_id;
        }
    }
    return results;
}

bool Datastructures::remove_town(TownID id)
{
    if (dataset.find(id) == dataset.end()) {
        return false;
    }
    for (auto it = distance_from_origin.begin(); it != distance_from_origin.end(); ++it) {
        if (it->second == id) {
            distance_from_origin.erase(it);
            break;
        }
    }
    auto removed_ptr = &dataset[id];
    // Get the master of the removed town (if any). If the removed town doesn't
    // have a master, it will be nullptr
    auto& master_of_removed = dataset[id]._master;
    // Get the vassals of the removed town (if any)
    auto& vassals_of_removed = dataset[id]._vassal;
    // Update ptr to new master in the vassal towns, and add them into the new master's vassals
    //for (auto it = vassals_of_removed.begin(); it != vassals_of_removed.end(); it++) {
    for (auto& vassal_ptr : vassals_of_removed) {
        vassal_ptr->_master = master_of_removed;
        // // If the removed town doesn't have a master, don't have to update level, just update
        // total net tax of its direct vassals
        if (master_of_removed == nullptr) {
            vassal_ptr->_total_net_tax /= 0.9;
        } else {
            // If master of removed town exists, add vassals of removed town into that town's
            // vassal vector, and levels vector
            master_of_removed->_vassal.push_back(vassal_ptr);
            master_of_removed->_levels_of_vassals.push_back(vassal_ptr->_level);
        }
    }
    // Every master upward from the removed town, will have its level -= 1
    int level_to_subtract = 1;
    float removed_net_tax = dataset[id]._total_net_tax;

    if (master_of_removed == nullptr) {
        // Just delete the town from data structure
        dataset.erase(dataset.find(id));
        return true;
    } else {
        // Remove the removed town from its master's vassal and level vector
        auto position_of_removed = std::distance(master_of_removed->_vassal.begin(),
                                                     std::find(master_of_removed->_vassal.begin(), master_of_removed->_vassal.end(), removed_ptr));
        master_of_removed->_vassal.erase(std::find(master_of_removed->_vassal.begin(), master_of_removed->_vassal.end(), removed_ptr));
        master_of_removed->_levels_of_vassals.erase(master_of_removed->_levels_of_vassals.begin() + position_of_removed);

        // Traversal upward from the removed town only if it has master
        TownID prev_master = NO_TOWNID;
        TownID masterid = master_of_removed->_id;
        int height_of_path = 0;

        while (true) {
            // Update the level of master town
            dataset[masterid]._level -= level_to_subtract;
            // Since we are traversing upward, check if the town we are currently in is the
            // original master, or is it master of the original master
            if (prev_master != NO_TOWNID) {
                // Updating the vassal vector of masters of the original master
                auto vassals_of_master = dataset[masterid]._vassal;
                auto prev_master_as_vassal = &dataset[prev_master];
                // Find the position of the previous master in the current master's vassal master
                auto position_of_prev_master = std::distance(vassals_of_master.begin(),
                                                             std::find(vassals_of_master.begin(), vassals_of_master.end(), prev_master_as_vassal));
                // Use this position to update the level of the previous master in the level vector
                dataset[masterid]._levels_of_vassals[position_of_prev_master] -= level_to_subtract;
            }
            // Stop traversing when reaching the top master of vassal path
            if (dataset[masterid]._master == nullptr) {
                // Apply the formula: += vassal net tax/(9*10^h)
                //dataset[masterid]._total_net_tax += std::floor((vassal_net_tax/(9 * std::pow(10, height_of_path))));
                dataset[masterid]._total_net_tax -= (removed_net_tax/(9 * std::pow(10, height_of_path)));
                break;
            }
            // Update total net tax for each master upward
            //                                              total net tax before *0.9                    add that with the new vassal tax
            //dataset[masterid]._total_net_tax = std::round(0.9 * ((dataset[masterid]._total_net_tax/0.9) + std::floor(vassal_net_tax/(9 * std::pow(10, height_of_path)))));
            dataset[masterid]._total_net_tax = 0.9 * ((dataset[masterid]._total_net_tax/0.9) - removed_net_tax/(9 * std::pow(10, height_of_path)));
            // Assign new IDs for the next loop
            prev_master = masterid;
            masterid = dataset[masterid]._master->_id;
            height_of_path++;
        }
        dataset.erase(dataset.find(id));
    }
    return true;
}

std::vector<TownID> Datastructures::towns_nearest(Coord coord)
{
    std::vector<TownID> results;
    std::multimap<int, TownID> new_dist_map;
    for (auto it = dataset.begin(); it != dataset.end(); it++) {
        Coord town_coord = it->second._coord;
        int new_dist =  sqrt(std::pow((coord.x - town_coord.x), 2) + std::pow((coord.y - town_coord.y), 2));
        new_dist_map.insert({new_dist, it->first});
    }
    for (auto const& pair : new_dist_map) {
        results.push_back(pair.second);
    }
    return results;
}

std::vector<TownID> Datastructures::longest_vassal_path(TownID id)
{
    std::vector<TownID> results;
    if (dataset.find(id) == dataset.end()) {
        results.push_back(NO_TOWNID);
        return results;
    }
    Town current_town = dataset[id];
    while (true) {
        results.push_back(current_town._id);
        std::vector<int> levels_of_current_town = dataset[id]._levels_of_vassals;
        auto max_level_iter = std::max_element(levels_of_current_town.begin(), levels_of_current_town.end());
        int index = max_level_iter - levels_of_current_town.begin();
        if (current_town._level ==  0) { break; }
        current_town = *current_town._vassal[index];
    }
    return results;
}

int Datastructures::total_net_tax(TownID id)
{
    if (dataset.find(id) == dataset.end()) {
        return NO_VALUE;
    }
    return std::floor(dataset[id]._total_net_tax);
}
