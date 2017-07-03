/*
 * fet.c
 *
 *  Created on: Sep 13, 2009
 *      Author: alainrc2005
 */

#include "fet.h"
#include "string.h"
#include "defs.h"

/********************************************************************/
FET *allocfet(int numfeatures)
{
   FET *fet;

   fet = (FET *)malloc(sizeof(FET));
   if (fet == (FET *)NULL)
      syserr("allocfet","malloc","fet");
   /* calloc here is required */
   fet->names = (char **)calloc(numfeatures, sizeof(char *));
   if (fet->names == (char **)NULL)
      syserr("allocfet","calloc","fet->names");
   fet->values = (char **)calloc(numfeatures, sizeof(char *));
   if (fet->values == (char **)NULL)
      syserr("allocfet","calloc","fet->values");
   fet->alloc = numfeatures;
   fet->num = 0;
   return(fet);
}

/********************************************************************/
int allocfet_ret(FET **ofet, int numfeatures)
{
   FET *fet;

   fet = (FET *)malloc(sizeof(FET));
   if (fet == (FET *)NULL){
      fprintf(stderr, "ERROR : allocfet_ret : malloc : fet\n");
      return(-2);
   }
   /* calloc here is required */
   fet->names = (char **)calloc(numfeatures, sizeof(char *));
   if (fet->names == (char **)NULL){
      fprintf(stderr, "ERROR : allocfet_ret : calloc : fet->names\n");
      free(fet);
      return(-3);
   }
   fet->values = (char **)calloc(numfeatures, sizeof(char *));
   if (fet->values == (char **)NULL){
      fprintf(stderr, "ERROR : allocfet_ret : calloc : fet->values\n");
      free(fet->names);
      free(fet);
      return(-4);
   }
   fet->alloc = numfeatures;
   fet->num = 0;

   *ofet = fet;

   return(0);
}

/********************************************************************/
FET *reallocfet(FET *fet, int newlen)
{
   if (fet == (FET *)NULL || fet->alloc == 0)
      return(allocfet(newlen));

   fet->names = (char **)realloc(fet->names, newlen * sizeof(char *));
   if (fet->names == (char **)NULL)
      fatalerr("reallocfet", "realloc", "space for increased fet->names");
   fet->values = (char **)realloc(fet->values, newlen * sizeof(char *));
   if (fet->values == (char **)NULL)
      fatalerr("reallocfet", "realloc", "space for increased fet->values");
   fet->alloc = newlen;

   return(fet);
}

/********************************************************************/
int reallocfet_ret(FET **ofet, int newlen)
{
   int ret;
   FET *fet;

   fet = *ofet;

   /* If fet not allocated ... */
   if ((fet == (FET *)NULL || fet->alloc == 0)){
      /* Allocate the fet. */
      if((ret = allocfet_ret(ofet, newlen)))
         /* Return error code. */
         return(ret);
      /* Otherwise allocation was successful. */
      return(0);
   }

   /* Oherwise, reallocate fet. */
   fet->names = (char **)realloc(fet->names, newlen * sizeof(char *));
   if (fet->names == (char **)NULL){
      fprintf(stderr, "ERROR : reallocfet_ret : realloc : fet->names\n");
      return(-2);
   }
   fet->values = (char **)realloc(fet->values, newlen * sizeof(char *));
   if (fet->values == (char **)NULL){
      fprintf(stderr, "ERROR : reallocfet_ret : realloc : fet->values");
      return(-3);
   }
   fet->alloc = newlen;

   return(0);
}

/*********************************************************************/
void deletefet(char *feature, FET *fet)
{
  int item;

  for (item = 0;
       (item < fet->num) && (strcmp(fet->names[item],feature) != 0);
       item++);
  if(item >= fet->num)
     fatalerr("deletefet",feature,"Feature not found");
  free(fet->names[item]);
  if(fet->values[item] != (char *)NULL)
     free(fet->values[item]);
  for (++item;item<fet->num;item++){
      fet->names[item-1] = fet->names[item];
      fet->values[item-1] = fet->values[item];
  }
  fet->names[fet->num-1] = '\0';
  fet->values[fet->num-1] = '\0';
  (fet->num)--;
}

