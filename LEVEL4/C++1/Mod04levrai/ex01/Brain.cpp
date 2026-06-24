#include "Brain.hpp"
#include "utils.hpp"
#include <iostream>

Brain::Brain(void) {
	std::cout << BOLD_CYAN << "Brain Name constructor called" << endofline;
}

Brain::Brain(const Brain &src) {
	std::cout << BOLD_BLUE << "Brain Copy constructor called" << endofline;
	*this = src;
}

Brain &Brain::operator=(const Brain &other) {
	std::cout << BOLD_BLUE << "Brain Copy assignment operator called" << endofline;
	if (this != &other) {
		for (int i = 0; i < 100; i++) {
			_ideas[i] = other._ideas[i];
		}
	}
	return (*this);
}

Brain::~Brain() {
	std::cout << BOLD_RED << "Brain Destructor called" << endofline;
}

void Brain::fillBrain(void) {
	for (int i = 0; i < 100; i++) {
		if (i % 2)
			_ideas[i] = "I am hungry";
		else
			_ideas[i] = "Life is good";
	}
}

void Brain::printBrain(void) {
	for (int i = 0; i < 100; i++) {
		if (i % 25 == 0) {
			std::cout << BOLD_RED << _ideas[i] << endofline;
			std::cout << "ADRESS IDEA = " << &_ideas[i] << endofline;
		}
	}
}