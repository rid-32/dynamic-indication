#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define DIGIT PORTD
#define CATEGORY PORTB 

const uint8_t digits[10] = {
  0x3F, 0x6, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x7, 0x7F, 0x6F
};

uint8_t separateDigits[4] = {0, 0, 0, 0};

uint8_t category = 0;
int16_t clock = 0;

uint16_t custom_pow(uint16_t num, int8_t deg) {
  uint16_t acc = 1;

  for (uint8_t i = 1; i <= deg; i++) {
    acc *= num;
  }

  return acc;
}

void displayOnIndicator(uint16_t number) {
  int8_t j, k;

  for (j = 3, k = 0; j >= 0; j--, k++) {
    const uint16_t divider = custom_pow(10, j);

    separateDigits[k] = number / divider;
    number %= divider;
  }
}

ISR(TIMER0_OVF_vect) {
  if (category > 3) category = 0;

  CATEGORY &= 0xF0;
  CATEGORY |= (1<<category);
  DIGIT = digits[separateDigits[category]];

  category++;
}

ISR(TIMER1_COMPA_vect) {
  clock--;

  if (clock < 0) clock = 0;
  if (clock == 0) {
    PORTC |= (1<<3);
  } else {
    PORTC &= ~(1<<3);
  }
}

void startTimer(void) {
  // F/64 = 15625Hz
  TCCR1B |= (1<<CS10) | (1<<CS11);
  TCCR1B &= ~(1<<CS12);

  TIMSK |= (1<<OCIE1A); // enable interrupt for TC1 by compare
  // write 15625 tacts to the compare register
  OCR1AH = 0x3D;
  OCR1AL = 0x09;

  TCNT1 = 0;
  TCCR1B |= (1<<WGM12); // set up CTC1 (reset by match)
}

void setup() {
  DDRD = 0xFF;
  DIGIT = 0x00;

  DDRB |= 0x0F;
  CATEGORY &= ~(0x0F);

  DDRC &= ~((1<<0) | (1<<1) | (1<<2));
  DDRC |= (1<<3);
  PORTC |= (1<<0) | (1<<1) | (1<<2);
  PORTC &= ~(1<<3);

  // F/8
  TCCR0 &= ~((1<<CS00) | (1<<CS02));
  TCCR0 |= (1<<CS01);
  // enable an interrupt by the TC0 overflow
  TIMSK |= (1<<0);
  TCNT0 = 0;
  SREG |= (1<<7);
}

void loop() {
  displayOnIndicator(clock);

  if (~PINC & (1<<1)) {
    if (clock != 9999) {
      clock++;
    }

    _delay_ms(200);
  }

  if (~PINC & (1<<0)) {
    if (clock != 0) {
      clock--;
    }

    _delay_ms(200);
  }

  if (~PINC & (1<<2)) {
    if (clock > 0) {
      startTimer();
    }

    _delay_ms(200);
  }
}

int main(void) {
  setup();

  while (1) {
    loop();
  }
}