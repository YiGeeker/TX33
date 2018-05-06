# socket connection
target remote | openocd -f ../openocd.cfg -c "gdb_port pipe; log_output ../openocd.log"

monitor reset halt
load
break main
continue
