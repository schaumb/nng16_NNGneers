#include <sstream>
#include <memory>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <set>
#include <stdexcept>
#define nembela
#ifdef nembela
namespace std
{
    template <typename T>
    string to_string(T val)
    {
        stringstream stream;
        stream << val;
        return stream.str();
    }
}
#define uint unsigned
#endif

enum class Type : char { 
    EMPTY = ' ', 
    WALL = '#', 
    CREEP = 'C', 
    CREEP_CANDIDATE = '+', 
    CREEP_RADIUS = '.', 
    HATCHERY = 'H',
    TUMOR_COOLDOWN = 'T',
    TUMOR_ACTIVE = 'A',
    TUMOR_INACTIVE='*',
    QUEEN = 'Q'
};

inline bool isCreep(Type type) {
    switch(type) {
    case Type::TUMOR_COOLDOWN:
    case Type::TUMOR_ACTIVE:
    case Type::TUMOR_INACTIVE:
    case Type::HATCHERY:
    case Type::CREEP:
        return true;
    default:
        return false;
    }
}

inline bool isCreepable(Type type) {
    switch(type) {
    case Type::EMPTY:
    case Type::CREEP_CANDIDATE:
    case Type::CREEP_RADIUS:
        return true;
    default:
        return false;
    }
}

inline int getMaxHealth(Type type) {
    switch(type) {
    case Type::TUMOR_COOLDOWN:
    case Type::TUMOR_ACTIVE:
    case Type::TUMOR_INACTIVE:
        return 200;
    case Type::HATCHERY:
        return 1500;
    case Type::QUEEN:
        return 175;
    default:
        return 0;
    }
}

inline int getSize(Type type) {
    switch(type) {
    case Type::HATCHERY:
        return 3;
    case Type::QUEEN:
        return 0;
    default:
        return 1;
    }
}

inline int getMaxEnergy(Type type) {
    switch(type) {
    case Type::QUEEN:
        return 51200;
    default:
        return 0;
    }
}

inline int getCreepSpreadRadius(Type type) {
    switch(type) {
    case Type::TUMOR_COOLDOWN:
    case Type::TUMOR_ACTIVE:
    case Type::TUMOR_INACTIVE:
    case Type::HATCHERY:
        return 10;
    default:
        return 0;
    }
}

inline int getCoolDownTime(Type type) {
    switch(type) {
    case Type::TUMOR_COOLDOWN:
        return 60;
    default:
        return 0;
    }
}

using Pos = sf::Vector2i;

const int energy_regeneration = 32;
const int health_regeneration = 16;
const int queen_build_time = 240;
const int spawn_creep_tumor_energy_cost = 6400;

class Object
{
    int id;
    Type type;
    int health = 0;

protected:
    void setType(Type type) {
        this->type = type;
    }
    
public:
    Object(Type type = Type::EMPTY, int id = -1)
        : Object(type, getMaxHealth(type), id) {}
    
    Object(Type type, int health, int id)
        : id {id}, type {type}, health {health} {}
        
    virtual ~Object() = default;
    
    virtual void tick()
    {
        health = std::min(getMaxHealth(type), health + health_regeneration);
    }
    
    int getId() const {
        return id;
    }
    
    Type getType() const {
        return type;
    }
    
    int getHealth() const {
        return health;
    }
    
    char getChar() const {
        return static_cast<char>(getType());
    }
    
    void printMyself(std::ostream& o) const {
        if(getId() != -1) {
            o << "id=" << getId() << "," << getChar() << getCapabilities() << std::endl; 
        }
    }
    virtual std::string getCapabilities() const { return ""; }
};

class Building : public Object
{
    Pos pos;
    int size;
    
    int coolDownTime;
public:
    Building(Pos pos, Type type, int id = -1) :
        Object(type, id), pos {pos}, size{ ::getSize(type) }, coolDownTime { ::getCoolDownTime(type) } {}
        
    Pos getPos() const {
        return pos;
    }
    
    int getSize() const {
        return size;
    }
    
    virtual std::string getCapabilities() const {
        return ",p=(" + std::to_string(pos.x) + "," + std::to_string(pos.y) + ")" + (coolDownTime ? ",cdt=" + std::to_string(coolDownTime) : "");
    }
    
