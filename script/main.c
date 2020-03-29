
#include <Sv7Sys.h>

#define SV_SIGNATURE "test 5"

int a;

void fun() {
  a = 5;
  }

int b;

void task1() {
  while(1) {
    svWaitTick( 1000 );
    b++;
    }
}

void main() {
  svCreateTask( task1, 100 );
  while(1) {
    a++;
    fun();
    a--;
    a++;
    }
  }
