
#define SV_SIGNATURE "test 2"

int a;

void fun() {
  a = 5;
  }

void main() {
  while(1) {
    a++;
    fun();
    a--;
    a++;
    }
  }