/*********************************************************************/
int deletefet_ret(char *feature, FET *fet)
{
  int item;

  for (item = 0;
       (item < fet->num) && (strcmp(fet->names[item],feature) != 0);
       item++);
  if(item >= fet->num){
    fprintf(stderr, "ERROR : deletefet_ret : feature %s not found\n",
            feature);
     return(-2);
  }
  free(fet->names[item]);
  if(fet->values[item] != (char *)NULL)
     free(fet->values[item]);
  for (++item;item<fet->num;item++){
      fet->names[item-1] = fet->names[item];
      fet->values[item-1] = fet->values[item];
  }
  fet->names[fet->num-1] = '\0';
  fet->values[fet->num-1] = '\0';
  (fet->num)--;

  return(0);
}

/*******************************************************************/
char *extractfet(char *feature, FET *fet)
{
  int item;
  char *value;

  for (item = 0;
       (item < fet->num) && (strcmp(fet->names[item],feature) != 0);
       item++);
  if (item>=fet->num)
     fatalerr("extractfet",feature,"not found");
  if(fet->values[item] != (char *)NULL){
      value = strdup(fet->values[item]);
      if (value == (char *)NULL)
         syserr("extractfet","strdup","value");
  }
  else
      value = (char *)NULL;
  return(value);
}

/*******************************************************************/
int extractfet_ret(char **ovalue, char *feature, FET *fet)
{
  int item;
  char *value;

  for (item = 0;
       (item < fet->num) && (strcmp(fet->names[item],feature) != 0);
       item++);
  if (item>=fet->num){
     fprintf(stderr, "ERROR : extractfet_ret : feature %s not found\n",
             feature);
     return(-2);
  }
  if(fet->values[item] != (char *)NULL){
      value = (char *)strdup(fet->values[item]);
      if (value == (char *)NULL){
         fprintf(stderr, "ERROR : extractfet_ret : strdup : value\n");
         return(-3);
     }
  }
  else
      value = (char *)NULL;

  *ovalue = value;

  return(0);
}

void freefet(FET *fet)
{
  int item;
  for (item=0;item<fet->num;item++){
      free (fet->names[item]);
      free (fet->values[item]);
  }
  free((char *)fet->names);
  free((char *)fet->values);
  free(fet);
}

/*******************************************************************/
int lookupfet(char **ovalue, char *feature, FET *fet)
{
  int item;
  char *value;

  for (item = 0;
       (item < fet->num) && (strcmp(fet->names[item],feature) != 0);
       item++);
  if (item>=fet->num){
     return(FALSE);
  }
  if(fet->values[item] != (char *)NULL){
      value = strdup(fet->values[item]);
      if (value == (char *)NULL){
         fprintf(stderr, "ERROR : lookupfet : strdup : value\n");
         return(-2);
     }
  }
  else
      value = (char *)NULL;

  *ovalue = value;

  return(TRUE);
}

void printfet(FILE *fp, FET *fet)
{
  int item;

  for (item = 0; item<fet->num; item++){
       if(fet->values[item] == (char *)NULL)
          fprintf(fp,"%s\n",fet->names[item]);
       else
          fprintf(fp,"%s %s\n",fet->names[item],fet->values[item]);
  }
}

/*****************************************************************/
int fet2string(char **ostr, FET *fet)
{
   int i, size;
   char *str;

   /* Calculate size of string. */
   size = 0;
   for(i = 0; i < fet->num; i++){
      size += strlen(fet->names[i]);
      size += strlen(fet->values[i]);
      size += 2;
   }
   /* Make room for NULL for final strlen() below. */
   size++;

   if((str = (char *)calloc(size, sizeof(char))) == (char *)NULL){
      fprintf(stderr, "ERROR : fet2string : malloc : str\n");
      return(-2);
   }

   for(i = 0; i < fet->num; i++){
      strcat(str, fet->names[i]);
      strcat(str, " ");
      strcat(str, fet->values[i]);
      strcat(str, "\n");
   }

   str[strlen(str)-1] = '\0';

   *ostr = str;
   return(0);
}

