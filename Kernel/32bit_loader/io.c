/* 
 * $Id: io.c 56 2008-10-31 23:21:02Z per $
 *
 * io.c - basic I/O support. (really only output, no input is supported. :-)
 *
 * Author: Per Lundberg <per@halleluja.nu>
 * Copyright: (C) 2008 Per Lundberg
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "memory.h"
#include "string.h"

/* Constants */
/* The number of columns. */
#define SCREEN_COLUMNS                  80

/* The number of rows. */
#define SCREEN_ROWS                     25

/* The video memory base address (for text-mode). */
#define VIDEO_MEMORY_BASE               0xB8000

/* The default attribute of a character printed by the kernel.  */
#define KERNEL_DEFAULT_ATTRIBUTE        0x07

/* Types. */
typedef struct
{
    uint8_t character;
    uint8_t attribute;
} console_character_t;

typedef struct
{
  int x;
  int y;
} cursor_t;

/* Variables. */
/* Pointer to the text video memory. */
static console_character_t *screen;

/* The cursor. */
static cursor_t cursor;

/* The current attribute to write text with. */
static uint8_t current_attribute = KERNEL_DEFAULT_ATTRIBUTE;

/* Clear the screen and initialize the I/O variables. */
void io_init()
{
    screen = (console_character_t *) VIDEO_MEMORY_BASE;
    
    cursor.x = 0;
    cursor.y = 0;
}

// Print a newline.
static void newline()
{
    cursor.y++;
    cursor.x = 0;

    if (cursor.y == SCREEN_ROWS)
    {
        // There are two bytes for each character on the screen, so we can perform the scrolling like this. We also reset
        // the bottom line to make this work properly.
        memory_copy(screen, &screen[SCREEN_COLUMNS], SCREEN_COLUMNS * SCREEN_ROWS * 2);
        memory_zero(&screen[SCREEN_COLUMNS * (SCREEN_ROWS - 1)], SCREEN_COLUMNS * 2);

        cursor.y = SCREEN_ROWS - 1;
    }        
}

// Convert an uint64 to arbitrary base string format. The base can be anything between 2 (binary) and 36, since by then we
// have reached the end of the alphabet... (26 letters + 10 digits)
static void number_to_string_uint64(uint64_t value, int base, char *output)
{
    char temporary_output[65];  // The maximum length the string can have (when the base is 2)
    int c = 0;

    // Do some basic argument checking here.
    if (base < 2 || base > 36)
    {
        string_copy(output, "(error)");
        return;
    }

    // We can pretty easily get each individual digit in the number by dividing it with the base (2, 10, 16 and so on) -- the
    // reminder of that operation is the digit. So, the algorithm for this function is pretty simple: take the reminder of a
    // division of the number with the base, add it to the string, and divide the number with the base. When the result of
    // that final division is <= 0, we are done.
    //
    // Now, the only problem with that algorithm is that if we do it like that, we will get the string backwards. So, either
    // we store it in the opposite order or we reverse it afterwards. Storing it in the opposite order is actually not
    // trivial, since we would have to figure out the length the string on beforehand. So, we just reverse it afterwards.

    // We must code this as a do-while loop rather than a while-loop, since we always want to print at least one
    // digit (think about the number 0).
    do
    {
        // Get the hexadecimal digit.
        int digit = value % base;

        // Obviously, you could have a lookup table for the digits. This method is perhaps a little bit slower, but uses
        // slightly less memory.
        if (digit < 10)
        {
            temporary_output[c] = '0' + digit;
        }
        else
        {
            temporary_output[c] = 'A' + (digit - 10);
        }
        value /= base;
        
        c++;
    } while (value > 0);

    // Now, let's reverse the string. We do it by looping over it backwards. The -1 and +1 here respectively are pretty
    // important. After the loop above has run, c will contain the "length" of the string, which is the position of the
    // highest index in the array PLUS ONE. In other words, if the string contain 5 digits, c will contain 6 -- since the
    // incrementation takes place at the very end of the loop.
    //
    // So... to make the mapping correct (6 => 0, 5 => 1 and so forth), we decrease it by one here. And, to place the NUL
    // character at the right place, we need to do the opposite -- we add one to the max_position.
    c--;
    int max_position = c;
    for (; c >= 0; c--)
    {
        output[max_position - c] = temporary_output[c];
        //output[max_position - c] = 'x';
    }
    output[max_position + 1] = '\0';
}

// Convert an uint32 to "arbitrary base" string format.
static void number_to_string_uint32(uint32_t value, int base, char *output)
{
    // Poor-man's function overloading... :-) Since a uint32 is totally "upward" compatible with a uint64 (but not the other
    // way around), this is totally possible. The only risk of doing like this is that 'output' could overflow its boundaries
    // if the value would somehow be larger than UINT32_MAX... but really, that isn't be possible, so it is totally safe to
    // do like this.
    number_to_string_uint64(value, base, output);
}

static void io_sleep()
{
    // Obviously, this method of generating a delay is quite lousy, it should at least be calibrated somehow... Using I/O
    // in() is probably a much better way if we want to do it at least a whee bit better :-)
    for (int i = 0; i < 3000; i++)
    {
        int x = 42;
        x++;
        x--;
        x = x * 23;
    }
}

/*
 * Print a string to the screen, 31337-style. :-)
 *
 * @param string the string to print.
 */
