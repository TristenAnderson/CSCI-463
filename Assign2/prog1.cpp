//***************************************************************************
//
//  prog1.cpp
//  CSCI 463 Assignment 2
//
//  Created by Tristen Anderson
//
//***************************************************************************
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <fstream>

using std::cout;
using std::endl;
using std::hex;
using std::dec;
using std::setw;

void printBinFloat(int32_t x);

/**
 * main function of the program, loops through stdin and calls print on the 32 bit inputs
 *
 * @return an integer to represent the exit status
 *
 ********************************************************************************/
int main() {
	
	uint32_t x;

	while (std::cin >> hex >> x) {
		printBinFloat(x);
	}
	
	if (!std::cin.eof())
		std::cerr << "Warning: Terminating due to input stream error" << endl;

	return 0;
}

/**
 * A function to take a 32 bit int and print it in IEEE binary format
 *
 * @param x A signed 32 bit integer to be printed in IEEE binary format
 *
 ********************************************************************************/
void printBinFloat(int32_t x)  {
	
	//get the sign of the number
	bool sign = (x & 0x80000000);
	//create a mask to extract exponent 
	uint32_t a = 0x7f80c000;
	//create a mask to extract the significand
	uint32_t b = 0x007fffff; 
	
	//extract the exponent from ieee num
	int32_t exp = ((x&a)>>23)-127;
	//extract the significand
	uint32_t sig = x&b;

	//print out the input in binary	
	cout << "0x" << setw(8) << std::setfill('0') << hex << x << " = ";
	for (int i = 0; i < 32; i++) {
		if ( i % 4 == 0 && i !=0 ) {
			cout << " ";
		}	
		cout << ((x<<i) & 0x80000000 ? '1' : '0' );
	}
	cout << endl;
	
	//print the sign, exp and sig 
	cout << "sign: " << (x & 0x80000000 ? '1' : '0' ) << endl;
	cout << std::setfill('0');
	cout << " exp: 0x" << hex << setw(8) << exp << " (" << dec << exp << ")" << endl;
	cout << " sig: 0x" << hex << setw(8) << sig << endl; 

	//print the appropriate sign
	if( sign == false ) {
		cout << '+';
	} else {
		cout << '-';
	}
	//check for special cases [inf and 0]
	if ( exp == (int32_t)0x00000080 && sig == 0x00000000 ) {
		cout << "inf" << endl;
	} 
	else if ( exp == (int32_t)0xffffff81 && sig == 0x00000000 ) {
		cout << "0" << endl;
	}
	//for a positive exponent 
	else if (exp >= 0) {
		
		//find the length of the value
		int len = exp + 23;
		//print the implied 1
		cout << "1";
		//shift the bits in the significand to the front
		sig = (sig << 9);
	
		//print the binary representation of the IEEE number
		for (int i = 0; i < len; i++) {
			if (i == (int)exp){
				cout << ".";
			}
			cout << ( sig & 0x80000000 ? '1' : '0' );
			sig = (sig << 1);

			if (i >= (int)exp && i > 23) {
				break;
			}
		}
		cout << endl;
	}
	//for a negative exponent
	else if (exp < 0) {
		
		//print the leading 0 and deimal point
		cout << "0.";
		//shift the bits in the significand to the front
		sig = (sig << 9);

		//for each number in the exponent print a 0 
		for (int i = -1; i > exp; i--) {
			cout << '0';
		}
		
		//print the implied 1
		cout << '1';

		//print the binary representation of the significand 
		for(int i = 0; i < 23; i++) {
			cout << ( sig & 0x80000000 ? '1' : '0' );
			sig = (sig << 1);
		}
		cout << endl;
	}
}
