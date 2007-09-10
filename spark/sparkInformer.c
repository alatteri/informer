/****************************************************************************
 *                                 Informer                                 *
 ****************************************************************************/
#include "spark.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define UI_NUM_ROWS 6

/*************************************
 * Informer notes data structure
 *************************************/
typedef struct {
    unsigned int    Id;                 /* Primary key of Note */
    char           User[255];           /* The user who created the note */
    char           Text[4096];          /* The actual note message */
    unsigned int   IsChecked;           /* Boolean: is the note checked? */
    char           DateAdded[100];      /* Date note was added */
    char           DateModified[100];   /* Last time modified */
} InformerNoteStruct;

/*************************************
 * Informer function prototypes
 *************************************/
static unsigned long *SayHello(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *SayHelloWaiting(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *SayHelloWaitingTo(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *SayHelloNoSh(int CallbackArg, SparkInfoStruct SparkInfo);

static unsigned long *NoteChanged(int CallbackArg, SparkInfoStruct SparkInfo);

const char *InformerGetSetupName(void);
const char *InformerGetGatewayPath(void);
int InformerGetNotes(void);

void InformerShowNoteRow(int row_num);
void InformerHideNoteRow(int row_num);
void InformerHideAllNoteRows(void);
void InformerToggleNoteRow(int row_num, int on_off);
void InformerRefreshNotesUI(void);
void InformerUpdateNotesRowUI(InformerNoteStruct source, int row_num);

static unsigned long *InformerNotesPagePrev(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesPageNext(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesPupChanged(int CallbackArg, SparkInfoStruct SparkInfo);

/* Informer Note Boolean CheckBoxes */
SparkBooleanStruct SparkBoolean7 =  { 0, "", NULL };
SparkBooleanStruct SparkBoolean8 =  { 0, "", NULL };
SparkBooleanStruct SparkBoolean9 =  { 0, "", NULL };
SparkBooleanStruct SparkBoolean10 = { 0, "", NULL };
SparkBooleanStruct SparkBoolean11 = { 0, "", NULL };
SparkBooleanStruct SparkBoolean12 = { 0, "", NULL };
SparkBooleanStruct *NoteBooleansUI[] = {&SparkBoolean7, &SparkBoolean8, &SparkBoolean9, &SparkBoolean10, &SparkBoolean11, &SparkBoolean12};

/* Informer Note Text fields */
SparkStringStruct SparkString14 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString15 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString16 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString17 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString18 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString19 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct *NoteTextUI[] = {&SparkString14, &SparkString15, &SparkString16, &SparkString17, &SparkString18, &SparkString19};

/* Informer From/Date fields */
SparkStringStruct SparkString28 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString29 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString30 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString31 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString32 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString33 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct *NoteFromUI[] = {&SparkString28, &SparkString29, &SparkString30, &SparkString31, &SparkString32, &SparkString33};

/* Informer Note Controls */
SparkStringStruct SparkString27 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkPushStruct SparkPush13 = { "<< Previous Page", InformerNotesPagePrev };
SparkPushStruct SparkPush20 = { "Next Page >>", InformerNotesPageNext };

SparkPupStruct SparkPup34 = {0, 2, InformerNotesPupChanged, {"Refresh Notes",
                                                             "Create New Note"}};

SparkStringStruct SparkString39 = { "", "", SPARK_FLAG_NO_INPUT, NULL };
/*
 * Informer Setup UI Elements
 * SparkStringStruct SparkSetupString15 = { "", "Hello.", NULL, NULL };
 */

/*
 * Aliases for Image Buffers
 */
static int RESULT_ID = 1;
static int FRONT_ID  = 2;

static SparkMemBufStruct SparkResult;
static SparkMemBufStruct SparkSource;

/*************************************
 * Informer globals
 *************************************/
unsigned int gNoteInit = 0;         /* Boolean: have the notes ever been checked? */
unsigned int gNotePage = 0;         /* The current page number */
unsigned int gNoteCount = 0;        /* The total number of notes */
InformerNoteStruct gNoteData[100];      /* The array of note data */

static char GATEWAY_STATUS_ERR[] = "Unable to get notes";
static char GET_NOTES_WAIT[] = "Getting notes from database...";

/****************************************************************************
 *                      Spark Base Function Calls                           *
 ****************************************************************************/

/*--------------------------------------------------------------------------*/
/* Called before initialising the Spark. Sparks defining                    */
/* SparkMemoryTempBuffers() will use the new memory buffer interface added  */
/* in the Spark API version 2.5                                             */
/*                                                                          */
/* Input clip buffers and the output buffers are registered by IFFF. The    */
/* output ID is 1 while input clips have ID's 2 to n inputs.                */
/*--------------------------------------------------------------------------*/
void SparkMemoryTempBuffers(void)
{
    /* no temporary buffers */
}

/*--------------------------------------------------------------------------*/
/* Called when the spark object is first loaded.                            */
/* Return value indicates this spark operates at the MODULE level.          */
/*--------------------------------------------------------------------------*/
unsigned int SparkInitialise(SparkInfoStruct spark_info)
{
    int i;
    char title[200];
    const char     *setup_name;
    setup_name = sparkGetLastSetupName();
    printf("----> SparkInitialise called <----\n");
    printf("[[[[ setup: %s ]]]]\n", setup_name);

    //AllNotes[1] = Note1;
    //AllNotes[2] = Note2;
    //sprintf(SparkString13.Value, "%s", Note3.Text );
    //sprintf(SparkString14.Value, "%s", Note4.Text );

    //printf("Note1.Text is: %s\n", AllNotes[1].Text);
    //printf("Note2.Text is: %s\n", AllNotes[2].Text);


    //sprintf(note_title, "Displaying %%d Note 1 to %d", 6);

    sparkControlTitle(SPARK_CONTROL_1, "Notes");
    sparkControlTitle(SPARK_CONTROL_2, "Elements");

    InformerHideAllNoteRows();

    return SPARK_MODULE;
}

/*--------------------------------------------------------------------------*/
/* Called when the spark is unloaded.                                       */
/*--------------------------------------------------------------------------*/
void SparkUnInitialise(SparkInfoStruct spark_info)
{
    printf("----> SparkUnInitialise called <----\n");
}

/*--------------------------------------------------------------------------*/
/* Returns the number of source clips that are required for the spark       */
/*--------------------------------------------------------------------------*/
int SparkClips(void)
{
    printf("----> SparkClips called <----\n");
    return 1;
}

/*--------------------------------------------------------------------------*/
/* This function is used to trigger the initial database query for the      */
/* current spark state.                                                     */
/* Return: indicates the number of frames to be generated for the current   */
/* input frame.                                                             */
/*--------------------------------------------------------------------------*/
int SparkProcessStart(SparkInfoStruct spark_info)
{
    const char *setup;
    const char *gateway;
    setup = InformerGetSetupName();
    gateway = InformerGetGatewayPath();
    printf("----> SparkProcessStart called: setup (%s) gateway (%s) <----\n",
           setup, gateway);
    InformerGetNotes();
    return 1;
}

/*--------------------------------------------------------------------------*/
/* The SparkProcess function performs the actual image manipulation by      */
/* simply copying the source image. The function returns the address of the */
/* buffer of processed data.                                                */
/*--------------------------------------------------------------------------*/
unsigned long *SparkProcess(SparkInfoStruct spark_info)
{
    printf("-----> SparkProcess called <------\n");

    if (sparkMemGetBuffer(FRONT_ID,  &SparkSource) == FALSE) return NULL;
    if (sparkMemGetBuffer(RESULT_ID, &SparkResult) == FALSE) return NULL;

    sparkCopyBuffer( SparkSource.Buffer, SparkResult.Buffer );
    return SparkResult.Buffer;
}

/*--------------------------------------------------------------------------*/
/* Callback to indicate various spark events have ocurred.                  */
/*--------------------------------------------------------------------------*/
void SparkEvent(SparkModuleEvent spark_event)
{
    /* I don't have a use for this right now... */
    printf("-----> SparkEvent (%d) <-----\n", spark_event);
}

/****************************************************************************
 *                      Informer Utility Calls                              *
 ****************************************************************************/

/*--------------------------------------------------------------------------*/
/* Returns the name of the current setup being worked on.                   */
/*--------------------------------------------------------------------------*/
const char *InformerGetSetupName(void)
{
    const char *setup;
    setup = sparkGetLastSetupName();
    return setup;
}

const char *InformerGetGatewayPath(void)
{
    char *home;
    char path[PATH_MAX];
    const char *const_path;

    home = getenv("HOME");
    sprintf(path, "%s/informer/bin/gateway", home);
    const_path = path;
    return const_path;
}

int InformerGetNotes(void)
{
    int pid = 0;
    int status = 0;
    int import_ok = 0;
    const char *setup;
    const char *gateway;
    const char *argv[8];

    sparkMessage(GET_NOTES_WAIT);
    InformerHideAllNoteRows();

    setup = InformerGetSetupName();
    gateway = InformerGetGatewayPath();

    printf("the gateway is [%s]\n", gateway);

    argv[0] = gateway;
    argv[1] = "-s";
    argv[2] = setup;
    argv[3] = "-a";
    argv[4] = "get_notes";
    argv[5] = "-o";
    argv[6] = "/tmp/trinity";
    argv[7] = 0;

    printf("argv[0] is [%s]\n", argv[0]);

    printf("About to make gateway call [%s %s %s %s %s %s %s]\n",
           argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);

    pid = sparkSystemNoSh(FALSE, argv[0], argv);
    sparkWaitpid(pid, &status, 0);
    printf("Gateway returned. status [%d]\n", status);

    if (0 != status) {
        sprintf(SparkString27.Value, "%s: status [%d]",
                GATEWAY_STATUS_ERR, status);
        return 0;
    } else {
        import_ok = InformerImportNotes("/tmp/trinity");
        if (1 == import_ok) {
            InformerRefreshNotesUI();
            return 1;
        } else {
            sprintf(SparkString27.Value, "%s", GET_NOTES_WAIT);
        }
    }
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int InformerImportNotes(char *filepath)
{
    FILE *fp;
    int i = 0;
    int result = 1;

    printf("here we go. reading datafile [%s]\n", filepath);

    if ((fp=fopen(filepath, "r")) == NULL) {
        sprintf(SparkString27.Value, "%s: can't open datafile [%s]",
                GATEWAY_STATUS_ERR, filepath);
        return 0;
    }

    /*
        read the data file:
        - first line is number of note objects (not lines...)
        - entries are of form
            key: value
    */
    result = fscanf(fp, "%d%*1[\n]", &gNoteCount);
    if (1 != result) {
        sprintf(SparkString27.Value, "%s: can't parse datafile [%s]",
                GATEWAY_STATUS_ERR, filepath);
        return 0;
    }

    printf("OK there are %d entries\n", gNoteCount);

    for (i=0; i<gNoteCount; i++) {
        /* skip the "key:" read the value */

        /* read the pk field -> Id */
        /* read the user field -> User */
        /* read the text field -> Text */
        /* read the is_checked field -> IsChecked */
        /* read the date_added field -> DateAdded */
        /* read the date_modified field -> DateModified */

        if ((fscanf(fp, "%*s %d%*1[\n]",     &gNoteData[i].Id) > 0) &&
            (fscanf(fp, "%*s %[^\n]%*1[\n]", &gNoteData[i].User) > 0) &&
            (fscanf(fp, "%*s %[^\n]%*1[\n]", &gNoteData[i].Text) > 0) &&
            (fscanf(fp, "%*s %d%*1[\n]",     &gNoteData[i].IsChecked) > 0) &&
            (fscanf(fp, "%*s %[^\n]%*1[\n]", &gNoteData[i].DateAdded) > 0) &&
            (fscanf(fp, "%*s %[^\n]%*1[\n]", &gNoteData[i].DateModified) > 0)) {
            /* looking good -- keep going */
            printf("... read note [%d] ok!\n", i);
        } else {
            sprintf(SparkString27.Value, "%s: can't parse datafile [%s]",
                    GATEWAY_STATUS_ERR, filepath);
            return 0;
        }
    }

    printf("All notes read ok. Word up.\n");

    return 1;
}

/****************************************************************************
 *                      Informer UI Function Calls                          *
 ****************************************************************************/
static unsigned long *InformerNotesPupChanged(int CallbackArg,
                                              SparkInfoStruct SparkInfo )
{
    printf("Hey! The pup change got called with: %d, value is: %d\n",
            CallbackArg, SparkPup34.Value);

    if (1 == CallbackArg) {
        InformerGetNotes();
    }

    return NULL;
}

static unsigned long *InformerNotesPagePrev(int CallbackArg,
                                            SparkInfoStruct SparkInfo )
{
    printf("You clicked Notes Page Prev\n");
    return NULL;
}

static unsigned long *InformerNotesPageNext(int CallbackArg,
                                            SparkInfoStruct SparkInfo )
{
    printf("You clicked Notes Page Next\n");
    return NULL;
}

void InformerHideAllNoteRows(void)
{
    int i;
    for (i=1; i<=UI_NUM_ROWS; i++) {
        InformerHideNoteRow(i);
    }
}

void InformerHideNoteRow(int row_num)
{
    printf("++++ call to disable note row %d\n", row_num);
    InformerToggleNoteRow(row_num, 0);
}

void InformerShowNoteRow(int row_num)
{
    printf("++++ call to enable note row %d\n", row_num);
    InformerToggleNoteRow(row_num, 1);
}

void InformerToggleNoteRow(int row_num, int on_off)
{
    void (*sparkToggle)(int, int) = NULL;

    if (1 <= row_num && UI_NUM_ROWS >= row_num) {
        if (0 == on_off) {
            sparkToggle = sparkDisableParameter;
        } else {
            sparkToggle = sparkEnableParameter;
        }

        /* The numbers below are the ui control column offsets */
        sparkToggle(SPARK_UI_CONTROL, row_num + 6);
        sparkToggle(SPARK_UI_CONTROL, row_num + 13);
        sparkToggle(SPARK_UI_CONTROL, row_num + 27);
    }
}

void InformerRefreshNotesUI(void)
{
    int i = 0;

    printf("****** Time to refresh the notes UI ********\n");
    sprintf(SparkString27.Value, "Displaying notes 1 - %d (of %d)",
            gNoteCount, gNoteCount);

    for (i=0; i<gNoteCount; i++)
    {
        InformerUpdateNotesRowUI(gNoteData[i], i+1);
    }
}

void InformerUpdateNotesRowUI(InformerNoteStruct source, int row_num)
{
    char checked[80];

    printf("Truing to update row UI with row_num [%d]\n", row_num);
    NoteBooleansUI[row_num-1]->Value = source.IsChecked;
    if (1 == source.IsChecked) {
        sprintf(checked, "Done. %s", source.DateModified);
        NoteBooleansUI[row_num-1]->Title = checked;
    } else {
        NoteBooleansUI[row_num-1]->Title = "TODO";
    }
    sprintf(NoteTextUI[row_num-1]->Value, "%s", source.Text);
    sprintf(NoteFromUI[row_num-1]->Value, "%s: %s", source.User, source.DateAdded);
    InformerShowNoteRow(row_num);
}

/*--------------------------------------------------------------------------*/
/*                      ------------ BULLSHIT ------------                  */
/*--------------------------------------------------------------------------*/
static unsigned long *SayHello(int CallbackArg, SparkInfoStruct SparkInfo)
{
    const char     *setup_name;
    int answer;
    sparkMessageDelay(2000, "Hello world");
    /*answer = sparkMessageConfirm("Hello world?");*/
    setup_name = sparkGetLastSetupName();
    printf("[[[[ setup: %s\n", setup_name);
    sparkSystemSh( FALSE, "THE_MSG=Hello; xmessage $THE_MSG");
    return NULL;
}

static unsigned long *SayHelloWaiting(int CallbackArg,
                                      SparkInfoStruct SparkInfo)
{
    sparkSystemSh( TRUE, "xmessage Hello");
    return NULL;
}

static unsigned long *SayHelloWaitingTo(int CallbackArg,
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

static unsigned long *SayHelloNoSh(int CallbackArg,
                                   SparkInfoStruct SparkInfo )
{
    const char *argv[3];
    argv[0] = "/bin/sh";
    argv[1] = "xmessage Hello";
    argv[2] = 0;
    sparkSystemNoSh( FALSE, argv[0], argv );
    return NULL;
}

static unsigned long *NoteChanged(int CallbackArg,
                                  SparkInfoStruct SparkInfo )
{
    //sprintf(note_title, "Displaying %%d Note 1 to %d", SparkInt34.Value);
    //printf("Hey! The note change got called with: %d, value is: %d\n", CallbackArg, SparkInt34.Value);
    return NULL;
}

