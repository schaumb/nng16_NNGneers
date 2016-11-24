#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <vector>
#include <map>
#include "Source.hpp"

sf::Color getColorByType(Type type) {
    switch(type) {
    case Type::EMPTY:
        return sf::Color::White; 
    case Type::WALL:
        return sf::Color::Black; 
    case Type::CREEP:
        return sf::Color::Green; 
    case Type::CREEP_CANDIDATE:
        return sf::Color(100, 100, 100);
    case Type::CREEP_RADIUS:
        return sf::Color(100, 200, 100);
    case Type::TUMOR_INACTIVE:
    case Type::HATCHERY:
        return sf::Color::Blue;
    case Type::TUMOR_COOLDOWN:
        return sf::Color::Yellow;
    case Type::TUMOR_ACTIVE:
        return sf::Color::Cyan;
    }
    return sf::Color::Red;
}

bool isIn(const sf::RectangleShape& rs, const sf::Vector2f& p) {
    sf::FloatRect rect(rs.getPosition(), rs.getSize());
    return rect.contains(p);
} 

int main(int argc, char ** argv)
{
    if(argc != 2)
    {
        std::cerr << "Need a parameter - map file name" << std::endl;
        return 1;
    }
    
    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        std::cerr << "No arial font" << std::endl;
        return 1;
    }
    
    Game game {std::ifstream(argv[ 1 ]), std::cin};
    const Game& cGame = game;
    const Pos maxPos = game.getMaxPos();
    
    // mezők
    std::vector<std::vector<sf::RectangleShape>> 
        rectangles(maxPos.x, std::vector<sf::RectangleShape>(maxPos.y));

    for(int i = 0; i < maxPos.x; ++i) {
        for(int j = 0; j < maxPos.y; ++j) {
            sf::RectangleShape& shape = rectangles[i][j] = sf::RectangleShape({10.0, 10.0});
            shape.setOutlineThickness(1.1);
            shape.setPosition(i * 10, (maxPos.y - j - 1) * 10);
        }
    }
    
    // idő frame-k
    const int maxTime = cGame.getMaxTime();
    std::vector<sf::RectangleShape> timeFrames(maxTime);
    for(int i = 0; i < maxTime; ++i) {
        sf::RectangleShape& shape = timeFrames[i] = sf::RectangleShape({1, 10});
        shape.setPosition(i%500 * 1 + 650, i/500 * 15 + 10);
    }
    
    // objektumok
    const int objMax = 40;
    std::vector<sf::RectangleShape> objects(objMax);
    for(int i = 0; i < objMax; ++i) {
        sf::RectangleShape& shape = objects[i] = sf::RectangleShape({150, 15});
        shape.setOutlineThickness(1.1);
        shape.setFillColor(sf::Color::Green);
        shape.setOutlineColor(sf::Color::Magenta);
        shape.setPosition(650 + i / 35 *160, (i % 35) * 15 + 90);
    }
    
    // akció gomb
    sf::RectangleShape save({150, 30});
    save.setPosition(810, 180);
    save.setFillColor(sf::Color::Yellow);
    
    sf::Text saveText("SAVE", font, 12);
    saveText.setPosition(810, 187);
    saveText.setColor(sf::Color::Black);
    
    
    sf::RectangleShape create({150, 30});
    create.setPosition(810, 225);
    create.setFillColor(sf::Color::Yellow);
    
    sf::Text createText("CREATE COMMAND", font, 12);
    createText.setPosition(810, 232);
    createText.setColor(sf::Color::Black);
    
    
    sf::RectangleShape remove({150, 30});
    remove.setPosition(810, 270);
    remove.setFillColor(sf::Color::Yellow);
    
    sf::Text removeText("REMOVE COMMAND", font, 12);
    removeText.setPosition(810, 277);
    removeText.setColor(sf::Color::Black);
    
    
    sf::RectangleShape play({150, 30});
    play.setPosition(810, 315);
    play.setFillColor(sf::Color::Yellow);
    
    sf::Text playText("PLAY", font, 12);
    playText.setPosition(810, 322);
    playText.setColor(sf::Color::Black);
    
    
    sf::RectangleShape replay({150, 30});
    replay.setPosition(810, 360);
    replay.setFillColor(sf::Color::Yellow);
    
    sf::Text replayText("REPLAY", font, 12);
    replayText.setPosition(810, 367);
    replayText.setColor(sf::Color::Black);
    
    
    sf::RectangleShape step({150, 30});
    step.setPosition(810, 405);
    step.setFillColor(sf::Color::Yellow);
    
    sf::Text stepText("STEP", font, 12);
    stepText.setPosition(810, 412);
    stepText.setColor(sf::Color::Black);
    
    
    sf::RectangleShape back({150, 30});
    back.setPosition(810, 450);
    back.setFillColor(sf::Color::Yellow);
    
    sf::Text backText("BACK", font, 12);
    backText.setPosition(810, 457);
    backText.setColor(sf::Color::Black);
    
    sf::Text statusText("STATUS", font, 16);
    statusText.setPosition(650, 620);
    std::string statusMessage = "";
    
    Pos selectedPos = {0, 0};
    int selectedObject = 2;
    Pos mouseOverPos = {0, 0};
    
    sf::RenderWindow window(sf::VideoMode(1250, 640), "Creep GUI!");
    
    bool isMouseButtonPressed = false;
    bool inReplay = false;
    int toReplay = 0;
    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                isMouseButtonPressed = true;
            }
            
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
            {
                isMouseButtonPressed = false;
            }
            auto pos = sf::Mouse::getPosition(window);
            sf::Vector2f point = sf::Vector2f(pos.x, pos.y);
            
            // mező felett
            for(int i = 0; i < maxPos.x; ++i) {
                for(int j = 0; j < maxPos.y; ++j) {
                    if(isIn(rectangles[i][j], point)) {
                        mouseOverPos = Pos {i, j};
                    }
                }
            }
            
            if(isMouseButtonPressed) {
                // mezőre klatty
                for(int i = 0; i < maxPos.x; ++i) {
                    for(int j = 0; j < maxPos.y; ++j) {
                        if(isIn(rectangles[i][j], point)) {
                            selectedPos = Pos {i, j};
                        }
                    }
                }
                
                // timeframre klatty
                for(int i = 0; i < maxTime; ++i) {
                    if(isIn(timeFrames[i], point)) {
                        game.setTime(i);
                        break;
                    }
                }
                
                // obj-ra klatty
                const int maxId = std::min(objMax - 1, cGame.getState().maxId);
                for(int i = 1; i <= maxId; ++i) {
                    if(isIn(objects[i], point)) {
                        selectedObject = i;
                        break;
                    }
                }
                if(selectedObject > maxId) {
                    selectedObject = 2;
                }
                
                // save
                if(isIn(save, point)) {
                    game.save();
                }
                
                // create
                if(isIn(create, point)) {
                    std::swap(selectedPos.x, selectedPos.y);
                    auto& state =  game.getState();
                    
                    bool has = false;
                    for(int i = 0; i < state.commands.size(); ++i) {
                        if(state.commands[i].id == selectedObject) {
                            has = true;
                            state.commands[i].pos = selectedPos;
                            break;
                        }
                    }
                    if(!has) {
                        int commandType = 2;
                        
                        auto& units = state.units;
                        for(int i = 0; i < units.size(); ++i) {
                            if(units[i].getId() == selectedObject) {
                                commandType = 1;
                                break;
                            }
                        }
                        
                        state.commands.push_back(Command{commandType, selectedObject, selectedPos});
                    }
                    std::swap(selectedPos.x, selectedPos.y);
                }
                
                // remove
                if(isIn(remove, point)) {
                    auto& state =  game.getState();
                    for(int i = 0; i < state.commands.size(); ++i) {
                        if(state.commands[i].id == selectedObject) {
                            state.commands.erase(state.commands.begin() + i);
                            break;
                        }
                    }
                }
                
                // play
                if(isIn(play, point)) {
                    statusMessage = game.play();
                }
                
                // replay
                if(isIn(replay, point)) {
                    inReplay = true;
                    toReplay = game.getTime();
                    game.setTime(0);
                    
                    clock.restart();
                }
                
                // step
                if(isIn(step, point)) {
                    statusMessage = game.play(true);
                }
                
                // back
                if(isIn(back, point)) {
                    for(int i = game.getTime() - 1; i >= 0; --i) {
                        game.setTime(i);
                        if(game.getState().commands.size()) {
                            break;
                        }
                    }
                }
            }
            
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        if(inReplay && clock.getElapsedTime() > sf::seconds(0.05f)) {
            if(game.getTime() >= toReplay) {
                inReplay = false;
                game.setTime(toReplay);
            } else {
                game.setTime(game.getTime() + 4);
                clock.restart();
            }
        }

        window.clear();
        
        auto& state = cGame.getState();
        
        // object hátterek
        for(int i = 0; i < objMax; ++i) {
            sf::RectangleShape& shape = objects[i];
            window.draw(shape);
        }
        
        std::map<Type, int> statistic;
        // mezők
        for(int i = 0; i < maxPos.x; ++i) {
            for(int j = 0; j < maxPos.y; ++j) {
                Pos pos{i, j};
                const Building& building = state.map[j][i];
                sf::RectangleShape& shape = rectangles[i][j];
                shape.setFillColor(getColorByType(building.getType()));
                shape.setOutlineColor(pos == selectedPos ? sf::Color::Blue : sf::Color::Magenta);
                shape.setOutlineThickness(1.1);
                window.draw(shape);
                
                switch(building.getType()) {
                case Type::EMPTY:
                case Type::WALL:
                    ++statistic[building.getType()];
                    break;
                case Type::CREEP_CANDIDATE:
                case Type::CREEP_RADIUS:
                    ++statistic[Type::CREEP_RADIUS];
                    break;
                case Type::HATCHERY:
                case Type::CREEP:
                case Type::TUMOR_INACTIVE:
                    ++statistic[Type::CREEP];
                    break;
                case Type::TUMOR_ACTIVE:
                case Type::TUMOR_COOLDOWN:
                    ++statistic[Type::TUMOR_ACTIVE];
                    break;
                }
                
                std::stringstream ss;
                building.printMyself(ss);
                std::string str = ss.str();
                if(str.size()) {
                    sf::Text timeText(ss.str(), font, 12);
                    timeText.setColor(building.getId() == selectedObject ? sf::Color::Blue : sf::Color::Black);
                    timeText.setPosition(650 + building.getId() / 35 *160, (building.getId() % 35) * 15 + 90);
                    window.draw(timeText);
                }
            }
        }
        
        int emptiesOver = 0;
        int creepablesOver = 0;
        for(auto& radPos : game.validCells(mouseOverPos, 
            getCreepSpreadRadius(Type::TUMOR_COOLDOWN)))
        {
            const Building& building = state.map[radPos.y][radPos.x];
            if(building.getType() != Type::WALL) {
                sf::RectangleShape over = rectangles[radPos.x][radPos.y];
                over.setOutlineColor(sf::Color(255, 255, 255, 0));
                over.setFillColor(sf::Color(0, 0, 0, 127));
                
                emptiesOver += building.getType() == Type::EMPTY;
                creepablesOver += isCreepable(building.getType());
                
                window.draw(over);
            }
        }
        
        window.draw(rectangles[selectedPos.x][selectedPos.y]);
        
        // statisztika
        std::stringstream statStream;
        for(auto& pair : statistic) {
            statStream << "'" << static_cast<char>(pair.first) << "': " << pair.second << std::endl;
        }
        sf::Text statText(statStream.str(), font, 12);
        statText.setPosition(1200, 10);
        window.draw(statText);
        
        
        // unitok
        auto& units = state.units;
        
        for(int i = 0; i < units.size(); ++i) {
            std::stringstream ss;
            units[i].printMyself(ss);
            sf::Text timeText(ss.str(), font, 12);
            timeText.setColor(units[i].getId() == selectedObject ? sf::Color::Blue : sf::Color::Black);
            timeText.setPosition(650 + units[i].getId() / 35 *160, (units[i].getId() % 35) * 15 + 90);
            window.draw(timeText);
        }
        
        // commandok
        auto& commands = state.commands;
        
        for(auto& command : commands) {
            int id = command.id;
            std::stringstream ss;
            
            ss << command.pos.x << ", " << command.pos.y;
            sf::Text timeText(ss.str(), font, 12);
            timeText.setColor(id == selectedObject ? sf::Color::Blue : sf::Color::Black);
            timeText.setPosition(650 + id / 35 *160 + 110, (id % 35) * 15 + 90);
            window.draw(timeText);
        }
        
        // time framek
        int time = cGame.getTime();
        for(int i = 0; i < maxTime; ++i) {
            sf::RectangleShape& shape = timeFrames[i];
            shape.setFillColor(i==time ? sf::Color::Blue : i < time ? sf::Color::Red : sf::Color(100, 100, 100));
            window.draw(shape);
        }
        
        // time / max time
        sf::Text timeText(std::to_string(time) + " / " + std::to_string(maxTime), font, 20);
        timeText.setPosition(1000, 90);
        window.draw(timeText);
        
        // buttons
        window.draw(save);
        window.draw(saveText);
        window.draw(create);
        window.draw(createText);
        window.draw(remove);
        window.draw(removeText);
        window.draw(play);
        window.draw(playText);
        window.draw(replay);
        window.draw(replayText);
        window.draw(step);
        window.draw(stepText);
        window.draw(back);
        window.draw(backText);
        
        // status bar:
        std::stringstream statusSS;
        statusSS << selectedPos.y << ", " << selectedPos.x << " - " << 
            mouseOverPos.y << ", " << mouseOverPos.x << " | " 
            << "empty: " << emptiesOver << " creepable: " << creepablesOver << " | "
            << statusMessage;

        statusText.setString(statusSS.str());
        window.draw(statusText);
        
        window.display();
    }
    
    return 0;
}
