#include <cassert>
#include <thread>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <functional>
#include <sstream>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <deque>
#include <mutex>

void printState();


using Coord = std::pair<std::size_t, std::size_t>;

class Node {
    friend void printState();

    const Coord coord;
    const std::size_t buildings;
    std::size_t neighboursCount;
    //std::mutex mtx;

    enum class Direction : char { UP, LEFT, DOWN, RIGHT };
    enum class State : char { NO, UNKNOWN, BEFORE, AFTER };

    std::array<State, 4> neighbours;

    //std::set<Node*> unknown;
    //std::set<Node*> before;
    //std::set<Node*> after;

    inline State& getState(Direction d) {
        return neighbours[static_cast<int>(d)];
    }
    
    inline const State& getState(Direction d) const {
        return neighbours[static_cast<int>(d)];
    }

    inline Node* getNeighbour(Direction d) {
        return this + directionSlider[static_cast<int>(d)];
    }
    
    inline const Node* getNeighbour(Direction d) const {
        return this + directionSlider[static_cast<int>(d)];
    }

    inline void setEdge(Direction d, bool isBefore) {
        if(getState(d) == State::UNKNOWN) {
            getState(d) = isBefore ? State::BEFORE : State::AFTER;
            getNeighbour(d)->getState(static_cast<Direction>((static_cast<int>(d) + 2) % 4)) = !isBefore ? State::BEFORE : State::AFTER;
        }
    }

    char beforeOrAfter(Direction d, char beforeChar = '<', char afterChar = '>', char unknownChar = ' ') const {
        switch(getState(d)) {
        case State::BEFORE:
            return beforeChar;
        case State::AFTER:
            return afterChar;
        case State::NO:
            return '?';
        default:
            return unknownChar;
        }
    }

    bool has(State hasThisState) const {
        for(State s : neighbours) {
            if(s == hasThisState)
                return true;
        }
        return false;
    }

    inline std::size_t countOf(State countOfThisState) const {
        return std::count(neighbours.begin(), neighbours.end(), countOfThisState);
    }

public:
    static std::array<int, 4> directionSlider;
    static Coord maxNode;
    static std::vector<Node> nodes;

    Node(Coord coord, std::size_t buildings) : coord(coord), buildings(buildings), neighboursCount(0), neighbours{} {
        if(coord.first > 0) {
            getState(Direction::UP) = State::UNKNOWN;
            ++neighboursCount;
        }
        if(coord.first < maxNode.first - 1) {
            getState(Direction::DOWN) = State::UNKNOWN;
            ++neighboursCount;
        }
        if(coord.second > 0) {
            getState(Direction::LEFT) = State::UNKNOWN;
            ++neighboursCount;
        }
        if(coord.second < maxNode.second - 1) {
            getState(Direction::RIGHT) = State::UNKNOWN;
            ++neighboursCount;
        }
    }
    
    Node(const Node& oth) :
        coord(oth.coord),
        buildings(oth.buildings),
        neighboursCount(oth.neighboursCount),
        neighbours(oth.neighbours)
    {}
    
    Node& operator=(const Node& oth) {
        neighbours = oth.neighbours;
    }

    static void findEdges(Coord start, Coord end) {
        std::deque<Node*> changed;
        
        const int diffX = start.first == end.first ? 0 : start.first < end.first ? 1 : -1;
        const int diffY = start.second == end.second ? 0 : start.second < end.second ? 1 : -1;

        while(start != end) {
            end.first -= diffX;
            end.second -= diffY;
            changed.push_back(get(end));
        }
        findEdgesD(changed);
    }
    static bool findEdgesD(std::deque<Node*> changed) {
        while(!changed.empty()) {
            Node& node = *changed.back();
            changed.pop_back();

            if(!node.checkAndAddEdges(changed)) return false;
        }
        return true;
    }
    
    static bool tryEdges() {
        Node* firstUN = nullptr;
        for(Node& n : nodes) {
            if(n.has(State::UNKNOWN) && n.buildings == 1) {
                const std::size_t index = &n - &nodes[0];
                auto cp = nodes;
                std::deque<Node*> tmp;
                n.setUnknownsToAndGetNodes(true, tmp);
                
                if(findEdgesD(tmp) && tryEdges()) {
                    return true;
                } else {
                    nodes = cp;
                    tmp.clear();
                    
                    nodes[index].setUnknownsToAndGetNodes(false, tmp);
                    const bool res = findEdgesD(tmp) && tryEdges();

                    if(!res) {
                        nodes = std::move(cp);
                    }
                    return res;
                }
            }
            if(firstUN == nullptr && n.has(State::UNKNOWN)) {
                firstUN = &n;
            }
        }
        if(firstUN == nullptr) {
            return true;
        }
        
        const std::size_t index = firstUN - &nodes[0];
        auto cp = nodes;
        
        Direction winner;
        for(int i = 0; i < 4; ++i) {
            if(firstUN->neighbours[i] == State::UNKNOWN) {
                winner = static_cast<Direction>(i);
                break;
            }
        }
        firstUN->setEdge(winner, true);
        
        if(findEdgesD({firstUN->getNeighbour(winner), firstUN}) && tryEdges()) {
            return true;
        } else {
            nodes = cp;
            
            nodes[index].setEdge(winner, false);
            const bool res = findEdgesD({nodes[index].getNeighbour(winner), &nodes[index]}) && tryEdges();
            
            if(!res) {
                nodes = std::move(cp);
            }
            return res;
        }
    }

    static inline Node* get(const Coord& coord) {
        return get(coord.first, coord.second);
    }

