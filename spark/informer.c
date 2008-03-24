/****************************************************************************
 *                                 Informer                                 *
 ****************************************************************************/
#include <Python.h>
#include "informer.h"
#include "gateway.h"
#include "rgb.h"
#include "spark.h"
#include "python.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <pwd.h>

extern int errno;
extern char **environ;

static char GET_NOTES_WAIT[] = "Getting notes from database...";
static char GET_ELEMS_WAIT[] = "Getting elements from database...";
static char UPDATE_NOTE_WAIT[] = "Updating database...";
static char UPDATE_ELEM_WAIT[] = "Updating database...";
static char CREATE_NOTE_WAIT[] = "Creating new note...";
static char CURRENT_USER_ERR[] = "Unable to determine user";
static char CREATE_NOTE_UI[] = "                  click here to create a new note";


/*************************************
 * Notes UI
 *************************************/
SparkCanvasStruct SparkCanvas1 = { InformerTableCanvasDraw, InformerTableCanvasInteract };

/* Informer Note Boolean CheckBoxes */
SparkBooleanStruct SparkBoolean8 =  { 0, "", InformerTableRow1BoolChanged };
SparkBooleanStruct SparkBoolean9 =  { 0, "", InformerTableRow2BoolChanged };
SparkBooleanStruct SparkBoolean10 = { 0, "", InformerTableRow3BoolChanged };
SparkBooleanStruct SparkBoolean11 = { 0, "", InformerTableRow4BoolChanged };
SparkBooleanStruct SparkBoolean12 = { 0, "", InformerTableRow5BoolChanged };

/* Informer Note Text fields */
SparkStringStruct SparkString15 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString16 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString17 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString18 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString19 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };

/* Informer From/Date fields */
SparkStringStruct SparkString29 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString30 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString31 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString32 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString33 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };

/* Informer Note Controls */
SparkPupStruct SparkPup7 = {0, 5, InformerNotesSortChanged, {"Sort by Date Created",
                                                             "Sort by Date Modified",
                                                             "Sort by Status",
                                                             "Sort by Artist",
                                                             "Sort by Author"}};

SparkStringStruct SparkString28 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkPushStruct SparkPush14 = { "<< Previous Page", InformerTableButtonA };
SparkPushStruct SparkPush21 = { "Next Page >>", InformerTableButtonB };

SparkPupStruct SparkPup34 = {0, 1, InformerNotesModeChanged, {"Refresh Notes"}};

SparkStringStruct SparkString13 = { "", CREATE_NOTE_UI, SPARK_FLAG_NONE,
                                    InformerNotesCreateNoteCallback };


/*************************************
 * Elements UI
 *************************************/
SparkCanvasStruct SparkCanvas2 = { InformerTableCanvasDraw, InformerTableCanvasInteract };

SparkPupStruct SparkPup36 = {0, 4, InformerElemsSortChanged, {"Sort by Date",
                                                              "Sort by Status",
                                                              "Sort by Kind",
                                                              "Sort by Author"}};

SparkStringStruct SparkString57 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkPushStruct SparkPush43 = { "<< Previous Page", InformerTableButtonA };
SparkPushStruct SparkPush50 = { "Next Page >>", InformerTableButtonB };

SparkPupStruct SparkPup63 = {0, 1, InformerElemsModeChanged, {"Refresh Elements"}};

/* Informer Element Boolean CheckBoxes */
SparkBooleanStruct SparkBoolean37 = { 0, "", InformerTableRow1BoolChanged };
SparkBooleanStruct SparkBoolean38 = { 0, "", InformerTableRow2BoolChanged };
SparkBooleanStruct SparkBoolean39 = { 0, "", InformerTableRow3BoolChanged };
SparkBooleanStruct SparkBoolean40 = { 0, "", InformerTableRow4BoolChanged };
SparkBooleanStruct SparkBoolean41 = { 0, "", InformerTableRow5BoolChanged };

/* Informer Element Text fields */
SparkStringStruct SparkString44 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString45 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString46 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString47 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString48 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };

/* Informer Element From/Date fields */
SparkStringStruct SparkString58 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString59 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString60 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString61 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString62 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };


/*************************************
 * Setup UI
 *************************************/
SparkStringStruct SparkSetupString15 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkSetupString16 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };


/*************************************
 * Informer globals
 *************************************/
InformerAppStruct    gApp;


/****************************************************************************
 *                      Spark Base Function Calls                           *
 ****************************************************************************/

/*
 * Aliases for Image Buffers
 */
static int RESULT_ID = 1;
static int FRONT_ID  = 2;
static int STAGE_ID;
static int RESIZE_ID;

static SparkMemBufStruct SparkResult;
static SparkMemBufStruct SparkSource;
static SparkMemBufStruct SparkStage;
static SparkMemBufStruct SparkResize;

/*--------------------------------------------------------------------------*/
/* Called before initialising the Spark. Sparks defining                    */
/* SparkMemoryTempBuffers() will use the new memory buffer interface added  */
/* in the Spark API version 2.5                                             */
/*                                                                          */
/* Input clip buffers and the output buffers are registered by IFFF. The    */
/* output ID is 1 while input clips have ID's 2 to n inputs.                */
/*--------------------------------------------------------------------------*/
void
SparkMemoryTempBuffers(void)
{
    /* one temp buffer for generating rgb[a] image */
    STAGE_ID = sparkMemRegisterBuffer();
    RESIZE_ID = sparkMemRegisterBuffer();
}

