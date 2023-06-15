#include <util/delay.h>
#include <avr/io.h>
#include <usart.h>
#include <display.h>
#include <usart.h>

int main() {
  initUSART();
  initDisplay();

  char string[] = "/lol"; // This thing can't display words properly because it doesn't have enough segments :(
  printf("\nDisplaying: %s", string);

  while (1) {
    writeString(string);
  }

  return 0;
}