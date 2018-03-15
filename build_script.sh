#!/bin/sh

rm light_task temp_task logger_task socket_task external_app main_task

gcc light_sensor.c -o light_sensor.o -g -c
gcc -c wrapper.c -o wrapper.o -g -c
gcc -o light_task light_sensor.o wrapper.o -lrt -lpthread -lm

gcc -c temperature_sensor.c -o temperature_sensor.o -g -c
gcc -o temp_task temperature_sensor.o wrapper.o -lrt -lpthread

gcc logger_task.c -o logger_task -lrt -lpthread

gcc socket_task.c -o socket_task -lrt -lpthread

gcc external_app.c -o external_app

gcc main_task.c -o main_task -lrt -lpthread