/*--------------------------------------------------------------------------*/
/* Called when the spark object is first loaded.                            */
/* Return value indicates this spark operates at the MODULE level.          */
/*--------------------------------------------------------------------------*/
unsigned int
SparkInitialise(SparkInfoStruct spark_info)
{
    int retval;
    char cmd[100];
    struct passwd *ef;
    pid_t pid;

    double rate;
    char *spark_name;
    char **env = environ;
    char *daemon = "/usr/discreet/sparks/instinctual/informer/bin/informerd";

    InformerAppStruct *app = InformerGetApp();
    InformerDEBUG("----> SparkInitialise called <----\n");

    InformerDEBUG("^^^^^^ The program is: %s ^^^^^^^^^^^\n", sparkProgramGetName());

    while (env != NULL && *env != NULL) {
        // InformerDEBUG("ENV: %s\n", *env);
        env++;
    }

    InformerSetAppState(app, INFORMER_APP_STATE_OK);

    /* ------ SETUP ------ */
    app->setup_ui_setup_path = InformerCreateStringUI(15, &SparkSetupString15, "");
    app->setup_ui_spark_name = InformerCreateStringUI(16, &SparkSetupString16, "");

    /* ------ NOTES DATA ------ */
    app->notes_data_count = 0;
    app->notes_data_been_read = FALSE;

    /* ------ NOTES UI ------ */
    app->notes_ui_mode = InformerCreatePupUI(34, &SparkPup34);
    app->notes_ui_create = InformerCreateStringUI(13, &SparkString13, "");

    app->notes_ui_table.cur_page = 1;
    app->notes_ui_table.row_count = UI_NUM_ROWS;

    app->notes_ui_table.sort = InformerCreatePupUI(7, &SparkPup7);
    app->notes_ui_table.button_a  = InformerCreatePushUI(14, &SparkPush14);
    app->notes_ui_table.button_b  = InformerCreatePushUI(21, &SparkPush21);
    app->notes_ui_table.statusbar = InformerCreateStringUI(28, &SparkString28,
                                                           "No notes to display");

    app->notes_ui_table.row[0].status = InformerCreateBooleanUI(8, &SparkBoolean8, "");
    app->notes_ui_table.row[1].status = InformerCreateBooleanUI(9, &SparkBoolean9, "");
    app->notes_ui_table.row[2].status = InformerCreateBooleanUI(10, &SparkBoolean10, "");
    app->notes_ui_table.row[3].status = InformerCreateBooleanUI(11, &SparkBoolean11, "");
    app->notes_ui_table.row[4].status = InformerCreateBooleanUI(12, &SparkBoolean12, "");

    app->notes_ui_table.row[0].message = InformerCreateStringUI(15, &SparkString15, "");
    app->notes_ui_table.row[1].message = InformerCreateStringUI(16, &SparkString16, "");
    app->notes_ui_table.row[2].message = InformerCreateStringUI(17, &SparkString17, "");
    app->notes_ui_table.row[3].message = InformerCreateStringUI(18, &SparkString18, "");
    app->notes_ui_table.row[4].message = InformerCreateStringUI(19, &SparkString19, "");

    app->notes_ui_table.row[0].from = InformerCreateStringUI(29, &SparkString29, "");
    app->notes_ui_table.row[1].from = InformerCreateStringUI(30, &SparkString30, "");
    app->notes_ui_table.row[2].from = InformerCreateStringUI(31, &SparkString31, "");
    app->notes_ui_table.row[3].from = InformerCreateStringUI(32, &SparkString32, "");
    app->notes_ui_table.row[4].from = InformerCreateStringUI(33, &SparkString33, "");

    /* ------ ELEMENTS DATA ------ */
    app->elems_data_count = 0;
    app->elems_data_been_read = FALSE;

    /* ------ ELEMENTS UI ------ */
    app->elems_ui_mode = InformerCreatePupUI(63, &SparkPup63);

    app->elems_ui_table.cur_page = 1;
    app->elems_ui_table.row_count = UI_NUM_ROWS;

    app->elems_ui_table.sort = InformerCreatePupUI(36, &SparkPup36);
    app->elems_ui_table.button_a  = InformerCreatePushUI(43, &SparkPush43);
    app->elems_ui_table.button_b  = InformerCreatePushUI(50, &SparkPush50);
    app->elems_ui_table.statusbar = InformerCreateStringUI(57, &SparkString57,
                                                           "No elements to display");

    app->elems_ui_table.row[0].status = InformerCreateBooleanUI(37, &SparkBoolean37, "");
    app->elems_ui_table.row[1].status = InformerCreateBooleanUI(38, &SparkBoolean38, "");
    app->elems_ui_table.row[2].status = InformerCreateBooleanUI(39, &SparkBoolean39, "");
    app->elems_ui_table.row[3].status = InformerCreateBooleanUI(40, &SparkBoolean40, "");
    app->elems_ui_table.row[4].status = InformerCreateBooleanUI(41, &SparkBoolean41, "");

    app->elems_ui_table.row[0].message = InformerCreateStringUI(44, &SparkString44, "");
    app->elems_ui_table.row[1].message = InformerCreateStringUI(45, &SparkString45, "");
    app->elems_ui_table.row[2].message = InformerCreateStringUI(46, &SparkString46, "");
    app->elems_ui_table.row[3].message = InformerCreateStringUI(47, &SparkString47, "");
    app->elems_ui_table.row[4].message = InformerCreateStringUI(48, &SparkString48, "");

    app->elems_ui_table.row[0].from = InformerCreateStringUI(58, &SparkString58, "");
    app->elems_ui_table.row[1].from = InformerCreateStringUI(59, &SparkString59, "");
    app->elems_ui_table.row[2].from = InformerCreateStringUI(60, &SparkString60, "");
    app->elems_ui_table.row[3].from = InformerCreateStringUI(61, &SparkString61, "");
    app->elems_ui_table.row[4].from = InformerCreateStringUI(62, &SparkString62, "");

    sparkControlTitle(SPARK_CONTROL_1, "Notes");
    sparkControlTitle(SPARK_CONTROL_2, "Elements");

    InformerSetAppMode(app, INFORMER_APP_MODE_NOTES);
    InformerTableHideAllRows();

    printf("OK before python, the spark is %s\n", InformerGetSparkName());

    PythonInitialize();

    rate = sparkFrameRate();
    InformerDEBUG("This is the frame rate [%f]\n", rate);
    GatewaySetFrameRate(rate);

    spark_name = GatewaySparkRegister(InformerGetSparkName());
    strncpy(app->spark_last_name, spark_name, 256);

    printf("OK after python, the spark is %s\n", InformerGetSparkName());

    printf("---- TRYING TO KICK OFF THE FOOBAR! -------\n");
    pid = getpid();
    printf("------ the current pid is: %d\n", pid);

    ef = getpwuid(geteuid());
    printf("------ the effective user is: %s\n", ef->pw_name);

    sprintf(cmd, "%s start %s %d", daemon, ef->pw_name, pid);
    InformerDEBUG("Going to run: %s\n", cmd);
    retval = sparkSystemSh(TRUE, cmd);
    printf("----- FOOBAR IS RUNNING retval(%d) ------\n", retval);

    return SPARK_MODULE;
}

/*--------------------------------------------------------------------------*/
/* Called when the spark is unloaded.                                       */
/*--------------------------------------------------------------------------*/
void
SparkUnInitialise(SparkInfoStruct spark_info)
{
    InformerAppStruct *app = InformerGetApp();
    InformerDEBUG("----> SparkUnInitialise called <----\n");
    PythonExit();
}

/*--------------------------------------------------------------------------*/
/* Returns the number of source clips that are required for the spark       */
/*--------------------------------------------------------------------------*/
int
SparkClips(void)
{
    InformerDEBUG("----> SparkClips called <----\n");
    return 1;
}

void _DumpSparkInfo(SparkInfoStruct spark_info)
{
    InformerDEBUG("&&&&&&&&&& _DumpSparkInfo &&&&&&&&&&&&\n");
    InformerDEBUG("Name: %s\n", spark_info.Name);
    InformerDEBUG("Mode: %d\n", spark_info.Mode);
    InformerDEBUG("Context: %d\n", spark_info.Context);
    InformerDEBUG("&&&&&&&&&& _DumpSparkInfo &&&&&&&&&&&&\n");
}

