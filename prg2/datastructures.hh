// Datastructures.hh
//
// Student name: Khoa Nguyen
// Student email: khoa.h.nguyen@tuni.fi
// Student number: 50359141

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <limits>
#include <functional>
#include <exception>
#include <unordered_map>
#include <map>
#include <stack>
#include <queue>


// Types for IDs
using TownID = std::string;
using Name = std::string;

// Return values for cases where required thing was not found
TownID const NO_TOWNID = "----------";

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
Name const NO_NAME = "!!NO_NAME!!";

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};

// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

struct CoordHash
{
    std::size_t operator()(Coord xy) const
    {
        auto hasher = std::hash<int>();
        auto xhash = hasher(xy.x);
        auto yhash = hasher(xy.y);
        // Combine hash values (magic!)
        return xhash ^ (yhash + 0x9e3779b9 + (xhash << 6) + (xhash >> 2));
    }
};

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

// Type for a distance (in metres)
using Distance = int;

// Return value for cases where Distance is unknown
Distance const NO_DISTANCE = NO_VALUE;

// This exception class is there just so that the user interface can notify
// about operations which are not (yet) implemented
class NotImplemented : public std::exception
{
public:
    NotImplemented() : msg_{} {}
    explicit NotImplemented(std::string const& msg) : msg_{msg + " not implemented"} {}

    virtual const char* what() const noexcept override
    {
        return msg_.c_str();
    }
private:
    std::string msg_;
};