/*****************************************************************/
int string2fet(FET **ofet, char *istr)
{
   int ret;
   char *iptr, *optr;
   char name[MAXFETLENGTH], value[MAXFETLENGTH], *vptr;
   FET *fet;

   if((ret = allocfet_ret(&fet, MAXFETS)))
      return(ret);

   iptr = istr;
   while(*iptr != '\0'){
      /* Get next name */
      optr = name;
      while((*iptr != '\0')&&(*iptr != ' ')&&(*iptr != '\t'))
         *optr++ = *iptr++;
      *optr = '\0';

      /* Skip white space */
      while((*iptr != '\0')&&
            ((*iptr == ' ')||(*iptr == '\t')))
         iptr++;

      /* Get next value */
      optr = value;
      while((*iptr != '\0')&&(*iptr != '\n'))
         *optr++ = *iptr++;
      *optr = '\0';

      /* Skip white space */
      while((*iptr != '\0')&&
            ((*iptr == ' ')||(*iptr == '\t')||(*iptr == '\n')))
         iptr++;

      /* Test (name,value) pair */
      if(strlen(name) == 0){
         fprintf(stderr, "ERROR : string2fet : empty name string found\n");
         return(-2);
      }
      if(strlen(value) == 0)
         vptr = (char *)NULL;
      else
         vptr = value;

      /* Store name and value pair into FET. */
      if((ret = updatefet_ret(name, vptr, fet))){
         freefet(fet);
         return(ret);
      }
   }

   *ofet = fet;
   return(0);
}

/***********************************************************************/
void updatefet(char *feature, char *value, FET *fet)
{
  int item;
  int increased, incr;

  for (item = 0;
       (item < fet->num) && (strcmp(fet->names[item],feature) != 0);
       item++);
  if (item < fet->num){
     if(fet->values[item] != (char *)NULL){
        free(fet->values[item]);
        fet->values[item] = (char *)NULL;
     }
     if(value != (char *)NULL){
        fet->values[item] = strdup(value);
        if(fet->values[item] == (char *)NULL)
           syserr("updatefet","strdup","fet->values[]");
     }
  }
  else{
     if(fet->num >= fet->alloc){
        incr      = fet->alloc / 10;		/* add 10% or 10 which-	*/
        increased = fet->alloc + max(10, incr);	/* ever is larger	*/
        reallocfet(fet, increased);
     }
     fet->names[fet->num] = (char *)strdup(feature);
     if(fet->names[fet->num] == (char *)NULL)
        syserr("updatefet","strdup","fet->names[]");
     if(value != (char *)NULL){
        fet->values[fet->num] = (char *)strdup(value);
        if(fet->values[fet->num] == (char *)NULL)
           syserr("updatefet","strdup","fet->values[]");
     }
     (fet->num)++;
  }
}

/***********************************************************************/
int updatefet_ret(char *feature, char *value, FET *fet)
{
  int ret, item;
  int increased, incr;

  for (item = 0;
       (item < fet->num) && (strcmp(fet->names[item],feature) != 0);
       item++);
  if (item < fet->num){
     if(fet->values[item] != (char *)NULL){
        free(fet->values[item]);
        fet->values[item] = (char *)NULL;
     }
     if(value != (char *)NULL){
        fet->values[item] = (char *)strdup(value);
        if(fet->values[item] == (char *)NULL){
           fprintf(stderr, "ERROR : updatefet_ret : strdup : fet->values[]\n");
           return(-2);
        }
     }
  }
  else{
     if(fet->num >= fet->alloc){
        incr      = fet->alloc / 10;		/* add 10% or 10 which-	*/
        increased = fet->alloc + max(10, incr);	/* ever is larger	*/
        if((ret = reallocfet_ret(&fet, increased)))
           return(ret);
     }
     fet->names[fet->num] = (char *)strdup(feature);
     if(fet->names[fet->num] == (char *)NULL){
        fprintf(stderr, "ERROR : updatefet_ret : strdup : fet->names[]\n");
        return(-3);
     }
     if(value != (char *)NULL){
        fet->values[fet->num] = (char *)strdup(value);
        if(fet->values[fet->num] == (char *)NULL){
           fprintf(stderr, "ERROR : updatefet_ret : strdup : fet->values[]\n");
           return(-4);
        }
     }
     (fet->num)++;
  }

  return(0);
}
