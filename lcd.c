/**
 * HB12864M2A Operating Lib
 * @author  ohdarling88 <ohdarling88@gmail.com>
 * @date    2011.07.19
 */

#include "lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

//const char *tty_dev = "/dev/tty.PL2303-007013FD";
const char *tty_dev 	= "/dev/ttyS0";

static int serial_fd;

void set_termios(struct termios *pNewtio, unsigned short uBaudRate)
{
    bzero(pNewtio,sizeof(struct termios));
    pNewtio->c_cflag = uBaudRate|CS8|CREAD|CLOCAL;
    pNewtio->c_iflag = IXOFF;
    pNewtio->c_oflag = 0;
    pNewtio->c_lflag = 0;
    pNewtio->c_cc[VINTR] = 0;
    pNewtio->c_cc[VQUIT] = 0;
    pNewtio->c_cc[VERASE] = 0;
    pNewtio->c_cc[VKILL] = 0;
    pNewtio->c_cc[VEOF] = 4;
    pNewtio->c_cc[VTIME] = 1;
    pNewtio->c_cc[VMIN] = 0;
    //pNewtio->c_cc[VSWTC] = 0;
    pNewtio->c_cc[VSTART] = 0;
    pNewtio->c_cc[VSTOP] = 0;
    pNewtio->c_cc[VSUSP] = 0;
    pNewtio->c_cc[VEOL] = 0;
    pNewtio->c_cc[VREPRINT] = 0;
    pNewtio->c_cc[VDISCARD] = 0;
    pNewtio->c_cc[VWERASE] = 0;
    pNewtio->c_cc[VLNEXT] = 0;
    pNewtio->c_cc[VEOL2] = 0;
}


int open_port() {
    int fd = open(tty_dev, O_RDWR | O_NOCTTY | O_NDELAY);
    
    if (fd == -1) {
        printf("open_port: Unable to open %s\n", tty_dev);
    } else {
        fcntl(fd, F_SETFL, 0);
    }
        
    printf("file handle %d\n", fd);
    
    return (fd);
}


void lcd_connect() {
	if (serial_fd == 0) {
		serial_fd = open_port();
		if (serial_fd > 0) {
		    struct termios options;
		    set_termios(&options, B9600);
		    tcsetattr(serial_fd, TCSANOW, &options);
			usleep(100);
		}
	}
}


void lcd_disconnect() {
    if (serial_fd > 0) {
        close(serial_fd);
    }
    serial_fd = 0;
}


void lcd_delay() {
    usleep(50000);
    //sleep(1);
}


int lcd_send_data(unsigned char *data, int len) {
	lcd_connect();
    int ret = write(serial_fd, data, len);
    return ret;
}


int lcd_send_byte(unsigned char byte) {
	lcd_connect();
    unsigned char data[1];
    data[0] = byte;
    int ret = write(serial_fd, data, 1);
    return ret;
}


void lcd_read() {
}


void lcd_clear(int fd) {
	unsigned char cmd[] = { 0xF4, 0xBB, 0xBB, 0xBB };
	lcd_send_data(cmd, 4);
	lcd_delay();
}


void lcd_reset() {
	lcd_send_byte(0xEF);
	sleep(1);
}


void lcd_set_backlight(int open) {
	unsigned char cmd[] = { 0xB5, 0x00 };
	cmd[1] = open == 0 ? 0 : 1;
	lcd_send_data(cmd, 2);
	lcd_delay();
}


void lcd_draw_string(int x, int y, char *str) {
	printf("lcd draw string: %s\n", str);
	unsigned char cmd[3] = { 0xDB, 0, 0 };
	cmd[2] = x;
	cmd[1] = y;
	lcd_send_data(cmd, 3);
	lcd_send_data((unsigned char*)str, strlen(str));
	lcd_send_byte('\0');
	lcd_delay();
	return;
	
	int i = 0;
	while (*str != '\0') {
	    lcd_send_data((unsigned char *)str, 1);
	    ++str;
	    ++i;
	    if (i % 16 == 0 && *str != '\0') {
	        lcd_send_byte('\0');
	        usleep(1000);
	        cmd[2] = (x+i)*5;
	        lcd_send_data(cmd, 3);
	    }
	}
	lcd_send_byte('\0');
	lcd_delay();
}


void lcd_draw_image(int x, int y, int width, int height, unsigned char *data) {
    unsigned char cmd[] = { 0xFD, 0, 0, 0, 0 };
    int width_bytes = (width+7) / 8;
    cmd[1] = y;
    cmd[2] = x;
    cmd[3] = width_bytes;
    cmd[4] = height;
    lcd_send_data(cmd, 5);
    lcd_send_data(data, width_bytes*height);
	lcd_delay();
}