// This is the class you are supposed to implement

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: constant
    // Short rationale for estimate: unordered_map::size() has constant time complexity
    unsigned int town_count();

    // Estimate of performance: linear in the number of towns
    // Short rationale for estimate: unordered_map::clear() has linear time complexity depends on the number of elements
    void clear_all();

    // Estimate of performance: average constant, worst case logarithmic
    // Short rationale for estimate: unordered_map::insert() has constant time. Worst case happens when the size of the
    // unordered_map has to be increased
    bool add_town(TownID id, Name const& name, Coord coord, int tax);

    // Estimate of performance: average constant, worst case linear on the size of container
    // Short rationale for estimate: unordered_map::find() has constant time. Worst case happen when the find key doesn't exist in map
    Name get_town_name(TownID id);

    // Estimate of performance: average constant, worst case linear on the size of container
    // Short rationale for estimate: unordered_map::find() has constant time. Worst case happen when the find key doesn't exist in map
    Coord get_town_coordinates(TownID id);

    // Estimate of performance: unordered_map::find() has constant time. Worst case happen when the find key doesn't exist in map
    // Short rationale for estimate: unordered_map::find() has constant time. Worst case happen when the find key doesn't exist in map
    int get_town_tax(TownID id);

    // Estimate of performance: armortized linear
    // Short rationale for estimate: for loop: linear with vector::push_back: armotized constant inside the loop
    std::vector<TownID> all_towns();

    // Estimate of performance: armortized linear
    // Short rationale for estimate: for loop: linear with vector::push_back: armotized constant inside the loop
    std::vector<TownID> find_towns(Name const& name);

    // Estimate of performance: constant on average, worst case linear with container size
    // Short rationale for estimate: unordered_map::find and unordered_map::at has constant time.
    // Worst case happen when find key doesn't exist in container
    bool change_town_name(TownID id, Name const& newname);

    // Estimate of performance: linearithmic in the size of the container.
    // Short rationale for estimate: for loop: linear, multimap::insert() in every loop: logarithmic
    std::vector<TownID> towns_alphabetically();

    // Estimate of performance: linear
    // Short rationale for estimate: for loop, then vector::push_back in constant time
    std::vector<TownID> towns_distance_increasing();

    // Estimate of performance: constant time
    // Short rationale for estimate: return only
    TownID min_distance();

    // Estimate of performance: constant time
    // Short rationale for estimate: return only
    TownID max_distance();

    // Estimate of performance: best case linear, worst case quadratic time
    // Short rationale for estimate: while loop, std::distance() runs in linear, or constant time in best case
    bool add_vassalship(TownID vassalid, TownID masterid);

    // Estimate of performance: linear
    // Short rationale for estimate: for loop linear, inside vector::push_back() in constant time
    std::vector<TownID> get_town_vassals(TownID id);

    // Estimate of performance: linear
    // Short rationale for estimate: for loop linear, inside vector::push_back() in constant time
    std::vector<TownID> taxer_path(TownID id);

    // Non-compulsory phase 1 operations

    // Estimate of performance: best case linear, worst case quadratic time
    // Short rationale for estimate: while loop, std::distance() runs in linear, or constant time in best case
    bool remove_town(TownID id);

    // Estimate of performance: linearithmic in the size of container
    // Short rationale for estimate: for loop has linear time, each loop multimap::insert() logarithmic time
    std::vector<TownID> towns_nearest(Coord coord);

    // Estimate of performance: quadratic, linear in best case when town has 0 to 1 vassal
    // Short rationale for estimate: while loop linear, with std::max_element() has time max{N-1, 0},
    // where N = std::distance(first, last)
    std::vector<TownID> longest_vassal_path(TownID id);

    // Estimate of performance: constant
    // Short rationale for estimate: multimap::at in const, and then return
    int total_net_tax(TownID id);

    // Phase 2 operations

    // Estimate of performance: quadratic in the size of containers
    // Short rationale for estimate: for loop linear, vector::clear() inside for loop linear in the size of container
    void clear_roads();

    // Estimate of performance: linearithmic
    // Short rationale for estimate: std::sort() is O(NlogN)
    std::vector<std::pair<TownID, TownID>> all_roads();

    // Estimate of performance: linear in worst case
    // Short rationale for estimate: std::vector::insert() linear depends on distance between pos and end
    bool add_road(TownID town1, TownID town2);

    // Estimate of performance: linearithmic
    // Short rationale for estimate: for loop with vector::push_back() linear, then sort() the  vector for O(NlogN)
    std::vector<TownID> get_roads_from(TownID id);

    // Estimate of performance: linear on average, quadratic on worst case (doesn't have any route and traverse whole graph)
    // Short rationale for estimate: DFS, so O(V+E), or O(N). Backtrack with while loop, and vector::insert() so O(N^2)
    std::vector<TownID> any_route(TownID fromid, TownID toid);

    // Non-compulsory phase 2 operations

    // Estimate of performance: linear
    // Short rationale for estimate: vector::erase() is O(N) depend on the distance between pos and end
    bool remove_road(TownID town1, TownID town2);

    // Estimate of performance: linear on average, quadratic on worst case (doesn't have any route and traverse whole graph)
    // Short rationale for estimate: BFS, so O(V+E), or O(N). Backtrack with while loop, and vector::insert() so O(N^2)
    std::vector<TownID> least_towns_route(TownID fromid, TownID toid);

    // Estimate of performance: linear on average, quadratic on worst case (doesn't have any route and traverse whole graph)
    // Short rationale for estimate: DFS, so O(V+E), or O(N). Backtrack with while loop, and vector::insert() so O(N^2)
    std::vector<TownID> road_cycle_route(TownID startid);

    // Estimate of performance: linearithmic for the algorithm, quadratic in the backtracking step
    // Short rationale for estimate: Dijkstra's shortest path algorithm is O(ELogV). Backtrack with while loop, and vector::insert() so O(N^2)
    std::vector<TownID> shortest_route(TownID fromid, TownID toid);

    // Estimate of performance: quadratic
    // Short rationale for estimate: Prim's algorithm to traverse the whole graph, so O(VE).
    Distance trim_road_network();


private:
    // Add stuff needed for your class implementation
    int INT_MAX = 1000000;
    enum State {VISITED, NOT_VISITED};
    struct Town {
        TownID _id;
        Name _name;
        Coord _coord;
        int _tax;
        float _total_net_tax;
        Distance _distance;
        Town* _master = nullptr;
        std::vector<Town*> _vassal = {};
        int _level = 0;
        std::vector<int> _levels_of_vassals = {};
        std::unordered_map<Town*, int> _roads_to_neighbor ={};
        State _state = NOT_VISITED;
        Town* _parent = nullptr;
        int _weight = 1000000;  // A very big number
    };
    struct Road {
        TownID _from;
        TownID _to;
        int _distance;
    };

    std::unordered_map<TownID, Town> dataset;
    std::multimap<int, TownID> distance_from_origin;
    //std::unordered_map<std::pair<TownID, TownID>, int> all_town_roads;
    std::vector<std::pair<TownID, TownID>> all_town_roads;

    struct dereference_compare_node : public std::binary_function<Town*, Town*, bool>
    {
        bool operator()(const Town* lhs, const Town* rhs) const
        {
            return lhs->_weight > rhs->_weight;
        }
    };

};

#endif // DATASTRUCTURES_HH
