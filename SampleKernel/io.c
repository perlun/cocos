#include <io.h>

/* Constants */
/* The number of columns.  */
#define COLUMNS			80

/* The number of lines.  */
#define LINES			24

/* The attribute of an character.  */
#define ATTRIBUTE		7

/* The video memory address.  */
#define VIDEO_BASE		0xB8000

/* Types. */
typedef unsigned char uint8_t;

typedef struct
{
  unsigned char character;
  unsigned char attribute;
} console_character_type;

typedef struct
{
  int x;
  int y;
} cursor_type;

/* Variables. */
/* Point to the video memory.  */
static volatile console_character_type *video;

/* The cursor. */
static cursor_type cursor;

/* Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', interpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal.  */
static void itoa (char *buf, int base, int d)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;
  
  /* If %d is specified and D is minus, put `-' in the head.  */
  if (base == 'd' && d < 0)
    {
      *p++ = '-';
      buf++;
      ud = -d;
    }
  else if (base == 'x')
    {
      divisor = 16;
    }

  /* Divide UD by DIVISOR until UD == 0.  */
  do
    {
      int remainder = ud % divisor;
      
      *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
  while (ud /= divisor);

  /* Terminate BUF.  */
  *p = 0;
  
  /* Reverse BUF.  */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2)
    {
      char tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2--;
    }
}

/* Clear the screen and initialize the IO variables.  */
void io_init (void)
{
  int i;

  video = (console_character_type *) VIDEO_BASE;
  
  // Clear the screen.
  for (i = 0; i < COLUMNS * LINES; i++)
    {
      video[i].character = 0;
      video[i].attribute = 0;
    }
  
  // Reset the cursor.
  cursor.x = 0;
  cursor.y = 0;
}

/* Print a newline to the screen. */
static void newline()
{
    cursor.x = 0;
    cursor.y++;
    if (cursor.y >= LINES)
      {
	  cursor.y = 0;
      }
}

/* Put the character C on the screen.  */
static void putchar (int c)
{
    if (c == '\n' || c == '\r')
	{
	    newline();
	    return;
	}
 
    // Ugly, we should probably define the screen to be a u16 array
    video[cursor.x + cursor.y * COLUMNS].character = c & 0xFF;
    video[cursor.x + cursor.y * COLUMNS].attribute = ATTRIBUTE;
    
    cursor.x++;
    if (cursor.x >= COLUMNS)
	{
	    newline();
	    return;
	}
}

/* Format a string and print it on the screen, similar to the libc
   function printf. Of course, this function is much more limited than a
   full ISO C printf() implementation. */
void io_printf (const char *format, ...)
{
  // Quite a rough way to handle variadic arguments... We should use
  // the standard C way rather than this, I think.
  char **arg = (char **) &format;
  int c;
  char buf[20];

  arg++;
  
  while ((c = *format++) != 0)
    {
      if (c != '%')
	{
	  putchar (c);
	}
      else
	{
	  char *p;
	  
	  c = *format++;
	  switch (c)
	    {
	    case 'd':
	    case 'u':
	    case 'x':
	      itoa (buf, c, *((int *) arg++));
	      p = buf;
	      goto string;
	      break;

	    case 's':
	      p = *arg++;
	      if (! p)
		{
		  p = "(null)";
		}

	    string:
	      while (*p)
		{
		  putchar (*p++);
		}
	      break;

	    default:
	      putchar (*((int *) arg++));
	      break;
	    }
	}
    }
}
