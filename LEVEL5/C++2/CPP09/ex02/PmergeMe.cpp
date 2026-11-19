#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include "PmergeMe.hpp"
#include "utils.hpp"

// ~TORS -------
//
PmergeMe::PmergeMe() : _vectorTimer(0), _dequeTimer(0) { }

PmergeMe::PmergeMe(const PmergeMe &src) { *this = src; }

PmergeMe& PmergeMe::operator= (const PmergeMe &other) {

    if (this != &other) {
        _vector = other._vector;
        _deque = other._deque;
        _vectorTimer = other._vectorTimer;
        _dequeTimer = other._dequeTimer;
    }
    return (*this);
}

PmergeMe::~PmergeMe() { }



// UTILS -------
// vector ---

static void makePairsVector(const std::vector<int> &c, std::vector<std::pair<int, int> > &pairs, std::vector<int> &winners) {

    for (size_t i = 0; i + 1 < c.size(); i += 2) {
        if (c[i] < c[i + 1])
            pairs.push_back(std::make_pair(c[i + 1], c[i]));
        else
            pairs.push_back(std::make_pair(c[i], c[i + 1]));
        winners.push_back(pairs.back().first);
    }
}

static void matchLosersVector(const std::vector<int> &winners, std::vector<std::pair<int, int> > &pairs, std::vector<int> &losers) {
    for (size_t i = 0; i < winners.size(); ++i)
    {
        size_t j = 0;
        while (pairs[j].first != winners[i])
            ++j;
        losers.push_back(pairs[j].second);
        pairs.erase(pairs.begin() + j);
    }
}

static void insertLoserVector(std::vector<int> &chain, std::vector<size_t> &positions, const std::vector<int> &losers, size_t idx) {

    size_t bound = idx < positions.size() ? positions[idx] : chain.size();

    std::vector<int>::iterator it = std::upper_bound(chain.begin(), chain.begin() + bound, losers[idx]);
    size_t p = it - chain.begin();

    chain.insert(it, losers[idx]);
    for (size_t i = 0; i < positions.size(); ++i)
        if (positions[i] >= p)
            ++positions[i];
}

static void insertAllLosersVector(std::vector<int> &chain, std::vector<size_t> &positions, const std::vector<int> &losers) {

    size_t m = losers.size();
    size_t prev = 1;
    size_t cur = 3;

    while (prev < m)
    {
        for (size_t k = std::min(cur, m); k > prev; --k)
            insertLoserVector(chain, positions, losers, k - 1);
        size_t next = cur + 2 * prev;
        prev = cur;
        cur = next;
    }
}

// deque ---
static void makePairsDeque(const std::deque<int> &c, std::deque<std::pair<int, int> > &pairs, std::deque<int> &winners) {

    for (size_t i = 0; i + 1 < c.size(); i += 2)
    {
        if (c[i] < c[i + 1])
            pairs.push_back(std::make_pair(c[i + 1], c[i]));
        else
            pairs.push_back(std::make_pair(c[i], c[i + 1]));
        winners.push_back(pairs.back().first);
    }
}

static void matchLosersDeque(const std::deque<int> &winners, std::deque<std::pair<int, int> > &pairs, std::deque<int> &losers) {

    for (size_t i = 0; i < winners.size(); ++i)
    {
        size_t j = 0;
        while (pairs[j].first != winners[i])
            ++j;
        losers.push_back(pairs[j].second);
        pairs.erase(pairs.begin() + j);
    }
}

static void insertLoserDeque(std::deque<int> &chain, std::deque<size_t> &positions, const std::deque<int> &losers, size_t idx) {

    size_t bound = idx < positions.size() ? positions[idx] : chain.size();
    std::deque<int>::iterator it = std::upper_bound(chain.begin(), chain.begin() + bound, losers[idx]);
    size_t p = it - chain.begin();

    chain.insert(it, losers[idx]);
    for (size_t i = 0; i < positions.size(); ++i)
        if (positions[i] >= p)
            ++positions[i];
}

static void insertAllLosersDeque(std::deque<int> &chain, std::deque<size_t> &positions, const std::deque<int> &losers) {

    size_t m = losers.size();
    size_t prev = 1;
    size_t cur = 3;

    while (prev < m)
    {
        for (size_t k = std::min(cur, m); k > prev; --k)
            insertLoserDeque(chain, positions, losers, k - 1);
        size_t next = cur + 2 * prev;
        prev = cur;
        cur = next;
    }
}

// METHODS -------
// vector ---

void    PmergeMe::sortVector(int argc, char **argv) {
    clock_t start = clock();

    for (int i = 1; i < argc; ++i)
        _vector.push_back(static_cast<int>(std::strtol(argv[i], NULL, 10)));
    fordJohnsonVector(_vector);
    _vectorTimer = static_cast<double>(clock() - start) / CLOCKS_PER_SEC * 1000000;
}

void    PmergeMe::fordJohnsonVector(std::vector<int> &c) {

    if (c.size() < 2)
        return ;

    std::vector<std::pair<int, int> > pairs;
    std::vector<int> winners;

    makePairsVector(c, pairs, winners);
    fordJohnsonVector(winners);

    std::vector<int> losers;
    matchLosersVector(winners, pairs, losers);
    if (c.size() % 2 != 0)
        losers.push_back(c.back());

    std::vector<int> chain;
    std::vector<size_t> positions;

    chain.push_back(losers[0]);

    for (size_t i = 0; i < winners.size(); ++i) {
        chain.push_back(winners[i]);
        positions.push_back(i + 1);
    }

    insertAllLosersVector(chain, positions, losers);
    c = chain;
}


// deque ---
void    PmergeMe::sortDeque(int argc, char **argv) {
    clock_t start = clock();

    for (int i = 1; i < argc; ++i)
        _deque.push_back(static_cast<int>(std::strtol(argv[i], NULL, 10)));
    fordJohnsonDeque(_deque);
    _dequeTimer = static_cast<double>(clock() - start) / CLOCKS_PER_SEC * 1000000;
}

void    PmergeMe::fordJohnsonDeque(std::deque<int> &c) {

    if (c.size() < 2)
        return ;

    std::deque<std::pair<int, int> > pairs;
    std::deque<int> winners;

    makePairsDeque(c, pairs, winners);
    fordJohnsonDeque(winners);

    std::deque<int> losers;
    matchLosersDeque(winners, pairs, losers);
    if (c.size() % 2 != 0)
        losers.push_back(c.back());

    std::deque<int> chain;
    std::deque<size_t> positions;

    chain.push_back(losers[0]);

    for (size_t i = 0; i < winners.size(); ++i) {
        chain.push_back(winners[i]);
        positions.push_back(i + 1);
    }

    insertAllLosersDeque(chain, positions, losers);
    c = chain;
}

// GETTERS / SETTERS -------

const std::vector<int>& PmergeMe::getVector(void) const { return (_vector); }

const std::deque<int>&  PmergeMe::getDeque(void) const { return (_deque); }

void    PmergeMe::setVector(const std::vector<int> &vector) { _vector = vector; }

void    PmergeMe::setDeque(const std::deque<int> &deque) { _deque = deque; }

double  PmergeMe::getVectorTimer(void) const { return (_vectorTimer); }

double  PmergeMe::getDequeTimer(void) const { return (_dequeTimer); }