    static inline Node* get(std::size_t i, std::size_t j) {
        return &nodes[i * maxNode.second + j];
    }
    
    struct Lambda {
        std::vector<Coord>& order;
        std::unordered_set<const Node*> used;
        
        Lambda(std::vector<Coord>& order) : order(order) {}
        
        inline void visit(const Node* node) {
            if(!used.count(node)) {
                used.insert(node);
                for(int i = 0; i < 4; ++i) {
                    if(node->neighbours[i] == State::BEFORE) {
                        visit(node->getNeighbour(static_cast<Direction>(i)));
                    }
                }

                order.push_back(node->coord);
            }
        }
    };

    static void topOrder(std::vector<Coord>& order) {
        order.clear();
        order.reserve(Node::nodes.size());

        Lambda lambda(order);
        
        for(Node& n : nodes) {
            lambda.visit(&n);
        }
    }

    inline void setUnknownsToAndGetNodes(bool isBefore, std::deque<Node*>& nexts) {
        for(int i = 0; i < 4; ++i) {
            if(neighbours[i] == State::UNKNOWN) {
                setEdge(static_cast<Direction>(i), isBefore);
                nexts.push_back(getNeighbour(static_cast<Direction>(i)));
            }
        }
    }

    inline bool checkAndAddEdges(std::deque<Node*>& nexts) {
        
        //bool success = mtx.try_lock(); // sok a cache miss. Mégse jó 
        //if(success) return true;
        //std::lock_guard<std::mutex> lck (mtx, std::adopt_lock);
        
        if(!has(State::UNKNOWN)) {
            bool success = countOf(State::AFTER) + 1 == buildings || (neighboursCount == 4 && buildings == 1 && countOf(State::AFTER) == 4);
            
            return success;
        }
            
        // before everyone -> neighbours built after
        if(neighboursCount + 1 == buildings) {
            setUnknownsToAndGetNodes(false, nexts);
        } else
        // after everyone -> neighbours built before
        if(neighboursCount < 4 && buildings == 1) {
            setUnknownsToAndGetNodes(true, nexts);
        } else
        // edgecase -> all before, or all after
        if(neighboursCount == 4 && buildings == 1) {
            if(has(State::BEFORE)) {
                setUnknownsToAndGetNodes(true, nexts);
            } else if(has(State::AFTER)) {
                setUnknownsToAndGetNodes(false, nexts);
            } else {
                return true;
            }
        } else
        // all before found
        if(countOf(State::AFTER) + 1 == buildings) {
            setUnknownsToAndGetNodes(true, nexts);
        } else
        // all after found (unknowns missing from before)
        if(countOf(State::AFTER) + 1 + countOf(State::UNKNOWN) == buildings) {
            setUnknownsToAndGetNodes(false, nexts);
        } else
        {
            // do some trick
            return true;
            
        }
        
        bool success = countOf(State::AFTER) + 1 == buildings || (neighboursCount == 4 && buildings == 1 && countOf(State::AFTER) == 4);
        
        return success;
    }
};

std::array<int, 4> Node::directionSlider;
Coord Node::maxNode{};
std::vector<Node> Node::nodes;

void printState() {
    std::ostream& of(std::cout);
    for(std::size_t i = 0; i < Node::maxNode.first; ++i) {
        std::stringstream ss;

        for(std::size_t j = 0; j < Node::maxNode.second; ++j) {
            Node* node = Node::get(i, j);

            if(j != 0) {
                ss << node->beforeOrAfter(Node::Direction::LEFT);
            }

            ss << node->buildings;

            if(i != 0) {
                of << node->beforeOrAfter(Node::Direction::UP, '^', 'V') << ' ';
            }
        }
        of << std::endl << ss.str() << std::endl;
    }
    of  << std::endl  << std::endl;
}

void CalculateBuildOrder(const std::vector< std::vector<int> >& buildings,
	std::vector< std::pair<std::size_t, std::size_t> >& solution)
{
    Node::maxNode = std::make_pair(buildings.size(), buildings[0].size());
    Node::nodes.clear();
    Node::nodes.reserve(buildings.size() * buildings[0].size());
    Node::directionSlider = {-buildings[0].size(), -1, buildings[0].size(), 1};

    for(std::size_t i = 0; i < buildings.size(); ++i) {
        for(std::size_t j = 0; j < buildings[i].size(); ++j) {
            Node::nodes.emplace_back(std::make_pair(i, j), buildings[i][j]);
        }
    }

    //std::thread t1(&Node::findEdges, Coord{buildings.size() - 1, 0}, Coord{buildings.size() - 1, buildings[0].size() - 1});
    //std::thread t2(&Node::findEdges, Coord{0, buildings[0].size() - 1}, Coord{0, 0});
    //std::thread t3(&Node::findEdges, Coord{buildings.size() - 1, buildings[0].size() - 1}, Coord{0, buildings[0].size() - 1});
    Node::findEdges(Coord{0, 0}, Coord{buildings.size(), 0});
    Node::findEdges(Coord{0, 0}, Coord{0, buildings[0].size()});
    Node::findEdges(Coord{0, buildings[0].size() - 1}, Coord{buildings.size(), buildings[0].size() - 1});
    Node::findEdges(Coord{buildings.size() - 1, 0}, Coord{buildings.size() - 1, buildings[0].size()});

    //t1.join();
    //t2.join();
    //t3.join();
    
    Node::tryEdges();
    
    
    Node::topOrder(solution);
    std::cout << "DONE" << std::endl;
    printState();
}

