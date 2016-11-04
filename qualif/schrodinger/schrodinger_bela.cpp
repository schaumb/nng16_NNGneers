#include <vector>
#include <cstddef>
#include <set>
#include <cmath>
#include <list>
#include <functional>
#include <algorithm>
#include <iostream>

std::size_t binarySearch(const std::set<std::size_t> set, std::function<bool (const std::vector<size_t>&)> fun) {
    int from = 0, to = set.size();
    auto begin = set.begin();

    while(from + 1 != to) {
        int middle = (from + to) / 2;

        bool res = fun(std::vector<std::size_t>(std::next(begin, from), std::next(begin, middle)));

        (res ? to : from) = middle;
    }
    return from;
}

std::vector<size_t> FindMyRadioactiveBalls(size_t NumberOfBalls, size_t RadioactiveBalls,
    std::function<bool (const std::vector<size_t>&)> TestFunction) {

    std::set<std::size_t> weDontKnow;
    std::set<std::size_t> radioactives;
    std::list<std::set<std::size_t>> myQuestions;

    for(std::size_t i = 0; i < NumberOfBalls; ++i) {
        weDontKnow.insert(i);
    }

    while(radioactives.size() != RadioactiveBalls) {
        int remainingBalls = RadioactiveBalls - radioactives.size();

        if(weDontKnow.size() == remainingBalls) {
            for(std::size_t s : weDontKnow) {
                radioactives.insert(s);
            }
            break;
        } else if(remainingBalls == 1) {
            radioactives.insert(binarySearch(weDontKnow, TestFunction));
            break;
        }
        
        std::vector<std::set<std::size_t>> preDefinedQuestions(remainingBalls);
        
        std::size_t index = 0;
        for(auto it = weDontKnow.begin(); it != weDontKnow.end(); ++it) {
            std::size_t setIndex = index++ % remainingBalls;
            preDefinedQuestions[setIndex].insert(*it);
        }
        
        auto whichHasDuplicateQuestions = [&preDefinedQuestions, &myQuestions]() -> int {
            int index = 0;
            for(auto& question : preDefinedQuestions) {
                bool allIn = false;
                for(auto& set : myQuestions) {
                    allIn = true;
                    for(std::size_t elem : set) {
                        if(!question.count(elem)) {
                            allIn = false;
                            break;
                        }
                    }
                    if(allIn) {
                        break;
                    }
                }
                if(allIn) {
                    return index;
                }
                ++index;
            }
            return -1;
        };
        
        std::size_t thisSetIsADuplicateQuestion = whichHasDuplicateQuestions();
        
        while(thisSetIsADuplicateQuestion != -1) {
            std::size_t other = rand() % remainingBalls;
            if(other == thisSetIsADuplicateQuestion) 
                ++other %= remainingBalls;
         
            auto& duplicateSet = preDefinedQuestions[thisSetIsADuplicateQuestion];
            auto& otherSet = preDefinedQuestions[other];
            
            std::size_t elementDuplicate = *std::next(duplicateSet.begin(), rand() % duplicateSet.size());
            std::size_t elementOther = *std::next(otherSet.begin(), rand() % otherSet.size());
            
            duplicateSet.erase(elementDuplicate);
            duplicateSet.insert(elementOther);
            
            otherSet.erase(elementOther);
            otherSet.insert(elementDuplicate);
            
            thisSetIsADuplicateQuestion = whichHasDuplicateQuestions();
        }
        
        bool allHas = true;
        for(auto& question : preDefinedQuestions) {
            bool res = TestFunction({question.begin(), question.end()});
            
            if(!res) {
                for(auto it = myQuestions.begin(); it != myQuestions.end();) {
                    for(std::size_t i : question) {
                        it->erase(i);
                    }

                    if(it->size() == 1) {
                        radioactives.insert(*it->begin());
                        it = myQuestions.erase(it);
                        continue;
                    }

                    ++it;
                }
                for(std::size_t i : question) {
                    weDontKnow.erase(i);
                }

                for(std::size_t ra : radioactives) {
                    weDontKnow.erase(ra);
                    for(auto it = myQuestions.begin(); it != myQuestions.end();) {
                        if(it->count(ra)) {
                            it = myQuestions.erase(it);
                        } else {
                            ++it;
                        }
                    }
                }
                myQuestions.sort();
                myQuestions.unique();

                allHas = false;
                break;
            } else if(question.size() == 1) {
                std::size_t elem = *question.begin();
                radioactives.insert(elem);
                weDontKnow.erase(elem);

                for(auto it = myQuestions.begin(); it != myQuestions.end();) {
                    if(it->count(elem)) {
                        it = myQuestions.erase(it);
                    } else {
                        ++it;
                    }
                }
            } else {
                myQuestions.push_back(question);
            }
        }

        if(allHas) {
            for(auto& question : preDefinedQuestions) {
                radioactives.insert(binarySearch(question, TestFunction));
            }
        }
    }
    return std::vector<std::size_t>(radioactives.begin(), radioactives.end());
}

std::vector<size_t> FindRadioactiveBalls(size_t NumberOfBalls, size_t RadioactiveBalls,
    bool (*TestFunction)(const std::vector<size_t>& BallsToTest)) {
    return FindMyRadioactiveBalls(NumberOfBalls, RadioactiveBalls, [&TestFunction](const std::vector<size_t>& BallsToTest) -> bool {
        bool result = TestFunction(BallsToTest);
        std::cout << "Tested: ";
        for(auto& elem : BallsToTest) {
            std::cout << elem << ", ";
        }
        std::cout << " result is: " << (result ? "true" : "false") << std::endl;
        return result;
    });
}
