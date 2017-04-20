#include "cypher_aes.h"
#include "int_128.h"
#include "node.h"
#include <stdint.h>
#include <inttypes.h>
#include <vector>
#include <array>
#include <cstdlib>


cypher_aes::cypher_aes()
{
	b = 179; //179 for 128bit key 
	ark_counter = 0;
}

std::vector<std::array<uint8_t, BYTE_NUM> > cypher_aes::encrypt(element e, int_128 key)
{
	//Bring input element in state; padding if required
	state.push_back((std::array<uint8_t, 4>) {0,0,0,0});
	state.push_back((std::array<uint8_t, 4>) {0,0,0,0});
	for (int i = 0; i < 4; i++)
	{
		//std::array<uint8_t, 4> s = {0,0,0,0};
		state.at(0).at(i % 4) = (uint8_t) (e.x >> (24 - 8 * i));
		state.at(1).at(i % 4) = (uint8_t) (e.y >> (24 - 8 * i));
	}

	state.push_back((std::array<uint8_t, 4>) {0,0,0,0});
	int j = 2;
	for (int i = 0; i < e.description.size() && i < 376; i++)
	{
		if ((i % 4 == 0) && (i != 0))
		{
			j++;
			state.push_back((std::array<uint8_t, 4>) {0,0,0,0});
		}

		state.at(j).at(i % 4) = (uint8_t) (e.description.at(i) >> (24 - 8 * i));
	}
	//padding
	j++;
	if (state.size() % 4 != 0)
	{
		state.push_back((std::array<uint8_t, 4>) {0,0,0,0});
		state.at(j).at(0) = (uint8_t) (state.size() % 4);
		state.at(j).at(1)  = (uint8_t) std::rand();
		state.at(j).at(2)  = (uint8_t) std::rand();
		state.at(j).at(3)  = (uint8_t) std::rand();
	}
	j++;
	for (int i = 0; i < (state.size() % 4); i++)
	{
		if ((i % 4 == 0) && (i != 0))
		{
			j++;
			state.push_back((std::array<uint8_t, 4>) {0,0,0,0});
		}
		state.at(j).at(i % 4) = (uint8_t) std::rand();
	}


	// Bring key in first 16 bytes of expanded key
	// Initialize expanded key
	j = 0;
	for (int i = 0; i < 16; i++) 
	{
		if ((i % 4 == 0) && (i != 0))
			j++;

		expanded_key[j][i % 4] = key.get_bytes(i);
	}

	initialize_aes_sbox();

	initialize_expanded_key();


	//Add round key step before start first round
	add_round_key();

	for (int i = 1; i < 9; i++)
	{
		sub_bytes();
		shift_rows();
		mix_columns();
		add_round_key();
	}

	//Round 10
	sub_bytes();
	shift_rows();
	add_round_key();

	return state;
}



void cypher_aes::add_round_key()
{
	for (int i = 0; i < 16; i++) 
	{
		if ((i % 4 == 0) && (i != 0))
			ark_counter++;

		state[ark_counter][i % 4] ^= expanded_key[ark_counter][i % 4];
	}
	ark_counter++;
}

void cypher_aes::sub_bytes()
{
	int j = 0;
	for (int i = 0; i < 16; i++) 
	{
		if ((i % 4 == 0) && (i != 0))
			j++;

		state[j][i % 4] = rcon[state[j][i % 4]];
	}
}

void cypher_aes::shift_rows()
{
	//second row shift;
	uint8_t fubr = state[1][0];
	state[1][0] = state[1][1];
	state[1][1] = state[1][2];
	state[1][2] = state[1][3];
	state[1][3] = fubr;

	//third row shift
	fubr = state[2][0];
	state[2][0] = state[2][2];
	state[2][2] = fubr;
	fubr = state[2][1];
	state[2][1] = state[2][3];
	state[2][3] = fubr;

	//fourth row shift
	fubr = state[3][0];
	state[3][0] = state[3][2];
	state[3][2] = fubr;
}

void cypher_aes::mix_columns() { 
   uint8_t ss[BYTE_NUM][BYTE_NUM];

   for (int c = 0; c < 4; c++) 
   {
      ss[0][c] = (uint8_t) (gal_field_mul(0x02, state[0][c]) ^ gal_field_mul(0x03, state[1][c]) ^ state[2][c] ^ state[3][c]);
      ss[1][c] = (uint8_t) (state[0][c] ^ gal_field_mul(0x02, state[1][c]) ^ gal_field_mul(0x03, state[2][c]) ^ state[3][c]);
      ss[2][c] = (uint8_t) (state[0][c] ^ state[1][c] ^ gal_field_mul(0x02, state[2][c]) ^ gal_field_mul(0x03, state[3][c]));
      ss[3][c] = (uint8_t) (gal_field_mul(0x03, state[0][c]) ^ state[1][c] ^ state[2][c] ^ gal_field_mul(0x02, state[3][c]));
   }

   //Deep array copy
   int j = 0;
   for (int i = 0; i < 16; i++) 
   {	
   		if ((i % 4 == 0) && (i != 0))
			j++;

   		state[j][i % 4]  = ss[j][i % 4]; 
   }
}

