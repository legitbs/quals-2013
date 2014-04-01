#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

struct termios orig_termios;

int ttyfd = 0;

int tty_reset(void)
   {
    /* flush and reset */
    if (tcsetattr(ttyfd,TCSAFLUSH,&orig_termios) < 0) return -1;
    return 0;
   }

void tty_exit(void) {
	tty_reset();
}

int main(int argc, char ** argv)
{
    if (! isatty(ttyfd)) perror("not on a tty");

    /* store current tty settings in orig_termios */
    if (tcgetattr(ttyfd,&orig_termios) < 0) perror("can't get tty settings");

    /* register the tty reset with the exit handler */
	atexit(tty_exit);
    tty_raw();      /* put tty in raw mode */
	int i = 0;
	printf("The key is: ");
	while(i < 64) {
		char got = getchar();
		if(got == '\r') {
			printf("\r\n");
			break;
		}
		i++;
	}
    return 0;       /* tty_atexit will restore terminal */
}

void tty_raw(void)
   {
    struct termios raw;

    raw = orig_termios;

    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 1; raw.c_cc[VTIME] = 0; /* after two bytes, no timer  */

    if (tcsetattr(ttyfd,TCSAFLUSH,&raw) < 0) perror("can't set raw mode");
   }