/*--------------------------------------------------------------------------*/
/* This function is used to trigger the initial database query for the      */
/* current spark state.                                                     */
/* Return: indicates the number of frames to be generated for the current   */
/* input frame.                                                             */
/*--------------------------------------------------------------------------*/
int
SparkProcessStart(SparkInfoStruct spark_info)
{
    const char *spark_name;
    InformerAppStruct *app = InformerGetApp();

    InformerDEBUG("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    InformerDEBUG("------------------------> SparkProcessStart called <----------------------------\n");
    InformerDEBUG("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");

    spark_name = InformerGetSparkName();
    GatewaySparkProcessStart(spark_name);

    if (FALSE == app->notes_data_been_read) {
            InformerGetNotes();
    } else {
        InformerDEBUG("Not getting notes. notes have been read.\n");
    }

    return 1;
}

unsigned long *
SparkAnalyse(SparkInfoStruct spark_info)
{
    InformerDEBUG("----> SparkAnalyse called <-----\n");
    return NULL;
}

/*--------------------------------------------------------------------------*/
/* The SparkProcess function performs the actual image manipulation by      */
/* simply copying the source image. The function returns the address of the */
/* buffer of processed data.                                                */
/*--------------------------------------------------------------------------*/
unsigned long *
SparkProcess(SparkInfoStruct spark_info)
{
    char *path;
    const char *spark_name;

    if (sparkMemGetBuffer(FRONT_ID,  &SparkSource) == FALSE) return NULL;
    if (sparkMemGetBuffer(RESULT_ID, &SparkResult) == FALSE) return NULL;
    if (sparkMemGetBuffer(STAGE_ID, &SparkStage) == FALSE) return NULL;
    if (sparkMemGetBuffer(RESIZE_ID, &SparkResize) == FALSE) return NULL;

    sparkCopyBuffer(SparkSource.Buffer, SparkResult.Buffer);
    spark_name = InformerGetSparkName();

    InformerDEBUG("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    InformerDEBUG("---------------------------> SparkProcess called <------------------------------\n");
    InformerDEBUG("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");

    InformerDEBUG("   mode is [%d]\n", spark_info.Mode);
    InformerDEBUG("   context is [%d]\n", spark_info.Context);

    path = GatewaySparkProcessFrameStart(spark_name, spark_info);

    // InformerDEBUG("-----> PATH: %s\n", path);
    // InformerDEBUG("          (SparkInfo)\n");

    InformerDEBUG("------ THE NAME IS: %s ---------\n", spark_info.Name);
    InformerDEBUG("*** frame %u/%u ***\n", spark_info.FrameNo + 1, spark_info.TotalFrameNo);

    // InformerDEBUG("*** width: %d, height: %d, depth: %d\n",
    //               spark_info.FrameWidth, spark_info.FrameHeight, spark_info.FrameDepth);
    // InformerDEBUG("*** TotalClips: %d, FramePixels: %d, FrameBytes: %d\n",
    //               spark_info.TotalClips, spark_info.FramePixels, spark_info.FrameBytes);

    // InformerDEBUG("          (SparkSource)\n");
    // InformerDEBUG("*** total buffers (%d) ***\n", SparkSource.TotalBuffers);
    // InformerDEBUG("*** buffer state (%d) ***\n", SparkSource.BufState);
    // InformerDEBUG("*** buffer size (%d) ***\n", SparkSource.BufSize);
    // InformerDEBUG("*** buffer format (%d) ***\n", SparkSource.BufFmt);

    // InformerDEBUG("*** buffer width (%d) ***\n", SparkSource.BufWidth);
    // InformerDEBUG("*** buffer height (%d) ***\n", SparkSource.BufHeight);
    // InformerDEBUG("*** buffer depth (%d) ***\n", SparkSource.BufDepth);
    // InformerDEBUG("*** buffer stride (%d) ***\n", SparkSource.Stride);
    // InformerDEBUG("*** buffer inc (%d) ***\n", SparkSource.Inc);

    if (NULL == path) {
        InformerDEBUG("------> GatewayProcess said not to save this\n");
    } else {
        RgbWriteSparkFrame(path, SparkResult, SparkStage, SparkResize, spark_info);

        // printf("The size of a ptr is p[%d]\n", sizeof(ulong));
        // printf("The number of pixels in a frame: %d\n", spark_info.FramePixels);
        // printf("The number of pixels in the buffer: %d\n", SparkResult.BufSize);

        GatewaySparkProcessFrameEnd(spark_name);
    }

    return SparkResult.Buffer;
}

void
SparkProcessEnd(SparkInfoStruct spark_info)
{
    const char *spark_name;

    InformerDEBUG("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
    InformerDEBUG("-------------------------> SparkProcessEnd called <-----------------------------\n");
    InformerDEBUG("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");

    spark_name = InformerGetSparkName();
    InformerDEBUG("now calling GatewaySparkProcessEnd(%s)\n", spark_name);
    GatewaySparkProcessEnd(spark_name);
}

/*--------------------------------------------------------------------------*/
/* Callback to indicate various spark events have ocurred.                  */
/*--------------------------------------------------------------------------*/
void
SparkEvent(SparkModuleEvent spark_event)
{
    InformerAppStruct *app = InformerGetApp();

    InformerDEBUG("################################################################################\n");
    InformerDEBUG("-----> SparkEvent (%d) <-----\n", spark_event);
    InformerDEBUG("################################################################################\n");

    if (SPARK_EVENT_SETUP == spark_event) {
        InformerSetAppMode(app, INFORMER_APP_MODE_SETUP);
    } else if (SPARK_EVENT_CONTROL1 == spark_event) {
        InformerSetAppMode(app, INFORMER_APP_MODE_NOTES);

        if (FALSE == app->notes_data_been_read) {
            InformerGetNotes();
        }
    } else if (SPARK_EVENT_CONTROL2 == spark_event) {
        InformerSetAppMode(app, INFORMER_APP_MODE_ELEMS);

        if (FALSE == app->elems_data_been_read) {
            InformerGetElems();
        }
    }
}

// void SparkIdle(void)
// {
//     InformerDEBUG("################################################################################\n");
//     InformerDEBUG("    [[[[[[[[[   SparkIdle called    ]]]]]]]]]\n");
//     InformerDEBUG("################################################################################\n");
// }

void SparkFrameChange(SparkInfoStruct SparkInfo)
{
    InformerDEBUG("################################################################################\n");
    InformerDEBUG("    [[[[[[[[[   SparkFrameChange called    ]]]]]]]]]\n");
    InformerDEBUG("################################################################################\n");
}

/*--------------------------------------------------------------------------*/
/* Callback for setup saving and loading                                    */
/*--------------------------------------------------------------------------*/
void
SparkSetupIOEvent(SparkModuleEvent event, char *path, char *name)
{
    InformerAppStruct *app = InformerGetApp();

    InformerDEBUG("################################################################################\n");
    InformerDEBUG("SparkSetupIOEvent called with [%d] path [%s] name [%s]\n", event, path, name);
    InformerDEBUG("################################################################################\n");

    if (SPARK_EVENT_LOADSETUP == event || SPARK_EVENT_SAVESETUP == event) {
        GatewaySparkRename(app->spark_last_name, name);

        if (NULL == strstr(path, "/_session") && 0 != strncmp(path, "/tmp", 4)) {
            /* ignore auto load/saves */
            InformerDEBUG("LOADING/SAVING SETUP: %s\n", path);
            InformerSetSetupPath(path);
        } else {
            InformerDEBUG("IGNORING LOAD/SAVE OF SETUP: %s\n", path);
        }

        InformerSetSparkName(name);
        strncpy(app->spark_last_name, name, 256);
    }
}

unsigned long *SparkInteract( SparkInfoStruct SparkInfo,
                              int SX, int SY, float Pressure,
                              float VX, float VY, float VZ )
{
    // InformerDEBUG("SparkInteract event called SX: %d, SY: %d, VX: %f, VY: %f, pressure %f ---\n", SX, SY, VX, VY, Pressure);
    return NULL;
}

void SparkOverlay(SparkInfoStruct SparkInfo, float zoom_factor)
{
    // InformerDEBUG("------- OVERLAY CALLED -------\n");
}


/****************************************************************************
 *                      Informer Utility Calls                              *
 ****************************************************************************/
void
InformerDEBUG(const char *format, ...)
{
    va_list args;
    char str[4096];

    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);

    fprintf(stderr, "DEBUG: %s", str);
}

void
InformerERROR(const char *format, ...)
{
    va_list args;
    char str[4096];
    InformerAppStruct *app = InformerGetApp();

    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);

    fprintf(stderr, "ERROR: %s", str);
    InformerSetAppState(app, INFORMER_APP_STATE_ERR);
    sparkError(str);
}

InformerNoteData
InformerCreateNoteData(void)
{
    InformerNoteData result;
    result.id = DATA_NONE;
    result.is_checked = DATA_NONE;
    sprintf(result.text, "");
    sprintf(result.created_by, "");
    result.created_on = DATA_NONE;
    sprintf(result.modified_by, "");
    result.modified_on = DATA_NONE;
    return result;
}

InformerElemData
InformerCreateElemData(void)
{
    InformerElemData result;
    result.id = DATA_NONE;
    sprintf(result.kind, "");
    sprintf(result.text, "");
    result.is_checked = DATA_NONE;
    sprintf(result.created_by, "");
    result.created_on = DATA_NONE;
    return result;
}

InformerPupUI
InformerCreatePupUI(unsigned int id, SparkPupStruct *ui)
{
    InformerPupUI result;
    result.id = id;
    result.ui = ui;
    return result;
}

InformerPushUI
InformerCreatePushUI(unsigned int id, SparkPushStruct *ui)
{
    InformerPushUI result;
    result.id = id;
    result.ui = ui;
    return result;
}

InformerStringUI
InformerCreateStringUI(unsigned int id, SparkStringStruct *ui, char *text)
{
    InformerStringUI result;
    result.id = id;
    result.ui = ui;
    sprintf(result.ui->Value, "%s", text);
    return result;
}

InformerBooleanUI
InformerCreateBooleanUI(unsigned int id, SparkBooleanStruct *ui, char *text)
{
    InformerBooleanUI result;
    result.id = id;
    result.ui = ui;
    sprintf(result.text, "%s", text);
    return result;
}

void
InformerSetAppState(InformerAppStruct *app, InformerAppStateChoice state)
{
    char *msg;

    printf("+++ about to set the app state\n");

    if (INFORMER_APP_STATE_OK == state)
        msg = "INFORMER_APP_STATE_OK";
    else if (INFORMER_APP_STATE_ERR == state)
        msg = "INFORMER_APP_STATE_ERR";
    else
        msg = "UNKNOWN APP STATE";

    InformerDEBUG("(((( now setting app state to: %s ))))\n", msg);
    app->app_state = state;

    printf("+++ done\n");
}

int
InformerIsAppStateOK(InformerAppStruct *app)
{
    if (INFORMER_APP_STATE_OK == app->app_state)
        return TRUE;
    else
        return FALSE;
}

/****************************************************************************
 *                      Informer UI Function Calls                          *
 ****************************************************************************/

int
InformerGetNotes(void)
{
    int result;
    InformerAppStruct *app = InformerGetApp();

    if (TRUE != InformerIsAppStateOK(app)) {
        InformerDEBUG("Not calling GetNotes: app state is not OK\n");
        return FALSE;
    }

    sparkMessage(GET_NOTES_WAIT);
    result = GatewayGetNotes(app->notes_data);

    InformerTableHideAllRows();

    if (result < 0) {
        // TODO: What should happen here?
        InformerERROR("Unable to get notes from the database\n");
        app->notes_data_count = 0;
        app->notes_data_been_read = FALSE;
        app->notes_ui_table.cur_page = 1;
        return FALSE;
    }

    InformerDEBUG("All notes read ok. Word up.\n");

    app->notes_data_count = result;
    app->notes_data_been_read = TRUE;
    app->notes_ui_table.cur_page = 1;

    InformerNoteDataSort();
    InformerTableRefreshUI();

    return TRUE;
}

int
InformerGetElems(void)
{
    int result;
    InformerAppStruct *app = InformerGetApp();

    if (TRUE != InformerIsAppStateOK(app)) {
        InformerDEBUG("Not calling GetElems: app state is not OK\n");
        return FALSE;
    }

    sparkMessage(GET_ELEMS_WAIT);
    result = GatewayGetElems(app->elems_data);

    InformerTableHideAllRows();

    if (result < 0) {
        // TODO: What should happen here?
        InformerERROR("Unable to get elements from the database\n");
        app->elems_data_count = 0;
        app->elems_data_been_read = FALSE;
        app->elems_ui_table.cur_page = 1;
        return FALSE;
    }

    InformerDEBUG("All elements read ok. Word up.\n");

    app->elems_data_count = result;
    app->elems_data_been_read = TRUE;
    app->elems_ui_table.cur_page = 1;

    InformerElemDataSort();
    InformerTableRefreshUI();

    return TRUE;
}

int
InformerUpdateNote(int index, int id, int is_checked)
{
    InformerNoteData *data;
    InformerAppStruct *app = InformerGetApp();

    data = &(app->notes_data[index]);
    sparkMessage(UPDATE_NOTE_WAIT);

    if (TRUE == GatewayUpdateNote(data, id, is_checked)) {
        InformerTableRefreshUI();
        return TRUE;
    } else {
        // TODO: what to do here exactly?
        InformerERROR("Unable to update note\n");
        return FALSE;
    }
}

int
InformerUpdateElem(int index, int id, int is_checked)
{
    InformerElemData *data;
    InformerAppStruct *app = InformerGetApp();

    data = &(app->elems_data[index]);
    sparkMessage(UPDATE_ELEM_WAIT);

    if (TRUE == GatewayUpdateElem(data, id, is_checked)) {
        InformerTableRefreshUI();
        return TRUE;
    } else {
        // TODO: what to do here exactly?
        InformerERROR("Unable to update element\n");
        return FALSE;
    }
}

InformerNoteModeChoice
InformerGetNoteUIMode(void)
{
    InformerAppStruct *app = InformerGetApp();
    return (InformerNoteModeChoice) app->notes_ui_mode.ui->Value;
}

InformerElemModeChoice
InformerGetElemUIMode(void)
{
    InformerAppStruct *app = InformerGetApp();
    return (InformerElemModeChoice) app->elems_ui_mode.ui->Value;
}

static unsigned long *
InformerNotesModeChanged(int CallbackArg, SparkInfoStruct SparkInfo)
{
    InformerAppStruct *app = InformerGetApp();
    InformerNoteModeChoice mode = InformerGetNoteUIMode();

    InformerDEBUG("Hey! The notes mode change got called with: value: %d\n", mode);

    if (INFORMER_NOTE_MODE_REFRESH == mode) {
        InformerSetAppState(app, INFORMER_APP_STATE_OK);
        InformerGetNotes();
    }

    return NULL;
}

static unsigned long *
InformerElemsModeChanged(int CallbackArg, SparkInfoStruct SparkInfo)
{
    InformerAppStruct *app = InformerGetApp();
    InformerElemModeChoice mode = InformerGetElemUIMode();

    InformerDEBUG("Hey! The elements mode change got called with: value: %d\n", mode);

    if (INFORMER_ELEM_MODE_REFRESH == mode) {
        InformerSetAppState(app, INFORMER_APP_STATE_OK);
        InformerGetElems();
    }

    return NULL;
}


static unsigned long *
InformerNotesSortChanged(int CallbackArg, SparkInfoStruct SparkInfo)
{
    InformerAppStruct *app = InformerGetApp();
    InformerDEBUG("Hey! The notes sort change got called with: %d, value is: %d\n",
            CallbackArg, app->notes_ui_table.sort.ui->Value);
    InformerNoteDataSort();
    InformerTableRefreshUI();
    return NULL;
}

static unsigned long *
InformerElemsSortChanged(int CallbackArg, SparkInfoStruct SparkInfo)
{
    InformerAppStruct *app = InformerGetApp();
    InformerDEBUG("Hey! The elements sort change got called with: %d, value is: %d\n",
            CallbackArg, app->elems_ui_table.sort.ui->Value);
    InformerElemDataSort();
    InformerTableRefreshUI();
    return NULL;
}

void
InformerTableRowUpdateUIWithNotes(int data_index, int row_num)
{
    char str[32];
    InformerAppStruct *app = InformerGetApp();
    InformerTableRowUI *row;
    InformerNoteData source = app->notes_data[data_index];

    InformerDEBUG("Trying to update row UI with row_num [%d]\n", row_num);

    row = &(app->notes_ui_table.row[row_num - 1]);
    InformerTableRowSetStatusValue(row, source.is_checked);

    if (1 == source.is_checked) {
        strftime(str, sizeof(str), "%m/%d", localtime(&source.modified_on));
        InformerTableRowSetStatusText(row, "by %s on %s", source.modified_by, str);
    } else {
        InformerTableRowSetStatusText(row, "Pending...");
    }

    InformerTableRowSetMessageText(row, "%s", source.text);
    strftime(str, sizeof(str), "%m/%d %I:%M%p", localtime(&source.created_on));
    InformerTableRowSetFromText(row, "from %s at %s", source.created_by, str);

    InformerTableRowShow(row);
}

void
InformerTableRowUpdateUIWithElems(int data_index, int row_num)
{
    char str[32];
    InformerAppStruct *app = InformerGetApp();
    InformerTableRowUI *row;
    InformerElemData source = app->elems_data[data_index];

    InformerDEBUG("Trying to update row UI with row_num [%d]\n", row_num);

    row = &(app->elems_ui_table.row[row_num - 1]);
    InformerTableRowSetStatusValue(row, source.is_checked);
    InformerTableRowSetStatusText(row, "%s", source.kind);

    InformerTableRowSetMessageText(row, "%s", source.text);
    strftime(str, sizeof(str), "%m/%d %I:%M%p", localtime(&source.created_on));
    InformerTableRowSetFromText(row, "from %s at %s", source.created_by, str);

    InformerTableRowShow(row);
}

static unsigned long *
InformerNotesCreateNoteCallback(int CallbackArg, SparkInfoStruct SparkInfo)
{
    char *input;
    InformerAppStruct *app = InformerGetApp();

    input = app->notes_ui_create.ui->Value;

    ///////////////////////////////////////////////////////////////
    // XXX THIS CAN NOT CALL ERROR OR YOU WILL NEVER
    // XXX EXIT THE KEYBOARD!  THIS MUST RETURN
    ///////////////////////////////////////////////////////////////
    // XXX InformerERROR Can not be used!
    ///////////////////////////////////////////////////////////////

    if (strncmp("", input, SPARK_MAX_STRING_LENGTH) == 0) {
        // ignore -- nothing was entered
        InformerDEBUG("Ignoring empty input\n");
    } else {
        InformerDEBUG("New note text was entered\n");
        InformerDEBUG("The text value is: [%s]\n", input);

        sparkMessage(CREATE_NOTE_WAIT);

        if (TRUE == GatewayCreateNote(0, input)) {
            InformerGetNotes();
        } else {
            // TODO: What should happen here?
            // InformerERROR("Unable to create your note\n");
            InformerSetAppState(app, INFORMER_APP_STATE_ERR);
            sparkMessageDelay(5000, "Unable to create your note");
        }
    }

    return NULL;
}

int
InformerNoteDataSort(void)
{
    InformerAppStruct *app = InformerGetApp();
    InformerNoteSortChoice sort_by = app->notes_ui_table.sort.ui->Value;
    int (*compare)(const void *, const void *) = NULL;

    if (app->notes_data_count < 2) {
        InformerDEBUG("Less than 2 data elements -- can't sort\n");
        return TRUE;
    } else if (INFORMER_NOTE_SORT_DATE_CREATED == sort_by) {
        compare = InformerNoteDataCompareByDateCreated;
    } else if (INFORMER_NOTE_SORT_DATE_MODIFIED == sort_by) {
        compare = InformerNoteDataCompareByDateModified;
    } else if (INFORMER_NOTE_SORT_STATUS == sort_by) {
        compare = InformerNoteDataCompareByStatus;
    } else if (INFORMER_NOTE_SORT_ARTIST == sort_by) {
        compare = InformerNoteDataCompareByArtist;
    } else if (INFORMER_NOTE_SORT_AUTHOR == sort_by) {
        compare = InformerNoteDataCompareByAuthor;
    } else {
        InformerERROR("Unable to sort: unknown menu value [%d]\n", sort_by);
        return FALSE;
    }

    InformerDEBUG("Now going to call qsort\n");
    qsort(app->notes_data, app->notes_data_count, sizeof(InformerNoteData), compare);
    return TRUE;
}

int
InformerNoteDataCompareByDateCreated(const void *a, const void *b)
{
    InformerNoteData *note_a = (InformerNoteData*) a;
    InformerNoteData *note_b = (InformerNoteData*) b;

    InformerDEBUG("InformerNoteDataCompareByDateCreated called\n");

    if (note_a->created_on > note_b->created_on)
        return -1;
    else if (note_a->created_on < note_b->created_on)
        return 1;
    else
        return 0;
}

int
InformerNoteDataCompareByDateModified(const void *a, const void *b)
{
    InformerNoteData *note_a = (InformerNoteData*) a;
    InformerNoteData *note_b = (InformerNoteData*) b;

    InformerDEBUG("InformerNoteDataCompareByDateModified called\n");

    if (note_a->modified_on > note_b->modified_on)
        return -1;
    else if (note_a->modified_on < note_b->modified_on)
        return 1;
    else
        return 0;
}

int
InformerNoteDataCompareByStatus(const void *a, const void *b)
{
    InformerNoteData *note_a = (InformerNoteData*) a;
    InformerNoteData *note_b = (InformerNoteData*) b;

    InformerDEBUG("InformerNoteDataCompareByStatus called\n");

    if (note_a->is_checked < note_b->is_checked)
        return -1;
    else if (note_a->is_checked > note_b->is_checked)
        return 1;
    else if (note_a->modified_on > note_b->modified_on)
        return -1;
    else if (note_a->modified_on < note_b->modified_on)
        return 1;
    else
        return 0;
}

int
InformerNoteDataCompareByArtist(const void *a, const void *b)
{
    int result;

    InformerNoteData *note_a = (InformerNoteData*) a;
    InformerNoteData *note_b = (InformerNoteData*) b;

    InformerDEBUG("InformerNoteDataCompareByArtist called\n");

    if (note_a->is_checked > note_b->is_checked) {
        // a is checked
        return -1;
    } else if (note_a->is_checked < note_b->is_checked) {
        // b is checked
        return 1;
    } else if (0 == note_a->is_checked) {
        // neither are checked
        if (note_a->created_on > note_b->created_on)
            return -1;
        else if (note_a->created_on < note_b->created_on)
            return 1;
        else
            return 0;
    } else {
        // they are both checked
        result = strncmp(note_a->modified_by, note_b->modified_by, USERNAME_MAX);

        if (0 != result)
            return result;
        else if (note_a->modified_on > note_b->modified_on)
            return -1;
        else if (note_a->modified_on < note_b->modified_on)
            return 1;
        else
            return 0;
    }
}

int
InformerNoteDataCompareByAuthor(const void *a, const void *b)
{
    int result;

    InformerNoteData *note_a = (InformerNoteData*) a;
    InformerNoteData *note_b = (InformerNoteData*) b;

    InformerDEBUG("InformerNoteDataCompareByAuthor called\n");

    result = strncmp(note_a->created_by, note_b->created_by, USERNAME_MAX);

    if (0 != result)
        return result;
    else if (note_a->created_on > note_b->created_on)
        return -1;
    else if (note_a->created_on < note_b->created_on)
        return 1;
    else
        return 0;
}

int
InformerElemDataSort(void)
{
    InformerAppStruct *app = InformerGetApp();
    InformerElemSortChoice sort_by = app->elems_ui_table.sort.ui->Value;
    int (*compare)(const void *, const void *) = NULL;

    if (app->elems_data_count < 2) {
        InformerDEBUG("Less than 2 data elements -- can't sort\n");
        return TRUE;
    } else if (INFORMER_ELEM_SORT_DATE_CREATED == sort_by) {
        compare = InformerElemDataCompareByDateCreated;
    } else if (INFORMER_ELEM_SORT_STATUS == sort_by) {
        compare = InformerElemDataCompareByStatus;
    } else if (INFORMER_ELEM_SORT_AUTHOR == sort_by) {
        compare = InformerElemDataCompareByAuthor;
    } else if (INFORMER_ELEM_SORT_KIND == sort_by) {
        compare = InformerElemDataCompareByKind;
    } else {
        InformerERROR("Unable to sort: unknown menu value [%d]\n", sort_by);
        return FALSE;
    }

    InformerDEBUG("Now going to call qsort\n");
    qsort(app->elems_data, app->elems_data_count, sizeof(InformerElemData), compare);
    return TRUE;
}

int
InformerElemDataCompareByKind(const void *a, const void *b)
{
    int result;

    InformerElemData *elem_a = (InformerElemData*) a;
    InformerElemData *elem_b = (InformerElemData*) b;

    InformerDEBUG("InformerElemDataCompareByAuthor called\n");

    result = strncmp(elem_a->kind, elem_b->kind, SPARK_MAX_STRING_LENGTH);

    if (0 != result)
        return result;
    else if (elem_a->created_on > elem_b->created_on)
        return -1;
    else if (elem_a->created_on < elem_b->created_on)
        return 1;
    else
        return 0;
}

int
InformerElemDataCompareByAuthor(const void *a, const void *b)
{
    int result;

    InformerElemData *elem_a = (InformerElemData*) a;
    InformerElemData *elem_b = (InformerElemData*) b;

    InformerDEBUG("InformerElemDataCompareByAuthor called\n");

    result = strncmp(elem_a->created_by, elem_b->created_by, USERNAME_MAX);

    if (0 != result)
        return result;
    else if (elem_a->created_on > elem_b->created_on)
        return -1;
    else if (elem_a->created_on < elem_b->created_on)
        return 1;
    else
        return 0;
}

int
InformerElemDataCompareByStatus(const void *a, const void *b)
{
    InformerElemData *elem_a = (InformerElemData*) a;
    InformerElemData *elem_b = (InformerElemData*) b;

    InformerDEBUG("InformerElemDataCompareByStatus called\n");

    if (elem_a->is_checked < elem_b->is_checked)
        return -1;
    else if (elem_a->is_checked > elem_b->is_checked)
        return 1;
    else if (elem_a->created_on > elem_b->created_on)
        return -1;
    else if (elem_a->created_on < elem_b->created_on)
        return 1;
    else
        return 0;
}

int
InformerElemDataCompareByDateCreated(const void *a, const void *b)
{
    InformerElemData *elem_a = (InformerElemData*) a;
    InformerElemData *elem_b = (InformerElemData*) b;

    InformerDEBUG("InformerElemDataCompareByDateCreated called\n");

    if (elem_a->created_on > elem_b->created_on)
        return -1;
    else if (elem_a->created_on < elem_b->created_on)
        return 1;
    else
        return 0;
}


/****************************************************************************/
/*** INFORMER APP ***********************************************************/

/*--------------------------------------------------------------------------*/
/* Returns the main application data object                                 */
/*--------------------------------------------------------------------------*/
InformerAppStruct *
InformerGetApp(void)
{
    return &gApp;
}

/*--------------------------------------------------------------------------*/
/* Returns the current table ui object of the app                           */
/*--------------------------------------------------------------------------*/
InformerTableUI *
InformerGetTableUI(InformerAppStruct *app)
{
    InformerAppModeChoice mode = InformerGetAppMode(app);

    if (mode == INFORMER_APP_MODE_NOTES) {
        return &(app->notes_ui_table);
    } else if (mode == INFORMER_APP_MODE_ELEMS) {
        return &(app->elems_ui_table);
    } else {
        InformerDEBUG("No current table UI in mode [%d]\n", mode);
        return NULL;
    }
}

int
InformerGetDataCount(InformerAppStruct *app)
{
    InformerAppModeChoice mode = InformerGetAppMode(app);

    if (mode == INFORMER_APP_MODE_NOTES) {
        return app->notes_data_count;
    } else if (mode == INFORMER_APP_MODE_ELEMS) {
        return app->elems_data_count;
    } else {
        InformerDEBUG("No current data count in mode [%d]\n", mode);
        return 0;
    }
}

/*--------------------------------------------------------------------------*/
/* Returns the name of the current setup being worked on.                   */
/*--------------------------------------------------------------------------*/
const char *
InformerGetSetupPath(void)
{
    SparkInfoStruct info;
    const char *setup;
    InformerAppStruct *app = InformerGetApp();

    setup = sparkGetLastSetupName();
    sparkGetInfo(&info);

    InformerDEBUG("------------------------------------------------\n");
    InformerDEBUG("app->setup_ui_setup_path---> [%s]\n", app->setup_ui_setup_path.ui->Value);
    InformerDEBUG("sparkGetLastSetupName --> [%s]\n", setup);
    InformerDEBUG("SparkInfoStruct.Name ---> [%s]\n", info.Name);
    InformerDEBUG("------------------------------------------------\n");
    return app->setup_ui_setup_path.ui->Value;
}

/*--------------------------------------------------------------------------*/
/* Stores the path specified as the path of the setup                       */
/*--------------------------------------------------------------------------*/
void
InformerSetSetupPath(char *path)
{
    // I THINK THIS CAN GO AWAY...
    InformerAppStruct *app = InformerGetApp();
    strncpy(app->setup_ui_setup_path.ui->Value, path, SPARK_MAX_STRING_LENGTH);
}

/*--------------------------------------------------------------------------*/
/* Stores the name of the spark                                             */
/*--------------------------------------------------------------------------*/
void
InformerSetSparkName(char *name)
{
    InformerAppStruct *app = InformerGetApp();
    strncpy(app->setup_ui_spark_name.ui->Value, name, SPARK_MAX_STRING_LENGTH);
}

/*--------------------------------------------------------------------------*/
/* Get the name of the spark                                                */
/*--------------------------------------------------------------------------*/
const char *
InformerGetSparkName(void)
{
    const char *name;
    InformerAppStruct *app = InformerGetApp();
    name = app->setup_ui_spark_name.ui->Value;
    return name;
}

InformerAppModeChoice
InformerGetAppMode(InformerAppStruct *app)
{
    return app->app_current_mode;
}

const char *
InformerGetAppModeStr(InformerAppStruct *app)
{
    InformerAppModeChoice mode = InformerGetAppMode(app);

    if (mode == INFORMER_APP_MODE_NOTES) {
        return "note";
    } else if (mode == INFORMER_APP_MODE_ELEMS) {
        return "element";
    } else if (mode == INFORMER_APP_MODE_SETUP) {
        return "setup";
    } else {
        InformerDEBUG("Unknown app mode [%d]\n", mode);
        return "unknown";
    }
}

void
InformerSetAppMode(InformerAppStruct *app, InformerAppModeChoice mode)
{
    InformerDEBUG("Now setting app mode to [%d]\n", mode);
    app->app_current_mode = mode;
}


/****************************************************************************/
/*** INFORMER TABLE *********************************************************/
static unsigned long *
InformerTableRow1BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo)
{
    InformerTableToggleRow(1);
    return NULL;
}

