#ifndef __NODE_H__
#define __NODE_H__

#include <vector>
#include <stdint.h>
#include <inttypes.h>


class int_128;

struct element
{
	int x, y; //16 bit
	std::vector<char> description;
};

class node
{

private:
	
	uint8_t * encrypted; //
	node * left_node;
	node * right_node;

public:
	node(element e);
	node();

	element * get_element();
	bool add_node(element e);
};

#endif //__NODE_H__