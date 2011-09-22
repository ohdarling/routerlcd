#CC      = gcc
CC	= mips-linux-gcc
CFLAGS  = -Wall -O --static -s
OBJS    = common.o lcd.o traffic.o sysinfo.o msgs.o main.o
TARGET  = routerlcd
SERVER  = root@192.168.18.1

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

routerlcd: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)
    
clean:
	rm -f *.o routerlcd
    
all: clean routerlcd

test: all
	./routerlcd

upload: all
	ssh $(SERVER) "mkdir -p /tmp/routerlcd"
	scp routerlcd $(SERVER):/tmp/routerlcd/routerlcd

routertest: upload
	ssh $(SERVER) "cd /tmp/routerlcd; echo br-lan>devs; echo tun0>>devs;/tmp/routerlcd/routerlcd"