static unsigned long *
InformerTableRow2BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo)
{
    InformerTableToggleRow(2);
    return NULL;
}

static unsigned long *
InformerTableRow3BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo)
{
    InformerTableToggleRow(3);
    return NULL;
}

static unsigned long *
InformerTableRow4BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo)
{
    InformerTableToggleRow(4);
    return NULL;
}

static unsigned long *
InformerTableRow5BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo)
{
    InformerTableToggleRow(5);
    return NULL;
}

void
InformerTableToggleRow(int row_num)
{
    int id;
    int index;
    int status = -1;
    int is_checked;
    int data_count;

    InformerAppStruct *app;
    InformerAppModeChoice mode;

    app = InformerGetApp();
    mode = InformerGetAppMode(app);
    index = InformerTableRowToIndex(row_num);
    data_count = InformerGetDataCount(app);

    if (index < data_count) {
        if (INFORMER_APP_MODE_NOTES == mode &&
            INFORMER_NOTE_MODE_REFRESH == InformerGetNoteUIMode()) {
            id = app->notes_data[index].id;
            is_checked = app->notes_ui_table.row[row_num-1].status.ui->Value;

            /* set the checkbox to the opposite in case the db fails */
            app->notes_ui_table.row[row_num-1].status.ui->Value = !is_checked;
            InformerUpdateNote(index, id, is_checked);
        } else if (INFORMER_APP_MODE_ELEMS == mode &&
                   INFORMER_ELEM_MODE_REFRESH == InformerGetElemUIMode()) {
            InformerDEBUG("--------> TODO: UpdateElement\n");
            id = app->elems_data[index].id;
            is_checked = app->elems_ui_table.row[row_num-1].status.ui->Value;

            /* set the checkbox to the opposite in case the db fails */
            app->elems_ui_table.row[row_num-1].status.ui->Value = !is_checked;
            InformerUpdateElem(index, id, is_checked);
        }

        InformerDEBUG("Status was [%d]\n", status);
    } else {
        InformerDEBUG("Whoa! can't toggle secret hidden notes! :)\n");
    }
}

