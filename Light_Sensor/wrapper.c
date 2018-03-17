/******************************************************************************
* Author:       Pavan Dhareshwar & Sridhar Pavithrapu
* Date:         03/07/2018
* File:         wrapper.c
* Description:  Source file describing the functionality and implementation
*               of wrapper for synchronization of light and temperature tasks.
******************************************************************************/

/*---------------------------------- INCLUDES -------------------------------*/
#include "wrapper.h"

sem_t *get_named_semaphore_handle(void)
{
    sem_t *sem;
    if ((sem = sem_open("wrapper_sem", O_CREAT, 0644, 1)) == SEM_FAILED)              
    {
        perror("sem_open failed");
        return SEM_FAILED;
    }
    else                                                                                              
    {
        //printf("Named semaphore created successfully\n");
        return sem;
    }   
}

ssize_t wrapper_write(int fd, void *buf, size_t count){
	
	ssize_t return_value = 0;

#if 1
    sem_t *wrapper_sem = get_named_semaphore_handle();
    if (wrapper_sem == SEM_FAILED)
    {
        return -1000;
    }

	if(sem_wait(wrapper_sem) == 0)
    { 
		 return_value = write(fd, buf, count);
    }    
	else{
		perror("sem_wait error in wrapper\n");
	}
	
	if(sem_post(wrapper_sem) != 0){
	
		perror("sem_post error in wrapper\n");
	}
#else
    return_value = write(fd, buf, count);
#endif
	return return_value;
}

ssize_t wrapper_read(int fd, void *buf, size_t count){
	
	ssize_t return_value = 0;

#if 1
    sem_t *wrapper_sem = get_named_semaphore_handle();
    if (wrapper_sem == SEM_FAILED)
    {
        return -1000;
    }
	
    if(sem_wait(wrapper_sem) == 0){ 
	
		 return_value = read(fd, buf, count);
    }    
	else{
		perror("sem_wait error in wrapper\n");
	}
	
	if(sem_post(wrapper_sem) != 0){
	
		perror("sem_post error in wrapper\n");
	}
#else
    return_value = read(fd, buf, count);
#endif

	return return_value;
}
