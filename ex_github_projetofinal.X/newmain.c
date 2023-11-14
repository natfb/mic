#include <xc.h>          //***inclus?o da biblioteca do compilador
#include <pic16f877a.h>  //***inclus?o da biblioteca do chip espec?fico
#include <stdio.h>       //***inclus?o da biblioteca standard padr?o "C"

#define _XTAL_FREQ 4000000 //***Define a frequencia do clock, 4Mhz neste caso


void main(void) 
{
   TRISB = 0x00;    //***configurando a porta B como sa?da
   TRISA = 0xff;    //***configurando a porta a como entrada
   PORTB = 0;             //*** inicializando port B com 0V em todos pinos
    
   
   while (1)
   {
       //PORTBbits.RB0 = 1; 
       if (PORTAbits.RA0 == 0) {
           PORTBbits.RB1 = 1;   //liga motor 
           PORTBbits.RB0 = 1; 
           //PORTBbits.RB0 = 1; 
       } else if (PORTAbits.RA0 == 1) {
           PORTBbits.RB1 = 0;    //desliga motor
           PORTBbits.RB0 = 0; 
       }
}
}