static unsigned long *
InformerTableButtonA(int CallbackArg, SparkInfoStruct SparkInfo)
{
    InformerTableButtonEvent(INFORMER_TABLE_BUTTON_A);
    return NULL;
}

static unsigned long *
InformerTableButtonB(int CallbackArg, SparkInfoStruct SparkInfo)
{
    InformerTableButtonEvent(INFORMER_TABLE_BUTTON_B);
    return NULL;
}

void
InformerTableButtonEvent(InformerTableButtonChoice button)
{
    int cur_page;
    int max_page;
    int data_count;

    const char *mode_str;
    InformerAppStruct *app;
    InformerTableUI *table;

    app = InformerGetApp();
    table = InformerGetTableUI(app);
    mode_str = InformerGetAppModeStr(app);
    cur_page = InformerTableGetCurPage(table);
    data_count = InformerGetDataCount(app);

    InformerDEBUG("---------> data_count is [%d]\n", data_count);

    if (INFORMER_TABLE_BUTTON_A == button) {
        InformerDEBUG("You clicked %s Page Prev\n", mode_str);
        if (cur_page > 1) {
            InformerTableSetCurPage(table, cur_page - 1);
            InformerTableRefreshUI();
        } else {
            InformerDEBUG("You can't go past page 1\n");
        }
    } else if (INFORMER_TABLE_BUTTON_B == button) {
        InformerDEBUG("You clicked %s Page Next\n", mode_str);
        max_page = InformerTableGetPageOfIndex(table, data_count - 1);
        InformerDEBUG("--------> the max is [%d]\n", max_page);
        if (cur_page < max_page) {
            InformerTableSetCurPage(table, cur_page + 1);
            InformerTableRefreshUI();
        } else {
            InformerDEBUG("You can't go past the last page (%d)\n", max_page);
        }
    }
}

