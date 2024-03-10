;--------------------------------------------------------------------------
;  refresh.c   - refresh test for KC85/4 and modul M036
;
;  Copyright (c) 2024 boert, CC BY-SA 2.0 de
;--------------------------------------------------------------------------

#include <stdlib.h> // __itoa
#include <stdint.h> // uint8_t
#include <conio.h>  // gotoxy, cputs
#include <string.h> // memset
#include <caos.h>

#define ARGN        *(volatile  uint8_t*)0xB781
#define ARG1        *(volatile uint16_t*)0xB782
#define ARG2        *(volatile uint16_t*)0xB784
#define ARG3        *(volatile uint16_t*)0xB786


#define CTC_CONTROL_WORD            ( 1 << 0)
#define CTC_SOFTWARE_RESET          ( 1 << 1)
#define CTC_TIME_CONSTANT_FOLLOW    ( 1 << 2)
#define CTC_AUTOMATIC_TRIGGER       ( 0 << 3)
#define CTC_CLK_STARTS_TIMER        ( 1 << 3)
#define CTC_FALLING_EDGE            ( 0 << 4)
#define CTC_RISING_EDGE             ( 1 << 4)
#define CTC_PRESCALER_16            ( 0 << 5)
#define CTC_PRESCALER_256           ( 1 << 5)
#define CTC_TIMER_MODE              ( 0 << 6)
#define CTC_COUNTER_MODE            ( 1 << 6)
#define CTC_INTERRUPT_ENABLE        ( 1 << 7)


#define TIMELINE    7
#define XPOS( x)    ( x * 8)
#define YPOS( y)    ( 255 - ( y * 8))

// Prototpyes
uint16_t    sum16( uint8_t* address, uint16_t length);
uint16_t    sum16_fast( uint8_t* address, uint16_t length);
void        setup_ctc_for_measure( void);
uint16_t    wait_inc( uint16_t value);
void        wait_20ms( uint16_t ticks);
uint8_t     wait_sec( uint16_t seconds);
void        print_int(int i);


// Hauptprogramm
void main( void)  
{

    uint8_t slot        = 0x0C;
    uint8_t control     = 0x1F;

    uint16_t sum, refsum;
    uint16_t wait_time; 

    uint8_t  index;
    uint8_t  value;

    uint8_t time_line   = TIMELINE;

    // -------------------------------
    // ggf. Argumente auswerten:
    // REFRESH <Slot> <Steuerbyte>
    if( ARGN > 0)
    {
        slot = ARG1;
    }
    if( ARGN > 1)
    {
        control = ARG2;
    }
    
    // -------------------------------
    // Initialisierungen
    crt (CLS);
    winin( 0, 0, 8, 32, 0);
    cputs( " Refresh    Test\r\n");
    line( XPOS( 0) + 2, YPOS( 2) - 2, XPOS( 7) + 3, YPOS( 2) - 2, YELLOW << 3);
    line( XPOS( 0) + 2, YPOS( 2) - 4, XPOS( 7) + 3, YPOS( 2) - 4, YELLOW << 3);
    // RAM4 ausschalten
    modu( 3, 0x04, 0);

    // Speichermodul einschalten
    cputs( "slot: "); ahex( slot);   
    cputs( "ctrl: "); ahex( control);
    modu( 3, slot, control);
    
    crlf();
    line( XPOS( 0) + 2, YPOS( 5) - 3, XPOS( 7) + 3, YPOS( 5) - 3, YELLOW << 3);

    // Speicher mit Muster füllen
    // einen Algorithmus auswählen
    #if 0
    value = 0xA5;
    for( index = 0; index < 0x20; index++)
    {
        memset( (void*)(( 0x40 + index ) << 8), value, 0x100);
        value -= 1;
    }
    #endif
    #if 0
    memset( (void*) 0x4000, 0x83, 0x2000);
    #endif
    #if 0
    for( index = 0; index < 253; index += 2)
    {
        line( XPOS( 8) + 2, index, XPOS( 38) + 6, index, WHITE << 3);
    }
    // abspeichern
    memcpy( (void*) 0x4000, (void*) 0x8800, 0x2000);
    #endif
    #if 1
    __asm
        ld  hl, #0x4000
        ld  bc, #0x2000
        filloop:
        ld  (hl), #0x55
        inc hl
        ld  (hl), #0xaa
        inc hl
        dec bc
        ld a, b
        or c
        jp nz, filloop
    __endasm;
    #endif

    // anzeigen
    memcpy( (void*) 0x8800, (void*) 0x4000, 0x2000);
    
    // Prüfsumme bilden
    refsum = sum16_fast( (void*) 0x4000, 0x2000);
    //hlhx( refsum);
    //refsum = sum16( (void*) 0x4000, 0x2000);
    //hlhx( refsum);

    // -------------------------------
    // erste Schleife mit Millisekunden bis 1000
    // (1 tick = 20 ms)
    // Startwartezeit: 20 ms
    wait_time = 1;

    cputs( "ms");

    do
    {
        gotoxy( 0, time_line);
        crt( CLL);
        print_int( wait_time * 20);
        
        // warten
        wait_20ms( wait_time);
        if( brkt() > 0)
        {
            winin( 0, 0, 40, 32, 0);
            crlf();
            return;
        }

        // lesen und Prüfsumme bilden
        memcpy( (void*) 0x8800, (void*) 0x4000, 0x2000);
        
        // Prüfsumme bilden
        sum = sum16_fast( (void*) 0x4000, 0x2000);

        if( sum == refsum)
        {
            cputs( " ok");
        }
        else
        {
            cputs( " FAIL");
        }
        time_line++;

        // Zeit erhoehen
        wait_time = wait_inc( wait_time);

    } while(( wait_time <= 50));

    // -------------------------------
    // zweite Runde mit Sekunden bis 1000
    time_line   = TIMELINE;
    gotoxy( 0, TIMELINE - 1);
    cputs( "sec ");
        
    // Startwartezeit: 2 s
    wait_time = 2;
    
    do
    {
        gotoxy( 0, time_line);
        crt( CLL); crt( CUD); crt( CLL); crt( CUU);
        print_int( wait_time);
            
        // warten
        if( wait_sec( wait_time) > 0)
        {
            winin( 0, 0, 40, 32, 0);
            crlf();
            return;
        }

        // lesen und Prüfsumme bilden
        memcpy( (void*) 0x8800, (void*) 0x4000, 0x2000);
        
        // Prüfsumme bilden
        sum = sum16_fast( (void*) 0x4000, 0x2000);

        if( sum == refsum)
        {
            cputs( " ok");
        }
        else
        {
            cputs( " FAIL");
        }

        if( time_line < 27)
        {
            time_line++;
        }
        else
        {
            time_line   = TIMELINE;
        }

        // kein Fehler -> Zeit verdoppeln
        wait_time = wait_inc( wait_time);

    } while( wait_time <= 1000);

    crlf();
    winin( 0, 0, 40, 32, 0);
}


