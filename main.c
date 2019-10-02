/*
 * try 109.c
 *
 * Created: 20-03-2019 14:27:41
 * Author : Deep
 */ 
#define F_CPU 14745600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#define RX  (1<<4)
#define TX  (1<<3)
#define TE  (1<<5)
#define RE  (1<<7)

volatile unsigned char data;
int i=0,flag=0,k=0;
char str1[5],str2[5],str3[5],str4[5],str5[5],rec;
unsigned int ADCv[3],tick1=0,tick2=0,ls,ms,rs,l=0,r=0,j=0,p=0,c_off=45;

void buzzer_config()
{
	DDRB=0x01; // setting pin 0 of PORTB as output.
}

void buzzer(unsigned int time)
{
	//for (int s=0;s<5;s++)
	{
		PORTB=0x01;// giving output at pin0 of PORTB.
		_delay_ms(5000);
		PORTB=0x00;  // stopping the output of pin0 of PORTB.
	}
	}

void magnet_pin_config()
{
	DDRH =0x01 ;
	
	PORTH =0x00 ;
}

void magnet_on()
{
	k++;
	PORTH =0x01 & ~PORTH ;//Giving output through pin0 of portH for magnetizing the magnet
}

void magnet_off()
{
	PORTH =0x00 ;//Switching off pin0 of portH for demagnetizing the magnet.
}

void motor_pin_config()
{
	ICR5=0xFFFF;         //Initializing Input Capture Register for Timer5 as 65535
	DDRL=0x18;
	PORTL=0x00;         //pins 3,4 of PORTL as output pins.
	DDRA =0x0F ;       // pins 0,1,2,3 of PORTA as output pins.
	PORTA =0x00 ;
	TCCR5A =1<<COM5A1|1<<COM5B1;//Selecting Waveform Generation Mode as Phase and Frequency PWM with TOP=ICR5
	TCCR5B =1<<WGM53;// Selecting Phase and Frequency control PWM method.
	TCCR5B|=1<<CS50;//Clock Select 0=1, which sets pre-scalar as 1
	
}
void forward()
{ 
	flag=0;
	l=0;
	r=0; 
	OCR5A=0x4000;  //Output Control Register A as pinL3 gives output when TCNT5 matches with OCR5A with Duty cycle=18%
	OCR5B=0x4678; //Output Control Register A as pinL4 gives output when TCNT5 matches with OCR5B with Duty cycle=12%
	PORTA =0x0A ;//Giving output through pin1 and pin3 of portA for forward motion
}
void forward1()
{
	flag=0;
	l=0;
	r=0;
	OCR5A=25055;  //Output Control Register A as pinL3 gives output when TCNT5 matches with OCR5A with Duty cycle=18%
	OCR5B=27055; //Output Control Register A as pinL4 gives output when TCNT5 matches with OCR5B with Duty cycle=12%
	PORTA =0x0A ;//Giving output through pin1 and pin3 of portA for forward motion
}

 void _slowforward()
{
    OCR5A=0x2FFF;  //Output Control Register A as pinL3 gives output when TCNT5 matches with OCR5A with Duty cycle=18%
    OCR5B=0x2FFF; //Output Control Register A as pinL4 gives output when TCNT5 matches with OCR5B with Duty cycle=12%
    PORTA =0x0A ;//Giving output through pin1 and pin3 of portA for forward motion
}

void backward()
{
	OCR5A=0x4000;  //Output Control Register A as pinL3 gives output when TCNT5 matches with OCR5A with Duty cycle=18%
	OCR5B=0x4678; //Output Control Register A as pinL4 gives output when TCNT5 matches with OCR5B with Duty cycle=12%
	PORTA =0x05 ;//Giving output through pin0 and pin2 of portA for backward motion
}

void left()
{
	OCR5A=18000;  //Output Control Register A as pinL3 gives output when TCNT5 matches with OCR5A with Duty cycle=18%
	OCR5B=18000; //Output Control Register A as pinL4 gives output when TCNT5 matches with OCR5B with Duty cycle=12%
	PORTA =0x09 ;//Giving output through pin0 of portA for left motion
}