int
InformerTableGetRowCount(InformerTableUI *table)
{
    return table->row_count;
}

int
InformerTableGetPageOfIndex(InformerTableUI *table, int data_index)
{
    return (int) ceil((float)(data_index + 1)/table->row_count);
}


int
InformerTableGetCurPage(InformerTableUI *table)
{
    return table->cur_page;
}

void
InformerTableSetCurPage(InformerTableUI *table, int cur_page)
{
    table->cur_page = cur_page;
}

void
InformerTableSetStatus(InformerTableUI *table, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(table->statusbar.ui->Value, format, args);
    va_end(args);
}

void
InformerTableHideAllRows(void)
{
    int i;
    InformerAppStruct *app;
    InformerTableUI *table;

    app = InformerGetApp();
    table = InformerGetTableUI(app);

    if (NULL != table) {
        for (i=0; i<InformerTableGetRowCount(table); i++) {
            InformerTableRowHide(&(table->row[i]));
        }
    }
}

void
InformerTableCanvasDraw(SparkCanvasInfo canvas_info)
{
    // InformerDEBUG("-- draw event called --\n");
}

int
InformerTableCanvasInteract(SparkCanvasInfo canvas_info, int x, int y, float pressure)
{
    // InformerDEBUG("--- interact event called x: %d, y: %d, pressure %f ---\n", x, y, pressure);
    /* return 1 for canvas display */
    return 1;
}

