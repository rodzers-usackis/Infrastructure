#include <util/delay.h>
#include <avr/io.h>
#include <usart.h>
#include <display.h>

#define DELAY 1000
#define moduloFunction(number) ((number+1)%10)

int main() {
  initUSART();
  initDisplay();

  int first = 1;
  int second = 2;
  int third = 3;
  int fourth = 4;

  int result;
  
  while(1) {
    result = first*1000 + second*100 + third*10 + fourth;

    printf("\nDisplayed number: %d", result); // Any number less than 4 digits long gets a 0 put in front of it, that's why "Displayed number: 123" is fine in this case.
    writeNumberAndWait(result, DELAY);

    first = moduloFunction(first);
    second = moduloFunction(second);
    third = moduloFunction(third);
    fourth = moduloFunction(fourth);
  }

  return 0;
}