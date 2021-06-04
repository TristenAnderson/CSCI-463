//***************************************************************************
//
//  Memory.cpp
//  CSCI 463 Assignment 3
//
//  Created by Tristen Anderson (z1820036)
//
//***************************************************************************

#include "memory.h"
#include "hex.h"
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip> 

/**
 * Constructor for memory, initializes the size and sets all memory to a5
 *
 * @param siz The size to initialize the memory array to
 *
 ********************************************************************************/
memory::memory(uint32_t siz) {

	siz = (siz+15)&0xfffffff0;
	size = siz;

	mem = new uint8_t[size];
	for (size_t i = 0; i < size; i++) {
		mem[i] = 0xa5;
	}
}

/**
 * Destructor to delete dynamic memory of mem array
 *
 ********************************************************************************/
memory::~memory() {
	delete[] mem;
}

/**
 * checks if the address supplied is in the valid memory array
 *
 * @param i The location of an address to check
 *
 * @return True if the address is valid, false if out of range
 *
 ********************************************************************************/
bool memory::check_address(uint32_t i) const {
	//check address in valid range
	//
	//uitn32_t addr = 0;
	//while (addr <= size){
	//	if(addr == i)
	//		return true;
	//	addr = addr+1;
	//})
	
	if(i <= size) {
		return true;
	}
	//if not valid print warning and return false
	std::cout << "WARNING: Address out of range: " << hex0x32(i) << std::endl;
        return false;	
}

/**
 * Returns the size of the memory array
 *
 * @return size The size of the memory array
 *
 ********************************************************************************/
uint32_t memory::get_size() const {
	return size;
}

/**
 * Gets the 8 bit value stored in memory for the requested address
 *
 * @param addr The address to get the value stored in memory for
 *
 * @return The 8 bit value stored at the given memory address
 *
 ********************************************************************************/
uint8_t memory::get8(uint32_t addr) const {
	if(check_address(addr))
		return mem[addr];
	return 0;
}

/**
 * Gets the 16 bit value stored in memory for the requested address
 *
 * @param addr The address to get the value stored in memory for
 *
 * @return The 16 bit value stored at the given memory address
 *
 ********************************************************************************/
uint16_t memory::get16(uint32_t addr) const {
	uint16_t first = get8(addr);
	uint16_t second = get8(addr+1); 
	return ((second << 8) | (first));
}

/**
 * Gets the 32 bit value stored in memory for the requested address
 *
 * @param addr The address to get the value stored in memory for
 *
 * @return The 32 bit value stored at the given memory address
 *
 ********************************************************************************/
uint32_t memory::get32(uint32_t addr) const {
	uint32_t first = get16(addr);
	uint32_t second = get16(addr+2);
	return ((second << 16) | first);
}

/**
 * Sets the supplied 8 bit value to the supplied address
 *
 * @param addr The address to set the value given at
 * @param val The 8 bit value to set at the given address
 *
 ********************************************************************************/
void memory::set8(uint32_t addr, uint8_t val) {
	if(check_address(addr))
		mem[addr] = val;
}

/**
 * Sets the 16 bit value stored in memory for the requested address
 *
 * @param addr The address to set the value given at
 * @param val The 16 value to set at the given address
 *
 ********************************************************************************/
void memory::set16(uint32_t addr, uint16_t val) {
	set8(addr, val);
	set8(addr+1, val >> 8);	
}

/**
 * Gets the 32 bit value stored in memory for the requested address
 *
 * @param addr The address to set the value given at
 * @param val The 32 bit value to set at the given address
 *
 ********************************************************************************/
void memory::set32(uint32_t addr, uint32_t val) {
	set16(addr, val);
	set16(addr+2, val >> 16);
}

/**
 * Prints the memory array as  memory dump, includes line headers and ascii footer
 *
 ********************************************************************************/
void memory::dump() const {
	
	char ascii[16];
	for (size_t i = 0; i < size; i++) {
		//print ascii array
		if(i != 0 && i%16 == 0) {
			std::cout << "*" << ascii << "*" << std::endl;
		}
		//print line header
		if (i%16 == 0)
			std::cout << std::hex <<std::setfill('0') << std::setw(8) << i << ": ";
		//print extra space at midpoint
		else if(i%8 == 0)
			std::cout << " ";
		
		std::cout << std::hex << std::setw(2) << std::setfill('0') << (uint16_t)mem[i] << ' ';
		uint8_t ch = get8(i);
		ascii[i%16] = isprint(ch) ? ch : '.';
		ascii[16] = '\0';
	}
	//print last ascii array 
	if(size > 0)
		std::cout <<  "*" << ascii << "*" << std::endl;
	std::cout << std::dec;
}

/**
 * Reads an input file in bit by bit and stores it in the memory array
 *
 * @param fname The file to read data from
 *
 * @return true if the file reads properly, false if the file read encounters an error
 *
 ********************************************************************************/
bool memory::load_file(const std::string &fname) {

	std::ifstream infile(fname, std::ios::in|std::ios::binary);
	//!infile.isopen()
	if(!infile) {
		std::cerr << "Can't open file '" << fname << "' for reading." << std::endl;
		return false;
	}
	int i = 0;
	while(infile.good()) {
		if(check_address(i)) {
			infile >> std::noskipws >> mem[i];
		} 
		else {
			std::cerr << "Program too big." << std::endl;
			infile.close();
			return false;
		}
		++i;
	}
	infile.close();
	return true;
}

