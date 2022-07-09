#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

void delay(void);
void instruction(void);
void init_kbi(void);
void irq_kbi(void);
void check(void);
void data(void);
void row(int column);

unsigned int index;

unsigned char digit[17] = {"123A456B789C*0#D"};

void main(void) {
	char i;
	unsigned char initialize[9] = {0x38,0x38,0x38,0x38,0x01,0x06,0x0F,0x89,0x00};
	
	SOPT1 = 0x12;
	PTADD = 0x07;
	PTAD = 0x04;
	PTBDD = 0xFF;
	
	for(i=0;i<9;i++) {
		PTBD = initialize[i];
		instruction();
	}
	
	
	PTBDD = 0xF0;
	PTAD_PTAD2 = 0x00;
	PTBD = 0xFF;
	
	init_kbi();
	EnableInterrupts;
	
	PTBD = 0x0F;
	for(;;);
}

void instruction(void) {
	PTAD_PTAD0 = 0;
	PTAD_PTAD1 = 0;
	PTAD_PTAD1 = 1;
	delay();
	PTAD_PTAD1 = 0;
}

void init_kbi(void) {
	KBIPE = 0xF0;
	KBIES = 0x00;
	KBISC = 0x02;
}

interrupt 18 void irq_kbi(void) {
	DisableInterrupts;
	KBISC_KBACK = 0x01;
	check();
	EnableInterrupts;
}

void check(void) {
	KBIPE = 0x00;
	delay();
	index = 16;
	PTBD = 0x7F;
	row(0);
	PTBD = 0xBF;
	row(1);
	PTBD = 0xDF;
	row(2);
	PTBD = 0xEF;
	row(3);
	
	PTAD_PTAD2 = 0x01;
	PTBDD = 0xFF;
	
	PTBD = digit[index];
	
	if(index < 16) {
		data();
	}
	PTBDD = 0xF0;
	PTBD = 0x0F;
	KBIPE = 0xF0;
	PTAD_PTAD2 = 0x00;
}

void row(int column) {
	if(!PTBD_PTBD0) {
		index = column;
	}
	else if(!PTBD_PTBD1) {
		index = column + 4;
	}
	else if(!PTBD_PTBD1) {
		index = column + 8;
	}
	else if(!PTBD_PTBD1) {
		index = column + 12;
	}
}

void data(void) {
	PTAD_PTAD0 = 1;
	PTAD_PTAD1 = 0;
	PTAD_PTAD1 = 1;
	PTAD_PTAD1 = 0;
}

void delay(void) {
	SRTISC = 0x04;
	while(SRTISC_RTIF == 0);
	SRTISC_RTIACK = 1;
	SRTISC = 0x00;
}
