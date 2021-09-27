#include <msp430.h>
#include <stdio.h>
#include <file.h>

/**
 * main.c
 */

int isr_add;
char data = 0;
FILE *f;

unsigned char *PRxData;
unsigned char RxBytectr;
volatile unsigned char RXdata[128];


void init_uart(void)
{
    UCA0CTL1 |= UCSSEL_2;           //SMCLK
    UCA0BR0 = 104;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;
    UCA0CTL1 &= ~UCSWRST;

}

void init_i2c(void)
{
    UCB0CTL1 |= UCSWRST;                    //Software reset
    UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC; //Set sync communication, set as master, and put in i2c mode
    UCB0CTL1 = UCSSEL_2 + UCSWRST;
    UCB0CTL0 &= ~UCSLA10;
    UCB0BR0 = 12;
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;                    //Software reset disabled


}

void txrx(int add)
{
    isr_add = add;          //send slave address to isr
    UCB0I2CSA = add;        //set slave address
    //
    UCB0CTL1 |= UCTR;
    UCB0CTL1 &= ~UCTXSTP;
    UCB0CTL1 |= UCTXSTT;


   //__bis_SR_register(GIE);
    //__no_operation();

    //UCB0CTL1 |= UCTR;       //set to transmit mode
    //UCB0CTL1 &= ~UCTXSTP;   //clear transmit stop signal
    //UCB0CTL1 |= UCTXSTT;    //set transmit start signal

    //printf(data);        //add data to file
    //fopen("test.txt", "w+");
    //fprintf(f, "%d", data);
    //fclose(f);


}

void clk(void)
{

    BCSCTL1 = CALBC1_1MHZ;
    BCSCTL1 = DIVA_3;
    DCOCTL = CALDCO_1MHZ;
}

void port_setup(void)
{
    //P1DIR |= BIT5;
    P1SEL |= BIT1 | BIT2 |BIT6 | BIT7; //P1.1 = RxD P1.2 = TxD P1.6 = SCL P1.7 = SDA
    P1SEL2 |=  BIT1| BIT2| BIT6 | BIT7;
    //P1OUT &= ~BIT5;
    //P1OUT |= BIT5;

}

/*
 * Addresses for peripherals
 * Soil Moisture Sensor                 0x36(default), 0x36-0x39
 * Temp,Humidity and Light Sensor       temp:0x48 humidity:0x40 light:0x44
 * NPK Sensor:
 * thl sensor                           scl:1.9 sda:1.10
 */
int main(void)
{
    static const int sms = 0x36;
    static const int temp = 0x48;
    static const int hum = 0x40;
    static const int light = 0x44;
    WDTCTL = WDTPW + WDTHOLD;
    clk();
    port_setup();
    init_i2c();
    //init_uart();
    IE2 |= UCA0RXIE;
    IE2 |= UCB0TXIE;

    __enable_interrupt();
    //LPM0;

    while (1)
    //
    {
        //txrx(temp);
        txrx(hum);
        //txrx(light);
    }

    //return 0;
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
{
    /*switch(IFG2){
    case UCB0TXIFG:
        UCB0TXBUF = 0x00;
        break;

    case UCB0RXIFG:
        data = UCB0RXBUF;
        break;

    default:
        break;
    }*/
    //transmit flag raised, out what memory address to read from (slave address?)
     //send address to slave

        UCB0TXBUF = 0x03;
        while((IFG2 & UCB0TXIFG));
        UCB0CTL1 &= ~UCTR;

        data = UCB0RXBUF;
        while((IFG2 & UCB0RXIFG));

        UCB0CTL1 &= ~UCTR;
        UCB0CTL1 &= ~UCTXSTT;
        UCB0CTL1 |= UCTXSTP;



     //read data from receive buffer
     //set transmit start flag
     //clear transmit start flag

}
