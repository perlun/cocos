/* io.c - basic I/O support. (really only output, no input is supported. :-) */
/* Copyright (C) 2008  Per Lundberg */

#include <stdint.h>

/* Constants */
/* The number of columns. */
#define SCREEN_COLUMNS                  80

/* The number of rows. */
#define SCREEN_ROWS                     24

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
static volatile console_character_t *screen;

/* The cursor. */
static cursor_t cursor;

/* The current attribute to write text with. */
//static uint8_t current_attribute = KERNEL_DEFAULT_ATTRIBUTE;

/* Clear the screen and initialize the I/O variables. */
void io_init(void)
{
    screen = (console_character_t *) VIDEO_MEMORY_BASE;
    
    // The 32-bit kernel usually prints one line, so we start one line down...
    cursor.x = 0;
    cursor.y = 1;
}

static void newline()
{
    cursor.y++;
    cursor.x = 0;
}

static void io_sleep()
{
    // Obviously, this method of generating a delay is quite lousy, it should at least be calibrated somehow... Using I/O
    // in() is probably a much better way if we want to do it at least a whee bit better :-)
    for (int i = 0; i < 30000; i++)
    {
        int x = 42;
        x++;
        x--;
        x = x * 23;
    }
}

/* Print a string to the console. 
 
   @param string the string to print. */
void io_print_line(const char *string)
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

        if (cursor.y > SCREEN_ROWS)
        {
            // FIXME: implement scrolling please. :-)
        }        
    }

    // ...and add the final newline.
    newline();
}
