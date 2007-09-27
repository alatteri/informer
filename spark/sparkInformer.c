/****************************************************************************
 *                                 Informer                                 *
 ****************************************************************************/
#include "spark.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define UI_NUM_ROWS 6
#define USERNAME_MAX 32

extern int errno;

/*************************************
 * Informer notes data structure
 *************************************/
typedef struct {
    unsigned int   id;                          /* Primary key of Note */
    char           text[4096];                  /* The actual note message */
    unsigned int   is_checked;                  /* Boolean: is the note checked? */
    char           created_by[USERNAME_MAX];    /* The user who created the note */
    char           created_on[100];             /* Date note was created */
    char           modified_by[USERNAME_MAX];   /* The user who last modified the note */
    char           modified_on[100];            /* Date note was last modified */
} InformerNoteDataStruct;


typedef struct {
    char                boolean_text[256];      /* Text for the Note boolean is_checked */
    SparkBooleanStruct  *boolean_ui;            /* Pointer to the row's check box UI element */
    SparkStringStruct   *text_ui;               /* Pointer to the row's message UI element */
    SparkStringStruct   *from_ui;               /* Pointer to the row's from UI element */
} InformerNoteUIStruct;


typedef struct {
    unsigned int                notes_ui_count;         /* Total number of NoteUI structs */
    unsigned int                notes_data_count;       /* Total number of NoteData structs */
    unsigned int                notes_cur_page;         /* Current note page number */
    InformerNoteDataStruct      notes_data[100];        /* All of the note data */
    InformerNoteUIStruct        notes_ui[UI_NUM_ROWS];  /* Data to describe each UI row */
} InformerAppStruct;

/*************************************
 * Informer function prototypes
 *************************************/
InformerAppStruct *InformerGetApp(void);

const char *DiscreetGetUserdbPath(void);

const char *InformerGetSetupName(void);
const char *InformerGetGatewayPath(void);
int InformerGetCurrentUser(char *user, int max_length);

int InformerImportNotes(char *filepath, int index, int update_count);
int InformerGetNotesDB(void);
int InformerUpdateNoteDB(int index, int note_id, int is_checked);
int InformerCallGateway(char *action, char *infile, char *outfile);

int InformerRowToIndex(int row_num);
void InformerShowNoteRow(int row_num);
void InformerHideNoteRow(int row_num);
void InformerHideAllNoteRows(void);
void InformerToggleNoteRow(int row_num, int on_off);
void InformerRefreshNotesUI(void);
void InformerUpdateNotesRowUI(InformerNoteDataStruct source, int row_num);

static unsigned long *InformerNotesPagePrev(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesPageNext(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesPupChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesRow1BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesRow2BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesRow3BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesRow4BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesRow4BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesRow5BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesRow6BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
void InformerNotesToggleRow(int row_num);

/* Informer Note Boolean CheckBoxes */
SparkBooleanStruct SparkBoolean7 =  { 0, "", InformerNotesRow1BoolChanged };
SparkBooleanStruct SparkBoolean8 =  { 0, "", InformerNotesRow2BoolChanged };
SparkBooleanStruct SparkBoolean9 =  { 0, "", InformerNotesRow3BoolChanged };
SparkBooleanStruct SparkBoolean10 = { 0, "", InformerNotesRow4BoolChanged };
SparkBooleanStruct SparkBoolean11 = { 0, "", InformerNotesRow5BoolChanged };
SparkBooleanStruct SparkBoolean12 = { 0, "", InformerNotesRow6BoolChanged };

/* Informer Note Text fields */
SparkStringStruct SparkString14 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString15 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString16 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString17 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString18 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString19 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };

/* Informer From/Date fields */
SparkStringStruct SparkString28 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString29 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString30 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString31 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString32 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkStringStruct SparkString33 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };

/* Informer Note Controls */
SparkPupStruct SparkPup6 = {0, 2, InformerNotesPupChanged, {"Sort by date",
                                                             "Sort by status"}};
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
InformerAppStruct    gApp;


/*************************************
 * Informer error strings
 *************************************/
