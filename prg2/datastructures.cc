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
#include <iostream>
#include <functional>

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

//
// Phase 2 operations
//


void Datastructures::clear_roads()
{
    // Replace the line below with your implementation
    //throw NotImplemented("clear_roads()");
    all_town_roads.clear();
    for (auto iter = dataset.begin(); iter != dataset.end(); ++iter) {
        iter->second._roads_to_neighbor.clear();
    }
}

std::vector<std::pair<TownID, TownID>> Datastructures::all_roads()
{
    // Replace the line below with your implementation
    //throw NotImplemented("all_roads()");
    std::sort(all_town_roads.begin(), all_town_roads.end());
    return all_town_roads;
}

bool Datastructures::add_road(TownID town1, TownID town2)
{
    // Replace the line below with your implementation
    // Also uncomment parameters ( /* param */ -> param )
    //throw NotImplemented("add_road()");
    std::pair<TownID, TownID> search_key;
    if (town1 < town2) {
        search_key = std::make_pair(town1, town2);
    } else {
        search_key = std::make_pair(town2, town1);
    }
    if (dataset.find(town1) == dataset.end() || dataset.find(town2) == dataset.end() ||
            std::find(all_town_roads.begin(), all_town_roads.end(), search_key) != all_town_roads.end()) {
        return false;
    }
    all_town_roads.push_back(search_key);
    Coord town1_coord = dataset[town1]._coord;
    Coord town2_coord = dataset[town2]._coord;
    Town* ptr_to_town1 = &dataset[town1];
    Town* ptr_to_town2 = &dataset[town2];
    int distance = std::floor(std::sqrt(std::pow(town1_coord.x - town2_coord.x, 2) + std::pow(town1_coord.y - town2_coord.y, 2)));
    dataset[town1]._roads_to_neighbor.insert({ptr_to_town2, distance});
    dataset[town2]._roads_to_neighbor.insert({ptr_to_town1, distance});
    return true;
}

std::vector<TownID> Datastructures::get_roads_from(TownID id)
{
    // Replace the line below with your implementation
    // Also uncomment parameters ( /* param */ -> param )
    //throw NotImplemented("get_roads_from()");
    std::vector<TownID> all_roads_from;
    auto const& roads_to_neighbor = dataset[id]._roads_to_neighbor;
    for (auto iter = roads_to_neighbor.begin(); iter != roads_to_neighbor.end(); ++iter) {
        all_roads_from.push_back(iter->first->_id);
    }
    std::sort(all_roads_from.begin(), all_roads_from.end());
    return all_roads_from;
}

std::vector<TownID> Datastructures::any_route(TownID fromid, TownID toid)
{
    // Replace the line below with your implementation
    // Also uncomment parameters ( /* param */ -> param )
    //throw NotImplemented("any_route()");
    std::vector<TownID> results;
    if (dataset.find(fromid) == dataset.end() || dataset.find(toid) == dataset.end()) {
        results = {NO_TOWNID};
        return results;
    }
    // Using DFS to find a
    // First, reset all the search variable for the towns
    for (auto iter = dataset.begin(); iter != dataset.end(); ++iter) {
        iter->second._state = NOT_VISITED;    // White = not visited
        iter->second._parent = nullptr;
    }
    // Create a stack to keep track of town for searching
    std::stack<Town*> town_ptrs_stack;
    // Add root town to the stack
    town_ptrs_stack.push(&dataset[fromid]);
    // Loop through the stack as long as it isn't empty
    while (!town_ptrs_stack.empty()) {
        // Get the top element as current, marked it as visited and remove it
        Town* current_town =town_ptrs_stack.top();
        current_town->_state = VISITED;   // Black = visited
        town_ptrs_stack.pop();
        // For every non visited adj. town of the current town, if it's the goal town, return
        // If not, add it to the stack. Update its parent to the current town
        for (auto iter = current_town->_roads_to_neighbor.begin(); iter != current_town->_roads_to_neighbor.end();  ++iter) {
            // If the adjacent town is not visited
            if (iter->first->_state == NOT_VISITED) {
                // Update its parent
                iter->first->_parent = current_town;
                // If the town is the same as goal, found a path, break
                //if (iter->first->_id == toid) {
                //    break;
                //}
                town_ptrs_stack.push(iter->first);
            }
        }
    }
    // Backtrack from the goal town: if it doesn't have a parent, a path is not found. If it does, found a path
    if (dataset[toid]._parent == nullptr) {
        results = {NO_TOWNID};
        return results;
    }
    //std::vector<TownID> path_from_goal_to_source;
    Town* current_town = &dataset[toid];
    while (true) {
        results.insert(results.begin(), current_town->_id);
        current_town = current_town->_parent;
        if (current_town == nullptr) { break; }
    }


    return results;
}

