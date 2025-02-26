; TODO INSERT CONFIG CODE HERE USING CONFIG BITS GENERATOR
#include <p18f4550.inc>
VARIAVEIS UDATA_ACS 0
 ;Setar uma vari�vel VAR de 8 bits
VAR RES 1
 ;Setar uma vari�vel cont pra ser o contador
CONT RES 1
 ;Setar uma vari�vel para ser o contador do la�o
CONTAUX RES 1
RES_VECT  CODE    0x0000            ; processor reset vector
    GOTO    START                   ; go to beginning of program

; TODO ADD INTERRUPTS HERE IF USED

MAIN_PROG CODE                      ; let linker place main program

START
    ;iniciar o contador em 8
    MOVLW d'8'
    MOVWF CONTAUX
    ;definir o valor de A
    MOVLW b'11101011'
    MOVWF VAR
    ;inicializar o contador em 0
    CLRF CONT
;introduzir um la�o de repeti��o (8 vezes)
LOOP
    ;em cada repeti��o, rotacionar os bits de VAR pra direita
    RRNCF VAR
    ;verificar se o bit 0 de A = 1
    BTFSC VAR, 0
    ; se sim, incrementa cont
    INCF CONT
    ; se n�o,verifica se contaux = 0 depois de decrement�-lo
    DECFSZ CONTAUX
    ;Repete o Loop se contaux != 0
    GOTO LOOP
    END