#ifndef _WRAPPER_H_
#define _WRAPPER_H_

#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

sem_t *get_named_semaphore_handle(void);

ssize_t wrapper_write(int fd, void *buf, size_t count);
ssize_t wrapper_read(int fd, void *buf, size_t count);

#endif
