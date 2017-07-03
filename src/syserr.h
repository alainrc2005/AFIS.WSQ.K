/*
 * syserr.h
 *
 *  Created on: Sep 13, 2009
 *      Author: alainrc2005
 */

#ifndef SYSERR_H_
#define SYSERR_H_

#include <stdio.h>
#include <stdlib.h>

void syserr(char *funcname, char *syscall, char *msg);
void fatalerr(char *s1, char *s2, char *s3);

#endif /* SYSERR_H_ */