void right()
{
    OCR5A=16000;  //Output Control Register A as pinL3 gives output when TCNT5 matches with OCR5A with Duty cycle=18%
    OCR5B=18000; //Output Control Register A as pinL4 gives output when TCNT5 matches with OCR5B with Duty cycle=12%
	PORTA =0x06 ;//Giving output through pin2 of portA for right motion
}

void soft_left()
{
	flag=1;
	l++;
	OCR5A=12000;   //Output Control Register A as pinL3 gives output when TCNT5 matches with OCR5A with Duty cycle=11%
	OCR5B=12000;  //Output Control Register B as pinL4 gives output when TCNT5 matches with OCR5B with Duty cycle=11%
	PORTA = 0x08;//Giving output through pin0 of portA for left motion
}
void soft_left_2()
{
	OCR5A=16000;   //Output Control Register A as pinL3 gives output when TCNT5 matches with OCR5A with Duty cycle=11%
	OCR5B=16000;  //Output Control Register B as pinL4 gives output when TCNT5 matches with OCR5B with Duty cycle=11%
	PORTA = 0x08;//Giving output through pin0 of portA for left motion

}
void soft_left_3()
{
	OCR5A=10000;   //Output Control Register A as pinL3 gives output when TCNT5 matches with OCR5A with Duty cycle=11%
	OCR5B=10000;  //Output Control Register B as pinL4 gives output when TCNT5 matches with OCR5B with Duty cycle=11%
	PORTA = 0x08;//Giving output through pin0 of portA for left motion

}
void soft_right()
{
	flag=2;
    r++;
	OCR5B=12000;
	OCR5A=12000;  //Output Control Register A as pinL3 gives output when TCNT5 matches with OCR5A with Duty cycle=11%
	PORTA = 0x02;//Giving output through pin2 of portA for right motion
}
void soft_right_3()
{
	flag=2;
	r++;
	OCR5B=10000;
	OCR5A=12000;  //Output Control Register A as pinL3 gives output when TCNT5 matches with OCR5A with Duty cycle=11%
	PORTA = 0x02;//Giving output through pin2 of portA for right motion
}

void soft_right_2()
{
	OCR5B=16000;
	OCR5A=16000;  //Output Control Register A as pinL3 gives output when TCNT5 matches with OCR5A with Duty cycle=11%
	PORTA = 0x02
	;//Giving output through pin2 of portA for right motion

}
void stop()
{
	PORTA &=~0x03;//Switching OFF pin 0 and 1 of PORTA.
	//_delay_ms(30) ;
	PORTA&=~0x0C;//Switching OFF pins 2 and 3 portA.
}

void uart0_init()
{
	UCSR0B = 0x00;							//disable while setting baud rate
	UCSR0A = 1<<RXC0 | 1<< TXC0;
	UCSR0C = 0x06;
	UBRR0L = 0x5F; 							//9600BPS at 14745600Hz
	UBRR0H = 0x00;
	UCSR0B = 0x98;
	UCSR0C = 3<<1;							//setting 8-bit character and 1 stop bit
	UCSR0B = RX | TX;
}


ISR(USART0_RX_vect)
{
	data = UDR0;
}

void uart_tx(char data)
{
	while(!(UCSR0A & TE));						//waiting to transmit
	UDR0 = data;
}

char uart_rx()
{
	while(!(UCSR0A & RE));//waiting to receive	
	return UDR0;
}
void startconv()
{
	ADCSRA |= (1 << ADSC);    // Start the ADC conversion
}

void start_encoder()
{
	EIMSK = 1<<INT4  | 1<<INT5;
	DDRE=0x0F;
}

void stop_encoder()
{
	DDRE = 0xF0;
}

ISR (INT5_vect)
{
	tick2++;
}

ISR (INT4_vect)
{
	tick1++;
}

void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x85;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}

//Function For ADC Conversion
unsigned char ADC_Conversion(unsigned char Ch)
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;
	ADMUX= 0x20| Ch;
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}

