//
// Created by wc on 2021/12/19.
//

#ifndef MYSEMOP_H
#define MYSEMOP_H
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <fcntl.h>
/* 信号量管理函数 */
int CreateSem(int value);
int SetSemValue(int sem_id,int value);
void DeleteSem(int sem_id);

int Psem(int sem_id);   /* P原语 */
int Vsem(int sem_id);  /* V原语 */
#endif

