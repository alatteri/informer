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
#include <math.h>

#define UI_NUM_ROWS 5
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
                                                /* Date note was created */
    char           created_on[SPARK_MAX_STRING_LENGTH];
    char           modified_by[USERNAME_MAX];   /* The user who last modified the note */
                                                /* Date note was last modified */
    char           modified_on[SPARK_MAX_STRING_LENGTH];
} InformerNoteData;

typedef struct {
    unsigned int        id;                             /* the id of the boolean element */
    SparkBooleanStruct  *ui;                            /* ptr to the ui element */
    char                text[SPARK_MAX_STRING_LENGTH];  /* the text to display with the checkbox */
} InformerBooleanUI;

typedef struct {
    unsigned int        id;                             /* the id of the string element */
    SparkStringStruct   *ui;                            /* ptr to the ui element */
} InformerStringUI;

typedef struct {
    unsigned int        id;                             /* the id of the popup element */
    SparkPupStruct      *ui;                            /* ptr to the ui element */
} InformerPupUI;

typedef struct {
    unsigned int        id;
    SparkPushStruct     *ui;
} InformerPushUI;

typedef struct {
    InformerBooleanUI   status;                         /* the status of the note */
    InformerStringUI    message;                        /* the note's message */
    InformerStringUI    from;                           /* who the note is from */
} InformerNoteRowUI;

typedef struct {
    InformerNoteData            notes_data[100];        /* All of the note data */
    unsigned int                notes_data_count;       /* Total number of NoteData structs */

    InformerNoteRowUI           notes_ui[UI_NUM_ROWS];  /* Data to describe each UI row */
    unsigned int                notes_ui_count;         /* Total number of NoteUI structs */
    unsigned int                notes_ui_cur_page;      /* Current note page number */

    InformerPupUI               notes_ui_mode;          /* popup to select the note mode */
    InformerPupUI               notes_ui_sort;          /* the sort popup */
    InformerPushUI              notes_ui_button_a;      /* the left button */
    InformerPushUI              notes_ui_button_b;      /* the right button */
    InformerStringUI            notes_ui_statusbar;     /* status bar for feedback/display */
} InformerAppStruct;

/*************************************
 * Informer enums
 *************************************/
typedef enum {
    INFORMER_NOTE_MODE_REFRESH = 0,
    INFORMER_NOTE_MODE_CREATE = 1
} InformerNoteModeChoice;

typedef enum {
    INFORMER_NOTE_SORT_STATUS = 0,
    INFORMER_NOTE_SORT_DATE = 1
} InformerNoteSortChoice;

typedef enum {
    INFORMER_NOTE_BUTTON_A = 0,
    INFORMER_NOTE_BUTTON_B = 1
} InformerNoteButtonChoice;


/*************************************
 * Informer function prototypes
 *************************************/
InformerAppStruct *InformerGetApp(void);
InformerPupUI InformerInitPupUI(unsigned int id, SparkPupStruct *ui);
InformerPushUI InformerInitPushUI(unsigned int id, SparkPushStruct *ui);
InformerStringUI InformerInitStringUI(unsigned int id, SparkStringStruct *ui);
InformerBooleanUI InformerInitBooleanUI(unsigned int id, SparkBooleanStruct *ui, char *text);

const char *DiscreetGetUserdbPath(void);

const char *InformerGetSetupName(void);
const char *InformerGetGatewayPath(void);
int InformerGetCurrentUser(char *user, int max_length);

InformerNoteModeChoice InformerGetNoteUIMode(void);
void InformerNotesButtonEvent(InformerNoteButtonChoice button);

int InformerImportNotes(char *filepath, int index, int update_count);
int InformerGetNotesDB(void);
int InformerUpdateNoteDB(int index, int note_id, int is_checked);
int InformerCallGateway(char *action, char *infile, char *outfile);

int InformerRowToIndex(int row_num);
void InformerShowNoteRow(int row_num);
void InformerHideNoteRow(int row_num);
void InformerHideAllNoteRows(void);
void InformerToggleNoteRow(int row_num, int on_off);
void InformerCreateNotesUI(void);
void InformerRefreshNotesUI(void);
void InformerUpdateNotesRowUI(InformerNoteData source, int row_num);

