/**
 * Router LCD Display
 * Traffic devices store in /tmp/routerlcd/devs
 * LCD back light seconds /tmp/routerlcd/light
 * Extra message store in /tmp/routerlcd/msg
 * @authoer ohdarling88 <ohdarling88@gmail.com>
 * @date	2011.07.19
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "common.h"
#include "lcd.h"
#include "traffic.h"
#include "sysinfo.h"
#include "msgs.h"


#define LCD_MAX_LINES   7

const char *file_devs	= "/tmp/routerlcd/devs";
const char *file_light	= "/tmp/routerlcd/light";
const char *file_msgs	= "/tmp/routerlcd/msgs";
const char *file_pid    = "/tmp/routerlcd/routerlcd.pid";

static int signal_term_received	= 0;

char *traffic_devs[MAX_TRAFFIC_DEVICES];
char *traffic_dev_names[MAX_TRAFFIC_DEVICES];
int traffic_devs_count = 0;


char* get_ip(char *dev) {
    char cmd[100];
    static char result[20];
    
    sprintf(cmd, "/sbin/ifconfig %s|grep \"inet \"|cut -d \":\" -f2|cut -d \" \" -f%s", dev, "1");
    sprintf(result, "%s", get_cmd_result(cmd));
    
    return result;
}


void parse_traffic_devices() {
	char buf[50];
	char *str = NULL;
	FILE *fp = fopen(file_devs, "r");
	while (fp != NULL &&
	       (str = fgets(buf, sizeof(buf), fp)) != NULL &&
	       traffic_devs_count < MAX_TRAFFIC_DEVICES) {
	    
	    // remove end of line   
		int len = strlen(str);
		if (len > 0) {
			char *newline = strchr(str, 0x0D);
			if (newline) { *newline = 0; }
			newline = strchr(str, 0x0A);
			if (newline) { *newline = 0; }
        }
        
        len = strlen(str);
		if (len > 0) {
		    printf("device %s\n", str);
			char *whitespace = strchr(str, ' ');
			char *name = NULL;
			if (whitespace != NULL) {
			    *whitespace = 0;
			    whitespace++;
			    name = malloc(sizeof(char)*(strlen(whitespace)+1));
			    strcpy(name, whitespace);
            }
			char *dev = malloc(sizeof(char)*(len+1));
			strcpy(dev, str);
			traffic_devs[traffic_devs_count] = dev;
			traffic_dev_names[traffic_devs_count] = name == NULL ? dev : name;
			traffic_devs_count++;
		}
	}
	if (fp != NULL) { fclose(fp); }
}


void set_lcd_backlight() {
	static int left_time = 10;
	
	char tmp[20];
	char *ret = NULL;
	FILE *fd = fopen(file_light, "r");
	if (fd) {
		ret = fgets(tmp, sizeof(tmp), fd);
		fclose(fd);
		fd = fopen(file_light, "w");
		fputs("", fd);
		fclose(fd);
	}
	if (ret != NULL) {
		left_time = atoi(ret);
	}
	
	left_time--;
	lcd_set_backlight(left_time > 0 ? 1 : 0);
}


void signal_term(int sig) {
	signal_term_received = 1;
}


void write_pid() {
    FILE *fp = fopen(file_pid, "w");
    if (fp != NULL) {
        char tmp[100];
        sprintf(tmp, "%d", getpid());
        fputs(tmp, fp);
        fclose(fp);
    }
}


int main(int argc, char* argv[])
{
	printf("Start\n");
	signal(SIGINT,  signal_term);
	signal(SIGTERM, signal_term);
    
    int daemon_mode = (argc > 1 && strcmp(argv[1], "-d") == 0) ? 1 : 0;
    if (daemon_mode) {
    	pid_t pid = fork();
    	if (pid < 0) {
    	    // fork failed
    	    return -1;
    	} else if (pid != 0) {
    	    // fork successed, parent exit
    	    return 0;
    	}
    	
    	write_pid();
    	setsid();
    	chdir("/");
    	umask(0755);
    	close(0);
    	close(1);
    	close(2);
	}
	
	parse_traffic_devices();
    
    lcd_connect();
    lcd_reset();
    lcd_set_backlight(1);
    lcd_clear();
    sleep(1);
    
    show_hostname();
    
    int i;
    
    while (!signal_term_received) {
    	show_time();
    	show_mems();
    	show_loads();
    	
    	for (i = 0; i < traffic_devs_count; ++i) {
    		show_traffic(i, traffic_devs[i], traffic_dev_names[i]);
    	}
    	
    	show_msgs(file_msgs,
    	          19 + traffic_devs_count * 9 + 1,
    	          LCD_MAX_LINES-traffic_devs_count-1);
    	
    	set_lcd_backlight();
    	
    	usleep(500000);
	}
	
	lcd_clear();
	lcd_draw_string(0, 0, "routerlcd daemon is down.");
	lcd_set_backlight(0);
    lcd_disconnect();
    unlink(file_pid);
    
	return 0;
}

