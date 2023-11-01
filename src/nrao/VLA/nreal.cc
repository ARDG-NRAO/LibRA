//# nreal.cc:  this defines VLA nearly realtime routines for read VLA data
//# Copyright (C) 1997,1999,2000
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: nreal.cc,v 19.1.34.5 2006/02/17 23:46:40 wyoung Exp $

//# Includes

/*
   _olroutines - Routines for fetching data from the current VLA disk file

   _olopen  - opens the on online status record
   _olread  - returns a data from the shared memory
   _olclose - close out the online status record

Dec 1992  Modified by George Martin to remove timeout while waiting for data

Dec 1992 Switched to _ol for AIPS compatibility

Feb 1993 Added changes made by Wes Young and a little other cleanup 

Jun 1993 Changed "sleep()" to sleep1() for fractions of a second 

Mar 1994 Begin changes to get rid of "slots", add printing of
         internet addresses instead of slot numbers 

Sep 1994 Changed routines to wait only when last_record == last_record
         avoids alot of hanging problems.  Also olopen_ forks aoc_clnt
         if it's aips.  Probably don't want this for miranda.


This is some real yucky code I wrote a long time ago but as it more or less
works, I'm reluctant to change it too much
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include <nrao/VLA/nreal.h>
#include <iostream>


/*int MAX_TIME = 60; */
int QUIT = 0;
int ONLINE = 0;
#define LEAP_SECONDS 33

static int Last_Record;

void  ThatsAllFolks(int);
void  sleep1(double);

static FILE *fpVisData = 0;
static FILE *timeData = 0;
static FILE *hostLog = 0;
static char logDataFile[131];
// int   fdVisData;
static long  lastMJAD = 0;
static long  lastTick = 0;
static long  MJAD = 0;
static off_t lastOffset;
static int oldDay =0;
static int previousDay = 0;
static char *tomorrowsFile = 0;

static const char *visDataDir = "/home/vis-serv-mirror/vladata/";

char *getTodaysFile(char *dataDir, char *aDate, int relDay){
int LeapSeconds = LEAP_SECONDS;
  time_t curTime, holdTime;          /* time vars */
  struct tm *tmTime;                 /* time structure */
   if(!aDate)
      aDate = (char *)calloc(1, 81);
   curTime = time(&holdTime) + LeapSeconds + relDay*86400;
   tmTime = gmtime(&curTime);
   sprintf(aDate, "%s/vla%04d-%02d-%02d.dat",dataDir, tmTime->tm_year+1900,
                         tmTime->tm_mon+1, tmTime->tm_mday);
   return(aDate);
}

char *getVisDir(){
   char *dataDir = 0;
   dataDir = getenv("VISDATADIR");
   if(!dataDir){
       dataDir = strdup(visDataDir);
   }
   return(dataDir);
}

int whatsToday(){
  time_t LeapSeconds = LEAP_SECONDS;
  time_t curTime, holdTime;          /* time vars */
  struct tm *tmTime;                 /* time structure */
  curTime = time(&holdTime) + LeapSeconds;
   tmTime = gmtime(&curTime);
   return tmTime->tm_yday;
}

