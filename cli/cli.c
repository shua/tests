#include <poll.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/** VT100 command to clear the screen. Use puts(VT100_CLEAR_SCREEN) to clear
 *  the screen. */
#define VT100_CLEAR_SCREEN "\033[2J"

/** VT100 command to reset the cursor to the top left hand corner of the
 *  screen. */
#define VT100_CURSOR_TO_ORIGIN "\033[H"

struct frame_s
{
	int x;
	int y;
	char *data;
};

static int draw_frame(struct frame_s *frame)
{
	int row;
	char *data;
	int attrib;

	puts(VT100_CLEAR_SCREEN);
	puts(VT100_CURSOR_TO_ORIGIN);

	for (row = 0, data = frame->data; row  < frame->y; row++, data += frame->x)
	{
		/*  0 for normal, 1 for bold, 7 for reverse. */
		attrib = 0;

		/*  The VT100 commands to move the cursor, set the attribute, and the
		 *  actual frame line. */
		fprintf(stdout, "\033[%d;%dH\033[0m\033[%dm%.*s", row + 1, 0, attrib, frame->x, data);
		fflush(stdout);
	}

	return (0);
}

int main(void)
{
	const struct timespec timeout = { .tv_sec = 1, .tv_nsec = 0 };
	struct frame_s frame;
	struct termios tty_old;
	struct termios tty_new;
	unsigned char line[128];
	unsigned int count = 0;
	int ret;
	struct pollfd fds[1];
	sigset_t sigmask;
	struct tm *tp;
	time_t current_time;

	/*  Set up a little frame. */
	frame.x = 80;
	frame.y = 5;
	frame.data = malloc(frame.x * frame.y);

	if (frame.data == NULL)
	{
		fprintf(stderr, "No memory\n");
		exit (1);
	}

	memset(frame.data, ' ', frame.x * frame.y);

	/*  Get the terminal state. */
	tcgetattr(STDIN_FILENO, &tty_old);
	tty_new = tty_old;

	/*  Turn off "cooked" mode (line buffering) and set minimum characters
	 *  to zero (i.e. non-blocking). */
	tty_new.c_lflag &= ~ICANON;
	tty_new.c_cc[VMIN] = 0;

	/*  Set the terminal attributes. */
	tcsetattr(STDIN_FILENO, TCSANOW, &tty_new);

	fds[0].events = POLLIN;
	fds[0].fd = STDIN_FILENO;

	/*  Loop forever waiting for key presses. Update the output on every key
	 *  press and every 1.0s (when ppoll() times out). */
	do
	{
		fds[0].revents = 0;
		ret = poll(fds, sizeof(fds)/sizeof(struct pollfd), 1000);

		if (fds[0].revents & POLLIN)
		{
			ret = read(STDIN_FILENO, &line[count], sizeof(line) - count);

			if (ret > 0)
			{
				line[count + ret] = '\0';

				if (strcmp(&line[count], "\033[A") == 0)
				{
					snprintf(frame.data, frame.x, "up");
					count = 0;
				}
				else if (strcmp(&line[count], "\033[B") == 0)
				{
					snprintf(frame.data, frame.x, "down");
					count = 0;
				}
				else if (line[count] == 127) // backspace
				{
					if (count != 0) { count -= ret;}
				}
				else if (line[count] == '\n')
				{
					snprintf(frame.data, frame.x, "entered: %s", line);
					count = 0;
				}
				else
				{
					count += ret;
				}
			}
		}

		/*  Print the current time to the output buffer. */
		current_time = time(NULL);
		tp = localtime(&current_time);
		strftime(&frame.data[1 * frame.x], frame.x, "%Y/%m/%d %H:%M:%S", tp);

		/*  Print the command line. */
		line[count] = '\0';
		snprintf(&frame.data[(frame.y - 1) * frame.x], frame.x, "$ %s", line);

		draw_frame(&frame);
	}
	while (1);

	/*  Restore terminal and free resources. */
	tcsetattr(STDIN_FILENO, TCSANOW, &tty_old);
	free(frame.data);

	return (0);
}
