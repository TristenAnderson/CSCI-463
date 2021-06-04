//***************************************************************************
//
//  Main.cpp
//  CSCI 463 Assignment 3
//
//  Created by Tristen Anderson (z120036)
//
//***************************************************************************

#include <iostream>
#include "memory.h"
#include "hex.h"

void usage();

/**
 * Main handles the logic of the program, loads the file to memory and calls dump
 *
 * @param argc The number of arguments recieved from command line
 * @param argv The array of character arguments
 *
 * @return an integer to indicate successful run
 *
 ********************************************************************************/
int main ( int argc , char **argv ){
if ( argc != 3)
       	usage();

 memory mem( std::stoul( argv[1] , 0 , 16));
 mem.dump();

if (!mem.load_file(argv[2]))
	usage();
 mem.dump();

 std::cout << mem.get_size () << std::endl;
 std::cout << hex32(mem.get8(0)) << std::endl;
 std::cout << hex32(mem.get16(0)) << std::endl;
 std::cout << hex32(mem.get32(0)) << std::endl;
 std::cout << hex0x32(mem.get8(0)) << std::endl;
 std::cout << hex0x32(mem.get16(0)) << std::endl;
 std::cout << hex0x32(mem.get32(0)) << std::endl;
 std::cout << hex8(mem.get8(0)) << std::endl;
 std::cout << hex8(mem.get16(0)) << std::endl;
 std::cout << hex8(mem.get32(0)) << std::endl;
 std::cout << hex0x32(mem.get32(0x1000)) << std::endl;

 mem.set8(0x10, 0x12 );
 mem.set16(0x14, 0x1234 );
 mem.set32(0x18, 0x87654321 );
 mem.dump();
	
 return 0;
}

/**
 * Prints a usage message when the command line is not used correctly.
 *
 ********************************************************************************/
void usage(){
	std::cerr << "Usage: ./memory filesize filename" << std::endl;
	exit(0);
}