// -------------------------------
// Wartezeit erhöhen (in Stufen)
uint16_t wait_inc( uint16_t value)
{
    if( value < 10)
    {
        return value + 1;
    }
    if( value < 20)
    {
        return value + 2;
    }
    if( value < 50)
    {
        return value + 5;
    }
    if( value < 100)
    {
        return value + 10;
    }
    if( value < 200)
    {
        return value + 20;
    }
    if( value < 500)
    {
        return value + 50;
    }
    if( value < 1000)
    {
        return value + 100;
    }
    return value + value / 2 + value / 3;
}


// -------------------------------
// 1 tick = 20 ms
void wait_20ms( uint16_t ticks)
{
    uint16_t end;

    setup_ctc_for_measure();
    end   = CTC2 - ticks;
    while( CTC2 > end) {};
}


// -------------------------------
// 1 tick = 1 Sekunde
// Abbruch mit Taste
uint8_t wait_sec( uint16_t seconds)
{
    #if 0
        // for faster debug
        wait_20ms( 5);
    #else
    while( seconds > 0)
    {
        wait_20ms( 50);
        seconds--;
        if( brkt() > 0)
        {
            return 1;
        }
    }
    #endif
    return 0;
}


// -------------------------------
// CTC einrichten
void setup_ctc_for_measure( void)
{
    // setup CTC with count value 256
    CTC2 = 
        CTC_CONTROL_WORD + 
        CTC_SOFTWARE_RESET +
        CTC_TIME_CONSTANT_FOLLOW +
        CTC_COUNTER_MODE;
    CTC2 = 0xff;
}

#if 0
// -------------------------------
// SUM function
uint16_t sum16( uint8_t* address, uint16_t length)
{
    uint16_t value = 0;
    for( uint16_t index = 0; index < length; index++)
    {
        value += *address;
        address++;
    }
    return( value);
}
#endif

//  C   2.68 seconds for 8 kByte
// ASM  0.3  seconds for 8 kByte
// ASM ohne fast loop: ca. 20% langsamer
uint16_t sum16_fast( uint8_t* address, uint16_t length) __naked
{
__asm
    ld  hl, #2
    add hl, sp

    // get parameters from stack
    // first address
    ld  e, (hl)
    inc hl
    ld  d, (hl)     ; DE = length
    inc hl  

    // second length
    ld  c, (hl)
    inc hl
    ld  b, (hl)     ; BC = address

    ld  h, b
    ld  l, c        ; HL = address

    exx
    ld  b, #0       ; high byte
    ld  hl, #0      ; Summe zuruecksetzen
    exx
    
    ; prepare fast loop:
    ; http://map.grauw.nl/articles/fast_loops.php
    ld  b, e
    dec de
    inc d

loop:
    ld  a, (hl)     ; neues Byte holen
    
    exx
    ld  c, a
    add hl, bc      ; addieren
    exx
    
    inc hl          ; naechste Adresse

    djnz loop       ; loop counter
    dec d
    jr  nz, loop

    exx             ; Ergebnis nach HL
    ld  a, h
    exx
    ld  h, a
    exx
    ld  a, l
    exx
    ld  l, a

    ret

__endasm;
}


// -------------------------------
// integer ausgeben
char ibuf[ 8];
void print_int( int i) 
{
    char c = i, len;
    if( i < 0)
    {
        crt( '-');
        i = -i;
    }
    else
    {
        //crt( ' ');
    }
    __itoa( i, ibuf, 10);
    len = strlen( ibuf);
    for (c = 0; c < len; c++)
        crt( ibuf[c]);
}
