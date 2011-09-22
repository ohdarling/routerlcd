/**
 * System info
 * @author  ohdarling88<ohdarling88@gmail.com>
 * @date    2011.07.20
 */

#include "sysinfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>

#include "common.h"
#include "lcd.h"


void show_hostname() {
	char hostname[50];
	gethostname(hostname, sizeof(hostname));
	lcd_draw_string(0, 0, hostname);
}


/**
 * @see http://linux.about.com/library/cmd/blcmdl2_sysinfo.htm
 */
struct sysinfo* get_sysinfo() {
    static struct sysinfo s_info;
    sysinfo(&s_info);
    return &s_info;
}


unsigned long *get_loads() {
    return get_sysinfo()->loads;
}


/**
 * @see http://www.linuxquestions.org/questions/programming-9/load-average-return-values-from-sysinfo-309720/
 */
void show_loads() {
	unsigned long *loads = get_loads();
	lcd_draw_string(64, 9, "Load");
	char tmp[20];
	sprintf(tmp, "%0.2f", loads[0]/65536.0);
	lcd_draw_string(86, 9, tmp);
	sprintf(tmp, "%0.2f", loads[1]/65536.0);
	lcd_draw_string(108, 9, tmp);
}


void show_time() {
	char tmp[10];
    sprintf(tmp, "%5s", get_cmd_result("date '+%H:%M'"));
    lcd_draw_string(128-5*5, 0, tmp);
}


void show_mems() {
	struct sysinfo *sinfo = get_sysinfo();
	char tmp[20];
	lcd_draw_string(0, 9, "Mem");
	sprintf(tmp, "%0.1f/%0.0fM", sinfo->freeram/1024.0/1024.0,
								 sinfo->totalram/1024.0/1024.0);
	lcd_draw_string(17, 9, tmp);
}

