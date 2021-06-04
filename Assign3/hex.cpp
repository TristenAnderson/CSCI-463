//***************************************************************************
//
//  hex.cpp
//  CSCI 463 Assignment 3
//
//  Created by Tristen Anderson (z1820036)
//
//***************************************************************************

#include "hex.h"
#include <sstream>	//include for std::ostringstream
#include <iomanip>	//include for std::setfill

/**
 * Converts an 8 bit integer as a string
 *
 * @param i The integer to be converted to a string
 *
 * @return a string containing the hex value
 *
 ********************************************************************************/
std::string hex8(uint8_t i){

	std::ostringstream os;
	os << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(i);
	return os.str();
}

/**
 * Converts a 16 bit number to a string
 *
 * @param i The number to be converted to a string
 *
 * @return a string containing the hex value
 *
 ********************************************************************************/
std::string hex32(uint32_t i){

	std::ostringstream os;
	os << std::hex << std::setfill('0') << std::setw(8) << (uint32_t) i;
	return os.str();
}

/**
 * Converts a 32 bit number to a string
 *
 * @param i The number to be converted to a string 
 *
 * @return a string containing the hex value
 *
 ********************************************************************************/
std::string hex0x32(uint32_t i){

	return std::string("0x")+hex32(i);
}

