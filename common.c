/**
 * Common functions
 * @author  ohdarling88<ohdarling88@gmail.com>
 * @date    2011.07.20
 */

#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* get_cmd_result(char *cmd) {
	static char cmd_result[1000];
    //printf("cmd: %s\n", cmd);
    
    FILE *fp;
    fp = popen(cmd, "r");
    char *ret = fgets(cmd_result, sizeof(cmd_result), fp);
    pclose(fp);
    
    if (ret != NULL) {
	    char *newline = strchr(cmd_result, '\n');
	    if (newline != NULL) {
    		*newline = '\0';
    	}
    } else {
    	cmd_result[0] = '\0';
   	}
    
    //printf("cmd result %s\n", cmd_result);
    return cmd_result;
}