int oladvf_(int* /*unit_no*/, int *files){
  char *visDataFile = 0;
  int rstatus = 0;

  if(!*files)
     rewind(fpVisData);
  else if(*files < 0){
      /* Reset the file name to the previous days data */

      if(fpVisData)
         fclose(fpVisData);
      visDataFile = getTodaysFile(getVisDir(), visDataFile, *files);
      fpVisData = fopen(visDataFile, "r");
      ONLINE = 1;
      previousDay = *files;
      oldDay += *files;
      if(fpVisData == NULL){
         rstatus = -1;
         printf("Unable to open file %s, sorry\n", visDataFile);
         printf("Historical data is not kept on-line very long, please try to get it from"
                 " the VLA archive.\n");
      }
  } else {
      rstatus = fseeko(fpVisData, 0, SEEK_END); 
  }
  return rstatus;
}
int olseek(off_t offset){
   int r_status = 0;
   if(fpVisData != NULL){
       if(offset < 0){
          r_status = fseeko(fpVisData, 0, SEEK_END); 
       }else {
          r_status = fseeko(fpVisData, offset, SEEK_SET);
       }
   }
   return r_status;
}
void attachFile(const char *filename)
{
   fpVisData = fopen(filename, "r");
   ONLINE=false;
   QUIT=false;
   //std::cerr << "Attaching to " << filename << std::endl;
}
void detachFile(){
	fclose(fpVisData);
        fpVisData = 0;
}
int olopen_(int *unit_no, char *filename, int /*charCount*/)
{
   char *visDataFile = 0;
   char iAm[81];
   signal(SIGINT, ThatsAllFolks);
   signal(SIGUSR1, ThatsAllFolks);
   signal(SIGUSR2, ThatsAllFolks);
   signal(SIGTERM, ThatsAllFolks);
   signal(SIGHUP, ThatsAllFolks);
   if(!strcasecmp(filename, "online")){
      ONLINE = 1;
      tomorrowsFile = getTodaysFile(getVisDir(), tomorrowsFile, 1);
      visDataFile = getTodaysFile(getVisDir(), visDataFile, 0);
      timeData = fopen("/home/vis-serv-mirror/vladata/lastdata", "r");
      if(timeData){
#ifdef __APPLE__
         fscanf(timeData, "%ld %ld %lld", &lastMJAD, &lastTick, &lastOffset);
#else
         fscanf(timeData, "%ld %ld %zd", &lastMJAD, &lastTick, &lastOffset);
#endif
         MJAD = lastMJAD;
      }
      gethostname(iAm, 81);
      sprintf(logDataFile, "%s/connected/%s.%ld", visDataDir, iAm, 
              static_cast<long>(getpid()));
      hostLog = fopen(logDataFile, "w");
      
   }else{
      visDataFile = filename;
      ONLINE = 0;
      if(fpVisData)
         fclose(fpVisData);
   }
   // printf ("Data file is %s\n", visDataFile);
   oldDay = whatsToday();


   if (*unit_no == -99){  /* called by aips */
      *unit_no = -1;
   } else if (*unit_no == 99) { /* non aips call */
      *unit_no = 1;
   } else { /* illegal call */
      *unit_no = 0;
      return (*unit_no);
   }
   fpVisData = fopen(visDataFile, "r");
   if(fpVisData){
      if(ONLINE){
         if(lastOffset > 0){
            fseeko(fpVisData, lastOffset, SEEK_SET);
	 }else{
            fseeko(fpVisData, 0, SEEK_END);
            printf("On-line disk opened");
         }
      }
   } else {
   // fdVisData = fileno(fpVisData);
      *unit_no = 0;
         printf("ERROR: On-line disk opened FAILED!");
   }

   /*free(tmTime);*/
   /*free(visDataFile);*/

   return(*unit_no);
}
/*
   Function to terminate Data stream
*/
void ThatsAllFolks(int /*theSignal*/){
   QUIT = 1;
   return;
}
void reattachCurrent(){
   char *visDataFile = 0;
   if(previousDay < 0) previousDay++;
   else if(previousDay > 0) previousDay--;
   visDataFile = getTodaysFile(getVisDir(), visDataFile, previousDay);
   if(fpVisData)
      fclose(fpVisData);
   fpVisData = fopen(visDataFile, "r");
   while(fpVisData == NULL){
       sleep1(5.0);
       visDataFile = getTodaysFile(getVisDir(), visDataFile, previousDay);
       fpVisData = fopen(visDataFile, "r");
   }
   free(visDataFile);
}

