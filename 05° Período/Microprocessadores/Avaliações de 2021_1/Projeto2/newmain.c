/*
 * File:   newmain.c
 * Author: marcelo
 *
 * Created on 10 de Agosto de 2021, 11:43
 */

#include <xc.h>

#pragma config WDT = OFF 
#pragma config MCLRE = OFF
#pragma config PBADEN = OFF

#define _XTAL_FREQ 4000000

// digitos[]: Guarda valores BCD a serem apresentados
// nos 4 displays
unsigned char digitos[4]; 

// bcd2seg[] tabela de decodifica��o BCD para sete segmentos
unsigned char bcd2seg[] = { 0xC0,  // 0
                            0xF9,  // 1
                            0xA4,  // 2
                            0xB0,  // 3
                            0x99,  // 4
                            0x92,  // 5
                            0x82,  // 6
                            0xF8,  // 7
                            0x80,  // 8
                            0x90 };// 9

// dec2digit : Rotina que separa as unidades, dezenas, centenas 
// e milhares de um inteiro positivo n realizando divis�es 
// sucessivas por 10.
// Os valores s�o colocados nas posi��es do vetor digitos[]
// para serem apresentados nos displays.

void dec2digit(unsigned int n) {
      unsigned char d; //vari�vel auxiliar q recebe os algarismos de n.
      unsigned char *ptr = &digitos[3]; //ponteiro q aponta pra quarta posi��o do vetor d�gitos[].
      
      // Se a tens�o a ser mostrada for 0 os valores de digitos[] s�o todos nulos.
      if (n == 0) {
          digitos[3] = 0;
          digitos[2] = 0;
          digitos[1] = 0;
          digitos[0] = 0;
          return;
      }
      
      for (unsigned char i =0;i<4;i++) {
       d = n % 10; //O m�dulo de n dividido por 10 faz com q d receba o valor atual do algarismo das unidades de n.
       n = n / 10; //O piso de n/10 faz com q n passe a ter um algarismo a menos, o algarismo das unids atual some.
       *ptr = d; //Conte�do apontado por ptr em digitos[] passa a ser d
       ptr--; //ptr passa a apontar para uma posi��o de digitos[] cujo �ndice � uma unid a menos q o anterior.
      }
}

// Programa principal
void main(void) {
    ADCON1 = 0x0E; // Configura RA0 como sendo AN0
    ADCON2 = 0x80; // Resultado da convers�o justificado para direita
    ADCON0bits.ADON = 1; // liga conversor A/D 
    
    // inicializa��es do Timer 2
    T2CONbits.T2CKPS = 0b11;   // PRESCALER 1:16 (16 us)
    T2CONbits.TOUTPS = 0b0000; // POSTSCALER 1:1 
    PR2 = 125;      // Valor do estouro do timer (2 ms)
    TMR2IE = 1; // Ativa interrup��o do timer 2
    T2CONbits.TMR2ON = 1; // Ativa contagem
    // Inicializa��es das portas utilizadas no acionamento dos displays
    PORTC = 0xFF;  
    TRISC = 0x00;  // PORTC como sa�da
    PORTD = 0x00;  
    TRISD = 0x00;  // PORTD como saida
    GIE = 1;     // Habilita todas as interrup��es
    PEIE = 1;   // Habilita interrup��o dos perif�ricos
   
    int resultado_AD = 0; // n�mero a ser apresentado.
                          // tem de ser menor que 9999      
    while (1) {
     ADCON0bits.GO = 1; // Inicia convers�o
     while (ADCON0bits.GO == 1); // Espera terminar convers�o
     resultado_AD = ADRES; //ADRES guarda o valor da convers�o e esse valor � atribu�do a resultado_AD.
     
     float result = resultado_AD * (5000/1023); //result vem da regra de 3 onde result est� para resultado_AD,assim como 5000 mV est� para 1023 
     resultado_AD = (int) result; //para trabalhar com inteiros, transformamos result e colocamos seu valor inteiro em resultado_AD
     dec2digit(resultado_AD);
     __delay_ms(50); 
     };
}  

// Rotina de interrup��o do timer 2 
// faz a temporiza��o para o acionamento 
// e apresenta��o dos valores do vetor digitos[] 
// nos displays. O acionamento dos displays se d� 
// por multiplexa��o acionando um display por vez, 
// sequencialmente, a cada 2 ms.

void __interrupt(low_priority) rotina_interrup(void){
  static unsigned char aux = 0; // aux indica o display a acionar
  unsigned char dig; // valor a apresentar no display
  static unsigned char conta = 0;

  if (TMR2IF){  // Verifica se � timer 2
   TMR2IF = 0;  // limpa flag.
     
 
   PORTC = 0xFF; // desliga todos displays.
   dig = digitos[aux]; // coloca o valor a apresentar em dig 
   PORTD = ~bcd2seg[dig]; // converte para segmentos e coloca em PORTD
   
   // Dependendo do valor de aux aciona o display correspondente 
   switch (aux) {
    case 0: // display 0
        PORTCbits.RC0 = 0;
        aux = 1; // prepara para acionar display 1 na pr�xima interrup��o
        break;
    case 1: // display 1
        PORTCbits.RC1 = 0;
        aux = 2; // prepara para acionar display 2 na pr�xima interrup��o
        break;
    case 2: // display 2
        PORTCbits.RC2 = 0;
        aux = 3; // prepara para acionar display 3 na pr�xima interrup��o       
        break;
    case 3: // display 3
        PORTCbits.RC6 = 0;
        aux = 0; // prepara para acionar display 0 na pr�xima interrup��o
        break;
      }
   }    
}