    virtual void tick() override {
        Object::tick();
        
        if(getType() == Type::TUMOR_COOLDOWN && !--coolDownTime) {
            setType(Type::TUMOR_ACTIVE);
        } 
    }
    
    void toCreep() {
        if(isCreepable(getType())) {
            setType(Type::CREEP);
        } else {
            std::cerr << "Not creepable: " << std::endl;
            printMyself(std::cerr);
            std::exit(1);
        }
    }
    
    void toCreepCandidate() {
        if(isCreepable(getType())) {
            setType(Type::CREEP_CANDIDATE);
        } else {
            std::cerr << "Not creep candidatable: " << std::endl;
            printMyself(std::cerr);
            std::exit(1);
        }
    }
    
    void toCreepRadius() {
        if(getType() == Type::EMPTY) {
            setType(Type::CREEP_RADIUS);
        }
    }
    
    void setToInactive() {
        if(getType() == Type::TUMOR_ACTIVE) {
            setType(Type::TUMOR_INACTIVE);
        } else {
            std::cerr << "Not INACTIVABLE: " << std::endl;
            printMyself(std::cerr);
            std::exit(1);
        }
    }
};

class Unit : public Object
{
    int energy;
public:
    Unit(Type type, int id) : 
        Unit(getMaxEnergy(type), type, id) {}

    Unit(int energy, Type type, int id) : 
        Object(type, id), energy {energy} {}
        
    virtual void tick() override {
        Object::tick();
        energy = std::min(getMaxEnergy(getType()), energy + energy_regeneration);
    }
    
    virtual std::string getCapabilities() const {
        return ",e=" + std::to_string(energy);
    }
    
    static Unit createQueen(int id) {
        return Unit(spawn_creep_tumor_energy_cost, Type::QUEEN, id);
    }
    
    void builtTumor() {
        energy -= spawn_creep_tumor_energy_cost;
    }
    
    bool canCreateTumor() const {
        return energy >= spawn_creep_tumor_energy_cost;
    }
};

struct Command
{
    int cmd;
    int id;
    Pos pos;
};

class Game
{
    const int timeLimit;
    const Pos maxPosition;
    struct State {
        std::vector<std::vector<Building>> map;
        int maxId;
        std::vector<Unit> units;
        std::vector<Command> commands;
    };
    std::vector<State> states;
    int time = 0;

    bool validPos(const Pos& pos) const
    {
        return 0 <= pos.x && pos.x < maxPosition.x &&
            0 <= pos.y && pos.y < maxPosition.y;
    }
    
    Building& getBuilding(const Pos& pos) {
        return getState().map[pos.x][pos.y];
    }

public:
    Game(std::ifstream&& input, std::istream& input2) :
        Game(input, input, input2) {}
    
    Game(std::ifstream& input, std::istream_iterator<int> it, std::istream_iterator<int> it2) :
        timeLimit {*it},
        maxPosition {*++it, *++it},
        states {{ [ this ](std::istream_iterator<std::string> inputIterator)
            {
                std::vector<std::vector<Building>> result(maxPosition.x);
                for(int i = maxPosition.x - 1; i >= 0; --i)
                {
                    std::string line = *inputIterator;
                    for(int j = 0; j < maxPosition.y ; ++j)
                    {
                        result[ i ].emplace_back(Pos{j, i}, line.at(j) == '#' ? Type::WALL : Type::EMPTY);
                    }
                    if(i > 0)
                    {
                        ++inputIterator;
                    }
                }
                return result;
            } (input) }}
    {
        Pos startingHatcheryPos = Pos{*++it + 1, *++it + 1};
        std::swap(startingHatcheryPos.x, startingHatcheryPos.y);
        input.close();
        
        addBuilding(startingHatcheryPos, Type::HATCHERY);
        getState().units.emplace_back(Unit::createQueen(++getState().maxId));
        createStartCreeps(startingHatcheryPos);
        
        states = std::vector<State>(timeLimit+1, states.back());
        
        const int count = *it2;
        for(int i = 0; i < count; ++i) {
            int time = *++it2;
            Command c{*++it2, *++it2, {*++it2, *++it2}};
            std::swap(c.pos.x, c.pos.y);
            if(time > timeLimit) {
                std::cerr << "Bigger than time limit" << std::endl;
                std::exit(1);
            }



            states[time].commands.push_back(c);
        }
    }
    