int diskread_(int */*unit_no*/, char *buffer)
{

   static int FirstRecord = 1;
   static int NumPhysRecords;
   static int PhysRecord;
   static int LastSize;

   long LRSize;
   int RecordSize = PHYS_RECORD_SIZE;
   struct stat;
   int curDay;
   size_t itemsGot = 0;
   static int feedback = 0;

/*
   Ok here we go, first see if we have to reopen the current data file.

   Need to make sure we're at the end of the file.
*/

   Last_Record++;
   /*free(tmTime);*/
   if(timeData){
      rewind(timeData);
#ifdef __APPLE__
      fscanf(timeData, "%ld %ld %lld", &MJAD, &lastTick, &lastOffset);
#else
      fscanf(timeData, "%ld %ld %zd", &MJAD, &lastTick, &lastOffset);
#endif
   }
   if(hostLog){
      rewind(hostLog);
#ifdef __APPLE__
      fprintf(hostLog, "%ld %ld %lld\n", MJAD, lastTick, lastOffset);
#else
      fprintf(hostLog, "%ld %ld %zd\n", MJAD, lastTick, lastOffset);
#endif
   }
   /* Now we check that we're at the end of file and try again after a bit */
   /* Double check there is enough data to read */
   /* Get the visibility data from the file */
   if(FirstRecord){
      if(feedback)
         printf("Fetching 0\n");
      PhysRecord = 0;
      itemsGot = fread(buffer, 2048, 1, fpVisData);
      // printf("itemsGot %d %d item size = 2048\n", itemsGot, ftell(fpVisData));
      if(!itemsGot || feof(fpVisData) || ferror(fpVisData)){
         clearerr(fpVisData);
         if(!ONLINE){
	    QUIT=1;
	    FirstRecord = 1;
	 } else{
            if(!feedback)
               printf("Fetching 1\n");
	    // feedback = 1;
	 }
         while(ONLINE && !QUIT){
            itemsGot = fread(buffer, 2048, 1, fpVisData);
            if(itemsGot){
	       break;
            }
            /* If it is the next day and eof close out and reattach */
            if(feof(fpVisData)){
               curDay = whatsToday();
               if(oldDay != curDay){
                  reattachCurrent();
	          if(feedback)
	             printf(".Next File found\n");
	          // feedback = 0;
                  oldDay +=1;
               }
               clearerr(fpVisData);
            }
	    sleep1(10.0);  /* should decode integration time and use it */
         }
      }
      if(QUIT){
	      FirstRecord = 1;
	      return(0);
      }
      memcpy((void *)&LRSize, buffer+sizeof(TapeHeader), sizeof(LRSize));
      LRSize = 2*ntohl(LRSize);
      // printf ("LRSize %d\n", LRSize);
      NumPhysRecords = LRSize/(PHYS_RECORD_SIZE-4);
      if(LRSize%(PHYS_RECORD_SIZE-4)){
         NumPhysRecords += 1;
         LastSize = 2048*((LRSize%(PHYS_RECORD_SIZE-4))/2048);
         if((LRSize%(PHYS_RECORD_SIZE-4))%2048)
             LastSize += 2048;  /* Ignore the Tape header */
      } else {
          LastSize = PHYS_RECORD_SIZE-4;
      }
      FirstRecord = 0;
   }
   if(feedback)
      printf(".\n");
   PhysRecord++;
   // std::cout << "PhysRecord, NumPhysRecords : " << PhysRecord << ", " << NumPhysRecords << std::endl;
   // std::cout << "RecordSize : " << RecordSize << std::endl;
   if(PhysRecord == NumPhysRecords){
         RecordSize = LastSize;
         FirstRecord = 1;
         if(feedback)
            printf("Fetched\n");
   }
   if(PhysRecord == 1){
       if (RecordSize > 2048) {
           // finish getting this record
           itemsGot = fread(buffer+2048, RecordSize-2048, 1, fpVisData);
           // std::cout << "A: itemsGot : " << itemsGot << " item size = " << (RecordSize-2048) << " feof(fpVisData) : " << feof(fpVisData) << " ferror(fpVisData) " << ferror(fpVisData) << std::endl;
       }
   } else {
      itemsGot = fread(buffer, RecordSize, 1, fpVisData);
      // std::cout << "B: itemsGot : " << itemsGot << " item size = " << RecordSize << " feof(fpVisData) : " << feof(fpVisData) << " ferror(fpVisData) " << ferror(fpVisData) << std::endl;
   }
   if(!itemsGot || feof(fpVisData) || ferror(fpVisData))
      QUIT= 1;
   if(QUIT){
	   FirstRecord =1;
	   return(0);}
   
   return(RecordSize);
}

