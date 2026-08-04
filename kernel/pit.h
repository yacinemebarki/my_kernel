#ifndef PIT_H
#define PIT_H
#define HZ 100

unsigned read_pit_count(void);
void pit_init(unsigned divisor);
unsigned long get_seconds(void);

#endif