bool Datastructures::remove_road(TownID town1, TownID town2)
{
    // Replace the line below with your implementation
    // Also uncomment parameters ( /* param */ -> param )
    //throw NotImplemented("remove_road()");
    std::pair<TownID, TownID> search_key;
    if (town1 < town2) {
        search_key = std::make_pair(town1, town2);
    } else {
        search_key = std::make_pair(town2, town1);
    }
    if (dataset.find(town1) == dataset.end() || dataset.find(town2) == dataset.end() ||
            std::find(all_town_roads.begin(), all_town_roads.end(), search_key) == all_town_roads.end()) {
        return false;
    }
    all_town_roads.erase(std::find(all_town_roads.begin(), all_town_roads.end(), search_key));
    dataset[town1]._roads_to_neighbor.erase(dataset[town1]._roads_to_neighbor.find(&dataset[town2]));
    dataset[town2]._roads_to_neighbor.erase(dataset[town2]._roads_to_neighbor.find(&dataset[town1]));
    return true;
}

std::vector<TownID> Datastructures::least_towns_route(TownID fromid, TownID toid)
{
    // Replace the line below with your implementation
    // Also uncomment parameters ( /* param */ -> param )
    //throw NotImplemented("least_towns_route()");
    std::vector<TownID> results;
    if (dataset.find(fromid) == dataset.end() || dataset.find(toid) == dataset.end()) {
        results = {NO_TOWNID};
        return results;
    }
    // Using BFS to find route with the smallest number of towns
    // First, reset all the search variable for the towns
    for (auto iter = dataset.begin(); iter != dataset.end(); ++iter) {
        iter->second._state = NOT_VISITED;
        iter->second._parent = nullptr;
    }
    // Create a stack to keep track of town for searching
    std::queue<Town*> town_ptrs_queue;
    // Add root town to the queue
    town_ptrs_queue.push(&dataset[fromid]);
    // Set the source town to visited
    dataset[fromid]._state = VISITED;
    // Loop through the queue as long as it  isn't empty
    while (!town_ptrs_queue.empty()) {
        // Get the top element as current, remove it
        Town* current_town =town_ptrs_queue.front();
        town_ptrs_queue.pop();
        // For every non visited adj. town of the current town, if it's the goal town, return
        // If not, add it to the stack. Update its parent to the current town
        for (auto adj_town_of_current = current_town->_roads_to_neighbor.begin();
             adj_town_of_current != current_town->_roads_to_neighbor.end();  ++adj_town_of_current) {
            // If the adjacent town is not visited
            if (adj_town_of_current->first->_state == NOT_VISITED) {
                // Update its parent
                adj_town_of_current->first->_parent = current_town;
                town_ptrs_queue.push(adj_town_of_current->first);
                adj_town_of_current->first->_state = VISITED;
            }
        }
    }
    // Backtrack from the goal town: if it doesn't have a parent, a path is not found. If it does, found a path
    if (dataset[toid]._parent == nullptr) {
        results = {NO_TOWNID};
        return results;
    }
    //std::vector<TownID> path_from_goal_to_source;
    Town* current_town = &dataset[toid];
    while (true) {
        results.insert(results.begin(), current_town->_id);
        current_town = current_town->_parent;
        if (current_town == nullptr) { break; }
    }
    return results;
}

