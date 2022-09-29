#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


unsigned char count =0;
unsigned char clock_ticks=0;
unsigned char seconds=0;
unsigned char minutes=0;
unsigned char hours=0;



ISR(TIMER1_COMPA_vect)
{
	count=1;
}

void Timer1_Init_CTC_Mode(unsigned short tick)
{
	TCNT1 = 0; // Set Timer Initial Value 0
	OCR1A =tick ; // Set Compare Value

	TIMSK |= (1<<OCIE1A); // Enable Timer0 Compare Interrupt
	TCCR1A=(1<<FOC1B)|(1<<FOC1A);
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
}


ISR(INT2_vect)
{
	//opposite to isr 1 to resume counting
	TCCR1B |= (1<<CS10) | (1<<CS12);
}

/* External INT1 enable and configuration function */
void INT2_Init(void)
{
	DDRB  &= (~(1<<PB2));  // Configure INT1/PD3 as input pin
	PORTB |= (1<<PB2);     // Enable the internal pull up resistor at PD3 pin
	// Trigger INT1 with the rising edge
	MCUCSR &=!(1<<ISC2);
	GICR  |= (1<<INT2);    // Enable external interrupt pin INT1

}


//10 configure external interupt
ISR(INT1_vect)
{
	//clear stopwatch selector
	TCCR1B &= 0xF8;
}

/* External INT1 enable and configuration function */
void INT1_Init(void)
{
	
	DDRD  &= (~(1<<PD3));  // Configure INT1/PD3 as input pin
	// Trigger INT1 with the rising edge
	MCUCR |= (1<<ISC11);
	MCUCR |= (1<<ISC10);
	GICR  |= (1<<INT1);    // Enable external interrupt pin INT1


}

ISR(INT0_vect)
{
	 seconds=0;
	 minutes=0;
	 hours=0;
}


//9 configure the int0
void INT0_Init(void)
{
	
	DDRD  &= (~(1<<PD2));  // Configure INT1/PD3 as input pin for the push button
	PORTD |= (1<<PD2);     // Enable the internal pull up resistor at PD3 pin
	
	// Trigger INT1 with the falling edge
	MCUCR |= (1<<ISC01);
	MCUCR &= ~(1<<ISC00);
	GICR  |= (1<<INT0);    // Enable external interrupt pin INT1

	

}

int main(void)
{
	//6 connect the 7447 decoder 4 pins to the first 4 in portc
	DDRC  |= 0x0F;
	PORTC &= 0xF0;
	//7 use first 6 pins in port A to enable and disbale pins for the 7 segment
	DDRA  |= 0x3F;
	PORTA &= 0x3F;
	// Enable global interrupts in MC.
	SREG  |= (1<<7);   
	Timer1_Init_CTC_Mode(1000); // Start the timer.
	//call the interuppts
	INT0_Init();
	INT1_Init();
	INT2_Init();
    while(1)
    {			
       if(count==1)
	   {	

		   seconds++;
		   if(seconds==60)
		   {
			   minutes++;
			   //add the minute counter and reset seconds to zero
			   seconds=0;
		   }
		   if(minutes==60)
		   {
			   hours++;
			   //add the minute counter and reset seconds to zero
			   minutes=0;
		   }
		   if(hours==24)
		   {
			   //the end of the stop watch reset all counter and reset seconds to zero
			   hours=0;
		   }
		   count=0;
	   }
	   else
    	{
    		// out the number of seconds
    		PORTA = (PORTA & 0xC0) | 0x01;
    		PORTC = (PORTC & 0xF0) | (seconds%10);

    		// make small delay to see the changes in the 7-segment
    		// 2Miliseconds delay will not effect the seconds count
    		_delay_ms(2);

    		PORTA = (PORTA & 0xC0) | 0x02;
    		PORTC = (PORTC & 0xF0) | (seconds/10);

    		_delay_ms(2);

    		// out the number of minutes
    		PORTA = (PORTA & 0xC0) | 0x04;
    		PORTC = (PORTC & 0xF0) | (minutes%10);

    		_delay_ms(2);

    		PORTA = (PORTA & 0xC0) | 0x08;
    	    PORTC = (PORTC & 0xF0) | (minutes/10);

    	    _delay_ms(2);

    	    // out the number of hours
    	    PORTA = (PORTA & 0xC0) | 0x10;
    	    PORTC = (PORTC & 0xF0) | (hours%10);

    	    _delay_ms(2);

    	    PORTA = (PORTA & 0xC0) | 0x20;
    	    PORTC = (PORTC & 0xF0) | (hours/10);

    	    _delay_ms(2);
    	}
    }
}
