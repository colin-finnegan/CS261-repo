/*
 * CS 261 PA4: Mini-ELF interpreter
 *
 * Name: Colin Finnegan
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "p4-interp.h"

// Helper function prototypes
static y86_register_t get_register_value(y86_t *cpu, y86_rnum_t reg);
static void set_register_value(y86_t *cpu, y86_rnum_t reg, y86_register_t value);
static bool check_condition(y86_t *cpu, y86_cmov_t cmov);
static y86_register_t execute_alu(y86_op_t op, y86_register_t a, y86_register_t b,
                                flag_t *zf, flag_t *sf, flag_t *of);
static void write_memory(memory_t memory, address_t addr, y86_register_t value, int size);
static y86_register_t read_memory(memory_t memory, address_t addr, int size);
static bool debug_mode = false;
static void set_debug_mode(bool mode);
static bool show_memory_ops = false;
static void set_show_memory_ops(bool enable);

//=======================================================================

void usage_p4()
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
    printf ( "  -e      Execute program\n" );
    printf ( "  -E      Execute program (debug trace mode)\n" );
    printf ( "Options must not be repeated neither explicitly nor implicitly.\n" );
}

//=======================================================================

bool parse_command_line_p4(int argc, char **argv,
                         bool *header, bool *segments, bool *membrief, bool *memfull,
                         bool *disas_code, bool *disas_data,
                         bool *exec_normal, bool *exec_debug, char **file)
{
    if ( !argv || !header || !segments || !membrief || !memfull || 
        !disas_code || !disas_data || !exec_normal || !exec_debug || !file ) 
	{
        usage_p4();
		return false;
    }

    // Initialize all flags to false
    *header = *segments = *membrief = *memfull = *disas_code = *disas_data = false;
    *exec_normal = *exec_debug = false;
    *file = NULL;

    opterr = 0; // Suppress getopt error messages
    const char *optionStr = "hHafsmMdDeE";
    int opt;
    
    while ( ( opt = getopt ( argc , argv , optionStr ) ) != -1 ) 
	{
        switch ( opt ) 
		{
            case 'h':
                usage_p4();
                return true;

            case 'H':
                *header = true;
                break;

            case 'a':
                *header = *segments = *membrief = true;
                break;

            case 'f':
                *header = *segments = *memfull = true;
                break;

            case 's':
                *segments = true;
                break;

            case 'm':
                *membrief = true;
                break;

            case 'M':
                *memfull = true;
                break;

            case 'd':
                *disas_code = true;
                break;

            case 'D':
                *disas_data = true;
                break;

            case 'e':
                *exec_normal = true;
				set_show_memory_ops ( true );
                break;

            case 'E':
                *exec_debug = true;
                *exec_normal = false;
				set_debug_mode ( true );
				set_show_memory_ops ( true );
                break;

            case '?':
                usage_p4();
                return false;

            default:
                usage_p4();
                return false;
        }
    }

    // Check if we have exactly one non-option argument (the filename)
    if ( optind != argc - 1 ) 
	{
        usage_p4();
        return false;
    }

	if ( optind < argc - 1 ) 
	{
        usage_p4();
        return false;
    }
    *file = argv [ optind ];
    return true;
}

//=======================================================================

void dump_cpu(const y86_t *cpu)
{
    printf ( "dump of Y86 CPU:\n  %%rip: %016lx   flags: SF%d ZF%d OF%d  " , cpu->pc, cpu->sf, cpu->zf, cpu->of );
    switch ( cpu->stat ) 
	{
        case AOK: printf ( "AOK\n" ); break;
        case HLT: printf ( "HLT\n" ); break;
        case ADR: printf ( "ADR\n" ); break;
        case INS: printf ( "INS\n" ); break;
    }
    
    printf ( "  %%rax: %016lx    %%rcx: %016lx\n" , cpu->rax , cpu->rcx );
    printf ( "  %%rdx: %016lx    %%rbx: %016lx\n" , cpu->rdx , cpu->rbx );
    printf ( "  %%rsp: %016lx    %%rbp: %016lx\n" , cpu->rsp , cpu->rbp );
    printf ( "  %%rsi: %016lx    %%rdi: %016lx\n" , cpu->rsi , cpu->rdi );
    printf ( "   %%r8: %016lx     %%r9: %016lx\n" , cpu->r8 , cpu->r9 );
    printf ( "  %%r10: %016lx    %%r11: %016lx\n" , cpu->r10 , cpu->r11 );
    printf ( "  %%r12: %016lx    %%r13: %016lx\n" , cpu->r12 , cpu->r13 );
    printf ( "  %%r14: %016lx\n\n", cpu->r14 );
} 

//=======================================================================

y86_register_t decode_execute(y86_t *cpu, bool *cond, const y86_inst_t *inst,
                            y86_register_t *valA)
{
    if ( cpu == NULL ) 
	{
        return 0;  // Can't set status if cpu is NULL
    }
    if ( cond == NULL || inst == NULL || valA == NULL ) 
	{
        cpu->stat = INS;
        return 0;
    }
    if ( cpu->stat != AOK ) 
	{
        return 0;
    }

    y86_register_t valE = 0;
    *cond = true;

    switch ( inst->type ) 
	{
        case HALT:
			cpu->pc += 1;
            cpu->stat = HLT;
            cpu->zf = cpu->sf = cpu->of = false;
            break;

        case NOP:
            break;

        case CMOV:
            *valA = get_register_value ( cpu , inst->ra );
            *cond = check_condition ( cpu , inst->cmov );
            valE = *valA; 
            break;

        case IRMOVQ:
            valE = inst->value;
            break;

        case RMMOVQ:
            *valA = get_register_value ( cpu , inst->ra );
            valE = get_register_value ( cpu , inst->rb ) + inst->d;
            break;

        case MRMOVQ:
            *valA = 0;
            valE = get_register_value ( cpu , inst->rb ) + inst->d;
            break;

        case OPQ: 
		{
            if ( inst->op >= BADOP ) 
			{
                cpu->stat = INS;
                return 0;
            }
			*valA = get_register_value ( cpu , inst->ra );
            y86_register_t valB = get_register_value ( cpu , inst->rb );
            valE = execute_alu ( inst->op , *valA , valB , &cpu->zf , &cpu->sf , &cpu->of );
            break;
        }

        case JUMP:
            valE = inst->dest;
            *cond = check_condition ( cpu , inst->jump );
            break;

        case CALL:
            valE = inst->dest;
            *valA = cpu->pc + inst->size;
            break;

        case RET:
            *valA = cpu->rsp;
            valE = cpu->rsp + 8;
            break;

        case PUSHQ:
            *valA = get_register_value ( cpu , inst->ra );
            valE = cpu->rsp - 8;
            break;

        case POPQ:
            *valA = cpu->rsp;
            valE = cpu->rsp + 8;
            break;

        default:
            cpu->stat = INS;
            return 0;
    }

    return valE;
}

//=======================================================================

void memory_wb_pc(y86_t *cpu, memory_t memory, bool cond,
                const y86_inst_t *inst, y86_register_t valE,
                y86_register_t valA)
{
    if ( !cpu || cpu->stat != AOK ) return;

    switch ( inst->type ) 
	{
        case HALT:
            cpu->pc += 1;  // Add this line
			cpu->stat = HLT;
            cpu->zf = cpu->sf = cpu->of = false;
            break;
			
		case CMOV:
            if ( cond ) set_register_value ( cpu , inst->rb , valE );
            cpu->pc += inst->size;
            break;

        case IRMOVQ:
            set_register_value ( cpu , inst->rb , valE );
            cpu->pc += inst->size;
            break;

        case RMMOVQ:
            if ( valE >= MEMSIZE ) 
			{
                cpu->stat = ADR;
            } 
			else 
			{
				if ( show_memory_ops ) 
				{
					printf("Memory write to 0x%04lx: 0x%lx\n", valE, valA);
				}
                write_memory(memory, valE, valA, 8);
            }
            cpu->pc += inst->size;
            break;

        case MRMOVQ:
            if ( valE >= MEMSIZE ) 
			{
                cpu->stat = ADR;
            } 
			else 
			{
                y86_register_t valM = read_memory( memory , valE , 8 );
                set_register_value ( cpu , inst->ra , valM );
            }
            cpu->pc += inst->size;
            break;

        case OPQ:
            set_register_value ( cpu , inst->rb , valE );
            cpu->pc += inst->size;
            break;

        case JUMP:
            cpu->pc = cond ? valE : cpu->pc + inst->size;
            break;

        case CALL:
            if ( valE >= MEMSIZE || cpu->rsp < 8 ) 
			{
                cpu->stat = ADR;
            } 
			else 
			{
				if ( show_memory_ops ) 
				{
                    printf ( "Memory write to 0x%04lx: 0x%lx\n" , 
                           cpu->rsp - 8 , valA );
                }
                write_memory ( memory , cpu->rsp - 8 , valA, 8 );
                cpu->rsp -= 8;
                cpu->pc = valE;
            }
            break;

        case RET:
            if ( cpu->rsp >= MEMSIZE - 8 ) 
			{
				cpu->stat = ADR;
			} 
			else 
			{
				// Pop return address from stack
				cpu->pc = read_memory ( memory , cpu->rsp , 8 );
				cpu->rsp += 8;  // Update stack pointer
			}
			break;

        case PUSHQ:
            if ( valE >= MEMSIZE ) 
			{
                cpu->stat = ADR;
            } 
			else 
			{
				if ( show_memory_ops ) 
				{
					printf ( "Memory write to 0x%04lx: 0x%lx\n" , valE, valA );
				}
                write_memory ( memory , valE , valA , 8 );
                cpu->rsp = valE;
                cpu->pc += inst->size;
            }
            break;

        case POPQ:
            if ( valA >= MEMSIZE ) 
			{
                cpu->stat = ADR;
            }
			else 
			{
                y86_register_t valM = read_memory ( memory , valA , 8 );
                set_register_value ( cpu , inst->ra , valM );
                cpu->rsp = valE;
                cpu->pc += inst->size;
            }
            break;

        default:
            cpu->pc += inst->size;
            break;
    }
}

// Helper Functions =====================================================

static y86_register_t get_register_value(y86_t *cpu, y86_rnum_t reg)
{
    switch ( reg ) 
	 {
        case RAX: return cpu->rax;
        case RCX: return cpu->rcx;
        case RDX: return cpu->rdx;
        case RBX: return cpu->rbx;
        case RSP: return cpu->rsp;
        case RBP: return cpu->rbp;
        case RSI: return cpu->rsi;
        case RDI: return cpu->rdi;
        case R8:  return cpu->r8;
        case R9:  return cpu->r9;
        case R10: return cpu->r10;
        case R11: return cpu->r11;
        case R12: return cpu->r12;
        case R13: return cpu->r13;
        case R14: return cpu->r14;
        default:  return 0;
    }
}

static void set_register_value(y86_t *cpu, y86_rnum_t reg, y86_register_t value)
{
    switch ( reg ) 
	{
        case RAX: cpu->rax = value; break;
        case RCX: cpu->rcx = value; break;
        case RDX: cpu->rdx = value; break;
        case RBX: cpu->rbx = value; break;
        case RSP: cpu->rsp = value; break;
        case RBP: cpu->rbp = value; break;
        case RSI: cpu->rsi = value; break;
        case RDI: cpu->rdi = value; break;
        case R8:  cpu->r8 = value; break;
        case R9:  cpu->r9 = value; break;
        case R10: cpu->r10 = value; break;
        case R11: cpu->r11 = value; break;
        case R12: cpu->r12 = value; break;
        case R13: cpu->r13 = value; break;
        case R14: cpu->r14 = value; break;
        default: break;
    }
}

static bool check_condition(y86_t *cpu, y86_cmov_t cmov)
{
    switch ( cmov ) 
	{
        case RRMOVQ: return true;
        case CMOVLE: return ( cpu->sf ^ cpu->of ) || cpu->zf;
        case CMOVL:  return ( cpu->sf ^ cpu->of );
        case CMOVE:  return cpu->zf;
        case CMOVNE: return !cpu->zf;
        case CMOVGE: return !( cpu->sf ^ cpu->of );
        case CMOVG:  return !( cpu->sf ^ cpu->of ) && !cpu->zf;
        default:     return false;
    }
}

static y86_register_t execute_alu(y86_op_t op, y86_register_t a, y86_register_t b,
                                flag_t *zf, flag_t *sf, flag_t *of)
{
    y86_register_t result = 0;
	int64_t signed_a =  ( int64_t )a;
    int64_t signed_b = ( int64_t )b;
    switch ( op ) 
	{
        case ADD:
            result = a + b;
            *of = ( ( signed_b > 0 && signed_a > 0 && ( int64_t ) result < 0 ) ||
                  ( signed_b < 0 && signed_a < 0 && ( int64_t ) result >= 0 ) );
            break; 
        case SUB:
            result = b - a;
            *of = ( ( signed_b > 0 && signed_a < 0 && ( int64_t ) result < 0 ) ||
                  ( signed_b < 0 && signed_a > 0 && ( int64_t ) result > 0 ) );
            break;
        case AND:
            result = a & b;
            *of = false;
            break;
        case XOR:
            result = a ^ b;
            *of = false;
            break;
        default:
            result = 0;
            break;
    }
    *zf = ( result == 0 );
    *sf = ( result >> 63 );
    return result;
}

static void write_memory(memory_t memory, address_t addr, y86_register_t value, int size)
{
    for ( int i = 0; i < size; i++ ) 
	{
        memory [ addr + i ] = ( value >> ( i * 8 ) ) & 0xFF;
    }
}

static y86_register_t read_memory(memory_t memory, address_t addr, int size)
{
    y86_register_t value = 0;
    for ( int i = 0; i < size; i++ ) 
	{
        value |= ( ( y86_register_t ) memory [ addr + i ] ) << ( i * 8 );
    }
    return value;
}

static void set_debug_mode(bool mode) 
{
    debug_mode = mode;
}
static void set_show_memory_ops(bool enable) 
{
    show_memory_ops = enable;
}