std::vector<TownID> Datastructures::road_cycle_route(TownID startid)
{
    // Replace the line below with your implementation
    // Also uncomment parameters ( /* param */ -> param )
    //throw NotImplemented("road_cycle_route()");
    std::vector<TownID> results;
    if (dataset.find(startid) == dataset.end()) {
        results = {NO_TOWNID};
        return results;
    }
    // Using DFS to find a cycle
    // First, reset all the search variable for the towns
    for (auto iter = dataset.begin(); iter != dataset.end(); ++iter) {
        iter->second._state = NOT_VISITED;
        iter->second._parent = nullptr;
    }
    // Last town in cycle
    Town* last_town_in_cycle = nullptr;
    // Create a flag to see if cycle can form
    bool can_form_cycle = false;
    // Create a stack to keep track of town for searching
    std::stack<Town*> town_ptrs_stack;
    // Add root town to the stack
    town_ptrs_stack.push(&dataset[startid]);
    // Loop through the stack as long as it isn't empty
    while (!town_ptrs_stack.empty()) {
        // Get the top element as current
        Town* current_town =town_ptrs_stack.top();
        town_ptrs_stack.pop();
        if (current_town->_state == VISITED) { break; }
        if (current_town->_state == BEING_VISITED) {

        }
        current_town->_state = BEING_VISITED;
        town_ptrs_stack.push(current_town);
        // For every non visited adj. town of the current town, if it's the goal town, return
        // If not, add it to the stack. Update its parent to the current town
        for (auto child_of_current = current_town->_roads_to_neighbor.begin(); child_of_current != current_town->_roads_to_neighbor.end();  ++child_of_current) {
            // If the adjacent town is not visited
            if (child_of_current->first->_state == NOT_VISITED) {
                // Update its parent
                child_of_current->first->_parent = current_town;
                town_ptrs_stack.push(child_of_current->first);
            } else if (child_of_current->first->_state == BEING_VISITED) {
                // Found a cycle
                can_form_cycle = true;
                // Update its parent
                child_of_current->first->_parent = current_town;
                last_town_in_cycle = child_of_current->first;
                break;
            }
        }

        current_town->_state = VISITED;
    }
    if (!can_form_cycle) {
        results = {NO_TOWNID};
        return results;
    }
    // Back track to form the cycle
    while (true) {
        results.insert(results.begin(), last_town_in_cycle->_id);
        last_town_in_cycle = last_town_in_cycle->_parent;
        if (last_town_in_cycle == nullptr) { break; }
    }
    return results;
}

std::vector<TownID> Datastructures::shortest_route(TownID fromid, TownID toid)
{
    // Replace the line below with your implementation
    // Also uncomment parameters ( /* param */ -> param )
    //throw NotImplemented("shortest_route()");
    std::vector<TownID> results;
    if (dataset.find(fromid) == dataset.end() || dataset.find(toid) == dataset.end()) {
        results = {NO_TOWNID};
        return results;
    }
    // Using Dijkstra's algorithm to find the shortest route
    // First, reset all the search variable for the towns
    for (auto iter = dataset.begin(); iter != dataset.end(); ++iter) {
        iter->second._state = NOT_VISITED;
        iter->second._parent = nullptr;
        iter->second._distance_from_root = INT_MAX;
    }
    // Assign distance from root of root node to 0 (root's distance to itself is 0)
    dataset[fromid]._weight = 0;
    std::priority_queue<Town*, std::vector<Town*>, dereference_compare_node> town_ptrs_prior_queue;
    town_ptrs_prior_queue.push(&dataset[fromid]);
    // Looping through the queue as long as it's not empty
    while (!town_ptrs_prior_queue.empty()){
        // Choose node with min distance from root
        Town* current_town = town_ptrs_prior_queue.top();
        // Remove it from queue, and marked it as visited
        current_town->_state = VISITED;
        town_ptrs_prior_queue.pop();
        // If the current node is the goal node, stop the loop
        if (current_town->_id == toid)  {
            // Denote parent to backtrack
            //current_town->_parent = prev_parrent;
            break;
        }
        // For every neigbor town of current town
        for (auto& adj_town_of_current : current_town->_roads_to_neighbor) {
            // If this neigbor is visited, skip this iteration
            if (adj_town_of_current.first->_state == VISITED) { continue; }
            int distance_from_current_to_this_child = adj_town_of_current.second;
            int temp_distance = current_town->_distance_from_root + distance_from_current_to_this_child;
            if (temp_distance < adj_town_of_current.first->_distance_from_root) {
                // A shorter path is found
                adj_town_of_current.first->_distance_from_root = temp_distance;
                // Push Town to prior queue
                town_ptrs_prior_queue.push(adj_town_of_current.first);
                // Denote parent to backtrack
                adj_town_of_current.first->_parent = current_town;
            }
        }
    }
    // Backtrack from the goal town: if it doesn't have a parent, a path is not found. If it does, found a path
    if (dataset[toid]._parent == nullptr) {
        results = {NO_TOWNID};
        return results;
    }
    Town* current_town = &dataset[toid];
    while (true) {
        results.insert(results.begin(), current_town->_id);
        current_town = current_town->_parent;
        if (current_town == nullptr) { break; }
    }
    return results;
}

