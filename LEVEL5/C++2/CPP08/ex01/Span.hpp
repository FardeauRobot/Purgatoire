#ifndef SPAN_HPP
# define SPAN_HPP

#include <cstddef>
#include <vector>
#include <iterator>
#include <exception>

class Span {

    size_t              _max_capacity;
    std::vector<int>    _storage;

public:
    Span();
    Span(unsigned int capacity);
    Span(const Span &src);
    Span& operator= (const Span &other);
    ~Span();

    void    addNumber(int new_nbr);
    void    printVector() const;
    size_t  size() const;
    int     shortestSpan() const;
    int     longestSpan() const;

    // All or nothing: the room is checked before inserting anything.
    template <typename InputIt>
    void    fillSpan(InputIt first, InputIt last) {
        if (static_cast<size_t>(std::distance(first, last)) > _max_capacity - _storage.size())
            throw OutOfBoundException();
        _storage.insert(_storage.end(), first, last);
    }

    class OutOfBoundException : public std::exception {
    public :
        virtual const char *what() const throw ();
    };

    class TooFewElemException : public std::exception {
    public :
        virtual const char *what() const throw ();
    };

    class SpanOverflowException : public std::exception {
    public :
        virtual const char *what() const throw ();
    };
};

#endif
