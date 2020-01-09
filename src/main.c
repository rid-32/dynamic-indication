#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#define DIGIT PORTD
#define CATEGORY PORTB 

const uint8_t digits[10] = {
  0x3F, 0x6, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x7, 0x7F, 0x6F
};

uint8_t separateDigits[4] = {0, 0, 0, 0};

void divideNumberOnDigits(uint16_t number) {
  uint16_t i = number;
  int8_t j = 3;

  while (j >= 0) {
    const uint16_t divider = pow(10, j);

    separateDigits[j] = i / divider;
    i %= divider;
    j--;
  }
}

void setup() {
  DDRD = 0xFF;
  DIGIT = 0x00;

  DDRB |= 0x0F;
  CATEGORY &= ~(0x0F);

  DDRC = 0xFF;
  PORTC = 0x00;

  divideNumberOnDigits(1111);

  CATEGORY |= (1<<0);
  DIGIT = digits[separateDigits[0]];
}

void loop() {}

int main(void) {
  setup();

  while (1) {
    loop();
  }
}