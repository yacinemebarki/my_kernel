#include "types.h"
#include "asm_operation.h"
#include "pit.h"

extern unsigned long ticks;

unsigned read_pit_count(void){
    unsigned count = 0;
    cli();
    outb(0x43, 0x00);

    count = inb(0x40);
    count |= inb(0x40) << 8;
    sti();
    return count;
}

void pit_init(unsigned divisor){
    cli();
    outb(0x43, 0b00110110);

    outb(0x40, divisor&0xFF);
    outb(0x40, (divisor>>8)&0xFF);
    sti();
}

unsigned long get_seconds(void){
    return ticks / HZ;
}


