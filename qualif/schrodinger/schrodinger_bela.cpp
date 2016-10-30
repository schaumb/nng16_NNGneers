#include <vector>
#include <cstddef>
#include <set>
#include <cmath>
#include <list>
#include <functional>
#include <algorithm>

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

std::vector<size_t> FindRadioactiveBalls(size_t NumberOfBalls, size_t RadioactiveBalls,
	bool (*TestFunction)(const std::vector<size_t>& BallsToTest)) {

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


        int prevTo = 0;
        bool allHas = true;

        for(int i = 0; i < remainingBalls; ++i) {
            int to = std::round(weDontKnow.size() * (i + 1.) / remainingBalls);

            if(prevTo == to) continue;

            std::vector<std::size_t> question(std::next(weDontKnow.begin(), prevTo), std::next(weDontKnow.begin(), to));

            // duplicate question
            bool allIn = false;
            for(auto& set : myQuestions) {
                allIn = true;
                for(std::size_t elem : set) {
                    if(std::find(question.begin(), question.end(), elem) == question.end()) {
                        allIn = false;
                        break;
                    }
                }
                if(allIn) {
                    break;
                }
            }
            if(allIn) {
                prevTo = to;
                continue;
            }

            bool res = TestFunction(question);

            if(!res) {

                for(auto it = myQuestions.begin(); it != myQuestions.end();) {
                    auto& set = *it;
                    for(std::size_t i : question) {
                        set.erase(i);
                    }

                    if(set.size() == 1) {
                        radioactives.insert(*set.begin());
                        it = myQuestions.erase(it);
                        continue;
                    }

                    ++it;
                }

                weDontKnow.erase(std::next(weDontKnow.begin(), prevTo), std::next(weDontKnow.begin(), to));

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
                radioactives.insert(question[0]);
                weDontKnow.erase(question[0]);

                for(auto it = myQuestions.begin(); it != myQuestions.end();) {
                    if(it->count(question[0])) {
                        it = myQuestions.erase(it);
                    } else {
                        ++it;
                    }
                }

            } else {
                myQuestions.push_back(std::set<std::size_t>(question.begin(), question.end()));
            }

            prevTo = to;
        }

        if(allHas) {

            int prevTo = 0;
            for(int i = 0; i < remainingBalls; ++i) {
                int to = std::round(weDontKnow.size() * (i + 1.) / remainingBalls);

                if(prevTo == to) continue;

                // binary search:
                std::set<std::size_t> question(std::next(weDontKnow.begin(), prevTo), std::next(weDontKnow.begin(), to));

                std::size_t sol = binarySearch(question, TestFunction);
                radioactives.insert(sol);
            }
        }
    }
    return std::vector<std::size_t>(radioactives.begin(), radioactives.end());
}