    void createStartCreeps(const Pos& from) {
        Building& start = getBuilding(from);
        auto&& cells = validCells(from, getCreepSpreadRadius(start.getType()));
        std::vector<Pos> creepablePositions;
        
        while((creepablePositions = filterCreepables(cells)).size()) {
            for(Pos& pos : creepablePositions) {
                getBuilding(pos).toCreep();
            }
        }
        for(Pos& pos : cells) {
            Building& b = getBuilding(pos);
            if(b.getType() == Type::EMPTY) {
                b.toCreepRadius();
            }
        }
    }
    
    std::vector<Pos> filterCreepables(const std::vector<Pos>& possibles) {
        std::vector<Pos> creepables;
        for(auto& pos : possibles) {
            if(creepablePos(pos)) {
                creepables.push_back(pos);
            }
        }
        return creepables;
    }
    
    bool creepablePos(const Pos & pos) {
        return isCreepable(getBuilding(pos).getType()) && (
            (validPos(pos + Pos{0, -1}) && isCreep(getBuilding(pos + Pos{0, -1}).getType())) ||
            (validPos(pos + Pos{0, 1}) && isCreep(getBuilding(pos + Pos{0, 1}).getType())) ||
            (validPos(pos + Pos{-1, 0}) && isCreep(getBuilding(pos + Pos{-1, 0}).getType())) ||
            (validPos(pos + Pos{1, 0}) && isCreep(getBuilding(pos + Pos{1, 0}).getType())));
    }
    
    std::vector<Pos> validCells(const Pos& from, int radius) {
        std::vector<Pos> result;
        for(int i = -radius + 1; i < radius; ++i) {
            for(int j = -radius + 1; j < radius; ++j) {
                Pos pos {from.x + i, from.y + j};
                
                if(!validPos(pos)) {
                    continue;
                }
                
                int distanceX = 2 * i + (0 < i ? 1 : -1);
                int distanceY = 2 * j + (0 < j ? 1 : -1);
                int distance = distanceX * distanceX + distanceY * distanceY;
                if(distance <= radius * radius * 4) {
                    result.push_back(pos);
                }
            }
        }
        return result;
    }
    
    bool addBuilding(Pos pos, Type type) {
        Building newBuilding(pos, type, ++getState().maxId);
        for(int i = -newBuilding.getSize() / 2; i <= newBuilding.getSize() / 2; ++i) {
            for(int j = -newBuilding.getSize() / 2; j <= newBuilding.getSize() / 2; ++j) {
                Building& b = getBuilding(pos + Pos{i, j});
                if(b.getType() != Type::EMPTY && b.getType() != Type::CREEP) {
                    return false;
                }
                b = newBuilding;
            }
        }
        std::vector<Pos> valids = validCells(pos, getCreepSpreadRadius(type));
        std::vector<Pos> creepables = filterCreepables(valids);
        
        for(Pos p : creepables) {
            getBuilding(p).toCreepCandidate();
        }
        for(Pos p : valids) {
            getBuilding(p).toCreepRadius();
        }
    }
    
    friend std::ostream &operator<< (std::ostream &o, const Game& g)
    {
        for(auto& unit : g.getState().units) {
            unit.printMyself(o);
        }
        
        std::stringstream map;
        for(auto lineIt = g.getState().map.rbegin(); lineIt != g.getState().map.rend(); ++lineIt)
        {
            for(auto& building : *lineIt)
            {
                building.printMyself(o);
                map << building.getChar();
            }
            map << std::endl;
        }
        
        return o << map.str() << std::endl;
    }
    
    const State& getState() const {
        return states.at(time);
    }
    
    State& getState() {
        return states.at(time);
    }
    
    Pos getMaxPos() const {
        return maxPosition;
    }
    
    int getMaxTime() const {
        return timeLimit;
    }
    
    int getTime() const {
        return time;
    }
    
    void setTime(int time) {
        this->time = time;
    }
    
