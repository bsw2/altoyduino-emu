#include "usart.h"

#include <stdio.h>
#include <sys/select.h>

#include "cpu.h"

#define USART_UCSR0A    0xc0
#define USART_UCSR0B    0xc1
#define USART_UDR0      0xc6

#define USART_UCSRA_UDRE    BIT(5)
#define USART_UCSRA_TXC     BIT(6)
#define USART_UCSRA_RXC     BIT(7)

#define USART_UCSRB_RXCIE   BIT(7)

#define USART_IRQ   19

u8 UCSRA;
u8 UCSRB;

u8 usart_read_ucsra(u16 addr)
{
    return UCSRA | USART_UCSRA_UDRE;
}

void usart_write_ucsra(u16 addr, u8 value)
{
    UCSRA &= ~(value & (USART_UCSRA_TXC));
}

u8 usart_read_ucsrb(u16 addr)
{
    return UCSRB;
}

void usart_write_ucsrb(u16 addr, u8 value)
{
    UCSRB = value;
    if (UCSRB & USART_UCSRB_RXCIE) {
    }
}

u8 usart_read_udr(u16 addr)
{
    if (UCSRA & USART_UCSRA_RXC) {
        UCSRA &= ~USART_UCSRA_RXC;
        return getchar();
    } else {
        return 0;
    }
}

void usart_write_udr(u16 addr, u8 value)
{
    putchar(value);
}

void usart_poll()
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    struct timeval timeout = {0, 0};
    if (select(1, &fds, NULL, NULL, &timeout) > 0) {
        UCSRA |= USART_UCSRA_RXC;
        if (UCSRB & USART_UCSRB_RXCIE) {
            irq(USART_IRQ);
        }
    }
}

void usart_init()
{
    register_io(USART_UCSR0A, usart_read_ucsra, usart_write_ucsra);
    register_io(USART_UCSR0B, usart_read_ucsrb, usart_write_ucsrb);
    register_io(USART_UDR0, usart_read_udr, usart_write_udr);
    register_poll(usart_poll);
}