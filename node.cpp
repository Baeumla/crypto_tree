#include "node.h"
#include "cypher_aes.h"
#include "int_128.h"
#include <stdint.h>
#include <inttypes.h>
#include <functional>
#include <string>
#include <unordered_set>
#include <array>
#include <cstdlib>


node::node(element e, std::string password)
{
	cypher_aes aes;
	std::string password_reverse;
	for (int i = password.length() - 1; i >= 0 ; i--)
	{
   		password_reverse[i] = password[i]; 
	}

	int64_t high = std::hash<std::string>()(password) ;
	uint64_t low = std::hash<std::string>()(password_reverse) ;

	// deep copy vector copy
	std::vector<std::array<uint8_t, 4> > * ex = aes.encrypt(e, int_128(high, low));
	for (int i = 0; i < ex->size(); i++) 
	{
		encrypted.push_back(ex->at(i));
	}

	left_node = nullptr;
	right_node = nullptr;
}	