int readVLALogRec(char *data){
   int dum(99);
   TapeHeader tapeHead;
   char  *physrec = new char[PHYS_RECORD_SIZE];    //Allocate the physical Record

   int bytesRead = diskread_(&dum,  physrec);  // Read first physical record
   // std::cout << "bytesRead : " << bytesRead << std::endl;

   memcpy(&tapeHead, physrec, sizeof(TapeHeader)); //Get the "tape" information

        //Calculate the Logical Record Size
   long LRSize ;
   memcpy((void *)&LRSize, physrec+sizeof(TapeHeader), sizeof(LRSize));
   LRSize = 2*ntohl(LRSize);
   // std::cout << "LRSize : " << LRSize << std::endl;
        // Now shift the bits from the physical to Logical Record
   int BeePtr(0);
   int NumPhysRecs(ntohs(tapeHead.total));
   int en = sizeof(TapeHeader);
   // std::cout << "NumPhysRecs : " << NumPhysRecs << std::endl;
   // std::cout << "en : " << en << std::endl;

   if (ntohs(tapeHead.current) == 1) {
         memcpy(data+BeePtr, physrec+en, PHYS_RECORD_SIZE-en);
         BeePtr = PHYS_RECORD_SIZE - en;
   }
   if((LRSize < MAX_LOGICAL_RECORD_SIZE) && (ntohs(tapeHead.current) <= ntohs(tapeHead.total))){
      for (int i=1; i<NumPhysRecs && !QUIT ; i++) {
         bytesRead += diskread_(&dum,  physrec);
         memcpy(data+BeePtr, physrec+en, PHYS_RECORD_SIZE - en);
         BeePtr += (PHYS_RECORD_SIZE - en);
      }
   } else {
       // std::cout << "QUIT=1 : " << std::endl;
      QUIT = 1;
   }

   delete [] physrec;
   if(QUIT)
	   LRSize = 0;
   return LRSize;
}



int olread_(int *unit_no, char *buffer, int /*buff_len*/)
{
    return diskread_(unit_no, buffer);
}

int olclose_(int */*unit_no*/)
{
   fclose(fpVisData);
   if(timeData)
      fclose(timeData);
   if(hostLog)
      fclose(hostLog);
   unlink(logDataFile);
   return 0;
}

/* Implement a "sleep()" function which also suspends for fractions of
   a second.  Following a GNU implementation of usleep(), this routine
   calls select() with NULL pointers of the I/O descriptor sets to 
   do the actual delay.  This is potentially better than sleep() which
   only does integer seconds and usleep() which only does micro seconds */

#include <sys/time.h>
#include <sys/types.h>
#ifdef R6000
#include <sys/select.h>
#endif

void sleep1(double sleep_time) {

   struct timeval wait;
   double wait_mu;
   int ready;

   if ( (sleep_time <= 0.0) || (sleep_time > 1.0e8) )
      return;

                /* convert floating seconds to integer
                   seconds and micro seconds */

   wait_mu = sleep_time*1000000.0;    /* secs to micro secs */

   wait.tv_sec = (long int) (wait_mu/1000000.0);
   wait.tv_usec = (long int) (wait_mu - ( (double) wait.tv_sec*1000000.0));

   ready = select(1, (fd_set *) NULL, (fd_set *) NULL, (fd_set *) NULL,  &wait);   if (ready < 0) {
      perror ("error on calling setitimer");
      return;
      }
   return;
   }

