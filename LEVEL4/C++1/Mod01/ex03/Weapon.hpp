#ifndef WEAPON_HPP
# define WEAPON_HPP

#include <iostream>

class Weapon
{
private:
	std::string type;

public:
	Weapon();
	Weapon(std::string s_type);
	Weapon(const Weapon &src);
	~Weapon();

	Weapon &operator=(const Weapon &other);

	const std::string& getType() const;
	void setType(const std::string &set_type);
};

#endif