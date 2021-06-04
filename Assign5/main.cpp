//***************************************************************************
//
//  main.cpp
//  CSCI 463 Assignment 5
//
//  Created by Tristen Anderson (z1820036)
//
//***************************************************************************

#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "memory.h"
#include "rv32i.h"

/**
 * Print a usage message and abort the program .
 *********************************************************************/
 static void usage () {
	std::cerr << "Usage : rv32i [ - m hex - mem - size ] [-l int - exec - limit] [-dirz] infile" << std::endl;
	std::cerr << "-m specify memory size ( default = 0 x10000 )" << std::endl;
	std::cerr << "-d disasembley before execution" << std::endl;
	std::cerr << "-i print instructions during execution" << std::endl;
	std::cerr << "-l specify execution limit (default = infinite)" << std::endl;
	std::cerr << "-r show a dump of the hart before each execution" << std::endl;
	std::cerr << "-z show a dump of the hart after simulation halted" << std::endl;
 	exit (1);
 }

 /**
 * Read a file of RV32I instructions and execute them 
 *
 * @param argc The command line argument count 
 * @param argv A character array of the command line arguments
 *
 * @return An integer representing the run status
 ********************************************************************/
 int main ( int argc , char ** argv ) {
 
	uint32_t memory_limit = 0x10000; // default memory size = 64ki
	uint64_t execution_limit = 0;
	int dflag = 0;
	bool iflag = false;
	bool rflag = false;
	int zflag = 0;
	int opt;
	
	//check which options were used and set flags
 	while (( opt = getopt ( argc , argv , "m:l:dirz" ) ) != -1) {
 		switch ( opt ) {
 		case 'm':
 			memory_limit = std::stoul( optarg , nullptr ,16);
 			break;
		case 'd':
			dflag = 1;
			break;
		case 'i':
			iflag = true;
			break;
		case 'r':
			rflag = true;
			break;
		case 'z':
			zflag = 1;
			break;
		case 'l':
			execution_limit = std::stoul(optarg, nullptr, 10);
			break;
 		default : /* ’? ’ */
 			usage ();
 			}
 	}

 	if ( optind >= argc )
 		usage (); // missing filename

 	memory mem(memory_limit);

 	if (!mem.load_file ( argv [ optind ]) )
 		usage ();

 	rv32i sim(&mem);

	if(dflag == 1) {
 		sim.disasm();
		sim.reset();
	}
	if(iflag) {
		sim.set_show_instructions(true);
	}
	if(rflag) {
		sim.set_show_registers(true);
	}
	sim.run(execution_limit);
	if(zflag) {
		sim.dump();
		mem.dump();
	}	

 	return 0;
 }