Distance Datastructures::trim_road_network()
{
    // Replace the line below with your implementation
    //throw NotImplemented("trim_road_network()");
    Distance result = 0;
    // Using Prim's algorithm to find the minimum spanning tree (MST)
    // First, reset all the search variable for the towns
    for (auto iter = dataset.begin(); iter != dataset.end(); ++iter) {
        iter->second._state = NOT_VISITED;
        iter->second._parent = nullptr;
        iter->second._weight = INT_MAX;
    }
    // Create an unordered_map to store all the roads, along with their weight for the MST
    //std::unordered_map<std::pair<TownID, TownID>, Distance> roads_and_dist;
    std::vector<std::pair<TownID, TownID>> MST_roads;
    // Pick any initial town as the starting point for Prim's algorithm
    // Let's pick the first town in the data structure
    Town* source = &dataset.begin()->second;
    // Set its cost (weight) as 0, since it's the source town
    source->_weight = 0;
    // And its parent to itself
    source->_parent = source;
    // Create a min priority queue, using weight source as key
    std::priority_queue<Town*, std::vector<Town*>, dereference_compare_node> town_ptrs_prior_queue;
    // Add source town
    town_ptrs_prior_queue.push(source);
    // Loop through the loop aslong as it isn't empty
    while (!town_ptrs_prior_queue.empty()){
        // Choose node with min distance from root
        Town* current_town = town_ptrs_prior_queue.top();
        // Remove it from queue, and marked it as visited
        town_ptrs_prior_queue.pop();
        current_town->_state = VISITED;
        // Add the edge of that town into the MST edge
        std::pair<TownID, TownID> edge_name;
        if (current_town->_id < current_town->_parent->_id) {
            edge_name = std::make_pair(current_town->_id, current_town->_parent->_id);
        } else {
            edge_name = std::make_pair(current_town->_parent->_id, current_town->_id);
        }
        //roads_and_dist.insert({edge_name, current_town->_weight});
        MST_roads.push_back(edge_name);
        // For every neigbor town of current town
        for (auto& adj_town_of_current : current_town->_roads_to_neighbor) {
            // If this neigbor is visited, skip this iteration
            if (adj_town_of_current.first->_state == VISITED) { continue; }
            // Push neighbors to prior queue
            town_ptrs_prior_queue.push(adj_town_of_current.first);
            int distance_from_current_to_this_child = adj_town_of_current.second;
            // Update new weight for the edge from this parent to this child, if
            // the weight is less than the current weight
            if (distance_from_current_to_this_child < adj_town_of_current.first->_weight) {
                // A shorter path is found
                adj_town_of_current.first->_weight = distance_from_current_to_this_child;
                // Denote parent
                adj_town_of_current.first->_parent = current_town;
            }
        }
    }
    // Trimming
    for (auto const& edge : all_town_roads) {
        if (std::find(MST_roads.begin(), MST_roads.end(), edge) == MST_roads.end()) {
            remove_road(edge.first, edge.second);
        }
    }
    // Calculate total distance
    for (auto const& edge : all_town_roads) {
        Coord town1 = dataset[edge.first]._coord;
        Coord town2 = dataset[edge.second]._coord;
        Distance distance = std::sqrt(std::pow(town1.x - town2.x, 2) + std::pow(town1.y - town2.y, 2));
        result += distance;
    }
    return result;
}
