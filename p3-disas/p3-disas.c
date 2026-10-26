/*
 * CS 261 PA3: Mini-ELF disassembler
 *
 * Name: Colin Finnegan
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#include "p3-disas.h"

//============================================================================
void usage_p3 ()
{
	printf ( "Usage: y86 <option(s)> mini-elf-file\n" );
    printf ( " Options are:\n" );
    printf ( "  -h      Display usage\n" );
    printf ( "  -H      Show the Mini-ELF header\n" );
    printf ( "  -a      Show all with brief memory\n" );
	printf ( "  -f      Show all with full memory\n" );
	printf ( "  -s      Show the program headers\n" );
    printf ( "  -m      Show the memory contents (brief)\n" );
    printf ( "  -M      Show the memory contents (full)\n" );
    printf ( "  -d      Disassemble code contents\n" );
    printf ( "  -D      Disassemble data contents\n" );
    printf ( "Options must not be repeated neither explicitly nor implicitly.\n" );
}

//============================================================================
bool parse_command_line_p3 (int argc, char **argv,
        bool *header, bool *segments, bool *membrief, bool *memfull,
        bool *disas_code, bool *disas_data, char **file)
{
    if ( !argv || !header || !segments || !membrief || !memfull || 
        !disas_code || !disas_data || !file ) 
	{
        return false;
    }

    // Initialize all flags to false
    *header = *segments = *membrief = *memfull = *disas_code = *disas_data = false;
    *file = NULL;

    opterr = 0; // Suppress getopt error messages
    const char *optionStr = "hHafsmMdD"; // Removed uppercase S
    int opt;
    
    // Track seen options to prevent duplicates
    bool h_seen = false, H_seen = false, a_seen = false, f_seen = false;
    bool s_seen = false, m_seen = false, M_seen = false;
    bool d_seen = false, D_seen = false;

    while ( ( opt = getopt ( argc , argv , optionStr ) ) != -1 ) 
	{
        switch ( opt ) 
		{
            case 'h':
                if ( h_seen ) 
				{
                    usage_p3();
                    return false;
                }
                h_seen = true;
                usage_p3();
                return true;

            case 'H':
                if ( H_seen ) 
				{
                    usage_p3();
                    return false;
                }
                H_seen = true;
                *header = true;
                break;

            case 'a':
                if ( a_seen || f_seen ) 
				{
                    usage_p3();
                    return false;
                }
                a_seen = true;
                *header = *segments = *membrief = true;
                break;

            case 'f':
                if ( f_seen || a_seen ) 
				{
                    usage_p3();
                    return false;
                }
                f_seen = true;
                *header = *segments = *memfull = true;
                break;

            case 's':
                if ( s_seen ) 
				{
                    usage_p3();
                    return false;
                }
                s_seen = true;
                *segments = true;
                break;

            case 'm':
                if ( m_seen || M_seen ) 
				{
                    usage_p3();
                    return false;
                }
                m_seen = true;
                *membrief = true;
                break;

            case 'M':
                if ( M_seen || m_seen ) 
				{
                    usage_p3();
                    return false;
                }
                M_seen = true;
                *memfull = true;
                break;

            case 'd':
                if ( d_seen ) 
				{
                    usage_p3();
                    return false;
                }
                d_seen = true;
                *disas_code = true;
                break;

            case 'D':
                if ( D_seen ) 
				{
                    usage_p3();
                    return false;
                }
                D_seen = true;
                *disas_data = true;
                break;

            case '?':
                usage_p3();
                return false;

            default:
                usage_p3();
                return false;
        }
    }

    // Check if we have exactly one non-option argument (the filename)
    if ( optind != argc - 1 ) 
	{
        if ( !h_seen ) 
		{  // Only show usage if -h wasn't used
            usage_p3();
        }
        return h_seen;  // Return true if -h was used, false otherwise
    }

    *file = argv [ optind ];
    return true;
}

//============================================================================
y86_inst_t fetch(y86_t *cpu, memory_t memory)
{
    y86_inst_t ins;
    // Initialize the instruction
    memset ( &ins , 0 , sizeof ( y86_inst_t ) );  // Clear all fields
    ins.type = INVALID;   // Invalid instruction until proven otherwise
    
	if ( cpu == NULL || memory == NULL ) 
	{
		return ins;
    }
	
    address_t instAddr;
    uint8_t byte0, byte1;
    y86_rnum_t ra, rb;

	
    instAddr = cpu->pc;   // Where to fetch instruction from
    
    // Check for invalid PC
    if ( instAddr >= MEMSIZE ) 
	{
        cpu->stat = ADR;  // an invalid fetch address
        return ins;       // an INVALID instruction
    }
    
    byte0 = memory [ instAddr ];  // fetch 1st byte of instr.
    ins.opcode = byte0;
    
    switch ( byte0 ) 
	{
        case 0x00:
            ins.type = HALT;
            ins.size = 1;
            break;
            
        case 0x10:
            ins.type = NOP;
            ins.size = 1;
            break;
            
        case 0x20: case 0x21: case 0x22: case 0x23:  // cmovXX
        case 0x24: case 0x25: case 0x26:
            // Is instruction too big to fit in remaining memory?
            if ( ( instAddr + 1 ) >= MEMSIZE ) 
			{
                cpu->stat = ADR;
                return ins;
            }
            byte1 = memory [ instAddr + 1 ];  // fetch 2nd byte
            ra = ( byte1 >> 4 ) & 0xF;
            rb = byte1 & 0xF;
            
            if ( ra >= BADREG || rb >= BADREG ) 
			{  // Invalid registers?
                cpu->stat = INS;
                return ins;
            }
            
            ins.size = 2;
            ins.type = CMOV;
            ins.cmov = byte0 - 0x20;
            ins.ra = ra;
            ins.rb = rb;
            break;
            
        case 0x30:  // irmovq
            if ( ( instAddr + 9 ) >= MEMSIZE ) 
			{
                cpu->stat = ADR;
                return ins;
            }
            byte1 = memory [ instAddr + 1 ];
            ra = ( byte1 >> 4 ) & 0xF;
            rb = byte1 & 0xF;
            
            if ( ra != 0xF || rb >= BADREG ) 
			{
                cpu->stat = INS;
                return ins;
            }
            
            ins.size = 10;
            ins.type = IRMOVQ;
            ins.ra = ra;
            ins.rb = rb;
            memcpy ( &ins.value , memory + instAddr + 2 , 8 );
            break;
            
        case 0x40:  // rmmovq
            if ( ( instAddr + 9 ) >= MEMSIZE ) 
			{
                cpu->stat = ADR;
                return ins;
            }
            byte1 = memory [ instAddr + 1 ];
            ra = ( byte1 >> 4 ) & 0xF;
            rb = byte1 & 0xF;
            
            if ( ra >= BADREG ) 
			{
                cpu->stat = INS;
                return ins;
            }
            
            ins.size = 10;
            ins.type = RMMOVQ;
            ins.ra = ra;
            ins.rb = rb;
            memcpy ( &ins.d, memory + instAddr + 2, 8 );
            break;
            
        case 0x50:  // mrmovq
            if ( ( instAddr + 9 ) >= MEMSIZE ) 
			{
                cpu->stat = ADR;
                return ins;
            }
            byte1 = memory [ instAddr + 1 ];
            ra = ( byte1 >> 4 ) & 0xF;
            rb = byte1 & 0xF;
            
            if ( ra >= BADREG || rb >= BADREG ) 
			{
                cpu->stat = INS;
                return ins;
            }
            
            ins.size = 10;
            ins.type = MRMOVQ;
            ins.ra = ra;
            ins.rb = rb;
            memcpy ( &ins.d , memory + instAddr + 2 , 8 );
            break;
            
        case 0x60: case 0x61: case 0x62: case 0x63:  // OPq
            if ( ( instAddr + 1 ) >= MEMSIZE ) 
			{
                cpu->stat = ADR;
                return ins;
            }
            byte1 = memory [ instAddr + 1 ];
            ra = ( byte1 >> 4 ) & 0xF;
            rb = byte1 & 0xF;
            
            if ( ra >= BADREG || rb >= BADREG ) 
			{
                cpu->stat = INS;
                return ins;
            }
            
            ins.size = 2;
            ins.type = OPQ;
            ins.op = byte0 - 0x60;
            ins.ra = ra;
            ins.rb = rb;
            break;
            
        case 0x70: case 0x71: case 0x72: case 0x73:  // jXX
        case 0x74: case 0x75: case 0x76:
            if ( ( instAddr + 8 ) >= MEMSIZE ) 
			{
                cpu->stat = ADR;
                return ins;
            }
            
            ins.size = 9;
            ins.type = JUMP;
            ins.jump = byte0 - 0x70;
            memcpy ( &ins.dest , memory + instAddr + 1 , 8 );
            break;
            
        case 0x80:  // call
            if ( ( instAddr + 8 ) >= MEMSIZE ) 
			{
                cpu->stat = ADR;
                return ins;
            }
            
            ins.size = 9;
            ins.type = CALL;
            memcpy ( &ins.dest , memory + instAddr + 1 , 8 );
            break;
            
        case 0x90:  // ret
            ins.type = RET;
            ins.size = 1;
            break;
            
        case 0xA0:  // pushq
            if ( ( instAddr + 1 ) >= MEMSIZE ) 
			{
                cpu->stat = ADR;
                return ins;
            }
            byte1 = memory [ instAddr + 1 ];
            ra = ( byte1 >> 4 ) & 0xF;
            rb = byte1 & 0xF;
            
            if ( ra >= BADREG || rb != 0xF ) 
			{  // rB must be 0xF
                cpu->stat = INS;
                return ins;
            }
            
            ins.size = 2;
            ins.type = PUSHQ;
            ins.ra = ra;
            ins.rb = rb;
            break;
            
        case 0xB0:  // popq
            if ( ( instAddr + 1 ) >= MEMSIZE ) 
			{
                cpu->stat = ADR;
                return ins;
            }
            byte1 = memory [ instAddr + 1 ];
            ra = ( byte1 >> 4 ) & 0xF;
            rb = byte1 & 0xF;
            
            if ( ra >= BADREG || rb != 0xF ) 
			{
                cpu->stat = INS;
                return ins;
            }
            
            ins.size = 2;
            ins.type = POPQ;
            ins.ra = ra;
            ins.rb = rb;
            break;
            
        default:
            ins.type = INVALID;
            cpu->stat = INS;
            break;
    }
    
    return ins;
}

//============================================================================
void disassemble (y86_inst_t inst)
{
	const char *reg_names[] = 
	{
        "%rax", "%rcx", "%rdx", "%rbx",
        "%rsp", "%rbp", "%rsi", "%rdi",
        "%r8", "%r9", "%r10", "%r11",
        "%r12", "%r13", "%r14", "%bad"
    };

    switch ( inst.type ) 
	{
        case HALT:
            printf ( "halt  " );
            break;
        case NOP:
            printf ( "nop   " );
            break;
        case CMOV:
            switch ( inst.cmov ) 
			{
                case RRMOVQ: printf ( "rrmovq " ); break;
                case CMOVLE: printf ( "cmovle " ); break;
                case CMOVL:  printf ( "cmovl  " ); break;
                case CMOVE:  printf ( "cmove  " ); break;
                case CMOVNE: printf ( "cmovne " ); break;
                case CMOVGE: printf ( "cmovge " ); break;
                case CMOVG:  printf ( "cmovg  " ); break;
                default:     printf ( "cmov?? " ); break;
            }
            printf ( "%s, %s", reg_names [ inst.ra ] , reg_names [ inst.rb ] );
            break;
        case IRMOVQ:
            printf ( "irmovq 0x%lx, %s" , inst.value , reg_names [ inst.rb ] );
            break;
        case RMMOVQ:
            printf ( "rmmovq %s, 0x%lx" , reg_names [ inst.ra ] , inst.d );
            if ( inst.rb <= 0xE ) 
			{  // Only show (%reg) if register is valid
                printf ("(%s)" , reg_names [ inst.rb ] );
            }
            break;

        case MRMOVQ:
            printf ( "mrmovq 0x%lx(%s), %s" , 
                  inst.d , reg_names [ inst.rb ] , reg_names [ inst.ra ] );
            break;
        case OPQ:
            switch ( inst.op ) 
			{
                case ADD: printf ( "addq   " ); break;
                case SUB: printf ( "subq   " ); break;
                case AND: printf ( "andq   " ); break;
                case XOR: printf ( "xorq   " ); break;
                default:  printf ( "opq??  " ); break;
            }
            printf ("%s, %s" , reg_names [ inst.ra ] , reg_names [ inst.rb ] );
            break;
        case JUMP:
            switch ( inst.jump ) 
			{
                case JMP: printf  ("jmp    " ); break;
                case JLE: printf ( "jle    " ); break;
                case JL:  printf ( "jl     " ); break;
                case JE:  printf ( "je     " ); break;
                case JNE: printf ( "jne    " ); break;
                case JGE: printf ( "jge    " ); break;
                case JG:  printf ( "jg     " ); break;
                default:  printf ( "j??    " ); break;
            }
            printf ( "0x%lx" , inst.dest );
            break;
        case CALL:
            printf ( "call   0x%lx" , inst.dest );
            break;
        case RET:
            printf ( "ret   " );
            break;
        case PUSHQ:
            printf ( "pushq  %s" , reg_names [ inst.ra ] );
            break;
        case POPQ:
            printf ( "popq   %s" , reg_names [ inst.ra ] );
            break;
        case INVALID:
            printf ( "invalid" );
            break;
    }
}

//============================================================================
void disassemble_code(memory_t memory, elf_phdr_t *phdr, elf_hdr_t *hdr)
{
    if ( !memory || !phdr || !hdr ) 
	{
		return;
	}
	
	printf ( "  0x%03x:                       |  .pos 0x%03x code\n" , 
           phdr->p_vaddr , phdr->p_vaddr );

    y86_t cpu = { 0 };
    cpu.pc = phdr->p_vaddr;
    address_t end = phdr->p_vaddr + phdr->p_filesz;
    bool entry_printed = false;

    while ( cpu.pc < end ) 
	{
        // Check if current PC is the entry point and we haven't printed it yet
        if ( cpu.pc == hdr->e_entry && !entry_printed ) 
		{
            printf ( "  0x%03x:                       |  _start:\n" , ( unsigned )cpu.pc );
            entry_printed = true;
        }

        y86_inst_t ins = fetch ( &cpu , memory );
		
		if ( ins.type == INVALID ) 
		{
            printf ( "Invalid opcode: 0x%02x\n" , ins.opcode );
            break;  // Stop disassembling this segment
        }
        
        // Print address and hex bytes
        printf ( "  0x%03x: " , ( unsigned )cpu.pc );
        for ( unsigned i = 0; i < ins.size; i++ ) 
		{
            printf ( "%02x" , memory [ cpu.pc + i ] );
        }
        printf ( "%*s  |    " , ( 10 - ins.size ) * 2, "" );  // Padding
        
        // Disassemble with proper spacing
        disassemble ( ins );
        printf ( "\n" );
        
        cpu.pc += ins.size;
    }
}

//============================================================================
void disassemble_data (memory_t memory, elf_phdr_t *phdr)
{
	if ( !memory || !phdr ) 
	{
		return;
	}
	
	printf ("  0x%03x:                       |  .pos 0x%03x data\n" , 
           phdr->p_vaddr, phdr->p_vaddr );

    for ( uint32_t addr = phdr->p_vaddr; addr < phdr->p_vaddr + phdr->p_filesz; addr += 8 ) 
	{
        // Print address with consistent spacing
        printf ( "  0x%03x: ", addr );
        
        // Print hex bytes (always 16 chars for quad words)
        for ( int i = 0; i < 8 && ( addr + i ) < phdr->p_vaddr + phdr->p_filesz; i++ ) 
		{
            printf ( "%02x" , memory [ addr + i ] );
        }
        
        // Print the .QUAD directive with proper spacing
        uint64_t value = 0;
        memcpy ( &value , memory + addr , 8 );
        printf ( "      |    .QUAD 0x%lx" , value );  // 6 spaces before |
        
        printf ( "\n" );
    }
}

//============================================================================
void disassemble_rodata (memory_t memory, elf_phdr_t *phdr)
{
	if ( !memory || !phdr ) 
	{
		return;
	}
	printf ( "  0x%03x:                       |  .pos 0x%03x rodata\n" , 
           phdr->p_vaddr , phdr->p_vaddr );

    uint32_t addr = phdr->p_vaddr;
    uint32_t end = phdr->p_vaddr + phdr->p_filesz;

    while ( addr < end ) 
	{
        // Find null-terminated string
        uint32_t str_start = addr;
        while ( addr < end && memory [ addr ] != '\0' ) 
		{
            addr++;
        }
        if ( addr >= end ) 
		{
			break;
		}
        
        // Print string with proper wrapping
        uint32_t current = str_start;
        bool first_line = true;
        
        while ( current <= addr ) 
		{
            printf ( "  0x%03x: " , current );
            uint32_t line_end = ( current + 10 <= addr ) ? current + 10 : addr + 1;
            
            // Print hex bytes
            for ( uint32_t i = current; i < line_end; i++ ) 
			{
                printf ( "%02x" , memory [ i ] );
            }
            
            // Add padding for partial lines
            if ( line_end - current < 10 ) 
			{
                printf ( "%*s" , ( 10 - ( line_end - current ) ) * 2, "" );
            }
            
            // Print string directive or continuation
            if ( first_line ) 
			{
                printf ( "  |    .STRING \"" );
                for ( uint32_t i = str_start; i < addr; i++ ) 
				{
                    putchar ( memory [ i ] );
                }
                printf ( "\"\n" );
                first_line = false;
            } 
			else 
			{
                printf ( "  |  \n" );
            }
            
            current = line_end;
        }
        
        addr++; // Skip null terminator
    }
}
//============================================================================

