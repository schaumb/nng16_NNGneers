#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <iterator>
#include <memory>
#include <queue>
#include <deque>
#include <set>
#include <random>
#include <algorithm>

auto compareTreesByDepth = [](auto& p1, auto& p2) {
    if(p1->getDepth() != p2->getDepth())
        return p1->getDepth() < p2->getDepth(); 
    return std::abs(p1->getPrefixes().size() - p1->getSuffixes().size()) > 
        std::abs(p2->getPrefixes().size() - p2->getSuffixes().size());
};
using CompareTreesByDepth = decltype(compareTreesByDepth);

class Tree;
class Chain;

using PQueueTree = std::priority_queue<std::shared_ptr<Tree>, std::vector<std::shared_ptr<Tree>>,
        CompareTreesByDepth>;

struct Word : std::string {
    std::vector<std::shared_ptr<Tree>> prefixEnds;
    std::vector<std::shared_ptr<Tree>> suffixEnds;
    Word(const std::string& str) : std::string(str) {}
    std::shared_ptr<Chain> in;
};

class Chain : public std::enable_shared_from_this<Chain> {
    std::deque<Word*> chain;

public:
    static void makeChain(Word* p1, Word* p2) {
        if(!p1->in) {
            p1->in = std::make_shared<Chain>(p1);
        }
        if(!p2->in) {
            p2->in = p1->in;
            p2->in->chain.push_back(p2);
        } else {
            p1->in->concatenateOtherChain(p2->in);
        }
    }

    Chain(Word* start) : chain {start} {}

    const std::deque<Word*>& getWords() const {
        return chain;
    }

    void concatenateOtherChain(std::shared_ptr<Chain> other) {
        if(shared_from_this() == other) return;

        chain.insert(chain.end(), other->chain.begin(), other->chain.end());
        for(Word* w : other->chain) {
            w->in = shared_from_this();
        }
    }

    static bool isRealPrefix(Word* w) {
        return !w->in || w->in->getWords().front() == w;
    }

    static bool isRealSuffix(Word* w) {
        return !w->in || w->in->getWords().back() == w;
    }
};



class Tree : public std::enable_shared_from_this<Tree> {
    Tree* parent = nullptr;
    int depth = 0;
    char c = '>';
    std::vector<Word*> prefix;
    std::vector<Word*> suffix;
    std::unordered_map<char, std::shared_ptr<Tree>> childs;

    void makeSuffix(const char* word, Word* ptr) {
        if(*word) {
            getChild(*word).makeSuffix(word + 1, ptr);
        } else {
            suffix.push_back(ptr);
            ptr->suffixEnds.push_back(shared_from_this());
        }
    }

    void makePrefix(const char* word, Word* ptr) {
        if(*word) {
            getChild(*word).makePrefix(word + 1, ptr);
        }
        
        prefix.push_back(ptr);
        ptr->prefixEnds.push_back(shared_from_this());
    }
public:
    Tree() = default;
    Tree(Tree* parent, int depth, char c) : parent(parent), depth(depth), c(c) {}

    Tree& getChild(char c) {
        auto& ptr = childs[c];
        if(!ptr) {
            ptr = std::make_shared<Tree>(this, depth + 1, c);
        }
        return *ptr;
    }

    std::unordered_map<char, std::shared_ptr<Tree>>& getChilds() {
        return childs;
    }

    void insert(Word& word) {
        for(const char* ptr = word.c_str(); *ptr; ++ptr) {
            makeSuffix(ptr, &word);
        }
        if(word.size()) {
            getChild(word[0]).makePrefix(word.c_str() + 1, &word);
        }
    }

    int getDepth() const {
        return depth;
    }

    std::string getIndent() const {
        return std::string(depth * 2, ' ');
    }

    const std::vector<Word*>& getPrefixes() const {
        return prefix;
    }

    const std::vector<Word*>& getSuffixes() const {
        return suffix;
    }

    char getChar() const {
        return c;
    }
};

int main() {
    using IIT = std::istream_iterator<std::string>;

    std::ifstream input("words_final.txt");

    // beolvassuk a szavakat...
    std::cout << "Reading words..." << std::endl;
    std::vector<Word> words{IIT(input), IIT()};

    std::cout << "Sorting words..." << std::endl;
    std::sort(words.begin(), words.end(), [](auto& w1, auto& w2) {
        if(w1.size() == w2.size()) return w1 < w2;
        return w1.size() < w2.size();
    });

    // csinálunk belőle egy prefix/suffix tree-t
    std::shared_ptr<Tree> tree = std::make_shared<Tree>();

    std::cout << "Making the tree..." << std::endl;
    for(Word& word : words) {
        tree->insert(word);
    }

    // priózzuk a tree node-jait a hosszuk szerint
    // az elkképzelés az, hogy először a hosszabb elemeket párosítsuk mohón.
    std::cout << "Collecting the nodes..." << std::endl;
    PQueueTree trees(compareTreesByDepth);

    { // itt szedjük ki a gráfból
        std::queue<std::shared_ptr<Tree>> treeGoThrough;
        treeGoThrough.push(tree);
        while(!treeGoThrough.empty()) {
            auto ptr = treeGoThrough.front();
            treeGoThrough.pop();

            trees.emplace(ptr);

            for(auto& pair : ptr->getChilds()) {
                treeGoThrough.push(pair.second);
            }
        }
    }

    // végigmegyünk a node-okon

    std::cout << "Finding prefix-suffix matches..." << std::endl;
    while(!trees.empty()) {
        auto treePtr = trees.top();
        trees.pop();

        // kivesszük a prefix / suffix szavakat
        auto prefixWords = treePtr->getPrefixes();
        auto suffixWords = treePtr->getSuffixes();
        
        if(prefixWords.size() && suffixWords.size()) {
            // valami alapján párosítsuk őket.
            // index szerint?? :D

            for(auto it = prefixWords.begin(); it != prefixWords.end(); ++it) {
                Word* prefixWord = *it;
                for(auto it = suffixWords.begin(); it != suffixWords.end(); ++it) {
                    Word* suffixWord = *it;
                    if(!prefixWord->in || !suffixWord->in || (prefixWord->in != suffixWord->in &&
                            Chain::isRealPrefix(prefixWord) && Chain::isRealSuffix(suffixWord))) {
                        Chain::makeChain(suffixWord, prefixWord);
                        break;
                    }
                }
            }
        }
    }

    // kiiratjuk a láncokat.
    std::set<std::shared_ptr<Chain>> usedChains;
    std::ofstream output("out.txt");
    std::cout << "Writing output..." << std::endl;
    
    std::size_t min_chain = 1000000;
    std::size_t max_chain = 0;
    int no_chained = 0;
    for(Word& w : words) {
        // ha van, de még nem volt, akkor az egész láncot
        if(w.in && !usedChains.count(w.in)) {
            if(w.in->getWords().size() == 1) {
                ++no_chained;
            } else {
                max_chain = std::max(max_chain, w.in->getWords().size());
                min_chain = std::min(min_chain, w.in->getWords().size());
            }
            
            usedChains.insert(w.in);
            for(Word* w : w.in->getWords()) {
                output << *w << std::endl;
            }
        } else if(!w.in) { // ha nincs láncban, akkor csak magát.
            ++no_chained;
            output << w << std::endl;
        }
    }
    std::cout << "Used chain: " << usedChains.size() << " max: " << max_chain << " min: " << min_chain << std::endl;
    std::cout << "Not in chain: " << no_chained << std::endl;
    
}
