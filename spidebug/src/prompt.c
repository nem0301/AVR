#ifdef SPI_MASTER
#include <stdio.h>
#include <string.h>

#include <common.h>

//global variables
char esc = 0;
char lbracket = 0;

char prompt[20] = "console > ";

typedef struct {
    char text[CMDSIZE];
}Console;

Console console[HISTSIZE];
char cmdbuf[CMDSIZE];
// cursors position
int x, y;
int cy = -1;

void clearScreen()
{
	printf("\033[H\033[J");
	esc = 0;
    lbracket = 0;
}

void setCursor(int x, int y)
{
	printf("\033[%d;%dH", (y)+1, (x)+1);
}

void key(char c)
{
    int i;
	switch(c)
	{
    // return and run command
	case 13:
	    x = 0;
	    putchar('\n');

	    if (strlen(cmdbuf) > 0)
        {
            strcpy(console[y].text, cmdbuf);
            func(cmdbuf);
            y = (y + 1) % HISTSIZE;
            console[y].text[0] = '\0';
            cmdbuf[0] = '\0';
        }

	    printf("%s", prompt);
	    cy = -1;
		break;
    // backspace
	case 8:
		if (x > 0)
        {
            x--;
            i = x;
            while (cmdbuf[i] != '\0')
            {
                cmdbuf[i] = cmdbuf[i+1];
                i++;
            }
            printf("%c[2K\r", 27);
            printf("%s%s", prompt, cmdbuf);

            int len = strlen(cmdbuf);

            for (i = 0; i < len - x; i++)
            {
                putchar(27);
                putchar(91);
                putchar('D');
            }
        }
		break;
    // default processing
    default:
        // get escape character
        if (!esc && !lbracket && c == 27)
        {
            esc = 1;
            putchar(c);
            return;
        }
        // get left bracket character
        else if (esc && !lbracket && c == 91)
        {
            lbracket = 1;
            putchar(c);
            return;
        }
        // arrow key handling
        else if (esc && lbracket && c > 64 && c < 69)
        {
            // ignore default action
            switch(c)
            {
            case 65:
            case 66:
            case 67:
            case 68:
            default:
                putchar('C');
                putchar(27);
                putchar(91);
                putchar('D');
            }

            // cy is cursor y coordinate.
            // scoping command history up and down
            if (cy == -1) cy = y;
            if (c == 65)
            {
                cy--;

                if (cy < 0)
                    cy = HISTSIZE + cy;

                if (console[cy].text[0] == '\0')
                {
                    cy = (cy + 1) % HISTSIZE;
                }
                else
                {
                    printf("%c[2K\r", 27);
                    printf("%s%s", prompt, console[cy].text);
                    strcpy(cmdbuf, console[cy].text);
                    x = strlen(cmdbuf);
                }
            }
            else if ( c == 66)
            {
                if (console[cy].text[0] == '\0' || y - cy == 0)
                {

                }
                else
                {
                    printf("%c[2K\r", 27);
                    cy = (cy + 1) % HISTSIZE;
                    printf("%s%s", prompt, console[cy].text);
                    strcpy(cmdbuf, console[cy].text);
                    x = strlen(cmdbuf);
                }
            }
            // move cursor in current command left and right
            else if (c == 67)
            {
                if(x < strlen(cmdbuf))
                {
                    putchar(27);
                    putchar(91);
                    putchar('C');
                    x++;
                }
            }
            else if (c == 68)
            {
                if(x > 0)
                {
                    putchar(27);
                    putchar(91);
                    putchar('D');
                    x--;
                }
            }

            esc = 0;
            lbracket = 0;
            return;
        }
        else
        {
            esc = 0;
            lbracket = 0;
        }

        // store string to command buffer
        if (strlen(cmdbuf) < CMDSIZE - 1)
        {
            for (i = CMDSIZE-2; i >= x; i--)
            {
                cmdbuf[i + 1] = cmdbuf[i];
            }
            cmdbuf[x] = c;
            x++;
            printf("%c[2K\r", 27);
            printf("%s%s", prompt, cmdbuf);
            int len = strlen(cmdbuf);

            // cursor relocation
            for (i = 0; i < len - x; i++)
            {
                putchar(27);
                putchar(91);
                putchar('D');
            }
        }

		break;
	}
}
#endif
