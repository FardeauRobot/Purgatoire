#include "Account.hpp"

Account::Account(int initial_deposit) {

}

Account::~Account() {}

int Account::getNbAccounts (void) {
	return (_nbAccounts);
}

int Account::getTotalAmount (void) {
	return (_totalAmount);
}

int Account::getNbDeposits (void) {
	return (_totalNbDeposits);
}

int Account::getNbWithdrawals (void) {
	return (_totalNbWithdrawals);
}