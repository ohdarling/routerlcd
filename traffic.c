/**
 * System traffic info
 * @author  ohdarling88 <ohdarling88@gmail.com>
 * @date    2011.07.20
 */

#include "traffic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "common.h"
#include "lcd.h"


unsigned char arrow_up[]    = { 0x20, 0x70, 0xA8, 0x20, 0x20, 0x20, 0x20 };
unsigned char arrow_down[]  = { 0x20, 0x20, 0x20, 0x20, 0xA8, 0x70, 0x20 };


void get_traffic(char *dev, int traffic[]) {
	char cmd[100];
	sprintf(cmd, "cat /proc/net/dev|grep \"%s\"", dev);
	char *line = get_cmd_result(cmd);
	
	int bytes = 0;
	int index = 0;
	while (*line != '\0') {
		if (*line != ' ') {
			bytes = 0;
			while (*line != '\0' && *line != ' ') {
				bytes = bytes * 10 + (*line)-'0';
				++line;
			}
			//printf("%d\n", bytes);
			if (index == 1) {
				traffic[1] = bytes;
			} else if (index == 9) {
				traffic[0] = bytes;
			}
			index++;
		}
		++line;
	}
}


void convert_bytes_to_rate_string(int bytes, char *buf) {
	char *suffix = "B/s";
	float result = bytes;
	if (result >= 1024) {
		suffix = "KB/s";
		result = result / 1024.0;
	}
	if (result >= 1024) {
		suffix = "MB/s";
		result = result / 1024.0;
	}
	if (result >= 1024) {
		suffix = "GB/s";
		result = result / 1024.0;
	}
	char fmt[10];
	sprintf(fmt, "%%0.%df%%s", (int)result == result ? 0 : (result < 10 ? 2 : (result < 100 ? 1 : 0)));
	sprintf(buf, fmt, result, suffix);
	int i = strlen(buf);
	for (; i < 8; ++i) {
		buf[i] = ' ';
	}
	buf[8] = '\0';
}


int is_link_down(char *dev) {
	char cmd[100];
	sprintf(cmd, "/sbin/ifconfig %s 2>/dev/null | grep \"Link encap\"", dev);
	char *ret = get_cmd_result(cmd);
	return ret[0] == 0 ? 1 : 0;
}


void show_traffic(int pos, char *dev, char *name) {
	static int prev_traffic[MAX_TRAFFIC_DEVICES][2] = { {0,0}, {0,0}, {0,0} };
	static int traffic_loaded[MAX_TRAFFIC_DEVICES] = { 0 };
	int y = pos * 9 + 19;
	
	if (is_link_down(dev)) {
		lcd_draw_string(0, y, name);
		lcd_draw_string(33, y, "link is down");
		return;
	}
	
	int prev_tx = prev_traffic[pos][0],
		prev_rx = prev_traffic[pos][1];
	int traffic[2];
	get_traffic(dev, traffic);
	//printf("traffic %d, %d\n", traffic[0], traffic[1]);
	
	int tx_bytes = traffic[0] - prev_tx,
		rx_bytes = traffic[1] - prev_rx;
	char tx_rate[10], rx_rate[10];
	
	convert_bytes_to_rate_string(tx_bytes, tx_rate);
	convert_bytes_to_rate_string(rx_bytes, rx_rate);
	prev_traffic[pos][0] = traffic[0];
	prev_traffic[pos][1] = traffic[1];
	
	if (traffic_loaded[pos] == 0) {
	    lcd_draw_string(0, y, name);
		lcd_draw_string(33, y, "            "); // clear link is down text
	    lcd_draw_image(33, y, 1, 7, arrow_up);
	    lcd_draw_image(82, y, 1, 7, arrow_down);
    	traffic_loaded[pos] = 1;
	}
	
	lcd_draw_string(39, y, tx_rate);
	lcd_draw_string(88, y, rx_rate);
}