/* Helper functions */

/**
*	Galois Field (256) Multiplication of two Bytes
**/
uint8_t cypher_aes::gal_field_mul(uint8_t a, uint8_t b) { 
   uint8_t p = 0;
   uint8_t counter;
   uint8_t hi_bit_set;
   for (counter = 0; counter < 8; counter++) {
      if ((b & 1) != 0) {
         p ^= a;
      }
      hi_bit_set = (uint8_t) (a & 0x80);
      a <<= 1;
      if (hi_bit_set != 0) {
         a ^= 0x1b; /* x^8 + x^4 + x^3 + x + 1 */
      }
      b >>= 1;
   }
   return p;
}


/**
*	Initializes Rijndael S-box
**/
void cypher_aes::initialize_aes_sbox() 
{
	uint8_t p = 1, q = 1;
	
	/* loop invariant: p * q == 1 in the Galois field */
	do {
		/* multiply p by 2 */
		p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);

		/* divide q by 2 */
		q ^= q << 1;
		q ^= q << 2;
		q ^= q << 4;
		q ^= q & 0x80 ? 0x09 : 0;

		/* compute the affine transformation */
		uint8_t xformed = q ^ (q << 1) ^ (q << 2) ^ (q << 3) ^ (q << 4);

		sbox[p] = xformed ^ 0x63;
	} while (p != 1);

	/* 0 is a special case since it has no inverse */
	sbox[0] = 0x63;
}


/**
*	Initializes expanded key
*	Prereq: Initial key already in array expanded_key
**/
void cypher_aes::initialize_expanded_key() 
{
	int i = 1, counter = 2, c;
	//ToDo number iterations
	while (i <= 10)
	{
		//Assign t the value of the previous four bytes in expanded_key
		int t = expanded_key[counter][0] << 24;
		t += expanded_key[counter][1] << 16;
		t += expanded_key[counter][2] << 8;
		t += expanded_key[counter][3];

		//Perform key schedule code on t
		key_schedule_core(t, i);

		i++;

		//Exclusive OR with the four-byte block n bytes before the new expanded key
		c = counter;
		counter++;
		expanded_key[counter][0] = (uint8_t) (t >> 24) ^ expanded_key[c][0];
		expanded_key[counter][0] = (uint8_t) (t >> 16) ^ expanded_key[c][1];
		expanded_key[counter][0] = (uint8_t) (t >> 8) ^ expanded_key[c][2];
		expanded_key[counter][0] = (uint8_t) (t ^ expanded_key[c][3]);

		for (int k = 0; k < 3; k++)
		{
			//Assign the value of previous 4 byte to t
			t = expanded_key[counter][0] << 24;
			t += expanded_key[counter][1] << 16;
			t += expanded_key[counter][2] << 8;
			t += expanded_key[counter][3];

			c = counter;
			counter++;
			expanded_key[counter][0] = (uint8_t) (t >> 24) ^ expanded_key[c][0];
			expanded_key[counter][0] = (uint8_t) (t >> 16) ^ expanded_key[c][1];
			expanded_key[counter][0] = (uint8_t) (t >> 8) ^ expanded_key[c][2];
			expanded_key[counter][0] = (uint8_t) (t ^ expanded_key[c][3]);
		}
	}

}

/**
*	Key schedule core
*
**/
int cypher_aes::key_schedule_core(int input, int i) 
{
	// Initialize result with input and rotate 8 bits left
	int result = input << 8, intermediate = input << 24;
	result += intermediate >> 24;

	//Apply S-Box on all four individual bytes of result
	intermediate = 0;
	intermediate = sbox[(uint8_t) result >> 24];
	intermediate += sbox[(uint8_t) result >> 16];
	intermediate += sbox[(uint8_t) result >> 8];
	intermediate += sbox[(uint8_t) result];
	result = intermediate;

	// Exclusive or with 2^i-1 of rcon
	intermediate = 0;
	intermediate = result << 24;
	intermediate = intermediate >> 8;
	intermediate += ((result >> 24) ^ rcon[i]) << 24;
	result = intermediate;

	return result;
}









































