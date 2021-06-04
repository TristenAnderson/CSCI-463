//***************************************************************************
//
//  rv32i.cpp
//  CSCI 463 Assignment 5
//
//  Created by Tristen Anderson (z1820036)
//
//***************************************************************************
#include <iostream>
#include <iomanip>
#include <string.h>
#include "memory.h"
#include "rv32i.h"
#include "hex.h"
#include "registerfile.h"

static constexpr int mnemonic_width = 8;
static constexpr int instruction_width = 35;

//Opcodes
static constexpr uint32_t opcode_lui            = 0b0110111;
static constexpr uint32_t opcode_auipc          = 0b0010111;
static constexpr uint32_t opcode_jal            = 0b1101111;
static constexpr uint32_t opcode_jalr           = 0b1100111;
static constexpr uint32_t opcode_btype          = 0b1100011;
static constexpr uint32_t opcode_load_imm       = 0b0000011;
static constexpr uint32_t opcode_rtype		= 0b0110011;
static constexpr uint32_t opcode_stype		= 0b0100011;
static constexpr uint32_t opcode_itype_alu	= 0b0010011;
static constexpr uint32_t opcode_itype_load	= 0b0000011;
static constexpr uint32_t opcode_e_x		= 0b1110011;
static constexpr uint32_t opcode_fence		= 0b0001111;

//R Type Instructions
static constexpr uint32_t funct3_add 		= 0b000;
static constexpr uint32_t funct3_sll		= 0b001;
static constexpr uint32_t funct3_slt		= 0b010;
static constexpr uint32_t funct3_sltu		= 0b011;
static constexpr uint32_t funct3_xor		= 0b100;
static constexpr uint32_t funct3_srx		= 0b101;
static constexpr uint32_t funct3_or		= 0b110;
static constexpr uint32_t funct3_and 		= 0b111;

static constexpr uint32_t funct7_add		= 0b0000000;
static constexpr uint32_t funct7_sub 		= 0b0100000;
static constexpr uint32_t funct7_srl		= 0b0000000;
static constexpr uint32_t funct7_sra		= 0b0100000;

//S Type Instructions
static constexpr uint32_t funct3_sb 		= 0b000;
static constexpr uint32_t funct3_sh		= 0b001;
static constexpr uint32_t funct3_sw		= 0b010;

//I Type ALU Instrcutions
static constexpr uint32_t funct3_addi 		= 0b000;
static constexpr uint32_t funct3_slli		= 0b001;
static constexpr uint32_t funct3_slti		= 0b010;
static constexpr uint32_t funct3_sltiu		= 0b011;
static constexpr uint32_t funct3_xori		= 0b100;
static constexpr uint32_t funct3_ori		= 0b110;
static constexpr uint32_t funct3_andi		= 0b111;

static constexpr uint32_t funct7_srli		= 0b0000000;
static constexpr uint32_t funct7_srai		= 0b0100000;

//I Type Load Instructions
static constexpr uint32_t funct3_lb		= 0b000;
static constexpr uint32_t funct3_lh		= 0b001;
static constexpr uint32_t funct3_lw		= 0b010;
static constexpr uint32_t funct3_lbu		= 0b100;
static constexpr uint32_t funct3_lhu		= 0b101;

// B Type Instructions
static constexpr uint32_t funct3_beq		= 0b000; 
static constexpr uint32_t funct3_bne		= 0b001;
static constexpr uint32_t funct3_blt		= 0b100;
static constexpr uint32_t funct3_bge		= 0b101;
static constexpr uint32_t funct3_bltu		= 0b110;
static constexpr uint32_t funct3_bgeu		= 0b111;

//Other Instructions
static constexpr uint32_t succ_ecall 		= 0b0000;
static constexpr uint32_t succ_ebreak		= 0b0001;

/**
 * Initializes the memory array.
 *
 * @param mem a pointer to a memory object
 ********************************************************************************/
rv32i::rv32i(memory *m) {
	mem = m;
}

/**
 * This method disassembles the instructions in the simulated memory
 *
 ********************************************************************************/
void rv32i::disasm() {
		
	pc = 0;
	uint32_t insn;
	while(pc < mem->get_size()) {
	
		std::cout << std::hex << std::setw(8) << std::setfill('0') << pc << ": ";	
		insn = mem->get32(pc);
		std::cout << std::hex << std::setw(8) << std::setfill('0') << insn;
		std::cout << "  " << decode(insn) << std::endl;
		pc = pc + 4;
	}
}

/*
 * Decodes the instruction passed in as a parameter
 *
 * @param insn An encoded instruction
 *
 * @return A string that contains the decoded instruction
 ********************************************************************************/
