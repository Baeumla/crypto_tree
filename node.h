#ifndef __NODE_H__
#define __NODE_H__

//#include "cypher_aes.h"

#include <vector>
#include <stdint.h>
#include <inttypes.h>
#include <string>
#include <array>
#include <cstdlib>


class int_128;

struct element
{
	int x, y; //16 bit
	std::vector<char> description;
};

class node
{

private:
	std::vector<std::array<uint8_t, 4> > encrypted; // encrypted byte array
	node * left_node;
	node * right_node;

public:
	node(element e, std::string password);

	element * get_element();
	bool add_node(element e);
};

#endif //__NODE_H__