static unsigned long *InformerNotesButtonA(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesButtonB(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesModeChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesSortChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesRow1BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesRow2BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesRow3BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesRow4BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesRow4BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesRow5BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
void InformerNotesToggleRow(int row_num);

void CanvasDraw(SparkCanvasInfo);
int  CanvasInteract(SparkCanvasInfo Canvas,
                    int PointerX, int PointerY,
                    float PointerPressure); /* return 1 for canvas display */

void CanvasDraw(SparkCanvasInfo canvas_info)
{
    // printf("-- draw event called --\n");
}

int CanvasInteract(SparkCanvasInfo canvas_info, int x, int y, float pressure)
{
    printf("--- interact event called x: %d, y: %d, pressure %f ---\n", x, y, pressure);
    return 1;
}


SparkCanvasStruct SparkCanvas1 = { CanvasDraw, CanvasInteract };

/* Informer Note Boolean CheckBoxes */
SparkBooleanStruct SparkBoolean8 =  { 0, "", InformerNotesRow1BoolChanged };
SparkBooleanStruct SparkBoolean9 =  { 0, "", InformerNotesRow2BoolChanged };
SparkBooleanStruct SparkBoolean10 = { 0, "", InformerNotesRow3BoolChanged };
SparkBooleanStruct SparkBoolean11 = { 0, "", InformerNotesRow4BoolChanged };
SparkBooleanStruct SparkBoolean12 = { 0, "", InformerNotesRow5BoolChanged };

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
SparkPupStruct SparkPup7 = {0, 2, InformerNotesSortChanged, {"Sort by date",
                                                             "Sort by status"}};
SparkStringStruct SparkString28 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkPushStruct SparkPush14 = { "<< Previous Page", InformerNotesButtonA };
SparkPushStruct SparkPush21 = { "Next Page >>", InformerNotesButtonB };

SparkPupStruct SparkPup34 = {0, 2, InformerNotesModeChanged, {"Refresh Notes",
                                                             "Create New Note"}};

SparkStringStruct SparkString13 = { "", "                click here to create a new note", SPARK_FLAG_NONE, NULL };

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
    printf("----> SparkInitialise called <----\n");

    /* Initialize the notes data container */
    app->notes_data_count = 0;

    /* Initialize the notes UI container */
    app->notes_ui_cur_page = 1;
    app->notes_ui_count = UI_NUM_ROWS;

    app->notes_ui_mode = InformerInitPupUI(34, &SparkPup34);
    app->notes_ui_sort = InformerInitPupUI(7, &SparkPup7);
    app->notes_ui_button_a  = InformerInitPushUI(14, &SparkPush14);
    app->notes_ui_button_b  = InformerInitPushUI(21, &SparkPush21);
    app->notes_ui_statusbar = InformerInitStringUI(28, &SparkString28);

    app->notes_ui[0].status = InformerInitBooleanUI(8, &SparkBoolean8, "");
    app->notes_ui[1].status = InformerInitBooleanUI(9, &SparkBoolean9, "");
    app->notes_ui[2].status = InformerInitBooleanUI(10, &SparkBoolean10, "");
    app->notes_ui[3].status = InformerInitBooleanUI(11, &SparkBoolean11, "");
    app->notes_ui[4].status = InformerInitBooleanUI(12, &SparkBoolean12, "");

    app->notes_ui[0].message = InformerInitStringUI(15, &SparkString15);
    app->notes_ui[1].message = InformerInitStringUI(16, &SparkString16);
    app->notes_ui[2].message = InformerInitStringUI(17, &SparkString17);
    app->notes_ui[3].message = InformerInitStringUI(18, &SparkString18);
    app->notes_ui[4].message = InformerInitStringUI(19, &SparkString19);

    app->notes_ui[0].from = InformerInitStringUI(29, &SparkString29);
    app->notes_ui[1].from = InformerInitStringUI(30, &SparkString30);
    app->notes_ui[2].from = InformerInitStringUI(31, &SparkString31);
    app->notes_ui[3].from = InformerInitStringUI(32, &SparkString32);
    app->notes_ui[4].from = InformerInitStringUI(33, &SparkString33);

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
    InformerAppStruct *app = InformerGetApp();
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
    InformerAppStruct *app = InformerGetApp();

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
InformerPupUI InformerInitPupUI(unsigned int id, SparkPupStruct *ui)
{
    InformerPupUI result;
    result.id = id;
    result.ui = ui;
    return result;
}

InformerPushUI InformerInitPushUI(unsigned int id, SparkPushStruct *ui)
{
    InformerPushUI result;
    result.id = id;
    result.ui = ui;
    return result;
}

InformerStringUI InformerInitStringUI(unsigned int id, SparkStringStruct *ui)
{
    InformerStringUI result;
    result.id = id;
    result.ui = ui;
    return result;
}

InformerBooleanUI InformerInitBooleanUI(unsigned int id, SparkBooleanStruct *ui, char *text)
{
    InformerBooleanUI result;
    result.id = id;
    result.ui = ui;
    sprintf(result.text, "%s", text);
    return result;
}

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
        sprintf(SparkString28.Value, "%s: can't open [%s]",
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
        sprintf(SparkString28.Value, "%s: unknown program [%s]",
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
            sprintf(SparkString28.Value, "%s", GET_NOTES_WAIT);
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
        sprintf(SparkString28.Value, "%s: status [%d]",
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
        sprintf(SparkString28.Value, "%s: can't write datafile [%s]",
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
        sprintf(SparkString28.Value, "%s: can't write datafile [%s]",
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
        sprintf(SparkString28.Value, "%s: can't open datafile [%s]",
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
        sprintf(SparkString28.Value, "%s: can't parse datafile [%s]",
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
            sprintf(SparkString28.Value, "%s: can't parse datafile [%s]",
                    GATEWAY_STATUS_ERR, filepath);
            return FALSE;
        }
    }

    if (TRUE == update_count) {
        app->notes_ui_cur_page = 1;
        app->notes_data_count = count;
    }

    printf("All notes read ok. Word up.\n");

    return TRUE;
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

InformerNoteModeChoice InformerGetNoteUIMode(void)
{
    InformerAppStruct *app = InformerGetApp();
    return (InformerNoteModeChoice) app->notes_ui_mode.ui->Value;
}

static unsigned long *InformerNotesModeChanged(int CallbackArg,
                                              SparkInfoStruct SparkInfo )
{
    InformerAppStruct *app = InformerGetApp();
    InformerNoteModeChoice mode = InformerGetNoteUIMode();

    printf("Hey! The notes mode change got called with: value: %d\n", mode);

    if (INFORMER_NOTE_MODE_REFRESH == mode) {
        InformerGetNotesDB();
    } else if (INFORMER_NOTE_MODE_CREATE == mode) {
        printf("Hey! Go create a note!\n");
        InformerCreateNotesUI();
    }

    return NULL;
}

static unsigned long *InformerNotesSortChanged(int CallbackArg,
                                              SparkInfoStruct SparkInfo )
{
    InformerAppStruct *app = InformerGetApp();
    printf("Hey! The notes sort change got called with: %d, value is: %d\n",
            CallbackArg, app->notes_ui_sort.ui->Value);
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

void InformerNotesToggleRow(int row_num)
{
    InformerAppStruct *app = InformerGetApp();

    int note_id;
    int is_checked;
    int index = InformerRowToIndex(row_num);
    InformerNoteModeChoice mode = InformerGetNoteUIMode();

    if (mode == INFORMER_NOTE_MODE_REFRESH && index < app->notes_data_count) {
        note_id = app->notes_data[index].id;
        is_checked = app->notes_ui[row_num-1].status.ui->Value;

        printf("It's note_id is: (%d)\n", note_id);
        printf("Row number (%d) was changed\n", row_num);
        printf("It's value is: %d\n", is_checked);

        InformerUpdateNoteDB(index, note_id, is_checked);
    } else {
        printf("Whoa! can't toggle secret hidden notes! :)\n");
    }
}

static unsigned long *InformerNotesButtonA(int CallbackArg,
                                            SparkInfoStruct SparkInfo )
{
    InformerNotesButtonEvent(INFORMER_NOTE_BUTTON_A);
    return NULL;
}

static unsigned long *InformerNotesButtonB(int CallbackArg,
                                            SparkInfoStruct SparkInfo )
{
    InformerNotesButtonEvent(INFORMER_NOTE_BUTTON_B);
    return NULL;
}

void InformerNotesButtonEvent(InformerNoteButtonChoice button)
{
    int max_page;
    InformerAppStruct *app = InformerGetApp();
    InformerNoteModeChoice mode = InformerGetNoteUIMode();

    if (INFORMER_NOTE_MODE_REFRESH == mode) {
        if (INFORMER_NOTE_BUTTON_A == button) {
            printf("You clicked Notes Page Prev\n");
            if (app->notes_ui_cur_page > 1) {
                app->notes_ui_cur_page -= 1;
                InformerRefreshNotesUI();
            } else {
                printf("You can't go past page 1\n");
            }
        } else if (INFORMER_NOTE_BUTTON_B == button) {
            printf("You clicked Notes Page Next\n");
            max_page = (int) ceil((float)app->notes_data_count/app->notes_ui_count);
            if (app->notes_ui_cur_page < max_page) {
                app->notes_ui_cur_page += 1;
                InformerRefreshNotesUI();
            } else {
                printf("You can't go past the last page (%d)\n", max_page);
            }
        }
    } else if (INFORMER_NOTE_MODE_CREATE == mode) {
        if (INFORMER_NOTE_BUTTON_A == button) {
            printf("-> cancel new note\n");
            InformerRefreshNotesUI();
        } else {
            printf("TODO: create new note\n");
        }
    }
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
    InformerAppStruct *app = InformerGetApp();
    void (*sparkToggle)(int, int) = NULL;

    if (1 <= row_num && UI_NUM_ROWS >= row_num) {
        if (0 == on_off) {
            sparkToggle = sparkDisableParameter;
        } else {
            sparkToggle = sparkEnableParameter;
        }

        /* The numbers below are the ui control column offsets */
        sparkToggle(SPARK_UI_CONTROL, app->notes_ui[row_num - 1].status.id);
        sparkToggle(SPARK_UI_CONTROL, app->notes_ui[row_num - 1].message.id);
        sparkToggle(SPARK_UI_CONTROL, app->notes_ui[row_num - 1].from.id);
    }
}

void InformerCreateNotesUI(void)
{
    InformerAppStruct *app = InformerGetApp();

    InformerHideAllNoteRows();
    sparkDisableParameter(SPARK_UI_CONTROL, app->notes_ui_sort.id);

    /* enable the text field for input */
    sparkEnableParameter(SPARK_UI_CONTROL, 14);  /* SparkString14 */
}

void InformerRefreshNotesUI(void)
{
    InformerAppStruct *app = InformerGetApp();

    int row;
    int index;
    int start = 0;
    int end = 0;

    InformerHideAllNoteRows();
    app->notes_ui_mode.ui->Value = INFORMER_NOTE_MODE_REFRESH;
    sparkEnableParameter(SPARK_UI_CONTROL, app->notes_ui_sort.id);

    printf("****** Time to refresh the notes UI ********\n");

    if (app->notes_data_count > 0) {
        if (1 == app->notes_data_count) {
            start = end = 1;
            sprintf(SparkString28.Value, "Displaying note 1 (of 1)");
        } else {
            start = (app->notes_ui_cur_page - 1) * app->notes_ui_count;
            if (app->notes_data_count - start < app->notes_ui_count) {
                /* if there is less than a page left, calculate partial */
                end = start - 1 + (app->notes_data_count - start) % app->notes_ui_count;
            } else {
                /* there is more than a full page left, max it out */
                end = start + app->notes_ui_count - 1;
            }

            sprintf(SparkString28.Value, "Displaying notes %d - %d (of %d)",
                    start + 1, end + 1, app->notes_data_count);
        }
    } else {
        sprintf(SparkString28.Value, "No notes to display");
    }

    for (row=1; row <= app->notes_ui_count; row++) {
        index = start + row - 1;
        if (index <= end) {
            printf("Going to display index [%d] at row [%d]\n", index, row);
            InformerUpdateNotesRowUI(app->notes_data[index], row);
        }
    }
}

void InformerUpdateNotesRowUI(InformerNoteData source, int row_num)
{
    InformerAppStruct *app = InformerGetApp();
    InformerNoteRowUI *note_ui;
    note_ui = &(app->notes_ui[row_num - 1]);

    printf("Truing to update row UI with row_num [%d]\n", row_num);
    note_ui->status.ui->Value = source.is_checked;
    if (1 == source.is_checked) {
        sprintf(note_ui->status.text, "by %s on %s", source.modified_by, source.modified_on);
    } else {
        sprintf(note_ui->status.text, "TODO");
    }

    note_ui->status.ui->Title = note_ui->status.text;
    sprintf(note_ui->message.ui->Value, "%s", source.text);
    sprintf(note_ui->from.ui->Value, "from %s at %s", source.created_by, source.created_on);
    InformerShowNoteRow(row_num);
}