std::string rv32i::decode(uint32_t insn) const {

	uint32_t opcode = get_opcode(insn);
	uint32_t funct3 = get_funct3(insn);
	uint32_t funct7 = get_funct7(insn);
	uint32_t succ = get_succ(insn);
	int32_t imm_i = get_imm_i(insn);

	switch(opcode) {

	default:		return render_illegal_insn();
	case opcode_lui: 	return render_lui(insn);
	case opcode_auipc:	return render_auipc(insn);
	case opcode_jal:	return render_jal(insn);
	case opcode_jalr:	return render_jalr(insn);
	case opcode_rtype:

		switch(funct3) {
		default:		return render_illegal_insn();
		case funct3_add:
			switch(funct7) {
				default:		return render_illegal_insn();
				case funct7_add:	return render_rtype(insn, "add");
				case funct7_sub:	return render_rtype(insn, "sub");
			}
		case funct3_sll:	return render_rtype(insn, "sll");
		case funct3_slt:	return render_rtype(insn, "slt");
		case funct3_sltu:	return render_rtype(insn, "sltu");
		case funct3_xor: 	return render_rtype(insn, "xor");
		case funct3_srx: 
			switch(funct7) {
				default:		return render_illegal_insn();
				case funct7_srl:	return render_rtype(insn, "srl");
				case funct7_sra:	return render_rtype(insn, "sra");
			}
		case funct3_or:		return render_rtype(insn, "or");
		case funct3_and:	return render_rtype(insn, "and");
		}
	case opcode_stype:

		switch(funct3) {
			default:		return render_illegal_insn();
			case funct3_sb:		return render_stype(insn, "sb");
			case funct3_sh:		return render_stype(insn, "sh");
			case funct3_sw:		return render_stype(insn, "sw");
		}
	case opcode_itype_load:

		switch(funct3) {
			default:		return render_illegal_insn();
			case funct3_lb:		return render_itype_load(insn, "lb");
			case funct3_lh:		return render_itype_load(insn, "lh");
			case funct3_lw:		return render_itype_load(insn, "lw");
			case funct3_lbu:	return render_itype_load(insn, "lbu");
			case funct3_lhu:	return render_itype_load(insn, "lhu");
		}
	case opcode_itype_alu:

		switch(funct3) {
			default:		return render_illegal_insn();
			case funct3_addi: 	return render_itype_alu(insn, "addi", imm_i);
			case funct3_slti:	return render_itype_alu(insn, "slti", imm_i);
			case funct3_slli:	return render_itype_alu(insn, "slli", imm_i%XLEN);
			case funct3_sltiu:	return render_itype_alu(insn, "sltiu", imm_i);
			case funct3_xori:	return render_itype_alu(insn, "xori", imm_i);
			case funct3_srx:
				switch(funct7){
					default:		return render_illegal_insn();
					case funct7_srli:	return render_itype_alu(insn, "srli", imm_i%XLEN);
					case funct7_srai:	return render_itype_alu(insn, "srai", imm_i%XLEN);
				}
			case funct3_ori:	return render_itype_alu(insn, "ori", imm_i);
			case funct3_andi:	return render_itype_alu(insn, "andi", imm_i);
		}
	case opcode_btype:

		switch(funct3) {
			default:		return render_illegal_insn();
			case funct3_beq:	return render_btype(insn, "beq");
			case funct3_bne:	return render_btype(insn, "bne");
			case funct3_blt:	return render_btype(insn, "blt");
			case funct3_bge:	return render_btype(insn, "bge");
			case funct3_bltu:	return render_btype(insn, "bltu");
			case funct3_bgeu:	return render_btype(insn, "bgeu");
		}
	case opcode_e_x: 

		switch(succ) {
			default:		return render_illegal_insn();
			case succ_ecall:	return "ecall";
			case succ_ebreak:	return "ebreak";
		}
	case opcode_fence:	return render_fence(insn);
	}
}

/**
 * Isolates the opcode for a an instruction in memory
 *
 * @param insn An encoded instruction
 *
 * @return The opcode of an instruction 
 ********************************************************************************/
uint32_t rv32i::get_opcode(uint32_t insn) {

	return (insn & 0x0000007f);
}

/**
 * Isolates the the rd value in an instruction in memory
 *
 * @param insn An encoded instruction
 *
 * @return The rd value of an instruction 
 ********************************************************************************/
uint32_t rv32i::get_rd(uint32_t insn) {

	return ((insn & 0x00000f80) >> 7);
}

/**
 * Isolates the the funct3 value in an instruction in memory
 *
 * @param insn An encoded instruction
 *
 * @return The funct3 value of an instruction 
 ********************************************************************************/
uint32_t rv32i::get_funct3(uint32_t insn) {

	return ((insn & 0x00007000) >> 12);
}

/**
 * Isolates the the rs1 value in an instruction in memory
 *
 * @param insn An encoded instruction
 *
 * @return The rs1 value of an instruction 
 ********************************************************************************/
uint32_t rv32i::get_rs1(uint32_t insn) {

	return ((insn & 0x000f8000) >> 15);
}

/**
 * Isolates the the rs2 value in an instruction in memory
 *
 * @param insn An encoded instruction
 *
 * @return The rs2 value of an instruction 
 ********************************************************************************/
uint32_t rv32i::get_rs2(uint32_t insn) {

	return ((insn & 0x01f00000) >> 20);
}

/**
 * Isolates the the funct7 value in an instruction in memory
 *
 * @param insn An encoded instruction
 *
 * @return The funct7 value of an instruction 
 ********************************************************************************/
uint32_t rv32i::get_funct7(uint32_t insn) {

	return ((insn & 0xfe000000) >> 25);
}

/**
 * Isolates the the succ value in an instruction in memory
 *
 * @param insn An encoded instruction
 *
 * @return The succ value of an instruction 
 ********************************************************************************/
uint32_t rv32i::get_succ(uint32_t insn) {

	return (insn & 0x00f00000) >> 20;
}

/**
 * Isolates the the imm_i value in an instruction in memory
 *
 * @param insn An encoded instruction
 *
 * @return The imm_i value of an instruction 
 ********************************************************************************/
int32_t rv32i::get_imm_i(uint32_t insn) {

	int32_t imm_i = ((insn & 0xfff00000) >> 20);	//extract bits 0-11
	
	if (insn & 0x80000000)				//sign extend
		imm_i |= 0xfffff000;

	return imm_i;
}

/**
 * Isolates the the imm_u value in an instruction in memory
 *
 * @param insn An encoded instruction
 *
 * @return The imm_u value of an instruction 
 ********************************************************************************/
int32_t rv32i::get_imm_u(uint32_t insn) {

	return (insn & 0xfffff000);
}

