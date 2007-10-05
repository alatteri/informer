/****************************************************************************
 *                                 Informer                                 *
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "spark.h"
#include "informer.h"
#include "informerUI.h"
#include "informerNotes.h"
#include "informerNotesUI.h"


/*************************************
 * Informer function prototypes
 *************************************/
int InformerRowToIndex(int row_num);    // need to have one for NotesUI and ElementsUI


/******************************************************************************************/
/*** INFORMER ELEMENTS ********************************************************************/
SparkPupStruct SparkPup36 = {0, 5, NULL, {"Sort by date added",
                                                             "Sort by last modified",
                                                             "Sort by status",
                                                             "Sort by artist",
                                                             "Sort by author"}};
SparkStringStruct SparkString57 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkPushStruct SparkPush43 = { "<< Previous Page", NULL };
SparkPushStruct SparkPush50 = { "Next Page >>", NULL };

SparkPupStruct SparkPup63 = {0, 1, NULL, {"Refresh Notes"}};

/* Informer Note Boolean CheckBoxes */
SparkBooleanStruct SparkBoolean37 = { 0, "", NULL };
SparkBooleanStruct SparkBoolean38 = { 0, "", NULL };
SparkBooleanStruct SparkBoolean39 = { 0, "", NULL };
SparkBooleanStruct SparkBoolean40 = { 0, "", NULL };
SparkBooleanStruct SparkBoolean41 = { 0, "", NULL };

/* Informer Note Text fields */
SparkStringStruct SparkString44 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString45 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString46 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString47 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString48 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };

/* Informer From/Date fields */
SparkStringStruct SparkString58 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString59 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString60 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString61 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString62 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };

/*
 * Aliases for Image Buffers
 */
static int RESULT_ID = 1;
static int FRONT_ID  = 2;

static SparkMemBufStruct SparkResult;
static SparkMemBufStruct SparkSource;


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
    const char *setup;
    InformerAppStruct *app = InformerGetApp();
    InformerDEBUG("----> SparkInitialise called <----\n");

    InformerInitSetupUI();
    InformerInitNotesData();
    InformerInitNotesUI();

    sparkControlTitle(SPARK_CONTROL_2, "Elements");

    InformerHideAllNoteRows();
    return SPARK_MODULE;
}

/*--------------------------------------------------------------------------*/
/* Called when the spark is unloaded.                                       */
/*--------------------------------------------------------------------------*/
void SparkUnInitialise(SparkInfoStruct spark_info)
{
    InformerAppStruct *app = InformerGetApp();
    InformerDEBUG("----> SparkUnInitialise called <----\n");
}

/*--------------------------------------------------------------------------*/
/* Returns the number of source clips that are required for the spark       */
/*--------------------------------------------------------------------------*/
int SparkClips(void)
{
    InformerDEBUG("----> SparkClips called <----\n");
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
    InformerAppStruct *app = InformerGetApp();

    InformerDEBUG("----> SparkProcessStart called <----\n");

    if (FALSE == app->notes_data_been_read) {
        InformerGetNotesDB();
    }

    return 1;
}

/*--------------------------------------------------------------------------*/
/* The SparkProcess function performs the actual image manipulation by      */
/* simply copying the source image. The function returns the address of the */
/* buffer of processed data.                                                */
/*--------------------------------------------------------------------------*/
unsigned long *SparkProcess(SparkInfoStruct spark_info)
{
    InformerDEBUG("-----> SparkProcess called <------\n");

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
    InformerDEBUG("-----> SparkEvent (%d) <-----\n", spark_event);
}

/*--------------------------------------------------------------------------*/
/* Callback for setup saving and loading                                    */
/*--------------------------------------------------------------------------*/
void SparkSetupIOEvent(SparkModuleEvent event, char *path, char *file)
{
    InformerAppStruct *app = InformerGetApp();

    printf("SparkSetupIOEvent called with [%d] path [%s] file [%s]\n",
            event, path, file);

    if (SPARK_EVENT_LOADSETUP == event || SPARK_EVENT_SAVESETUP == event) {
        if (NULL == strstr(path, "/_session")) {
            /* ignore auto load/saves */
            InformerDEBUG("LOADING/SAVING SETUP: %s\n", path);
            strncpy(app->setup_ui_setup_path.ui->Value, path, SPARK_MAX_STRING_LENGTH);
        }
    }
}

/****************************************************************************
 *                      Informer UI Function Calls                          *
 ****************************************************************************/
int InformerRowToIndex(int row_num)
{
    // TODO: make this work with pages
    InformerAppStruct *app = InformerGetApp();
    return (app->notes_ui_cur_page - 1) * app->notes_ui_count + row_num - 1;
}
