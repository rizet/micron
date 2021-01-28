#include "kernel/io.hxx"

void io::pic::pic_send_eoi(unsigned char irq)
{
	if(irq >= 8)
		io::outb(PIC2_COMMAND,PIC_EOI);

	io::outb(PIC1_COMMAND,PIC_EOI);
}

void io::pic::pic_remap(int offset1, int offset2)
{
	unsigned char a1, a2;

	a1 = io::inb(PIC1_DATA);                        // save masks
	a2 = io::inb(PIC2_DATA);

	io::outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io::io_wait();
	io::outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io::io_wait();
	io::outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	io::io_wait();
	io::outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	io::io_wait();
	io::outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io::io_wait();
	io::outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io::io_wait();

	io::outb(PIC1_DATA, ICW4_8086);
	io::io_wait();
	io::outb(PIC2_DATA, ICW4_8086);
	io::io_wait();

	io::outb(PIC1_DATA, a1);   // restore saved masks.
	io::outb(PIC2_DATA, a2);
}

void io::pic::irq_mask(unsigned char IRQLine) {
    uint16_t port;
    uint8_t value;
 
    if(IRQLine < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQLine -= 8;
    }
    value = io::inb(port) | (1 << IRQLine);
    io::outb(port, value);        
}
 
void io::pic::irq_unmask(uint8_t IRQLine)
{
    uint16_t port;
    uint8_t value;
 
    if (IRQLine < 8)
        port = PIC1_DATA;
    else {
        port = PIC2_DATA;
        IRQLine -= 8;
    }
    value = io::inb(port) & ~(1 << IRQLine);
    io::outb(port, value);        
}

extern "C" void configure_pic() {
	io::pic::pic_remap(0x20, 0x28);
}