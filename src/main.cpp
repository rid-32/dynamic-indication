#include <avr/io.h>
#include <util/delay.h>

#define DIGIT PORTD
#define CATEGORY PORTB 

const uint8_t digits[10] = {
  0x3F, 0x6, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x7, 0x7F, 0x6F
};

uint8_t separateDigits[4] = {0, 0, 0, 0};

uint16_t custom_pow(uint16_t num, int8_t deg) {
  uint16_t acc = 1;

  for (uint8_t i = 1; i <= deg; i++) {
    acc *= num;
  }

  return acc;
}

void divideNumberOnDigits(uint16_t number) {
  int8_t j, k;

  for (j = 3, k = 0; j >= 0; j--, k++) {
    const uint16_t divider = custom_pow(10, j);

    separateDigits[k] = number / divider;
    number %= divider;
  }
}

void setup() {
  DDRD = 0xFF;
  DIGIT = 0x00;

  DDRB |= 0x0F;
  CATEGORY &= ~(0x0F);

  DDRC = 0xFF;
  PORTC = 0x00;
}

void loop() {
  divideNumberOnDigits(876);

  for (uint8_t i = 0; i < 4; i++) {
    CATEGORY &= 0xF0;
    CATEGORY |= (1<<i);
    DIGIT = digits[separateDigits[i]];

    _delay_ms(3);
  }
}

int main(void) {
  setup();

  while (1) {
    loop();
  }
}