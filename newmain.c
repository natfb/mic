#include <xc.h>
#include <pic16f877a.h>  
#include <stdio.h> 
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
//#include "config.h"
#define _XTAL_FREQ 4000000
//--[ Pin Definitions ]--
#define Rev RB0   // Reverse Direction Button
#define LV0 RB1   // 0% Speed Button
#define LV1 RB2   // 50% Speed Button
#define LV2 RB3   // 75% Speed Button
#define LV3 RB4   // 100% Speed Button

// Configura??o do PIC16F877A
#pragma config FOSC = HS        // Oscilador em modo HS
#pragma config WDTE = OFF       // Desabilita o Watchdog Timer
#pragma config PWRTE = OFF      // Desabilita o Power-up Timer
#pragma config BOREN = OFF      // Desabilita o Brown-out Reset
#pragma config LVP = OFF        // Desabilita a programa??o de baixa voltagem
#pragma config CPD = OFF        // Desabilita a prote??o de c?digo de dados
#pragma config WRT = OFF        // Desabilita a grava??o no c?digo de programa
#pragma config CP = OFF         // Desabilita a prote??o de c?digo de programa

// Defini??es de pinos
#define LDR_PIN     RA0       // Pino anal?gico conectado ao LDR
#define MOTOR_PIN   RB0       // Pino de controle do motor
#define OPEN   RD0 
#define CLOSE   RD1 
// Fun??o de inicializa??o
void PWM1_Set_Duty(uint16_t);

void initialize() {
    // Configura??o do PORTA como entrada para o LDR
    TRISB = 0b11111111;       //configura pinos sa?da (0)
    TRISD = 0b00111111;
    TRISA = 0b11111111;       //configura pinos de entrada
    PORTD = 0;
    OPTION_REGbits.nRBPU = 0; //Ativa resistores de pull-ups
    MOTOR_PIN = 0;  // Desliga o motor inicialmente
}

// Fun??o para leitura do LDR
int readLDR() {
    ADCON0 = 0b00000001;     // Seleciona canal RA0
    ADCON1 = 0b00001110;     // Configura o resultado da convers?o como direita justificada
    
    // Inicia a convers?o ADC
    GO_nDONE = 1;
    
    // Aguarda o t?rmino da convers?o
    while (GO_nDONE);
    
    // Retorna o valor convertido
    return (ADRESH << 8) | ADRESL;
}
void __interrupt() isr(void);

// Define flags for manual control
volatile int manualControl = 0;  // 0 for automatic control, 1 for manual control
volatile int reverseDirection = 0;
// Fun??o principal
void main() {
    initialize();
    
    // Set The Direction To Be Output Pins
    TRISD0 = 0;
    TRISD1 = 0;
    // Initially//--[ (0, 1) Say it's ClockWise rotation!
    // For Reversing The Direction Write (1, 0)
    RD0 = 0;
    RD1 = 0;

    //--[ Configure The CCP Module For PWM Mode ]--
    CCP1M3 = 1;
    CCP1M2 = 1;
    TRISC2 = 0; // The CCP1 Output Pin (PWM)
    // Set The PWM Frequency (2kHz)
    PR2 = 124;
    // Set The PS For Timer2 (1:4 Ratio)
    T2CKPS0 = 1;
    T2CKPS1 = 0;
    // Start CCP1 PWM !
    TMR2ON = 1;
    
    INTCONbits.GIE=1;    //Pode ser feito assim tamb?m
    INTCONbits.INTE=1;   //Pode ser feito assim tamb?m

    // The Main Loop (Routine)


while (1)
{
    int ldrValue = readLDR();

    if (manualControl == 0)
        {
            // Automatic control based on light sensor
            if (ldrValue < 500)
            {
                // Close the curtain
                RD0 = 0;
                RD1 = 0;
                __delay_ms(20);
                PWM1_Set_Duty(0);
                RD0 = 0;
                RD1 = 1;
                PWM1_Set_Duty(100);
            }
            else
            {
                // Open the curtain
                RD0 = 0;
                RD1 = 0;
                __delay_ms(20);
                PWM1_Set_Duty(0);
                RD0 = 1;
                RD1 = 0;
                PWM1_Set_Duty(100);
            }
        }
        else
        {
            // Manual control based on buttons
            if (reverseDirection == 1)
            {
                // Reverse direction
                RD0 = 0;
                RD1 = 0;
                __delay_ms(20);
                PWM1_Set_Duty(0);
                RD0 = 0;
                RD1 = 1;
                PWM1_Set_Duty(100);
            }
            else
            {
                // Stop the curtain
                RD0 = 0;
                RD1 = 0;
                __delay_ms(20);
                PWM1_Set_Duty(0);
            }
        }

        // Add a delay for stability
        __delay_ms(20);
    }
}

void __interrupt() isr(void)
{
    // Check button RB0 interrupt
    if (INTF)
    {
        manualControl = 1 - manualControl;
        reverseDirection = 0; // Reset reverse direction flag
        INTCONbits.INTF = 0; // Clear interrupt flag
    }
}
// Definition For PWM1_Set_Duty Function
void PWM1_Set_Duty(uint16_t DC)
{
  // Check The DC Value To Make Sure it's Within 10-Bit Range
  if(DC<1024)
  {
    CCP1Y = DC & 1;
    CCP1X = DC & 2;
    CCPR1L = DC >> 2;
  }
}