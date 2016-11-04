#include <vector>
#include <cstddef>
#include <set>
#include <cmath>
#include <list>
#include <functional>
#include <algorithm>
#include <iostream>

template<typename Container> 
std::size_t binarySearch(const Container& testSet, std::function<bool (const std::vector<std::size_t>&)> fun) {
    std::size_t from = 0, to = testSet.size();
    auto begin = testSet.begin();

    while(from + 1 != to) {
        std::size_t middle = (from + to) / 2;

        bool res = fun(std::vector<std::size_t>(std::next(begin, from), std::next(begin, middle)));

        (res ? to : from) = middle;
    }
    return from; // returns an index!!!
}

std::set<std::size_t> linearSearch(const std::set<std::size_t>& testSet, std::size_t count, std::function<bool (const std::vector<std::size_t>&)> fun) {
    std::size_t countFalse = testSet.size() - count;
    std::set<std::size_t> trues;
    std::set<std::size_t> falses;

    for(auto it = testSet.begin(); it != testSet.end(); ++it) {
        bool res = fun({*it});
        (res ? trues : falses).insert(*it);

        if(countFalse == falses.size()) {
            trues.insert(++it, testSet.end());
        }
        if(count == trues.size()) {
            break;
        }
    }

    return trues;
}

std::vector<std::size_t> FindMyRadioactiveBalls(std::size_t NumberOfBalls, std::size_t RadioactiveBalls,
    std::function<bool (const std::vector<std::size_t>&)> fun) {

    std::set<std::size_t> weDontKnow;
    std::set<std::size_t> radioactives;

    for(std::size_t i = 0; i < NumberOfBalls; ++i) {
        weDontKnow.insert(i);
    }

    while(radioactives.size() != RadioactiveBalls) {
        int remainingBalls = RadioactiveBalls - radioactives.size();

        if(weDontKnow.size() == remainingBalls) {
            for(std::size_t s : weDontKnow) {
                radioactives.insert(s);
            }
        } else if(remainingBalls == 1) {
            radioactives.insert(*std::next(weDontKnow.begin(), binarySearch(weDontKnow, fun)));
        } else if(weDontKnow.size() < 2 * remainingBalls - 1) {
            for(std::size_t s : linearSearch(weDontKnow, remainingBalls, fun)) {
                radioactives.insert(s);
            }
        } else {
            std::size_t count = std::pow(2, std::floor(std::log2((weDontKnow.size() - remainingBalls + 1) / remainingBalls)));
            std::vector<std::size_t> testVec{weDontKnow.begin(), std::next(weDontKnow.begin(), count)};
            bool res = fun(testVec);
            if(res) {
                std::size_t index = binarySearch(testVec, fun);
                radioactives.insert(testVec[index]);
                weDontKnow.erase(weDontKnow.begin(), std::next(weDontKnow.begin(), index + 1));
            } else {
                weDontKnow.erase(weDontKnow.begin(), std::next(weDontKnow.begin(), count));
            }
        }
    }

    return {radioactives.begin(), radioactives.end()};
}

std::vector<std::size_t> FindRadioactiveBalls(std::size_t NumberOfBalls, std::size_t RadioactiveBalls,
    bool (*TestFunction)(const std::vector<std::size_t>& BallsToTest)) {
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
