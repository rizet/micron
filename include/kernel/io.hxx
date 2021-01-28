#pragma once
#include <stdint.h>

#define PIC1		    0x20
#define PIC2		    0xA0
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	    (PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	    (PIC2+1)
#define PIC_EOI		    0x20

#define ICW1_ICW4	    0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	    0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	    0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	    0x10		/* Initialization - required! */

#define ICW4_8086	    0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	    0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */

namespace io {

    void                    outb(uint16_t port, uint8_t val);
    uint8_t                 inb(uint16_t port);
    void                    io_wait(void);

    namespace serial {
        extern "C" void     serial_msg(const char *val);
        extern "C" void     serial_byte(uint8_t val);
    }

    namespace pic {
        void                irq_mask(unsigned char IRQLine);
        void                irq_unmask(unsigned char IRQLine);
        extern "C" void     pic_remap(int offset1, int offset2);
        extern "C" void     pic_send_eoi(unsigned char irq);
    }
}