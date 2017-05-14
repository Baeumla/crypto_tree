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

/**
*	public: Default constructor
*/
node::node()
{
	left_node = nullptr;
	right_node = nullptr;
}

/**
*	public: Custom constructor
*/
node::node(element e, std::string password)
{
	cypher_aes aes;
	std::string password_reverse;
	for (int i = password.length() - 1; i >= 0 ; i--)
	{
   		password_reverse[i] = password[i]; 
	}

	int64_t high = std::hash<std::string>()(password);
	uint64_t low = std::hash<std::string>()(password_reverse);

	pwd = int_128(high, low);

	// deep copy vector copy
	std::vector<std::array<uint8_t, 4> > * ex = aes.encrypt(e, pwd);
	for (int i = 0; i < ex->size(); i++) 
	{
		encrypted.push_back(ex->at(i));
	}

	left_node = nullptr;
	right_node = nullptr;
	is_root = true;
}	

/**
*	public: returns element if password provided correctly
*	_______
*	To do: 
*		- Decryption implementation
*/
element * node::get_element(std::string password)
{
	std::string password_reverse;
	for (int i = password.length() - 1; i >= 0 ; i--)
	{
   		password_reverse[i] = password[i]; 
	}

	int64_t high = std::hash<std::string>()(password);
	uint64_t low = std::hash<std::string>()(password_reverse);
	//Decrypt requested element

	return nullptr;
}

/**
*	public: Adds node to tree
*/
node * node::add_node(element e)
{
	cypher_aes aes;
	std::vector<std::array<uint8_t, 4> > * ex = aes.encrypt(e, pwd);

	//Randomized tree implementation
	//Complexity O(n) if probability is 1/(n+1)
	int n = this->get_count_right() + this->get_count_left();
	int randNum = rand() % (n + 1);
	
	if (randNum == (int) (n / 2))
	{
		//Insert node as root
		return insert_root(ex);
	} 
	else
	{
		//Insert node as leaf
		if (ex->at(0).at(0) > encrypted.at(0).at(0))
		{
			return right_node->insert_node(ex);
		} 
		else if (ex->at(0).at(0) < encrypted.at(0).at(0))
		{
			return left_node->insert_node(ex);
		}
		return this;
	}

	
}


/**
*	private: Inserts node as leaf and rotates until it is root
*/
node * node::insert_root(std::vector<std::array<uint8_t, 4> > * ex)
{
	node * new_root = insert_node(ex);
	node * tmp;
	//Check side of insertion
	
	if (ex->at(0).at(0) > encrypted.at(0).at(0))
	{
		//inserted in right tree part
		//rotate to left until new_node is root
		while (new_root != tmp)
		{
			tmp = rotate_left(this);
		}
	} else if (ex->at(0).at(0) < encrypted.at(0).at(0))
	{
		//inserted in left tree part
		//rotate to right until new_nore is root
		while (new_root != tmp)
		{
			tmp = rotate_right(this);
		}
	} else 
	{
		//inserted node is already root	
		return this;
	}
	
	return tmp;

}

/**
*	private: Rotates tree around root x to the left
*/
node * node::rotate_left(node * x)
{
	node * y = x->get_right_node();
	x->set_right_node(y->get_left_node());
	y->set_left_node(x);
	return y;
}

/**
*	private: Rotates tree around root x to the right
*/
node * node::rotate_right(node * x)
{
	node * y = x->get_left_node();
	x->set_left_node(y->get_right_node());
	y->set_right_node(x);
	return y;
}

/**
*	private: Set left node
*/
void node::set_left_node(node * x)
{
	left_node = x;
}

/**
*	private: Set right node
*/
void node::set_right_node(node * x)
{
	right_node = x;
}

/**
*	private: Returns left node
*/
node * node::get_left_node()
{
	return right_node;
}

/**
*	private: Returns right node
*/
node * node::get_right_node()
{
	return left_node;
}

/**
*	private: Inserts node as leaf
*/
node * node::insert_node(std::vector<std::array<uint8_t, 4> > * ex)
{
	if (encrypted.empty())
	{
		for (int i = 0; i < ex->size(); i++) 
		{
			encrypted.push_back(ex->at(i));
		}

		left_node = nullptr;
		right_node = nullptr;
		is_root = false;
		return this;
	}
	else if (ex->at(0).at(0) > encrypted.at(0).at(0))
	{
		return right_node->insert_node(ex);
	}
	else if (ex->at(0).at(0) < encrypted.at(0).at(0))
	{
		return left_node->insert_node(ex);
	}
	return this;
}

/**
*	private: Returns number of the elements to the right + root
*/
int node::get_count_right()
{
	if (right_node != nullptr)
	{
		return get_count_right() + 1;
	}
	if (left_node != nullptr)
	{
		return get_count_left() + 1;
	}

	return +1;
}

/**
*	private: Returns number of the elements to the left + root
*/
int node::get_count_left()
{
	if (right_node != nullptr)
	{
		return get_count_right() + 1;
	}
	if (left_node != nullptr)
	{
		return get_count_left() + 1;
	}

	return +1;
}
