//***************************************************************************
//
//  registerfile.cpp
//  CSCI 463 Assignment 5
//
//  Created by Tristen Anderson (z1820036)
//
//***************************************************************************
#include "registerfile.h"
#include <cstdint>
#include <iostream>
#include <iomanip>

/**
 * Constructor, resets the harte
 ********************************************************************************/
registerfile::registerfile() {
    reset();
}

/**
 * Reset the state of the hart
 ********************************************************************************/
void registerfile::reset() {

    reg[0]= 0x00000000;

    for(int i=1; i<32; i++) {
        reg[i] = 0xf0f0f0f0;
    }
}

/**
 * Sets the value of a register
 *
 * @param r The register to set  
 * @param val The value to set the register to
 ********************************************************************************/
void registerfile::set(uint32_t r, int32_t val)
{
    if(r != 0) {
      reg[r] = val;
    }
}

/**
 * Gets the value of a requested register 
 *
 * @param r The register to return
 ********************************************************************************/
int32_t registerfile::get(uint32_t r) const {
    if(r == 0) {
        return 0;
    }
    else {
        return reg[r];
    }
}

/**
 * Prints a dump of the state of the hart 
 ********************************************************************************/
void registerfile::dump() const {
    //Dump the contents of the register
    for (size_t i = 0; i < 32; i++) {
	//if not first line print newline after 8th entry
	if(i != 0 && i % 8 == 0) {
		std::cout << std::endl;
	}
	//print headers
	if(i % 8 == 0) {
		std::string head;
		if(i == 0)
			head = "x0";
		else if(i == 8)
			head = "x8";
		else if(i == 16)
			head = "x16";
		else if(i == 24)
			head = "x24";
		else
			head = "";		

		std::cout << std::dec << std::right << std::setw(3) << std::setfill(' ') << head <<  " ";  
	}
	//print reg content
	if (i % 8 == 7)
		std::cout << std::hex << std::setw(8) << std::setfill('0') << reg[i];
	else 	
		std::cout << std::hex << std::setw(8) << std::setfill('0') << reg[i] << " ";
    }
    std::cout << std::endl;
}
