#ifndef __NODE_H__
#define __NODE_H__

//#include "cypher_aes.h"

#include <vector>
#include <stdint.h>
#include <inttypes.h>
#include <string>
#include <array>
#include <cstdlib>
#include "int_128.h"


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
	bool is_root;
	int_128 pwd;
	int get_count_right();
	int get_count_left();
	node * insert_node(std::vector<std::array<uint8_t, 4> > * ex);
	node * insert_root(std::vector<std::array<uint8_t, 4> > * ex);

	node * rotate_left(node * x);
	node * rotate_right(node * x);

	node * get_left_node();
	node * get_right_node();

	void set_left_node(node * x);
	void set_right_node(node * x);


public:
	node();
	node(element e, std::string password);
	element * get_element(std::string password);
	node * add_node(element e);
};

#endif //__NODE_H__