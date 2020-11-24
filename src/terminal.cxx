#include "kernel/terminal.hxx"
#include "kernel/string.hxx"
#include "kernel/io.hxx"
#include "kernel/logo.hxx"
#include "kernel/uart.hxx"
#include "kernel/kutil.hxx"
#include "kernel/gfx.hxx"

enum vga_color
{
    VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15
};

#define CHECK_COLOR(farbe)          \
	if (strcomp(#farbe, colorCode)) \
		color = VGA_COLOR_##farbe;

Terminal::Terminal() : row(0), column(0)
{
	uint32_t *buffer = (uint32_t *) gop.framebuffer_base_addr;

	for (int i = 0; i < 1024 * 768; i++)
	{
		buffer[i] = 0x00000000;
	}
}

void Terminal::clear()
{
	for (int y = 0; y < VGA_HEIGHT; y++)
	{
		for (int x = 0; x < VGA_WIDTH; x++)
		{
			auto index = (y * VGA_WIDTH) + x;
			buffer[index] = 0;
		}
	}
}

void Terminal::put_entry_at(char c, uint8_t color, size_t x, size_t y)
{
	uint64_t font_selector = FONT[c]; // hardcoded (temp), 'A'

	uint8_t bits[64];

	for (uint8_t i = 1; i <= 64; i++)
	{
		bits[i] = get_bit(font_selector, i);
	}

	uint8_t new_bits[64];

	int revIndex = 0;
	int arrIndex = 64 - 1;
	while (arrIndex >= 0)
	{
		/* Copy value from original array to reverse array */
		new_bits[revIndex] = bits[arrIndex];

		revIndex++;
		arrIndex--;
	}

//	for (int i = 63; i >= 0; i--)
//	{
//		if ((i + 1) % 8 == 0)
//			serial_msg('\n');
//		serial_msg(new_bits[i] + 48); // 48, ASCII code '0'
//	}

\


	for (int x = 0, y = 0; (x * y) <= 64; x++, y++);
}

void Terminal::put_char(char c, uint8_t color)
{
	if (c == 0)
		return;

	if (c != '\n')
	{
		put_entry_at(c, color, column, row);
	}
	if (++column >= VGA_WIDTH)
	{
		column = 0;
		if (++row >= VGA_HEIGHT)
		{
			shift();
		}
	}
	if (c == '\n')
	{
		if (++row >= VGA_HEIGHT)
		{
			shift();
		}
		column = 0;
	}
}

void Terminal::write(const char *data, size_t size)
{
	uint8_t color = VGA_COLOR_LIGHT_GREY;
	char colorCode[15];

	for (; *data != '\0'; data++)
	{
		if (*data == '$')
		{
			int iter = 0;
			data++;
			while (true)
			{
				if (*data != '!')
				{
					colorCode[iter] = *data;
					iter++;
					data++;
				}
				else
				{
					data++;
					break;
				}
			}
			colorCode[iter + 1] = 0;
			CHECK_COLOR(BLACK)
			CHECK_COLOR(BLUE)
			CHECK_COLOR(GREEN)
			CHECK_COLOR(CYAN)
			CHECK_COLOR(RED)
			CHECK_COLOR(MAGENTA)
			CHECK_COLOR(BROWN)
			CHECK_COLOR(LIGHT_GREY)
			CHECK_COLOR(DARK_GREY)
			CHECK_COLOR(LIGHT_BLUE)
			CHECK_COLOR(LIGHT_GREEN)
			CHECK_COLOR(LIGHT_CYAN)
			CHECK_COLOR(LIGHT_RED)
			CHECK_COLOR(LIGHT_MAGENTA)
			CHECK_COLOR(LIGHT_BROWN)
			CHECK_COLOR(WHITE)
		}
		put_char(*data, color);
	}
}
void Terminal::write(const char *data)
{
	write(data, strlen(data));
}
void Terminal::setCursor(size_t columnc, size_t rowc)
{
	column = columnc;
	row = rowc;
}
void Terminal::write(int data)
{
	auto convert = [](unsigned int num)
	{
		static char numberCharacters[]= "0123456789";
		static char buffer[64];
		char *ptr;

		ptr = &buffer[49];
		*ptr = '\0';

		do {
			*--ptr = numberCharacters[num % 10];
			num /= 10;
		} while(num != 0);

		return ptr;
	};

	if (data < 0) {
		write("-");
	}
	write(convert(data));
}
void Terminal::println(const char *data)
{
	write(data);
	write("\n");
}

void Terminal::shift()
{
    //for (int i = 0; i < 80; i++)
   // {
 //       buffer[i] = vga_entry(0, 0);
//    }

    for (int i = 0; i < (int)(VGA_HEIGHT * VGA_WIDTH); i++) {
        buffer[i] = i>((VGA_HEIGHT-1) * VGA_WIDTH) ? 0 : buffer[i+VGA_WIDTH];
    }

    if (staticLogo)
    {
      //  for (int i = 0; i < (VGA_WIDTH * 15); i++)
    //    {
  //          buffer[i] = vga_entry(0, 0);
//        }

        size_t rowtemp = row;
        size_t coltemp = column;
        setCursor(0, 0);
        Terminal::instance() << logo;
        setCursor(coltemp, rowtemp);
    }

	row--;
}

Terminal& Terminal::instance()
{
	static Terminal instance;
	return instance;
}