    std::string play(bool stopWhenNoCommand = false) {
        if(!stopWhenNoCommand) {
            time = 0;
        }
        bool wasChange = true;
        for(; wasChange && time < timeLimit;) {
            wasChange = false;
            auto& commands = getState().commands;
            State& prevState = getState();
            ++time;
            State& state = getState();
            state.map = prevState.map;
            state.maxId = prevState.maxId;
            state.units = prevState.units;
            
            for(Command& c : commands) {
                if(c.cmd == 1) {
                    Unit* unit = getQueen(c.id);
                    if(unit == nullptr) { --time; return "WRONG ID - no queen"; }
                    if(!unit->canCreateTumor()) { --time; return "CAN NOT CREATE A TUMOR - not enough energy"; }
                    Building& b = getBuilding(c.pos);
                    if(b.getType() != Type::CREEP) { --time; return "NOT ON CREEP - queen"; }
                    addBuilding(c.pos, Type::TUMOR_COOLDOWN);
                    unit->builtTumor();
                    wasChange = true;
                } else if(c.cmd == 2) {
                    Building* b = getBuilding(c.id);
                    if(b == nullptr) { --time; return "WRONG ID - no building"; }
                    
                    auto&& valids = validCells(b->getPos(), getCreepSpreadRadius(b->getType()));
                    
                    if(std::none_of(valids.begin(), valids.end(), [&c](const Pos& p) { return p == c.pos; })) {
                        --time;
                        return "Too far";
                    }
                    
                    Building& bn = getBuilding(c.pos);
                    if(bn.getType() != Type::CREEP) { --time; return "NOT ON CREEP"; }
                    addBuilding(c.pos, Type::TUMOR_COOLDOWN);
                    b->setToInactive();
                    wasChange = true;
                }
            }
            
            // spread creep:
            auto l = [](Building* p1, Building* p2) { return p1->getId() < p2->getId(); };
            std::set<Building*, decltype(l)> buildings(l);
            
            
            for(auto& line : state.map) {
                for(auto& building : line) {
                
                    if(stopWhenNoCommand && building.getType() == Type::TUMOR_ACTIVE) {
                        --time;
                        return "NO tumor for child";
                    }
                    building.tick();
                    if(building.getId() != -1) {
                        buildings.insert(&building);
                    }
                }
            }
            
            for(Building* ptr : buildings) {
            
                
                std::vector<Pos> valids = validCells(ptr->getPos(), getCreepSpreadRadius(ptr->getType()));
                std::vector<Pos> poss = filterCreepables(valids);
                
                if(poss.size()) {
                    uint k=((time-1)*(time-1)+37)%poss.size();
                    Pos valasztott=poss[k];
                    getBuilding(valasztott).toCreep();
                    wasChange = true;
                }
                
                for(Pos cand  : filterCreepables(valids)) {
                    getBuilding(cand).toCreepCandidate();
                }
            }
            
            for(auto& unit : state.units) {
                if(stopWhenNoCommand && unit.canCreateTumor()) {
                    --time;
                    return "unit not created tumor";
                }
                unit.tick();
            }
            
            if(time % queen_build_time == 0) {
                state.units.emplace_back(Unit::createQueen(++state.maxId));
            }
        }
        --time;
        return "";
    }
    
    Unit* getQueen(int id) {
        for(Unit& unit : getState().units) {
            if(unit.getId() == id) {
                return &unit;
            }
        }
        return nullptr;
    }
    
    Building* getBuilding(int id) {
        auto& map = getState().map;
        
        for(int i = 0; i < map.size(); ++i) {
            for(int j = 0; j < map[i].size(); ++j) {
                if(map[i][j].getId() == id && map[i][j].getPos() == Pos{i, j}) {
                    return &map[i][j];
                }
            }
        }
        return nullptr;
    }
    
    void save() {
        int count = 0;
        std::stringstream ss;
        for(int i = 0; i < states.size(); ++i) {
            for(Command c : states[i].commands) {
                ss << i << " " << c.cmd << " " << c.id << " " << c.pos.y << " " << c.pos.x << std::endl;
                ++count;
            }
        }
        std::ofstream out("possible_sol.out");
        
        out << count << std::endl << ss.str();
    }
};
