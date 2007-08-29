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

static unsigned long *NoteChanged( int CallbackArg, SparkInfoStruct SparkInfo );
static unsigned long *PupChanged( int CallbackArg, SparkInfoStruct SparkInfo );

/*********************************
 * Informer Notes Data structure
 *********************************/
typedef struct {
    unsigned int    Id;             /* Primary key of Note */
    char           *User;           /* The user who created the note */
    char           *Text;           /* The actual note message */
    int             IsChecked;      /* Boolean: is the note checked? */
    char           *DateAdded;      /* Date note was added */
    char           *DateModified;   /* Last time modified */
} InformerNoteStruct;

InformerNoteStruct Note1 = {1, "Alan", "This is note 1", 0, "1/4/07 4:20PM", "2/1/07 4:20PM"};
InformerNoteStruct Note2 = {2, "Anna", "This is note 2", 0, "2/4/07 4:20PM", "2/2/07 4:20PM"};
InformerNoteStruct Note3 = {3, "Josh", "This is note 3", 0, "3/4/07 4:20PM", "2/3/07 4:20PM"};
InformerNoteStruct Note4 = {4, "Luna", "This is note 4", 0, "4/4/07 4:20PM", "2/4/07 4:20PM"};
InformerNoteStruct Note5 = {5, "Joey", "This is note 5", 0, "5/4/07 4:20PM", "2/5/07 4:20PM"};
InformerNoteStruct Note6 = {6, "Meow", "This is note 6", 0, "6/4/07 4:20PM", "2/6/07 4:20PM"};

InformerNoteStruct AllNotes[6];
unsigned int NoteCount = 6;

/*******************************
 * ------ Informer NOTES -------
 *******************************/

/*
 * Informer Note Boolean CheckBoxes
 */
SparkBooleanStruct SparkBoolean6 =  { 0, "", NULL };
SparkBooleanStruct SparkBoolean7 =  { 0, "", NULL };
SparkBooleanStruct SparkBoolean8 =  { 0, "", NULL };
SparkBooleanStruct SparkBoolean9 =  { 0, "", NULL };
SparkBooleanStruct SparkBoolean10 = { 0, "", NULL };
SparkBooleanStruct SparkBoolean11 = { 0, "", NULL };

/*
 * Informer Note Text fields
 */
SparkStringStruct SparkString13 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString14 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString15 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString16 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString17 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString18 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };

/*
 * Informer From/Date fields
 */
SparkStringStruct SparkString27 = { "", "", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString28 = { "", "", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString29 = { "", "", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString30 = { "", "", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString31 = { "", "", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString32 = { "", "", SPARK_FLAG_NO_INPUT, NULL };

/*
 * Informer Note Controls
 */
SparkPushStruct SparkPush19 = { "<< Previous Page", SayHelloNoSh };
SparkPushStruct SparkPush26 = { "Next Page >>", SayHelloNoSh };
SparkStringStruct SparkString33 = { "",
				  "Notes 1 - 6 (of 240): Page 1 of 4",
				  SPARK_FLAG_NO_INPUT,
				  NULL };

SparkPupStruct SparkPup34 = {0, 2, PupChanged, {"Refresh Notes", "Create New Note"}};
char note_title[200] = "%d";
/* SparkIntStruct SparkInt34 = {0, 0, 10, 1, NULL, note_title, NoteChanged}; */


/*
 * Informer Setup UI Elements
SparkStringStruct SparkSetupString15 = { "",
				  "Hello from setup.",
				  NULL,
				  NULL };
 */

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
    char title[200];
    char     *setup_name;
    setup_name = sparkGetLastSetupName();
    printf("---- SparkInitialise called ----\n");
    printf("[[[[ setup: %s\n", setup_name);


    AllNotes[1] = Note1;
    AllNotes[2] = Note2;
    //sprintf(SparkString13.Value, "%s", Note3.Text );
    //sprintf(SparkString14.Value, "%s", Note4.Text );

    printf("Note1.Text is: %s\n", AllNotes[1].Text);
    printf("Note2.Text is: %s\n", AllNotes[2].Text);


    sprintf(note_title, "Displaying %%d Note 1 to %d", 6);

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
   int answer;
   sparkMessageDelay(2000, "Hello world");
   /*answer = sparkMessageConfirm("Hello world?");*/
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
    /* 
  sprintf( SparkString15.Value, "%d", status );
  sparkEnableParameter(SPARK_UI_CONTROL, 15);
    */

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

static unsigned long *NoteChanged( int CallbackArg,
				    SparkInfoStruct SparkInfo )
{
    //sprintf(note_title, "Displaying %%d Note 1 to %d", SparkInt34.Value);
    //printf("Hey! The note change got called with: %d, value is: %d\n", CallbackArg, SparkInt34.Value);
    return NULL;
}

static unsigned long *PupChanged( int CallbackArg,
				    SparkInfoStruct SparkInfo )
{
    printf("Hey! The pup change got called with: %d, value is: %d\n", CallbackArg, SparkPup34.Value);
    return NULL;
}