/**
 * Isolates the the imm_b value in an instruction in memory
 *
 * @param insn An encoded instruction
 *
 * @return The imm_b value of an instruction 
 ********************************************************************************/
int32_t rv32i::get_imm_b(uint32_t insn) {
	
	int32_t imm_b = (insn & 0x80000000) >> (31 - 12);	//extract bit 12
	imm_b |= (insn & 0x7e000000) >> (25 - 5);		//extract bits 5-10
	imm_b |= (insn & 0x00000f00) >> (8 - 1);		//extract bits 1-4
	imm_b |= (insn & 0x00000080) << (11 - 7);		//extract bit 11

	if (insn & 0x80000000)				//sign extend
		imm_b |= 0xfffff000;

	return imm_b;
}

/**
 * Isolates the the imm_s value in an instruction in memory
 *
 * @param insn An encoded instruction
 *
 * @return The imm_s value of an instruction 
 ********************************************************************************/
int32_t rv32i::get_imm_s(uint32_t insn) {

	int32_t imm_s = (insn & 0xfe000000) >> (25 - 5);	//extract and shift bits 5-11
	imm_s |= (insn & 0x00000f80) >> (7 - 0);		//extract and shidt bits 0-4
	
	if (insn & 0x80000000)				//sign extend
		imm_s |= 0xfffff000;

	return imm_s;
}

/**
 * Isolates the the imm_j value in an instruction in memory
 *
 * @param insn An encoded instruction
 *
 * @return The imm_j value of an instruction 
 ********************************************************************************/
int32_t rv32i::get_imm_j(uint32_t insn) {

	int32_t imm_j = (insn & 0x000ff000);		//get bits 12-19
	imm_j |= (insn & 0x00100000) >> (20 - 11);	//get bit 11
	imm_j |= (insn & 0x7fe00000) >> (30 - 10);	//get bits 1-10
	imm_j |= (insn & 0x80000000) >> (31 - 20);	//get bit 20

	if (insn & 0x80000000)
		imm_j |= 0xfff00000;

	return imm_j;
}

/**
 * Prints an error message to screen when an invalid instruction is decoded
 *
 * @return The error message  
 ********************************************************************************/
std::string rv32i::render_illegal_insn() const {

	return("ERROR: UNIMPLEMENTED INSTRUCTION");
}

/**
 * Formats the disassembled lui instruction
 *
 * @param insn An encoded instruction
 *
 * @return A formatted string containing the dissambled instruction
 ********************************************************************************/
std::string rv32i::render_lui(uint32_t insn) const {

	uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_u(insn);

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << "lui" << "x" << std::dec << rd << ",0x" << std::hex << ((imm_u >> 12)&0x0fffff);
	return os.str();
}

/**
 * Formats the disassembled auipc instruction
 *
 * @param insn An encoded instruction
 *
 * @return A formatted string containing the dissambled instruction
 ********************************************************************************/
std::string rv32i::render_auipc(uint32_t insn) const {
	
	uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_u(insn);

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << "auipc" << "x" << std::dec << rd << ",0x" << std::hex << ((imm_u >> 12)&0x0fffff);
	return os.str();
}

/**
 * Formats the disassembled jal instruction
 *
 * @param insn An encoded instruction
 *
 * @return A formatted string containing the dissambled instruction
 ********************************************************************************/
std::string rv32i::render_jal(uint32_t insn) const {

	uint32_t rd = get_rd(insn);
	int32_t imm_j = get_imm_j(insn);
	int32_t pcrel = imm_j + pc;

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << "jal" << "x" << std::dec << rd << ",0x" << std::hex << pcrel;
	return os.str();
}

/**
 * Formats the disassembled jalr instruction
 *
 * @param insn An encoded instruction
 *
 * @return A formatted string containing the dissambled instruction
 ********************************************************************************/
std::string rv32i::render_jalr(uint32_t insn) const {

	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << "jalr" << "x" << std::dec << rd << "," << imm_i << "(x" << rs1 << ")";
	return os.str();
}

/**
 * Formats the disassembled rtype instruction
 *
 * @param insn An encoded instruction
 * @param *mnemonic A character string for the instruction mnemonic
 *
 * @return A formatted string containing the dissambled instruction
 ********************************************************************************/
std::string rv32i::render_rtype(uint32_t insn, const char *mnemonic) const {
	
	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	uint32_t rd = get_rd(insn);
	
	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << mnemonic << "x" << std::dec << rd << ",x" << rs1 << ",x" << rs2;
	return os.str();
}

/**
 * Formats the disassembled stype instruction
 *
 * @param insn An encoded instruction
 * @param *mnemonic A character string for the instruction mnemonic
 *
 * @return A formatted string containing the dissambled instruction
 ********************************************************************************/