int
InformerTableRowToIndex(int row_num)
{
    int cur_page;
    int row_count;
    InformerAppStruct *app;
    InformerTableUI *table;

    app = InformerGetApp();
    table = InformerGetTableUI(app);
    cur_page = InformerTableGetCurPage(table);
    row_count = InformerTableGetRowCount(table);

    return (cur_page - 1) * row_count + row_num - 1;
}

void
InformerTableRefreshUI(void)
{
    int row;
    int index;
    int start = 0;
    int end = 0;
    int cur_page = 0;
    int row_count = 0;
    int data_count = 0;

    const char *mode_str;

    InformerAppStruct *app;
    InformerTableUI *table;

    app = InformerGetApp();
    table = InformerGetTableUI(app);
    mode_str = InformerGetAppModeStr(app);

    cur_page = InformerTableGetCurPage(table);
    row_count = InformerTableGetRowCount(table);
    data_count = InformerGetDataCount(app);

    InformerTableHideAllRows();

    InformerDEBUG("****** Time to refresh the %s UI ********\n", mode_str);
    InformerDEBUG("I think there are [%d] %ss\n", data_count, mode_str);

    if (data_count > 0) {
        if (1 == data_count) {
            start = end = 0;
            InformerTableSetStatus(table, "Displaying %s 1 (of 1)", mode_str);
        } else {
            start = (cur_page - 1) * row_count;
            if (data_count - start < row_count) {
                /* if there is less than a page left, calculate partial */
                end = start - 1 + (data_count - start) % row_count;
            } else {
                /* there is more than a full page left, max it out */
                end = start + row_count - 1;
            }

            InformerTableSetStatus(table, "Displaying %ss %d - %d (of %d)",
                                   mode_str, start + 1, end + 1, data_count);
        }

        for (row=1; row <= row_count; row++) {
            index = start + row - 1;
            if (index <= end) {
                InformerDEBUG("Going to display index [%d] at row [%d]\n", index, row);
                InformerTableRowUpdateUI(index, row);
            }
        }
    } else {
        InformerTableSetStatus(table, "No %ss to display", mode_str);
    }
}