static char GATEWAY_STATUS_ERR[] = "Unable to get notes";
static char GET_NOTES_WAIT[] = "Getting notes from database...";
static char UPDATE_NOTE_WAIT[] = "Updating database...";
static char CURRENT_USER_ERR[] = "Unable to determine user";

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
    InformerAppStruct *app = InformerGetApp();

    /* Initialize the notes data container */
    app->notes_ui_count = UI_NUM_ROWS;
    app->notes_data_count = 0;

    /* Initialize the notes UI container */
    app->notes_ui[0].boolean_ui = &SparkBoolean7;
    app->notes_ui[1].boolean_ui = &SparkBoolean8;
    app->notes_ui[2].boolean_ui = &SparkBoolean9;
    app->notes_ui[3].boolean_ui = &SparkBoolean10;
    app->notes_ui[4].boolean_ui = &SparkBoolean11;
    app->notes_ui[5].boolean_ui = &SparkBoolean12;

    app->notes_ui[0].text_ui = &SparkString14;
    app->notes_ui[1].text_ui = &SparkString15;
    app->notes_ui[2].text_ui = &SparkString16;
    app->notes_ui[3].text_ui = &SparkString17;
    app->notes_ui[4].text_ui = &SparkString18;
    app->notes_ui[5].text_ui = &SparkString19;

    app->notes_ui[0].from_ui = &SparkString28;
    app->notes_ui[1].from_ui = &SparkString29;
    app->notes_ui[2].from_ui = &SparkString30;
    app->notes_ui[3].from_ui = &SparkString31;
    app->notes_ui[4].from_ui = &SparkString32;
    app->notes_ui[5].from_ui = &SparkString33;

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
    InformerGetNotesDB();
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
/* Returns the main application data object                                 */
/*--------------------------------------------------------------------------*/
InformerAppStruct *InformerGetApp(void)
{
    return &gApp;
}

/*--------------------------------------------------------------------------*/
/* Returns the name of the current setup being worked on.                   */
/*--------------------------------------------------------------------------*/
const char *InformerGetSetupName(void)
{
    const char *setup;
    setup = sparkGetLastSetupName();
    return setup;
}

/*--------------------------------------------------------------------------*/
/* Returns path to the informer gateway script                              */
/*--------------------------------------------------------------------------*/
const char *InformerGetGatewayPath(void)
{
    const char *path = "/usr/discreet/instinctual/informer/bin/gateway";
    return path;
}

/*--------------------------------------------------------------------------*/
/* Get the current spark user into the char* argument                       */
/* Returns TRUE on success, FALSE on faliure                                */
/*--------------------------------------------------------------------------*/
int InformerGetCurrentUser(char *user, int max_length)
{
    FILE *fp;
    char line[1024];
    const char *filepath;

    int i;
    char *c;
    char *start;
    char *end;

    max_length -= 1;

    filepath = DiscreetGetUserdbPath();
    if (NULL == filepath) {
        return FALSE;
    }

    if ((fp=fopen(filepath, "r")) == NULL) {
        printf("I couldn't open %s: %s\n", filepath, strerror(errno));
        sprintf(SparkString27.Value, "%s: can't open [%s]",
                CURRENT_USER_ERR, filepath);
        return FALSE;
    }

    do {
        c = fgets(line, 1024, fp);
        if (c != NULL) {
            printf("Checking line: %s\n", line);
            if (strstr(line, "UserGroupStatus:UsrGroup1={") != NULL) {
                printf("MATCH! -> %s\n", line);
                start = index(line, '{');
                end = rindex(line, '}');
                if (end != NULL && start != NULL) {
                    if (end - start < max_length - 1)
                        max_length = end - start;
                    printf("start [%c], end [%c], max_length [%d]\n", *start, *end, max_length);

                    strncpy(user, start + 1, max_length - 1);
                    user[max_length-1] = '\0';
                    printf("Going to return user [%s]\n", user);
                    return TRUE;
                }
            }
        }
    } while (c != NULL);


    return FALSE;
}

const char *DiscreetGetUserdbPath(void)
{
    const char *program;
    const char *effects = "/usr/discreet/user/effects/user.db";
    const char *editing = "/usr/discreet/user/editing/user.db";

    program = sparkProgramGetName();

    if (0 == strcmp("flame", program) ||
        0 == strcmp("flint", program) ||
        0 == strcmp("inferno", program)) {
        return effects;
    } else if (0 == strcmp("fire", program) ||
               0 == strcmp("smoke", program)) {
        return editing;
    } else {
        sprintf(SparkString27.Value, "%s: unknown program [%s]",
                CURRENT_USER_ERR, program);
        return NULL;
    }
}

int InformerGetNotesDB(void)
{
    sparkMessage(GET_NOTES_WAIT);
    InformerHideAllNoteRows();

    if (TRUE == InformerCallGateway("get_notes", NULL, "/tmp/trinity")) {
        if (TRUE == InformerImportNotes("/tmp/trinity", 0, TRUE)) {
            InformerRefreshNotesUI();
            return TRUE;
        } else {
            // TODO: What should happen here?
            sprintf(SparkString27.Value, "%s", GET_NOTES_WAIT);
            return FALSE;
        }
    }
}

