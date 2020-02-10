/** @file sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
#include "gio.h"
#include "het.h"
#include "FreeRTOS.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "fec.h"
#include "FreeRTOS.h"
#include "os_task.h"
/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */
#define RATE (1./2.)
#define MAXBYTES 8

double Gain = 32.0;
int Verbose = 0;
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
void vTask1(void *pvParameters){
    int f = 20;
    //NOTE: ONLY THE _char and _port PARTS OF THE FEC LIBRARY ARE INCLUDED
    //SO THAT CCS BUILDS IT PROPERLY

    int i,tr;
    int sr=0,trials = 10000,errcnt,framebits=2048;
    long long int tot_errs=0;
    unsigned char bits[MAXBYTES];
    unsigned char data[MAXBYTES];
    unsigned char xordata[MAXBYTES];
    unsigned char symbols[8*2*(MAXBYTES+6)];
    void *vp;
    extern char *optarg;
    //double extime;
    double gain,esn0,ebn0;
    //time_t t;
    int badframes=0;

    ebn0 = -100;
//  #if HAVE_GETOPT_LONG
//    while((d = getopt_long(argc,argv,"l:n:te:g:vapmst",Options,NULL)) != EOF){
//  #else
//    while((d = getopt(argc,argv,"l:n:te:g:vapmst")) != EOF){
//  #endif
//      switch(d){
//      case 'a':
//        Cpu_mode = ALTIVEC;
//        break;
//      case 'p':
//        Cpu_mode = PORT;
//        break;
//      case 'm':
//        Cpu_mode = MMX;
//        break;
//      case 's':
//        Cpu_mode = SSE;
//        break;
//      case 't':
//        Cpu_mode = SSE2;
//        break;
//      case 'l':
//        framebits = atoi(optarg);
//        break;
//      case 'n':
//        trials = atoi(optarg);
//        break;
//      case 'e':
//        ebn0 = atof(optarg);
//        break;
//      case 'g':
//        Gain = atof(optarg);
//        break;
//      case 'v':
//        Verbose++;
//        break;
//      }

    if(framebits > 8*MAXBYTES){
      fprintf(stderr,"Frame limited to %d bits\n",MAXBYTES*8);
      framebits = MAXBYTES*8;
    }
    if((vp = create_viterbi27(framebits)) == NULL){
      fprintf(stderr,"create_viterbi27 failed\n");
      exit(1);
    }
    if(ebn0 != -100){
      esn0 = ebn0 + 10*log10((double)RATE); /* Es/No in dB */
      /* Compute noise voltage. The 0.5 factor accounts for BPSK seeing
       * only half the noise power, and the sqrt() converts power to
       * voltage.
       */
      gain = 1./sqrt(0.5/pow(10.,esn0/10.));

      fprintf(stderr,"nframes = %d framesize = %d ebn0 = %.2f dB gain = %g\n",trials,framebits,ebn0,Gain);

      for(tr=0;tr<trials;tr++){
        /* Encode a frame of random data */
        for(i=0;i<framebits+6;i++){
      int bit = (i < framebits) ? (rand() & 1) : 0;

      sr = (sr << 1) | bit;
      bits[i/8] = sr & 0xff;
      symbols[2*i+0] = addnoise(parity(sr & V27POLYA),gain,Gain,127.5,255);
      symbols[2*i+1] = addnoise(parity(sr & V27POLYB),gain,Gain,127.5,255);
        }
        /* Decode it and make sure we get the right answer */
        /* Initialize Viterbi decoder */
        init_viterbi27(vp,0);

        /* Decode block */
        update_viterbi27_blk(vp,symbols,framebits+6);

        /* Do Viterbi chainback */
        chainback_viterbi27(vp,data,framebits,0);
        errcnt = 0;
        for(i=0;i<framebits/8;i++){
      int e = Bitcnt[xordata[i] = data[i] ^ bits[i]];
      errcnt += e;
      tot_errs += e;
        }
        if(errcnt != 0)
      badframes++;
        if(Verbose > 1 && errcnt != 0){
      fprintf(stderr,"frame %d, %d errors: ",tr,errcnt);
      for(i=0;i<framebits/8;i++){
        fprintf(stderr,"%02x",xordata[i]);
      }
      fprintf(stderr,"\n");
        }
        if(Verbose)
      fprintf(stderr,"BER %lld/%lld (%10.3g) FER %d/%d (%10.3g)\r",
             tot_errs,(long long)framebits*(tr+1),tot_errs/((double)framebits*(tr+1)),
             badframes,tr+1,(double)badframes/(tr+1));
        fflush(stdout);
      }
      if(Verbose > 1)
        fprintf(stderr,"nframes = %d framesize = %d ebn0 = %.2f dB gain = %g\n",trials,framebits,ebn0,Gain);
      else if(Verbose == 0)
        fprintf(stderr,"BER %lld/%lld (%.3g) FER %d/%d (%.3g)\n",
           tot_errs,(long long)framebits*trials,tot_errs/((double)framebits*trials),
           badframes,tr+1,(double)badframes/(tr+1));
      else
        printf("\n");

    } else {
      /* Do time trials */
      memset(symbols,127,sizeof(symbols));
      fprintf(stderr,"Starting time trials\n");
      //getrusage(RUSAGE_SELF,&start);
      for(tr=0;tr < trials;tr++){
        /* Initialize Viterbi decoder */
        init_viterbi27(vp,0);

        /* Decode block */
        update_viterbi27_blk(vp,symbols,framebits);

        /* Do Viterbi chainback */
        chainback_viterbi27(vp,data,framebits,0);
      }
      //getrusage(RUSAGE_SELF,&finish);
      //extime = finish.ru_utime.tv_sec - start.ru_utime.tv_sec + 1e-6*(finish.ru_utime.tv_usec - start.ru_utime.tv_usec);
//      fprintf(stderr,"Execution time for %d %d-bit frames: %.2f sec\n",trials,
//         framebits,extime);
//      fprintf(stderr,"decoder speed: %g bits/s\n",trials*framebits/extime);
    }
    while(1);
}
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */



    if(xTaskCreate( vTask1, "Task1", 1024, NULL, 1, NULL )!= pdTRUE)
    {
        /* Task could not be created */
        while(1);
    }

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    /*
    int temp, delay;
    delay = 0x200000;
    gioInit();
    gioSetDirection(gioPORTB, 0xFFFFFFFF);
    while (1) {
        gioToggleBit(gioPORTB, 1);
        for (temp = 0; temp < delay; temp++);
        gioToggleBit(gioPORTB, 1);
        for (temp = 0; temp < delay; temp++);

    }
    */
/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */

/* USER CODE END */