std::string rv32i::render_stype(uint32_t insn, const char *mnemonic) const {

	uint32_t rs2 = get_rs2(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_s = get_imm_s(insn);

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << mnemonic << "x" << std::dec << rs2 << "," << imm_s << "(x" << rs1 << ")";
        return os.str();	
}

/**
 * Formats the disassembled itype load instruction
 *
 * @param insn An encoded instruction
 * @param *mnemonic A character string for the instruction mnemonic
 *
 * @return A formatted string containing the dissambled instruction
 ********************************************************************************/
std::string rv32i::render_itype_load(uint32_t insn, const char *mnemonic) const {
	
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill (' ') << std::left << mnemonic << 'x' << std::dec << rd << "," << imm_i << "(x" << rs1 << ")";
	return os.str();
}

/**
 * Formats the disassembled itype alu instruction
 *
 * @param insn An encoded instruction
 * @param *mnemonic A character string for the instruction mnemonic
 * @param imm_i A signed integer conatining the imm_i value of the instruction
 *
 * @return A formatted string containing the dissambled instruction
 ********************************************************************************/
std::string rv32i::render_itype_alu(uint32_t insn, const char *mnemonic, int32_t imm_i) const {

	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << mnemonic << "x" << std::dec << rd << ",x" << rs1 << "," << imm_i;
	return os.str();
}

/**
 * Formats the disassembled btype instruction
 *
 * @param insn An encoded instruction
 * @param *mnemonic A character string for the instruction mnemonic
 *
 * @return A formatted string containing the dissambled instruction
 ********************************************************************************/
std::string rv32i::render_btype(uint32_t insn, const char *mnemonic) const {

	uint32_t rs1 = get_rs1(insn);
	uint32_t rs2 = get_rs2(insn);
	int32_t imm_b = get_imm_b(insn);
	int32_t pcrel = imm_b + pc;

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ') << std::left << mnemonic << "x" << std::dec << rs1 << ",x" << rs2 << ",0x" << std::hex << pcrel;
	return os.str();
}

/**
 * Formats the disassembled fence instruction
 *
 * @param insn An encoded instruction
 * @param *mnemonic A character string for the instruction mnemonic
 *
 * @return A formatted string containing the dissambled instruction
 ********************************************************************************/
std::string rv32i::render_fence(uint32_t insn) const {

	std::ostringstream os;
	os << std::setw(mnemonic_width) << std::setfill(' ')  << std::left << "fence"
	       	<< (insn & 0x08000000 ? "i" : "") << (insn & 0x04000000 ? "o" : "") << (insn & 0x02000000 ? "r" : "") << (insn & 0x01000000 ? "w" : "") << ","
		<< (insn & 0x00800000 ? "i" : "") << (insn & 0x00400000 ? "o" : "") << (insn & 0x00200000 ? "r" : "") << (insn & 0x00100000 ? "w" : "");
	return os.str();
}


/**
 * Sets the show instruction flag
 *
 * @param b A boolean to represent whether the instructions should be shown
 ********************************************************************************/
void rv32i::set_show_instructions(bool b) {
	show_instructions = b;
}

/**
 * Sets the show registers flag
 *
 * @param b A boolean to represent whether the registers flag is used 
 ********************************************************************************/
void rv32i::set_show_registers(bool b) {
	show_registers = b;
}

/**
 * Returns the value of halted
 ********************************************************************************/
bool rv32i::is_halted() const {
	return halt; 
}

/**
 * Resets the rv32i object
 ********************************************************************************/
void rv32i::reset() {

	pc = 0;
	insn_counter = 0;
	halt = false;
	regs.reset();
}

/**
 * Sets the show instruction flag
 ********************************************************************************/
void rv32i::dump() const {

	regs.dump();
	std::cout << " pc " << std::hex << std::setfill('0') << std::setw(8) << pc << std::endl; 
}

/**
 * Renders the illegal instruction 
 *
 * @param pos a pointer to the outputstream 
 ********************************************************************************/
void rv32i::exec_illegal_insn(std::ostream* pos) {

	if(pos) {
		std::string s = render_illegal_insn();
		s.resize(instruction_width, ' ');
		*pos << s << std::endl;
	}
	halt = true;
}

/**
 * Increments the instruction counter and pulls the next instruction 
 ********************************************************************************/
void rv32i::tick() {

	if (halt == true) {
		//do nothing 
	}
	else {
		insn_counter = insn_counter + 1;
		if(show_registers == true) {
			dump();
		}
		uint32_t insn = mem->get32(pc);
		if(show_instructions == true) {
			std::cout << std::hex << std::setfill('0') << std::setw(8) << pc << ": " << std::setw(8) << insn << "  ";
			dcex(insn, &std::cout);
		}
		else {
			dcex(insn, nullptr);
		}
	}
}

/**
 * Runs the instructions until HALT or the limit is reached
 *
 * @param limit The limit to the number of instructions that can be run 
 ********************************************************************************/
void rv32i::run(uint64_t limit) {

	regs.set(2, mem->get_size());

	while ((halt != true && insn_counter < limit) || (halt != true && limit == 0)) {
		tick();
	}
	//check if last insn was ebreak
	if (get_succ(mem->get32(pc)) == succ_ebreak)
		std::cout << "Execution terminated by EBREAK instruction" << std::endl;

	std::cout << std::dec << insn_counter << " instructions executed" << std::endl;	
}

/**
 * Identify the instruction to be executed and call its function
 *
 * @param insn The insn to execute 
 * @param pos a pointer to the outputstream 
 ********************************************************************************/
void rv32i::dcex(uint32_t insn, std::ostream* pos) {

	uint32_t opcode = get_opcode(insn);
	uint32_t funct3 = get_funct3(insn);
	uint32_t funct7 = get_funct7(insn);
	uint32_t succ = get_succ(insn);

	switch(opcode) {
	
	default:		exec_illegal_insn(pos);	return;
	case opcode_lui:	exec_lui(insn, pos);	return;
	case opcode_auipc:	exec_auipc(insn, pos);	return;
	case opcode_jal:	exec_jal(insn, pos);	return;
	case opcode_jalr:	exec_jalr(insn, pos);	return;
	case opcode_e_x:

		switch(succ) {
			default:		exec_illegal_insn(pos);	return;
			case succ_ebreak:	exec_ebreak(pos);	return;
		}
	case opcode_btype:

		switch(funct3) {
			default:		exec_illegal_insn(pos);	return;
			case funct3_bne:	exec_bne(insn, pos);	return;
			case funct3_blt:	exec_blt(insn, pos);	return;
			case funct3_bge:	exec_bge(insn, pos);	return;
			case funct3_bltu:	exec_bltu(insn, pos);	return;
			case funct3_bgeu:	exec_bgeu(insn, pos);	return;
			case funct3_beq:	exec_beq(insn, pos);	return;	
		}
	case opcode_itype_load:

		switch(funct3) {
			default:		exec_illegal_insn(pos);	return;
			case funct3_lbu:	exec_lbu(insn, pos);	return;
			case funct3_lhu:	exec_lhu(insn, pos);	return;
			case funct3_lb:		exec_lb(insn, pos);	return;
			case funct3_lh:		exec_lh(insn, pos);	return;
			case funct3_lw:		exec_lw(insn, pos);	return;
		}
	case opcode_itype_alu:

		switch(funct3) {
			default:		exec_illegal_insn(pos);	return;
			case funct3_addi:	exec_addi(insn, pos);	return;
			case funct3_slti:	exec_slti(insn, pos);	return;
			case funct3_sltiu:	exec_sltiu(insn, pos);	return;
			case funct3_xori:	exec_xori(insn, pos);	return;
			case funct3_ori:	exec_ori(insn, pos);	return;
			case funct3_andi:	exec_andi(insn, pos);	return;
			case funct3_slli:	exec_slli(insn, pos);	return;
			case funct3_srx:
					switch(funct7) {
						default:		exec_illegal_insn(pos);	return;
						case funct7_srli:	exec_srli(insn, pos);	return;
						case funct7_srai:	exec_srai(insn, pos);	return;
					}
		}
	case opcode_stype:

		switch(funct3) {
			default:		exec_illegal_insn(pos);	return;
			case funct3_sb:		exec_sb(insn, pos);	return;
			case funct3_sh:		exec_sh(insn, pos);	return;
			case funct3_sw:		exec_sw(insn, pos);	return;
		}
	case opcode_rtype:

		switch(funct3) {
			default:		exec_illegal_insn(pos);	return;
			case funct3_add:
				switch(funct7) {
					default:		exec_illegal_insn(pos);	return;
					case funct7_add:	exec_add(insn, pos);	return;
					case funct7_sub:	exec_sub(insn, pos);	return;
				}
			case funct3_sll:	exec_sll(insn, pos);	return;
			case funct3_slt:	exec_slt(insn, pos);	return;
			case funct3_sltu:	exec_sltu(insn,pos);	return;
			case funct3_xor:	exec_xor(insn, pos);	return;
			case funct3_srx: 
				switch(funct7) {
					default:		exec_illegal_insn(pos);	return;
					case funct7_srl:	exec_srl(insn, pos);	return;
					case funct7_sra:	exec_sra(insn, pos);	return;
				}
			case funct3_or:		exec_or(insn, pos);	return;
			case funct3_and:	exec_and(insn, pos);	return;
		}
	case opcode_fence:		exec_fence(insn, pos);	return;
	}
}

/**
 * execute the ebreak instruction
 *
 * @param pos a pointer to the outputstream 
 ********************************************************************************/
void rv32i::exec_ebreak( std::ostream* pos) {

	if(pos) {
		std::string s = "ebreak";
		s.resize(instruction_width, ' ');
		*pos << s << "// HALT" << std::endl;
	}
	halt = true;
}

/**
 * Executes the lui instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_lui(uint32_t insn, std::ostream* pos) {
	
	int32_t rd = get_rd(insn);
	int32_t imm = get_imm_u(insn);

	if(pos) {
		std::string s = render_lui(insn);
		s.resize(instruction_width, ' ');
		*pos << s << "// " << "x" << std::dec << rd << " = 0x" << hex32(imm) << std::endl;
	}
	regs.set(rd, imm);
	pc += 4;
}	

/**
 * Executes the auipc instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_auipc(uint32_t insn, std::ostream* pos) {

	int32_t rd = get_rd(insn);
	int32_t imm = get_imm_u(insn);

	int32_t val = imm + pc;

	if(pos) {
		std::string s = render_auipc(insn);
		s.resize(instruction_width, ' ');
		*pos << s << "// " << "x" << std::dec << rd << " = 0x" << hex32(pc) << " + " << "0x" << hex32(imm) << " = 0x" << hex32(val) << std::endl; 
	}
	regs.set(rd, val);
	pc += 4;
}

/**
 * Executes the jal instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_jal(uint32_t insn, std::ostream* pos) {

	int32_t imm = get_imm_j(insn);
	int32_t rd = get_rd(insn);

	int32_t val = pc + imm;

	if(pos) {
		std::string s = render_jal(insn);
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(pc + 4) << ",  pc = 0x" << hex32(pc) << " + 0x" << hex32(imm) << " = 0x" << hex32(val) << std::endl;
	}
	regs.set(rd, pc + 4);
	pc = val;
}

/**
 * Executes the jalr instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_jalr(uint32_t insn, std::ostream* pos) {

	int32_t imm = get_imm_i(insn);
	int32_t rd = get_rd(insn);
	int32_t rs1 = get_rs1(insn);
	//turn off 1 bit
	int32_t target_addr  = (regs.get(rs1) + imm) & 0xfffffffe;

	if(pos) {
		std::string s = render_jalr(insn);
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(pc + 4) << ",  pc = (0x" << hex32(imm) << " + 0x" << hex32(regs.get(rs1)) << ") & 0xfffffffe" << " = 0x" << hex32(target_addr) << std::endl;	
	}
	regs.set(rd, pc + 4);
	pc = target_addr;
}

/**
 * Executes the bne instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_bne(uint32_t insn, std::ostream* pos) {
	
	int32_t imm = get_imm_b(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t target_addr;

	if(regs.get(rs1) != regs.get(rs2)) {
		target_addr = imm + pc;
	}
	else {
		target_addr = pc + 4;
	}

	if(pos) {
		std::string s = render_btype(insn, "bne");
		s.resize(instruction_width, ' ');
		*pos << s << "// pc += (0x" << hex32(regs.get(rs1)) << " != 0x" << hex32(regs.get(rs2)) << " ? 0x" << hex32(imm)  << " : 4" << ") = 0x" << hex32(target_addr) << std::endl;
	}
	pc = target_addr;
}

/**
 * Executes the bge instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_bge(uint32_t insn, std::ostream* pos) {

	int32_t imm = get_imm_b(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t target_addr;

	if(regs.get(rs1) >= regs.get(rs2)) {
		target_addr = imm + pc;
	}
	else {
		target_addr = pc + 4;
	}

	if(pos) {
		std::string s = render_btype(insn, "bge");
		s.resize(instruction_width, ' ');
		*pos << s << "// pc += (0x" << hex32(regs.get(rs1)) << " >= 0x" << hex32(regs.get(rs2)) << " ? 0x" << hex32(imm)  << " : 4" << ") = 0x" << hex32(target_addr) << std::endl;
	}
	pc = target_addr;
}

/**
 * Executes the blt instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_blt(uint32_t insn, std::ostream* pos) {

	int32_t imm = get_imm_b(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t target_addr;

	if(regs.get(rs1) < regs.get(rs2)) {
		target_addr = imm + pc;
	}
	else {
		target_addr = pc + 4;
	}

	if(pos) {
		std::string s = render_btype(insn, "blt");
		s.resize(instruction_width, ' ');
		*pos << s << "// pc += (0x" << hex32(regs.get(rs1)) << " < 0x" << hex32(regs.get(rs2)) << " ? 0x" << hex32(imm)  << " : 4" << ") = 0x" << hex32(target_addr) << std::endl;
	}
	pc = target_addr;
}

/**
 * Executes the bltu instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_bltu(uint32_t insn, std::ostream* pos) {

	int32_t imm = get_imm_b(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t target_addr;

	if((uint32_t)regs.get(rs1) < (uint32_t)regs.get(rs2)) {
		target_addr = imm + pc;
	}
	else {
		target_addr = pc + 4;
	}

	if(pos) {
		std::string s = render_btype(insn, "bltu");
		s.resize(instruction_width, ' ');
		*pos << s << "// pc += (0x" << hex32(regs.get(rs1)) << " <U 0x" << hex32(regs.get(rs2)) << " ? 0x" << hex32(imm)  << " : 4" << ") = 0x" << hex32(target_addr) << std::endl;
	}
	pc = target_addr;
}

/**
 * Executes the bgeu instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_bgeu(uint32_t insn, std::ostream* pos) {

	int32_t imm = get_imm_b(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t target_addr;

	if((uint32_t)regs.get(rs1) >= (uint32_t)regs.get(rs2)) {
		target_addr = imm + pc;
	}
	else {
		target_addr = pc + 4;
	}

	if(pos) {
		std::string s = render_btype(insn, "bgeu");
		s.resize(instruction_width, ' ');
		*pos << s << "// pc += (0x" << hex32(regs.get(rs1)) << " >=U 0x" << hex32(regs.get(rs2)) << " ? 0x" << hex32(imm)  << " : 4" << ") = 0x" << hex32(target_addr) << std::endl;
	}
	pc = target_addr;
}

/**
 * Executes the beq instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_beq(uint32_t insn, std::ostream* pos) {

	int32_t imm = get_imm_b(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t target_addr;

	if((uint32_t)regs.get(rs1) == (uint32_t)regs.get(rs2)) {
		target_addr = imm + pc;
	}
	else {
		target_addr = pc + 4;
	}

	if(pos) {
		std::string s = render_btype(insn, "beq");
		s.resize(instruction_width, ' ');
		*pos << s << "// pc += (0x" << hex32(regs.get(rs1)) << " == 0x" << hex32(regs.get(rs2)) << " ? 0x" << hex32(imm)  << " : 4" << ") = 0x" << hex32(target_addr) << std::endl;
	}
	pc = target_addr;
}

/**
 * Executes the addi instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_addi(uint32_t insn, std::ostream* pos) {
	
	int32_t imm = get_imm_i(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rd = get_rd(insn);

	uint32_t sum = regs.get(rs1) + imm;

	if(pos) {
		std::string s = render_itype_alu(insn, "addi", imm);
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm) << " = 0x" << hex32(sum) << std::endl; 
	}
	regs.set(rd, sum);
	pc = pc + 4;
}

/**
 * Executes the lbu instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_lbu(uint32_t insn, std::ostream* pos) {
	
	int32_t imm = get_imm_i(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rd = get_rd(insn);

	uint32_t addr = regs.get(rs1) + imm;
	uint32_t memory  = mem->get8(addr) & 0x000000ff;

	if(pos) {
		std::string s = render_itype_load(insn, "lbu");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = zx(m8(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm) << ")) = 0x" << hex32(memory) << std::endl;
	}
	regs.set(rd, memory);
	pc = pc + 4;
}

/**
 * Executes the lhu instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_lhu(uint32_t insn, std::ostream* pos) {

	int32_t imm = get_imm_i(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rd = get_rd(insn);

	uint32_t addr = regs.get(rs1) + imm;
	uint32_t memory = mem->get16(addr) & 0x0000ffff;

	if(pos) {
		std::string s = render_itype_load(insn, "lhu");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = zx(m16(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm) << ")) = 0x" << hex32(memory) << std::endl;
	}
	regs.set(rd, memory);
	pc = pc + 4;	
}

/**
 * Executes the lb instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_lb(uint32_t insn, std::ostream* pos) {

	
	int32_t imm = get_imm_i(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rd = get_rd(insn);

	uint32_t addr = regs.get(rs1) + imm;
	uint32_t memory = mem->get8(addr);
	
	//sign extend
	int32_t val = (0xff & memory);
	int32_t num = 0x80;
	if (num & val) {
		val += 0xffffff00;
	}

	if(pos) {
		std::string s = render_itype_load(insn, "lb");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = sx(m8(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm) << ")) = 0x" << hex32(val) << std::endl;
	}
	regs.set(rd, val);
	pc = pc + 4;
}

/**
 * Executes the lh instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_lh(uint32_t insn, std::ostream* pos) {

	int32_t rd = get_rd(insn);
	int32_t imm = get_imm_i(insn);
	int32_t rs1 = get_rs1(insn);

	int32_t addr = regs.get(rs1) + imm;
	uint32_t memory = mem->get16(addr);
	
	//sign extend 
	int32_t val = (0xffff & memory);
	int32_t num = 0x8000;
	if (num & val) {
		val += 0xffff0000;
	}	

	if(pos) {
		std::string s = render_itype_load(insn, "lh");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = sx(m16(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm) << ")) = 0x" << hex32(val) << std::endl;
	}
	regs.set(rd, val);
	pc += 4;
}

/**
 * Executes the lw instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_lw(uint32_t insn, std::ostream* pos) {

	
	int32_t imm = get_imm_i(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rd = get_rd(insn);

	uint32_t addr = regs.get(rs1) + imm;
	uint32_t memory = mem->get32(addr);

	if(pos) {
		std::string s = render_itype_load(insn, "lw");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = sx(m32(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm) << ")) = 0x" << hex32(memory) << std::endl;
	}
	regs.set(rd, memory);
	pc += 4;
}

/**
 * Executes the sb instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_sb(uint32_t insn, std::ostream* pos) {

	int32_t imm = get_imm_s(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);

	uint32_t addr = regs.get(rs1) + imm;
	uint32_t val = regs.get(rs2) & 0x000000ff;

	if(pos) {
		std::string s = render_stype(insn, "sb");
		s.resize(instruction_width, ' ');
		*pos << s << "// m8(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm) << ") = 0x" << hex32(val) << std::endl; 
	}
	mem->set8(addr, val);
	pc += 4;
}

/**
 * Executes the sh instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_sh(uint32_t insn, std::ostream* pos) {

	int32_t imm = get_imm_s(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);

	uint32_t addr = regs.get(rs1) + imm;
	uint32_t val = regs.get(rs2) & 0x0000ffff;

	if(pos) {
		std::string s = render_stype(insn, "sh");
		s.resize(instruction_width, ' ');
		*pos << s << "// m16(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm) << ") = 0x" << hex32(val) << std::endl; 
	}
	mem->set16(addr, val);
	pc += 4;
}

/**
 * Executes the lw instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_sw(uint32_t insn, std::ostream* pos) {

	int32_t imm = get_imm_s(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);

	uint32_t addr = regs.get(rs1) + imm;
	uint32_t val = regs.get(rs2) & 0xffffffff;

	if(pos) {
		std::string s = render_stype(insn, "sw");
		s.resize(instruction_width, ' ');
		*pos << s << "// m32(0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(imm) << ") = 0x" << hex32(val) << std::endl; 
	}
	mem->set32(addr, val);
	pc += 4;
}

/**
 * Executes the slti instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_slti(uint32_t insn, std::ostream* pos) {
	
	int32_t imm = get_imm_i(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rd = get_rd(insn);

	int comp = (regs.get(rs1) < imm);

	if(pos) {
		std::string s = render_itype_alu(insn, "slti", imm);
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = (0x" << hex32(regs.get(rs1)) << " < " << std::dec << imm << ") ? 1 : 0 = 0x" << hex32(comp) << std::endl;
	}

	regs.set(rd, comp);
	pc += 4;
}

/**
 * Executes the sltiu instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_sltiu(uint32_t insn, std::ostream* pos) {
	
	int32_t imm = get_imm_i(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t rd = get_rd(insn);

	int comp = ((uint32_t)regs.get(rs1) < (uint32_t)imm);

	if(pos) {
		std::string s = render_itype_alu(insn, "sltiu", imm);
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = (0x" << hex32(regs.get(rs1)) << " <U " << std::dec << imm << ") ? 1 : 0 = 0x" << hex32(comp) << std::endl;
	}

	regs.set(rd, comp);
	pc += 4;
}

/**
 * Executes the xori instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_xori(uint32_t insn, std::ostream* pos) {

	int32_t imm = get_imm_i(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t	rd = get_rd(insn);

	int32_t res = (regs.get(rs1) ^ imm);
	
	if(pos) {
		std::string s = render_itype_alu(insn, "xori", imm);
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " ^ 0x" << hex32(imm) << " = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the ori instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_ori(uint32_t insn, std::ostream* pos) {

	int32_t imm = get_imm_i(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t	rd = get_rd(insn);

	int32_t res = (regs.get(rs1) | imm);
	
	if(pos) {
		std::string s = render_itype_alu(insn, "ori", imm);
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " | 0x" << hex32(imm) << " = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the andi instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_andi(uint32_t insn, std::ostream* pos) {

	int32_t imm = get_imm_i(insn);
	int32_t rs1 = get_rs1(insn);
	int32_t	rd = get_rd(insn);

	int32_t res = (regs.get(rs1) & imm);
	
	if(pos) {
		std::string s = render_itype_alu(insn, "andi", imm);
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " & 0x" << hex32(imm) << " = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the slli instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_slli(uint32_t insn, std::ostream* pos) {

	int32_t shamt = get_imm_i(insn)%XLEN;
	int32_t rs1 = get_rs1(insn);
	int32_t	rd = get_rd(insn);

	int32_t res = (regs.get(rs1) << shamt);
	
	if(pos) {
		std::string s = render_itype_alu(insn, "slli", shamt);
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " << " << std::dec << shamt << " = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;

}

/**
 * Executes the srli instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_srli(uint32_t insn, std::ostream* pos) {

	int32_t shamt = get_imm_i(insn)%XLEN;
	int32_t rs1 = get_rs1(insn);
	int32_t	rd = get_rd(insn);

	int32_t res = ((uint32_t)regs.get(rs1) >> shamt);
	
	if(pos) {
		std::string s = render_itype_alu(insn, "srli", shamt);
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " >> " << std::dec << shamt << " = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the srai instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_srai(uint32_t insn, std::ostream* pos) {

	int32_t shamt = get_imm_i(insn)%XLEN;
	int32_t rs1 = get_rs1(insn);
	int32_t	rd = get_rd(insn);

	int32_t res = (regs.get(rs1) >> shamt);
	
	if(pos) {
		std::string s = render_itype_alu(insn, "srai", shamt);
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " >> " << std::dec << shamt << " = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the add instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_add(uint32_t insn, std::ostream* pos) {
	
	int32_t	rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t rd = get_rd(insn);

	int32_t res = regs.get(rs1) + regs.get(rs2);

	if(pos) {
		std::string s = render_rtype(insn, "add");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " + 0x" << hex32(regs.get(rs2)) << " = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the sub instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_sub(uint32_t insn, std::ostream* pos) {
	
	int32_t	rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t rd = get_rd(insn);

	int32_t res = regs.get(rs1) - regs.get(rs2);

	if(pos) {
		std::string s = render_rtype(insn, "sub");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " - 0x" << hex32(regs.get(rs2)) << " = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the sll instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_sll(uint32_t insn, std::ostream* pos) {
	
	int32_t	rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t rd = get_rd(insn);
	
	int32_t shift = regs.get(rs2) & 0x0000001f;

	int32_t res = regs.get(rs1) << shift;

	if(pos) {
		std::string s = render_rtype(insn, "sll");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " << " << std::dec << shift << " = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the slt instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_slt(uint32_t insn, std::ostream* pos) {
	
	int32_t	rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t rd = get_rd(insn);

	int32_t res = (regs.get(rs1) < regs.get(rs2));

	if(pos) {
		std::string s = render_rtype(insn, "slt");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = (0x" << hex32(regs.get(rs1)) << " < 0x" << hex32(regs.get(rs2)) << ") ? 1 : 0 = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the sltu instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_sltu(uint32_t insn, std::ostream* pos) {
	
	int32_t	rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t rd = get_rd(insn);

	int32_t res = ((uint32_t)regs.get(rs1) < (uint32_t)regs.get(rs2));

	if(pos) {
		std::string s = render_rtype(insn, "sltu");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = (0x" << hex32(regs.get(rs1)) << " <U 0x" << hex32(regs.get(rs2)) << ") ? 1 : 0 = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the xor instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_xor(uint32_t insn, std::ostream* pos) {

	int32_t	rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t rd = get_rd(insn);

	int32_t res = (regs.get(rs1) ^ regs.get(rs2));

	if(pos) {
		std::string s = render_rtype(insn, "xor");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " ^ 0x" << hex32(regs.get(rs2)) << " = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the srl instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_srl(uint32_t insn, std::ostream* pos) {

	int32_t	rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t rd = get_rd(insn);

	int32_t shift = regs.get(rs2) & 0x0000001f;

	int32_t res = ((uint32_t)regs.get(rs1)) >> shift;

	if(pos) {
		std::string s = render_rtype(insn, "srl");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " >> " << std::dec << shift << " = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the sra instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_sra(uint32_t insn, std::ostream* pos) {

	int32_t	rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t rd = get_rd(insn);

	int32_t shift = regs.get(rs2) & 0x0000001f;
	int32_t res = regs.get(rs1) >> shift;

	if(pos) {
		std::string s = render_rtype(insn, "sra");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " >> " << std::dec << shift << " = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the and instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_and(uint32_t insn, std::ostream* pos) {

	int32_t	rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t rd = get_rd(insn);

	int32_t res = regs.get(rs1) & regs.get(rs2);

	if(pos) {
		std::string s = render_rtype(insn, "and");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " & 0x" << hex32(regs.get(rs2)) << " = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the or instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_or(uint32_t insn, std::ostream* pos) {

	int32_t	rs1 = get_rs1(insn);
	int32_t rs2 = get_rs2(insn);
	int32_t rd = get_rd(insn);

	int32_t res = regs.get(rs1) | regs.get(rs2);

	if(pos) {
		std::string s = render_rtype(insn, "or");
		s.resize(instruction_width, ' ');
		*pos << s << "// x" << std::dec << rd << " = 0x" << hex32(regs.get(rs1)) << " | 0x" << hex32(regs.get(rs2)) << " = 0x" << hex32(res) << std::endl; 
	}
	regs.set(rd, res);
	pc += 4;
}

/**
 * Executes the fence instruction 
 *
 * @param pos a pointer to the outputstream 
 * @param insn The instruction to execute
 ********************************************************************************/
void rv32i::exec_fence(uint32_t insn, std::ostream* pos) {

	if(pos) {
		std::string s = render_fence(insn);
		s.resize(instruction_width, ' ');
		*pos << s << "// fence" << std::endl;
	}
	pc += 4;
}

