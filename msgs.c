/**
 * Show external message
 * @author  ohdarling88 <ohdarling88@gmail.com>
 * @date    2011.07.20
 */

#include "msgs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "lcd.h"


void show_msgs(const char *file, int y, int maxlines) {
    char buf[50];
    char *str = NULL;
    int lines = 0, i;
    FILE *fp = fopen(file, "r");
    
	while (fp != NULL &&
	       (str = fgets(buf, sizeof(buf), fp)) != NULL &&
	       lines < maxlines) {
	    
	    // remove end of line   
		int len = strlen(str);
		if (len > 0) {
			char *newline = strchr(str, 0x0D);
			if (newline) { *newline = 0; }
			newline = strchr(str, 0x0A);
			if (newline) { *newline = 0; }
        }
        
        len = strlen(str);
        if (len > 25) {
            *(str+25) = 0;
        } else {
            for (i = len; i < 25; ++i) {
                buf[i] = ' ';
            }
            buf[25] = 0;
        }
        
        lcd_draw_string(0, y + lines * 9, buf);
        lines++;
	}
	
	sprintf(buf, "%25s", " ");
	for (i = lines; i < maxlines; ++i) {
	    lcd_draw_string(0, y + lines * 9, buf);
	}
	
	if (fp != NULL) { fclose(fp); }
}

