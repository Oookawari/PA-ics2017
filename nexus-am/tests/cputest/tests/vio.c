#include "trap.h"
void fun(){
	

}
int main() {
        unsigned char *p = (void*)0x8049000;
	*p = 0;
	while(*p != 0xff);
	*p = 0x33;
	*p = 0x34;
	*p = 0x86;
	return 0;
}
