#include <msp430.h>
#include <stdio.h>
#include <file.h>

/**
 * main.c
 */

int isr_add;
char data = 0;
FILE *f;
IE2 |= UCA0RXIE;
IE2 |= UCB0TXIE;

void init_uart(void)
{
    UCA0CTL1 |= UCSWRST | UCSSEL_2;
    UCA0BR0 = 104;
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_1;
    UCA0CTL1 &= ~UCSWRST;

}

void init_i2c(void)
{
    UCB0CTL1 |= UCSWRST;                    //Software reset
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC; //Set sync communication, set as master, and put in i2c mode
    UCB0CTL1 = UCSSEL_2 | UCSWRST;
    UCB0BR0 = 10;
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;                    //Software reset disabled
    //transmit interrupt enabled

}

void txrx(int add)
{
    isr_add = add;          //send slave address to isr
    UCB0I2CSA = add;        //set slave address
    UCB0CTL1 |= UCTR;       //set to transmit mode
    UCB0CTL1 &= ~UCTXSTP;   //clear transmit stop signal
    UCB0CTL1 |= UCTXSTT;    //set transmit start signal

    printf("hello");        //add data to file
    fopen("test.txt", "w+");
    fprintf(f, "%d", data);
    fclose(f);
}

void clk(void)
{

    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
}

void port_setup(void)
{
    //P1DIR |= BIT5;
    P1SEL |= BIT1 | BIT2 | BIT6 | BIT7; //P1.1 = RxD P1.2 = TxD P1.6 = SCL P1.7 = SDA
    P1SEL2 |= BIT1 | BIT2 | BIT6 | BIT7;
    //P1OUT &= ~BIT5;
    //P1OUT |= BIT5;

}

/*
 * Addresses for peripherals
 * Soil Moisture Sensor                 0x36(default), 0x36-0x39
 * Temp,Humidity and Light Sensor       temp: humidity: light:
 * NPK Sensor
 */
int main(void)
{
    static const int sms = 0x36;
    WDTCTL = WDTPW + WDTHOLD;
    clk();
    port_setup();
    init_i2c();
    init_uart();
    __enable_interrupt();

    while (1)
    {
        txrx(sms);

    }

    //return 0;
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void transmit(void)
{

    //transmit flag raised, out what memory address to read from (slave address?)
    UCB0TXBUF = isr_add; //send address to slave

    data = UCB0RXBUF;     //read data from receive buffer

    UCB0CTL1 |= UCTXSTP;  //set transmit start flag
    UCB0CTL1 &= ~UCTXSTT; //clear transmit start flag

}