int InformerUpdateNoteDB(int index, int note_id, int is_checked)
{
    int export_ok = 0;
    char user[USERNAME_MAX];

    sparkMessage(UPDATE_NOTE_WAIT);

    if (TRUE != InformerGetCurrentUser(user, USERNAME_MAX)) {
        printf("Unable to determine the current user!\n");
        return FALSE;
    }

    if (TRUE == InformerExportNote("/tmp/trinity2", note_id, is_checked, user)) {
        printf("--------- update note ----------\n");
        printf("is_checked [%d], user [%s], id [%d], index [%d]\n",
               is_checked, user, note_id, index);
        printf("--------- update note ----------\n");
        if (TRUE == InformerCallGateway("update_note", "/tmp/trinity2", "/tmp/trinity3")) {
            if (TRUE == InformerImportNotes("/tmp/trinity3", index, FALSE)) {
                InformerRefreshNotesUI();
                return TRUE;
            }
        }
    }

    return FALSE;
}

int InformerCallGateway(char *action, char *infile, char *outfile)
{
    int pid = 0;
    int index = 0;
    int status = 0;
    const char *setup;
    const char *gateway;
    const char *argv[10];
    const char **ptr = argv;

    setup = InformerGetSetupName();
    gateway = InformerGetGatewayPath();

    printf("the gateway is [%s]\n", gateway);

    argv[index++] = gateway;
    argv[index++] = "-s";
    argv[index++] = setup;
    argv[index++] = "-a";
    argv[index++] = action;

    if (infile != NULL) {
        argv[index++] = "-i";
        argv[index++] = infile;
    }

    if (outfile != NULL) {
        argv[index++] = "-o";
        argv[index++] = outfile;
    }

    argv[index] = 0;    /* This is required to end the array */

    printf("---- calling gateway ----\n");
    while (*ptr) {
        printf("%s ", *ptr++);
    }
    printf("\n---- calling gateway ----\n");

    pid = sparkSystemNoSh(FALSE, argv[0], argv);
    sparkWaitpid(pid, &status, 0);
    printf("Gateway returned. status [%d]\n", status);

    if (0 != status) {
        // TODO: Map the status to a human readable string
        sprintf(SparkString27.Value, "%s: status [%d]",
                GATEWAY_STATUS_ERR, status);
        return FALSE;
    } else {
        return TRUE;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int InformerExportNote(char *filepath, int note_id, int is_checked, char *modified_by)
{
    FILE *fp;

    printf("here we go. writing datafile [%s]\n", filepath);

    if ((fp=fopen(filepath, "w")) == NULL) {
        sprintf(SparkString27.Value, "%s: can't write datafile [%s]",
                GATEWAY_STATUS_ERR, filepath);
        return FALSE;
    }

    printf("-------- OK -------- opened the file for writing!\n");

    if ((fprintf(fp, "1\n") > 0) &&
        (fprintf(fp, "id: %d\n", note_id) > 0) &&
        (fprintf(fp, "is_checked: %d\n", is_checked) > 0) &&
        (fprintf(fp, "modified_by: %s\n", modified_by) > 0)) {
        printf("... write of note ok!\n");
        fclose(fp);
        return TRUE;
    } else {
        printf("------- WHOA! error writing to datafile!\n");
        sprintf(SparkString27.Value, "%s: can't write datafile [%s]",
                GATEWAY_STATUS_ERR, filepath);
        return FALSE;
    }
}

int InformerImportNotes(char *filepath, int index, int update_count)
{
    FILE *fp;
    int i = 0;
    int count = 0;
    int result = 1;
    InformerAppStruct *app = InformerGetApp();

    printf("ImportNotes: here we go. reading datafile [%s], index [%d], update? [%d]\n",
           filepath, index, update_count);

    if ((fp=fopen(filepath, "r")) == NULL) {
        sprintf(SparkString27.Value, "%s: can't open datafile [%s]",
                GATEWAY_STATUS_ERR, filepath);
        return FALSE;
    }

    /*
        read the data file:
        - first line is number of note objects (not lines...)
        - entries are of form
            key: value
    */
    result = fscanf(fp, "%d%*1[\n]", &count);
    if (1 != result) {
        sprintf(SparkString27.Value, "%s: can't parse datafile [%s]",
                GATEWAY_STATUS_ERR, filepath);
        return FALSE;
    }

    printf("OK there are %d entries\n", count);

    for (i=0; i<count; i++) {
        /* skip the "key:" read the value */

        /* read the id field -> Id */
        /* read the text field -> Text */
        /* read the is_checked field -> IsChecked */
        /* read the date_added field -> DateAdded */
        /* read the date_modified field -> DateModified */

        if ((fscanf(fp, "%*s %d%*1[\n]",     &(app->notes_data[index+i].id)) > 0) &&
            (fscanf(fp, "%*s %[^\n]%*1[\n]", &(app->notes_data[index+i].text)) > 0) &&
            (fscanf(fp, "%*s %d%*1[\n]",     &(app->notes_data[index+i].is_checked)) > 0) &&
            (fscanf(fp, "%*s %[^\n]%*1[\n]", &(app->notes_data[index+i].created_by)) > 0) &&
            (fscanf(fp, "%*s %[^\n]%*1[\n]", &(app->notes_data[index+i].created_on)) > 0) &&
            (fscanf(fp, "%*s %[^\n]%*1[\n]", &(app->notes_data[index+i].modified_by)) > 0) &&
            (fscanf(fp, "%*s %[^\n]%*1[\n]", &(app->notes_data[index+i].modified_on)) > 0)) {
            /* looking good -- keep going */
            printf("... read note [%d] ok!\n", i);
        } else {
            sprintf(SparkString27.Value, "%s: can't parse datafile [%s]",
                    GATEWAY_STATUS_ERR, filepath);
            return FALSE;
        }
    }

    if (TRUE == update_count)
        app->notes_data_count = count;

    printf("All notes read ok. Word up.\n");

    return TRUE;
}

/****************************************************************************
 *                      Informer UI Function Calls                          *
 ****************************************************************************/
int InformerRowToIndex(int row_num)
{
    // TODO: make this work with pages
    return row_num - 1;
}

static unsigned long *InformerNotesPupChanged(int CallbackArg,
                                              SparkInfoStruct SparkInfo )
{
    printf("Hey! The pup change got called with: %d, value is: %d\n",
            CallbackArg, SparkPup34.Value);

    if (1 == CallbackArg) {
        InformerGetNotesDB();
    }

    return NULL;
}

static unsigned long *InformerNotesRow1BoolChanged(int CallbackArg,
                                                   SparkInfoStruct SparkInfo )
{
    InformerNotesToggleRow(1);
    return NULL;
}

static unsigned long *InformerNotesRow2BoolChanged(int CallbackArg,
                                                   SparkInfoStruct SparkInfo )
{
    InformerNotesToggleRow(2);
    return NULL;
}

static unsigned long *InformerNotesRow3BoolChanged(int CallbackArg,
                                                   SparkInfoStruct SparkInfo )
{
    InformerNotesToggleRow(3);
    return NULL;
}

static unsigned long *InformerNotesRow4BoolChanged(int CallbackArg,
                                                   SparkInfoStruct SparkInfo )
{
    InformerNotesToggleRow(4);
    return NULL;
}

static unsigned long *InformerNotesRow5BoolChanged(int CallbackArg,
                                                   SparkInfoStruct SparkInfo )
{
    InformerNotesToggleRow(5);
    return NULL;
}

static unsigned long *InformerNotesRow6BoolChanged(int CallbackArg,
                                                   SparkInfoStruct SparkInfo )
{
    InformerNotesToggleRow(6);
    return NULL;
}

void InformerNotesToggleRow(int row_num)
{
    InformerAppStruct *app = InformerGetApp();

    int status = 0;
    int index = InformerRowToIndex(row_num);
    int note_id = app->notes_data[index].id;
    int is_checked = app->notes_ui[row_num-1].boolean_ui->Value;

    printf("It's note_id is: (%d)\n", note_id);
    printf("Row number (%d) was changed\n", row_num);
    printf("It's value is: %d\n", is_checked);

    status = InformerUpdateNoteDB(index, note_id, is_checked);

    /* if (1 == status) {
        InformerRefreshNotesUI();
    }*/
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
    InformerAppStruct *app = InformerGetApp();
    int i = 0;

    printf("****** Time to refresh the notes UI ********\n");
    sprintf(SparkString27.Value, "Displaying notes 1 - %d (of %d)",
            app->notes_data_count, app->notes_data_count);

    for (i=0; i<app->notes_ui_count; i++)
    {
        InformerUpdateNotesRowUI(app->notes_data[i], i+1);
    }
}

void InformerUpdateNotesRowUI(InformerNoteDataStruct source, int row_num)
{
    InformerAppStruct *app = InformerGetApp();
    InformerNoteUIStruct *note_ui;
    note_ui = &(app->notes_ui[row_num - 1]);

    printf("Truing to update row UI with row_num [%d]\n", row_num);
    note_ui->boolean_ui->Value = source.is_checked;
    if (1 == source.is_checked) {
        sprintf(note_ui->boolean_text, "by %s on %s", source.modified_by, source.modified_on);
    } else {
        sprintf(note_ui->boolean_text, "TODO");
    }
    note_ui->boolean_ui->Title = note_ui->boolean_text;
    sprintf(note_ui->text_ui->Value, "%s", source.text);
    sprintf(note_ui->from_ui->Value, "from %s at %s", source.created_by, source.created_on);
    InformerShowNoteRow(row_num);
}