void io_leet_print(const char *string)
{
    for (int i = 0; string[i] != '\0'; i++)
    {
        screen[(cursor.y * SCREEN_COLUMNS) + cursor.x].character = string[i];
        screen[(cursor.y * SCREEN_COLUMNS) + cursor.x].attribute = 0x03;

        io_sleep();

        screen[(cursor.y * SCREEN_COLUMNS) + cursor.x].character = string[i];
        screen[(cursor.y * SCREEN_COLUMNS) + cursor.x].attribute = 0x07;

        io_sleep();

        screen[(cursor.y * SCREEN_COLUMNS) + cursor.x].character = string[i];
        screen[(cursor.y * SCREEN_COLUMNS) + cursor.x].attribute = 0x0F;
        cursor.x++;

        if (cursor.x == SCREEN_COLUMNS)
        {
            newline();
        }
    }
}

/*
 * Print a character to the screen.
 *
 * @param c the character to print.
 */
static void io_print_character(char c)
{
    switch (c)
    {
        case '\n':
        {
            newline();
            break;
        }
        
        default:
        {
            screen[(cursor.y * SCREEN_COLUMNS) + cursor.x].character = c;
            screen[(cursor.y * SCREEN_COLUMNS) + cursor.x].attribute = current_attribute;
            
            cursor.x++;
            
            if (cursor.x == SCREEN_COLUMNS)
            {
                newline();
            }
            
            if (cursor.y > SCREEN_ROWS)
            {
                // FIXME: implement scrolling please. :-)
            }
        }
    }
}

/*
 * Print a string to the screen.
 *
 * @param string the string to print.
 */
void io_print(const char *string)
{
    for (int i = 0; string[i] != '\0'; i++)
    {
        io_print_character(string[i]);
    }
}

/*
 * Print a string to the console, with a terminating newline.
 *
 * @param string the string to print. 
 */
void io_print_line(const char *string)
{
    io_print(string);

    // ...and add the final newline.
    newline();
}

/*
 * Print a string to the screen, using printf()-like formatting specifiers.
 *
 * @param format_string the format string to use.
 */
void io_print_formatted(const char *format_string, ...)
{
    int c = 0;
    bool previous_was_percent = false;

    // Initialize the variable argument stuff.
    va_list arguments;
    va_start(arguments, format_string);

    // Go through the string until we find a NUL byte.
    while (format_string[c] != '\0')
    {
        if (previous_was_percent)
        {
            // The previous character was a percent sign. In other words, this character is a specifier for how the data
            // should be output. The implementation here is a very stripped-down version of the ISO printf() function. We
            // only implement the stuff that we actually need.
            switch (format_string[c])
            {
                // We differ here a bit from the regular printf(). In printf(), %x means "hexadecimal with lowercase
                // letters". Here, we treat %x as "hexadecimal 32-bit integer" and %X as "hexadecimal 64-bit integer".
                case 'x':
                {
                    char string[9];             // A 32-bit integer can be no longer than 8 hex characters + a NUL
                                                // terminator.
                    number_to_string_uint32(va_arg(arguments, uint32_t), 16, string);
                    io_print(string);
                    break;
                }

                case 'X':
                {
                    char string[17];            // A 64-bit integer can be no longer than 16 hex characters + a NUL terminator.
                    number_to_string_uint64(va_arg(arguments, uint64_t), 16, string);
                    io_print(string);
                    break;
                }
                
                // Base 10 (decimal), 32-bit unsigned integer.
                case 'u':
                {
                    char string[11];            // A 32-bit integer can be no longer than 10 decimal characters + a NUL
                                                // terminator.
                    number_to_string_uint32(va_arg(arguments, uint32_t), 10, string);
                    io_print(string);
                    break;
                }

                // Base 10 (decimal), 64-bit unsigned integer.
                case 'U':
                {
                    char string[21];            // A 64-bit integer can be no longer than 20 decimal characters + a NUL
                                                // terminator.
                    number_to_string_uint64(va_arg(arguments, uint64_t), 10, string);
                    io_print(string);
                    break;
                    break;
                }

                // Base 2 (binary), 32-bit unsigned integer.
                case 'b':
                {
                    char string[11];            // A 32-bit integer can be no longer than 10 decimal characters + a NUL
                                                // terminator.
                    number_to_string_uint32(va_arg(arguments, uint32_t), 2, string);
                    io_print(string);
                    break;
                }

                // Base 2 (binary), 64-bit unsigned integer.
                case 'B':
                {
                    char string[21];            // A 64-bit integer can be no longer than 20 decimal characters + a NUL
                                                // terminator.
                    number_to_string_uint64(va_arg(arguments, uint64_t), 2, string);
                    io_print(string);
                    break;
                }

                default:
                {
                    // Just like printf(), if we encounter an unparseable percent specification, we just print it out
                    // verbatim.
                    io_print_character('%');
                    io_print_character(format_string[c]);
                }
            }

            previous_was_percent = false;
        }
        else 
        {
            // The previous character was not a percent sign (%)
            switch (format_string[c])
            {
                case '%':
                {
                    if (previous_was_percent)
                    {
                        io_print_character(format_string[c]);
                    }
                    else // !previous_was_precent
                    {
                        previous_was_percent = true;
                    }

                    break;
                }
                
                default:
                {
                    io_print_character(format_string[c]);
                }
            }
        }

        c++;
    }

    va_end(arguments);
}