void movement()//uint8_t rs,uint8_t ms ,uint8_t ls)
{
	ls = ADC_Conversion(3);
	ms = ADC_Conversion(2);
	rs = ADC_Conversion(1);
 
      if (ls<=40 && ms<=40 && rs<=40) // Check if all three sensors are on white area.
	{
		   //uart_tx('1');
		    backward();
			_delay_ms(80);
			stop();// stop the bot
			//_delay_ms(200);
			if(k==6)
			{
				buzzer(5000);
			}
            //buzzer(100);
			uart_tx('r');  // transmit 'r' to tell the pc that bot has reached the node.
			rec= uart_rx();
			//start_encoder();
			sei();
			
			tick1 =0;
			tick2 =0;
			
			if (rec=='b')
			{
				buzzer(5000);
			}
			if (rec == 'a')
			{
				while(1)
				{
					ls=ADC_Conversion(3);
					rs=ADC_Conversion(1);
					ms=ADC_Conversion(2);
					if (ls<60 && ms<60 &&rs<60)
					{
						backward();
						stop();
						_delay_ms(100);
						break;
						
					}
					_slowforward();
				}
				
				while (1)
				{
					ls=ADC_Conversion(3);
					rs=ADC_Conversion(1);
					ms=ADC_Conversion(2);
					if (ls<150 && ms>40 &&rs<c_off)
					{
						stop();
						break;
					}
					right();
				}
				movement();
			}
			
			if (rec=='d')
			{
				while(1)
				{
					ls=ADC_Conversion(3);
					rs=ADC_Conversion(1);
					ms=ADC_Conversion(2);
					if (ls<60 && ms<60 &&rs<60)
					{
						backward();
						stop();
						//_delay_ms(100);
						break;
					}
					_slowforward();
				}
				
				while (1)
				{
					ls=ADC_Conversion(3);
					rs=ADC_Conversion(1);
					ms=ADC_Conversion(2);
					if (ls<c_off && ms>40 && rs<150)
					{
						stop();
						break;
					}
					left();
				}
				movement();
			}
			 else if (rec=='x')
			 {
				 start_encoder();
				 //stop_encoder();
				 while ((tick1<425) || (tick2<425))
				 {
					 right();
				 }
				 stop();
				 _delay_ms(200);
				 forward();
				 _delay_ms(430);
				 stop();
				 _delay_ms(100);
				 magnet_on();
				 _delay_ms(1500);
				 stop_encoder();
				 backward();
				 _delay_ms(450);
				 stop();
				 _delay_ms(100);
				 movement();
				 
			 }
			 
			else if (rec=='c')
			{
				start_encoder();
				while ((tick1<425) || (tick2<425))
				{
					left();
				}
				
		   stop();
		   _delay_ms(200);
		   forward();
		   _delay_ms(420);
		   stop();
		   _delay_ms(100);
		   magnet_on();
		   _delay_ms(1000);
		   stop_encoder();
		   
		   backward();
		   _delay_ms(450);
		   stop();
		   _delay_ms(100);
			}
 			else if(rec=='p')
		
		{
			
			//start_encoder();
			forward();
			_delay_ms(430);
			stop();
			_delay_ms(100);
			magnet_on();
			_delay_ms(1500);
			backward();
			_delay_ms(455);
			stop();
			_delay_ms(150);
			}
		
			else if (rec=='y')
			{
				start_encoder();
				while ((tick1<685) || (tick2<685))
				{
					left();
				}
				forward();
				_delay_ms(150);
				stop();
				}
			
			cli();
			stop_encoder();
			sei();
			movement();
		}	
	else if (rs > c_off && ms <= c_off && ls <=c_off ) // check if bot is deflected to right of the black line 
	{
		//uart_tx('2');
		soft_left_2(); // make the bot move slight left for re alignment.
		//movement();
	}
	else if(ls > c_off && ms <=c_off && rs<=c_off) // check if bot is deflected to the left of black line 
	{
		//uart_tx('3');
		soft_right_2(); // make the bot move slight right for re alignment
	   //movement();
	}
	else if (ls >c_off && ms> c_off && rs<=c_off) // check if bot is deflected to left side of black line.
	{
		//uart_tx('4');
		soft_right();// make the bot move slight right for re alignment
	   //movement();
	}
	else if (rs>c_off && ms >c_off && ls<=c_off)// check if the bot is deflected to the right side of black line.
	{
		//uart_tx('5');
		soft_left();// move the bot slightly left.
		//movement();
		}
	else if (ls <= c_off && rs <= c_off && ms >c_off)// verify that the bot is moving on the middle of the black line. 
	{
		//uart_tx('6');
		forward1();// make the bot move in the forward direction
	}
	else //(ls>c_off && ms>c_off && rs>c_off)// check if the bot has reached the node
	{
		//uart_tx('7');
		backward();
		_delay_ms(80);
		stop();// stop the bot
		//if(k==6)
		{
			//buzzer(5000);
		}
		
		uart_tx('r');  // transmit 'r' to tell the pc that bot has reached the node.
		rec= uart_rx();
		_delay_ms(100);
		start_encoder();
		sei();
		
		tick1 =0;
		tick2 =0;
		if (rec=='b')
		{
			buzzer(5000);
		}
		if (rec == 'a')
		{ 
			while(1)
			{
			ls=ADC_Conversion(3);
			rs=ADC_Conversion(1);
			ms=ADC_Conversion(2);
			if (ls<60 &&ms<60 &&rs<60)
			{
				backward();
				stop();
				_delay_ms(100);
				break;
				
			}
			_slowforward();	
			}
			while (1)
		{
			ls=ADC_Conversion(3);
			rs=ADC_Conversion(1);
			ms=ADC_Conversion(2);
			if (ls<150 && ms>35 &&rs<c_off)
			{
				stop();
				break;
			}
			 right();
		}
			movement();
	    }
			
		 if (rec=='d')
		{
			while(1)
			{
				ls=ADC_Conversion(3);
				rs=ADC_Conversion(1);
				ms=ADC_Conversion(2);
				if (ls<60 && ms<60 &&rs<60)
				{
					backward();
					stop();
					_delay_ms(100);
					break;
				}
				_slowforward();
			}
			while (1)
			{
				ls=ADC_Conversion(3);
				rs=ADC_Conversion(1);
				ms=ADC_Conversion(2);
				if (ls<c_off && ms>35 && rs<150)
				{
					stop();
					break;
				}
				left();
		}
		movement();
		}
		else if (rec=='y')
		{
			start_encoder();
			while ((tick1<650 || tick2<650))
			{
				left();
			}
			forward();
			_delay_ms(150);
			stop();
			
			stop();
			movement();	
       }
		else if (rec=='x')
		{
			
			start_encoder();
			while ((tick1<415))
			{
				right();
			}
			stop();
			_delay_ms(200);
			stop_encoder();
			
		forward();
		_delay_ms(500);
		stop();
		_delay_ms(100);
		magnet_on();
		_delay_ms(1000);
		backward();
		_delay_ms(538);
		stop();
		_delay_ms(100);
		
		}
		else if (rec=='c')
		{
			start_encoder();
			while (tick1<420)
			{
				left();
			}
			stop();
			_delay_ms(200);
			stop_encoder();
			forward();
		_delay_ms(550);
		stop();
		_delay_ms(100);
		magnet_on();
		_delay_ms(1000);
		backward();
		_delay_ms(588);
		stop();
		_delay_ms(100);
		
		}
		else if(rec=='p')
		{
			{
			forward();
			_delay_ms(570);
			stop();
			_delay_ms(100);
			magnet_on();
			_delay_ms(1000);
			backward();
	       _delay_ms(600);
			stop();
			_delay_ms(150);
			
			}
		}
		
		
		
		cli();
		stop_encoder();
		sei();
	}
	movement();
	}


void encoder_config()
{
	EICRB = 1<<ISC51 | 1<<ISC41;
	EIFR  = 1<<INTF5 | 1<<INTF4;
	PORTE = 0x0F;
}

int main(void)
{
	buzzer_config();       // configure buzzer pins .
	magnet_pin_config();  // configure magnet pins.
	uart0_init();        //configure transmitting/receiving.
	motor_pin_config(); //configure motor pins.
	adc_init();
	encoder_config(); 
	sei();           // enable global interrupts
	
	while (1)
	{
	j=uart_rx();  // receive character for next movement.
	if (j=='w')
	//uart_tx(j);
	forward();
	_delay_ms(500);
	while(1)
	{
		ls=ADC_Conversion(3);
		rs=ADC_Conversion(1);
		ms=ADC_Conversion(2);
		if (ls<40 && rs<40 && ms>40)
		{
			stop();
			break;
		}
		forward();
	}
	movement();
	}
}



