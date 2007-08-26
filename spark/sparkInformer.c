/**********************************************************************
                           SparkSh example
  
  
     Copyright (c) 2004 Autodesk Canada Inc. / Autodesk, Inc.
     All rights reserved.
                                                                   
     These coded instructions, statements, and computer programs     
     contain unpublished proprietary information written by Autodesk
     Canada, and are protected by Federal copyright law. They may not 
     be disclosed to third parties or copied or duplicated in any    
     form, in whole or in part, without the prior written consent of 
     Autodesk Canada.    
  
                         ----------------------
     
     SparkSh is a example of how to use the shell command function
     supply by the Spark API. By displaying a xmessage saying "Hello",
     It show examples of the use of  sparkSystemSh, sparkWaitpid and
     sparkSystemNoSh.  

 **********************************************************************/

/*************************************
 * Include files
 *************************************/
#include "spark.h"

/*************************************
 * Local function prototypes
 *************************************/

/*
 * Callback functions for user interface fields
 */
static unsigned long *SayHello( int CallbackArg, SparkInfoStruct SparkInfo );
static unsigned long *SayHelloWaiting( int CallbackArg, SparkInfoStruct SparkInfo );
static unsigned long *SayHelloWaitingTo( int CallbackArg, SparkInfoStruct SparkInfo );
static unsigned long *SayHelloNoSh( int CallbackArg, SparkInfoStruct SparkInfo );

/******************************
 * User Interface Definitions
 ******************************/

/*
 * Module Level
 */

SparkPushStruct SparkPush6 = { "Say all in", SayHello };
SparkPushStruct SparkPush7 = { "Say all in and wait", SayHelloWaiting };
SparkPushStruct SparkPush8 = { "Say all in and wait", SayHelloWaitingTo };
SparkPushStruct SparkPush9 = { "Say all in", SayHelloNoSh };
SparkStringStruct SparkString15 = { "",
				  "Last exit status: %s",
				  SPARK_FLAG_NO_INPUT,
				  NULL };

/*
 * Aliases for Image Buffers
 */
 
static int RESULT_ID = 1;
static int FRONT_ID  = 2;

static SparkMemBufStruct SparkResult;
static SparkMemBufStruct SparkSource;
    
/****************************************************************************
 * SPARK BASE FUNCTION CALLS
 ****************************************************************************/

/********************************************************************/
/* Called before initialising the Spark.                            */
/* Sparks defining SparkMemoryTempBuffers() will use the new memory */
/* buffer interface added in the Spark API version 2.5              */
/********************************************************************/
void	   SparkMemoryTempBuffers( void )
{
    /* Input clip buffers and the output buffers are registered */
    /* by IFFF. The output ID is 1 while input clips have ID's  */
    /* 2 to n inputs.                                           */

    /* no temporary buffers */
}



 /*
  * Called when we load a spark object.
  * Return value indicates that this spark can
  * operate at the MODULE level.
  */
unsigned int    SparkInitialise( SparkInfoStruct spark_info )
{
   char     *setup_name;
   setup_name = sparkGetLastSetupName();
   printf("---- SparkInitialise called ----\n");
   printf("[[[[ setup: %s\n", setup_name);
   sparkDisableParameter(SPARK_UI_CONTROL, 15);

   return ( SPARK_MODULE );
}

 /*
  * Called when we close a spark object
  */
void            SparkUnInitialise( SparkInfoStruct spark_info )
{
   printf("---- SparkUnInitialise called ----\n");
}

 /*
  * This routine returns a value of 1 as the Spark will
  * generate an output frame based on one input clip
  */
int             SparkClips( void )
{
 
   return ( 1 );
}

/****************************************************************************
 * SPARK PROCESSING FUNCTIONS
 ****************************************************************************/

 /*
  * Function returns the number of output frames to be
  * generated per input frame. It is called once for each
  * input frame, before Processing actually begins.
  */
int             SparkProcessStart( SparkInfoStruct spark_info )
{
   return ( 1 );
}

 /*
  * The SparkProcess function performs the actual image
  * manipulation by simply copying the source image.
  * The function returns the address of the buffer of
  * processed data.
  */
unsigned long  *SparkProcess( SparkInfoStruct spark_info )
{
    if ( sparkMemGetBuffer(FRONT_ID,  &SparkSource) == FALSE ) return NULL;
    if ( sparkMemGetBuffer(RESULT_ID, &SparkResult) == FALSE ) return NULL;

    sparkCopyBuffer( SparkSource.Buffer, SparkResult.Buffer );

    return SparkResult.Buffer;
}

/*******************************
 * SPARK CALLBACK FUNCTIONS
 *******************************/
static unsigned long *SayHello( int CallbackArg,
				SparkInfoStruct SparkInfo )
{
  char     *setup_name;
  setup_name = sparkGetLastSetupName();
  printf("[[[[ setup: %s\n", setup_name);
  sparkSystemSh( FALSE, "THE_MSG=Hello; xmessage $THE_MSG");
  return NULL;
}

static unsigned long *SayHelloWaiting( int CallbackArg,
				       SparkInfoStruct SparkInfo )
{
  sparkSystemSh( TRUE, "xmessage Hello");
  return NULL;
}

static unsigned long *SayHelloWaitingTo( int CallbackArg,
					 SparkInfoStruct SparkInfo )
{
  int pid = sparkSystemSh( FALSE, "xmessage Hello");
  int status = 0;
  sparkWaitpid( pid, &status, 0 );
  sprintf( SparkString15.Value, "%d", status );
  sparkEnableParameter(SPARK_UI_CONTROL, 15);

  return NULL;
}

static unsigned long *SayHelloNoSh( int CallbackArg,
				    SparkInfoStruct SparkInfo )
{
  const char *argv[3];
  argv[0] = "/bin/sh";
  argv[1] = "xmessage Hello";
  argv[2] = 0;
  sparkSystemNoSh( FALSE, argv[0], argv );
  return NULL;
}