/****************************************************************************/
/*** INFORMER TABLE ROW *****************************************************/
void
InformerTableRowHide(InformerTableRowUI *row)
{
    InformerDEBUG("++++ call to hide row\n");
    sparkDisableParameter(SPARK_UI_CONTROL, row->status.id);
    sparkDisableParameter(SPARK_UI_CONTROL, row->message.id);
    sparkDisableParameter(SPARK_UI_CONTROL, row->from.id);
}

void
InformerTableRowShow(InformerTableRowUI *row)
{
    InformerDEBUG("++++ call to hide row\n");
    sparkEnableParameter(SPARK_UI_CONTROL, row->status.id);
    sparkEnableParameter(SPARK_UI_CONTROL, row->message.id);
    sparkEnableParameter(SPARK_UI_CONTROL, row->from.id);
}

void
InformerTableRowSetFromText(InformerTableRowUI *row, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(row->from.ui->Value, format, args);
    va_end(args);
}

void
InformerTableRowSetMessageText(InformerTableRowUI *row, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(row->message.ui->Value, format, args);
    va_end(args);
}

void
InformerTableRowSetStatusValue(InformerTableRowUI *row, int is_checked)
{
    row->status.ui->Value = is_checked;
}

void
InformerTableRowSetStatusText(InformerTableRowUI *row, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(row->status.text, format, args);
    va_end(args);

    row->status.ui->Title = row->status.text;
}

void InformerTableRowUpdateUI(int data_index, int row_num)
{
    InformerAppStruct *app;
    InformerAppModeChoice mode;

    app = InformerGetApp();
    mode = InformerGetAppMode(app);

    if (mode == INFORMER_APP_MODE_NOTES) {
        InformerTableRowUpdateUIWithNotes(data_index, row_num);
    } else if (mode == INFORMER_APP_MODE_ELEMS) {
        InformerTableRowUpdateUIWithElems(data_index, row_num);
    }
}
