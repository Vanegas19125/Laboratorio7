/*
 * File:   main.c
 * Author: Jose Vanegas
 *
 * Created on 13 de abril de 2021, 05:47 PM
 */
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

//Funcion que nos devuelve un binario para el 7seg
static char tableDisplay[] = { 0b00111111,//0
                                 0b00000110,//1
                                 0b01011011,//2
                                 0b01001111,//3
                                 0b01100110,//4
                                 0b01101101,//5
                                 0b01111101,//6
                                 0b00000111,//7
                                 0b01111111,//8
                                 0b01101111};//9


volatile char contador = 0; //Variable para el contador
volatile char Decimal[3]; //Variable para almacenar el decimal

//Función que convierte el numero binario en decimal
void ConvertDec(int valor){
    
   
    Decimal[2] = valor/100;//Centenas
    valor -= Decimal[2]*100;
    Decimal[1] = valor/10;//Decenas
    Decimal[0] = valor%10;//Unidades
    
    
}

void __interrupt() ISR(){
    
        
    if(INTCONbits.T0IF){
        TMR0 = 217;
        
        
        //Hacemos el multiplexado con un switch
        switch(contador){
            case 0:
                //apagar todos los display
                PORTD = 0 ;
                PORTA = tableDisplay[ Decimal[0] ];
                // encender display de unidades
                PORTDbits.RD2 = 1;
                contador = 1; 
                break;
            case 1:
                //Apagar todos los displays
                PORTD= 0;
                PORTA = tableDisplay[Decimal[1]];
                //Encender display decenas
                PORTDbits.RD1 = 1;
                contador = 2;
                break;
            case 2:
                //Apagar todos los displays
                PORTD= 0;
                PORTA = tableDisplay[Decimal[2]];
                //Encender display unidades
                PORTDbits.RD0 = 1;
                contador = 0;
                break;
                
        }
        INTCONbits.T0IF = 0;
                
    }
    
    if(INTCONbits.RBIF){
        
        //Verificamos cual boton se presiono 
        if(PORTBbits.RB0 == 0){
            PORTC ++; //Incrementar puerto C
        }
        
        if(PORTBbits.RB1 == 0){
            PORTC --; //Decrementar el puerto C         
        }
        ConvertDec(PORTC);
        INTCONbits.RBIF = 0; //Apagamos bandera tmr0       
    }    

}
void main(void) {
    //Puertos A,C,D como salidas.. Pin 0 y 1 del puerto B como entradas
    TRISB = 3;
    TRISC = 0;
    TRISD = 0;
    TRISA = 0;
    
    //Desactivar entradas anologicas
    ANSEL = 0;
    ANSELH = 0;
    //Puertos CDA en 0
    PORTC = 0;
    PORTD = 0;
    PORTA = 0;
    
    
    OPTION_REGbits.nRBPU = 0; //Pull ups portb
    OPTION_REGbits.PS = 6; //Prescaler 1:128
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;//Prescales is assigned to the Timer0 module
    
    TMR0 = 217;
    
    IOCB = 3;
    
    INTCONbits.GIE = 1;
    INTCONbits.T0IE = 1;
    INTCONbits.RBIE = 1;
    while(1){
    }  
    return;
}

