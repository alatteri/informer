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
#include <time.h>
#include <stdarg.h>

#define UI_NUM_ROWS 5
#define USERNAME_MAX 32
#define NOTE_NONE -1

extern int errno;

static char GATEWAY_STATUS_ERR[] = "Unable to get notes";
static char GET_NOTES_WAIT[] = "Getting notes from database...";
static char UPDATE_NOTE_WAIT[] = "Updating database...";
static char CREATE_NOTE_WAIT[] = "Creating new note...";
static char CURRENT_USER_ERR[] = "Unable to determine user";
static char CREATE_NOTE_UI[] = "                click here to create a new note";


/*************************************
 * Informer enums
 *************************************/
typedef enum {
    INFORMER_NOTE_MODE_REFRESH
} InformerNoteModeChoice;

typedef enum {
    INFORMER_NOTE_SORT_DATE_CREATED,
    INFORMER_NOTE_SORT_DATE_MODIFIED,
    INFORMER_NOTE_SORT_STATUS,
    INFORMER_NOTE_SORT_ARTIST,
    INFORMER_NOTE_SORT_AUTHOR
} InformerNoteSortChoice;

typedef enum {
    INFORMER_TABLE_BUTTON_A,
    INFORMER_TABLE_BUTTON_B
} InformerTableButtonChoice;

typedef enum {
    INFORMER_APP_MODE_SETUP,
    INFORMER_APP_MODE_NOTES,
    INFORMER_APP_MODE_ELEMS
} InformerAppModeChoice;


/*************************************
 * Informer notes data structure
 *************************************/
typedef struct {
    int                 id;                             /* Primary key of Note */
    char                text[4096];                     /* The actual note message */
    int                 is_checked;                     /* Boolean: is the note checked? */
    char                created_by[USERNAME_MAX];       /* The user who created the note */
    time_t              created_on;                     /* Date note was created */
    char                modified_by[USERNAME_MAX];      /* The user who last modified the note */
    time_t              modified_on;                    /* Date note was last modified */
} InformerNoteData;

typedef struct {
    int                 id;                             /* Primary key of the element */
    char                text[4096];                     /* the element path/message */
    int                 is_checked;                     /* is the element checked? */
    char                created_by[USERNAME_MAX];       /* the user who created the element */
    time_t              created_on;                     /* when the element was created */
} InformerElemData;

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
    unsigned int        id;                             /* the id of the button element */
    SparkPushStruct     *ui;                            /* prt to the ui element */
} InformerPushUI;

typedef struct {
    InformerBooleanUI   status;                         /* the status of the row */
    InformerStringUI    message;                        /* the row's message */
    InformerStringUI    from;                           /* who the row is from */
} InformerTableRowUI;

typedef struct {
    InformerTableRowUI          row[UI_NUM_ROWS];       /* Data to describe each UI row */
    unsigned int                row_count;              /* Total number of row structs */
    unsigned int                cur_page;               /* Current page number */

    InformerPupUI               sort;                   /* the sort popup */
    InformerPushUI              button_a;               /* the left button */
    InformerPushUI              button_b;               /* the right button */
    InformerStringUI            statusbar;              /* status bar for feedback/display */
} InformerTableUI;

typedef struct {
    InformerAppModeChoice       app_current_mode;       /* The current mode: setup, notes, elem */

    /* SETUP UI */
    InformerStringUI            setup_ui_setup_path;    /* the path of the batch setup file */

    /* NOTES DATA */
    InformerNoteData            notes_data[100];        /* All of the note data */
    unsigned int                notes_data_count;       /* Total number of NoteData structs */
    int                         notes_data_been_read;   /* has any note data been read? */

    /* NOTES UI */
    InformerPupUI               notes_ui_mode;          /* popup to select the note mode */
    InformerTableUI             notes_ui_table;         /* the data table for notes */
    InformerStringUI            notes_ui_create;        /* text field to input new notes */

    /* ELEMENTS DATA */
    InformerElemData            elems_data[100];        /* all the element data */
    unsigned int                elems_data_count;       /* Total number of ElemData structs */
    int                         elems_data_been_read;   /* has any elem data been read? */

    /* ELEMENTS UI */
    InformerPupUI               elems_ui_mode;          /* popup to select the element mode */
    InformerTableUI             elems_ui_table;         /* the data table for elements */
} InformerAppStruct;


/*************************************
 * Informer function prototypes
 *************************************/
void InformerDEBUG(const char *format, ...);
void InformerERROR(const char *format, ...);

/* app level functions */
InformerAppModeChoice InformerGetAppMode(InformerAppStruct *app);
void InformerSetAppMode(InformerAppStruct *app, InformerAppModeChoice mode);

int InformerSortNoteData(void);
int InformerCompareNoteByDateCreated(const void *a, const void *b);
int InformerCompareNoteByDateModified(const void *a, const void *b);
int InformerCompareNoteByStatus(const void *a, const void *b);
int InformerCompareNoteByArtist(const void *a, const void *b);
int InformerCompareNoteByAuthor(const void *a, const void *b);

InformerAppStruct *InformerGetApp(void);
InformerNoteData InformerInitNoteData(void);
InformerPupUI InformerInitPupUI(unsigned int id, SparkPupStruct *ui);
InformerPushUI InformerInitPushUI(unsigned int id, SparkPushStruct *ui);
InformerStringUI InformerInitStringUI(unsigned int id, SparkStringStruct *ui);
InformerBooleanUI InformerInitBooleanUI(unsigned int id, SparkBooleanStruct *ui, char *text);

const char *InformerGetSetupPath(void);
const char *InformerGetGatewayPath(void);
int InformerGetCurrentUser(char *user, int max_length);

InformerNoteModeChoice InformerGetNoteUIMode(void);
void InformerTableButtonEvent(InformerTableButtonChoice button);

int InformerExportNote(char *filepath, InformerNoteData *note);
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
void InformerUpdateNotesRowUI(InformerNoteData source, int row_num);

int InformerReadValAsInt(FILE *fp, int *data);
int InformerReadValAsLong(FILE *fp, long int *data);
int InformerReadValAsString(FILE *fp, char *data);

static unsigned long *InformerTableButtonA(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerTableButtonB(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesModeChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesSortChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerTableRow1BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerTableRow2BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerTableRow3BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerTableRow4BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerTableRow4BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerTableRow5BoolChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesCreateNoteCallback(int CallbackArg, SparkInfoStruct SparkInfo);
void InformerTableToggleRow(int row_num);

void InformerTableCanvasDraw(SparkCanvasInfo);
int  InformerTableCanvasInteract(SparkCanvasInfo Canvas, int PointerX, int PointerY, float PointerPressure);


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
SparkPupStruct SparkPup7 = {0, 5, InformerNotesSortChanged, {"Sort by date created",
                                                             "Sort by date modified",
                                                             "Sort by status",
                                                             "Sort by artist",
                                                             "Sort by author"}};
SparkStringStruct SparkString28 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkPushStruct SparkPush14 = { "<< Previous Page", InformerTableButtonA };
SparkPushStruct SparkPush21 = { "Next Page >>", InformerTableButtonB };

SparkPupStruct SparkPup34 = {0, 1, InformerNotesModeChanged, {"Refresh Notes"}};

SparkStringStruct SparkString13 = { "", CREATE_NOTE_UI, SPARK_FLAG_NONE, InformerNotesCreateNoteCallback };


/*************************************
 * Elements UI
 *************************************/
SparkPupStruct SparkPup36 = {0, 5, NULL, {"Sort by date added",
                                          "Sort by last modified",
                                          "Sort by status",
                                          "Sort by artist",
                                          "Sort by author"}};

SparkStringStruct SparkString57 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkPushStruct SparkPush43 = { "<< Previous Page", NULL };
SparkPushStruct SparkPush50 = { "Next Page >>", NULL };

SparkPupStruct SparkPup63 = {0, 1, NULL, {"Refresh Elements"}};

/* Informer Element Boolean CheckBoxes */
SparkBooleanStruct SparkBoolean37 = { 0, "", NULL };
SparkBooleanStruct SparkBoolean38 = { 0, "", NULL };
SparkBooleanStruct SparkBoolean39 = { 0, "", NULL };
SparkBooleanStruct SparkBoolean40 = { 0, "", NULL };
SparkBooleanStruct SparkBoolean41 = { 0, "", NULL };

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

static SparkMemBufStruct SparkResult;
static SparkMemBufStruct SparkSource;

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
    /* no temporary buffers */
}

/*--------------------------------------------------------------------------*/
/* Called when the spark object is first loaded.                            */
/* Return value indicates this spark operates at the MODULE level.          */
/*--------------------------------------------------------------------------*/
unsigned int
SparkInitialise(SparkInfoStruct spark_info)
{
    const char *setup;
    InformerAppStruct *app = InformerGetApp();
    InformerDEBUG("----> SparkInitialise called <----\n");

    double rate = sparkFrameRate();
    printf("This is the frame rate [%f]\n", rate);

    app->setup_ui_setup_path = InformerInitStringUI(15, &SparkSetupString15);

    /* Initialize the notes data container */
    app->notes_data_count = 0;
    app->notes_data_been_read = FALSE;

    /* Initialize the notes UI */
    app->notes_ui_mode = InformerInitPupUI(34, &SparkPup34);
    app->notes_ui_create = InformerInitStringUI(13, &SparkString13);

    app->notes_ui_table.cur_page = 1;
    app->notes_ui_table.row_count = UI_NUM_ROWS;

    app->notes_ui_table.sort = InformerInitPupUI(7, &SparkPup7);
    app->notes_ui_table.button_a  = InformerInitPushUI(14, &SparkPush14);
    app->notes_ui_table.button_b  = InformerInitPushUI(21, &SparkPush21);
    app->notes_ui_table.statusbar = InformerInitStringUI(28, &SparkString28);

    app->notes_ui_table.row[0].status = InformerInitBooleanUI(8, &SparkBoolean8, "");
    app->notes_ui_table.row[1].status = InformerInitBooleanUI(9, &SparkBoolean9, "");
    app->notes_ui_table.row[2].status = InformerInitBooleanUI(10, &SparkBoolean10, "");
    app->notes_ui_table.row[3].status = InformerInitBooleanUI(11, &SparkBoolean11, "");
    app->notes_ui_table.row[4].status = InformerInitBooleanUI(12, &SparkBoolean12, "");

    app->notes_ui_table.row[0].message = InformerInitStringUI(15, &SparkString15);
    app->notes_ui_table.row[1].message = InformerInitStringUI(16, &SparkString16);
    app->notes_ui_table.row[2].message = InformerInitStringUI(17, &SparkString17);
    app->notes_ui_table.row[3].message = InformerInitStringUI(18, &SparkString18);
    app->notes_ui_table.row[4].message = InformerInitStringUI(19, &SparkString19);

    app->notes_ui_table.row[0].from = InformerInitStringUI(29, &SparkString29);
    app->notes_ui_table.row[1].from = InformerInitStringUI(30, &SparkString30);
    app->notes_ui_table.row[2].from = InformerInitStringUI(31, &SparkString31);
    app->notes_ui_table.row[3].from = InformerInitStringUI(32, &SparkString32);
    app->notes_ui_table.row[4].from = InformerInitStringUI(33, &SparkString33);

    sparkControlTitle(SPARK_CONTROL_1, "Notes");
    sparkControlTitle(SPARK_CONTROL_2, "Elements");

    InformerSetAppMode(app, INFORMER_APP_MODE_NOTES);
    InformerHideAllNoteRows();
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

/*--------------------------------------------------------------------------*/
/* This function is used to trigger the initial database query for the      */
/* current spark state.                                                     */
/* Return: indicates the number of frames to be generated for the current   */
/* input frame.                                                             */
/*--------------------------------------------------------------------------*/
int
SparkProcessStart(SparkInfoStruct spark_info)
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
unsigned long *
SparkProcess(SparkInfoStruct spark_info)
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
void
SparkEvent(SparkModuleEvent spark_event)
{
    InformerAppStruct *app = InformerGetApp();

    /* I don't have a use for this right now... */
    InformerDEBUG("-----> SparkEvent (%d) <-----\n", spark_event);

    if (7 == spark_event) {
        InformerSetAppMode(app, INFORMER_APP_MODE_SETUP);
    } else if (9 == spark_event) {
        InformerSetAppMode(app, INFORMER_APP_MODE_NOTES);
    } else if (10 == spark_event) {
        InformerSetAppMode(app, INFORMER_APP_MODE_ELEMS);
    }
}

/*--------------------------------------------------------------------------*/
/* Callback for setup saving and loading                                    */
/*--------------------------------------------------------------------------*/
void
SparkSetupIOEvent(SparkModuleEvent event, char *path, char *file)
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

    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);

    fprintf(stderr, "ERROR: %s", str);
    sparkError(str);
}

InformerNoteData
InformerInitNoteData(void)
{
    InformerNoteData result;
    result.id = NOTE_NONE;
    result.is_checked = NOTE_NONE;
    sprintf(result.text, "");
    sprintf(result.created_by, "");
    result.created_on = NOTE_NONE;
    sprintf(result.modified_by, "");
    result.modified_on = NOTE_NONE;
    return result;
}

InformerPupUI
InformerInitPupUI(unsigned int id, SparkPupStruct *ui)
{
    InformerPupUI result;
    result.id = id;
    result.ui = ui;
    return result;
}

InformerPushUI
InformerInitPushUI(unsigned int id, SparkPushStruct *ui)
{
    InformerPushUI result;
    result.id = id;
    result.ui = ui;
    return result;
}

InformerStringUI
InformerInitStringUI(unsigned int id, SparkStringStruct *ui)
{
    InformerStringUI result;
    result.id = id;
    result.ui = ui;
    return result;
}

InformerBooleanUI
InformerInitBooleanUI(unsigned int id, SparkBooleanStruct *ui, char *text)
{
    InformerBooleanUI result;
    result.id = id;
    result.ui = ui;
    sprintf(result.text, "%s", text);
    return result;
}

int
InformerGetNotesDB(void)
{
    InformerAppStruct *app = InformerGetApp();
    sparkMessage(GET_NOTES_WAIT);
    InformerHideAllNoteRows();

    if (TRUE == InformerCallGateway("get_notes", NULL, "/tmp/trinity")) {
        if (TRUE == InformerImportNotes("/tmp/trinity", 0, TRUE)) {
            app->notes_data_been_read = TRUE;
            InformerRefreshNotesUI();
            return TRUE;
        } else {
            // TODO: What should happen here?
            InformerERROR("%s", GET_NOTES_WAIT);
            return FALSE;
        }
    }
}

int
InformerUpdateNoteDB(int index, int note_id, int is_checked)
{
    InformerNoteData note_data = InformerInitNoteData();

    sparkMessage(UPDATE_NOTE_WAIT);

    if (TRUE != InformerGetCurrentUser(note_data.modified_by, USERNAME_MAX)) {
        InformerERROR("Unable to determine the current user\n");
        return FALSE;
    }

    note_data.id = note_id;
    note_data.is_checked = is_checked;

    if (TRUE == InformerExportNote("/tmp/trinity2", &note_data)) {
        InformerDEBUG("--------- update note ----------\n");
        InformerDEBUG("is_checked [%d], user [%s], id [%d], index [%d]\n",
               note_data.is_checked, note_data.modified_by, note_data.id, index);
        InformerDEBUG("--------- update note ----------\n");
        if (TRUE == InformerCallGateway("update_note", "/tmp/trinity2", "/tmp/trinity3")) {
            if (TRUE == InformerImportNotes("/tmp/trinity3", index, FALSE)) {
                InformerRefreshNotesUI();
                return TRUE;
            }
        }
    }

    return FALSE;
}

int
InformerCallGateway(char *action, char *infile, char *outfile)
{
    int pid = 0;
    int index = 0;
    int status = 0;
    const char *setup;
    const char *gateway;
    const char *argv[10];
    const char **ptr = argv;

    setup = InformerGetSetupPath();
    gateway = InformerGetGatewayPath();

    InformerDEBUG("the gateway is [%s]\n", gateway);

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

    InformerDEBUG("---- calling gateway ----\n");
    while (*ptr) {
        InformerDEBUG("%s ", *ptr++);
    }
    InformerDEBUG("\n---- calling gateway ----\n");

    pid = sparkSystemNoSh(FALSE, argv[0], argv);
    sparkWaitpid(pid, &status, 0);
    InformerDEBUG("Gateway returned. status [%d]\n", status);

    if (0 != status) {
        // TODO: Map the status to a human readable string
        InformerERROR("%s: status [%d]", GATEWAY_STATUS_ERR, status);
        return FALSE;
    } else {
        return TRUE;
    }
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int
InformerExportNote(char *filepath, InformerNoteData *note)
{
    FILE *fp;
    int result = 1;

    InformerDEBUG("here we go. writing datafile [%s]\n", filepath);

    if ((fp=fopen(filepath, "w")) == NULL) {
        InformerERROR("%s: can't write datafile [%s]",
                      GATEWAY_STATUS_ERR, filepath);
        return FALSE;
    }

    InformerDEBUG("-------- OK -------- opened the file for writing!\n");

    if (result > 0) {
        result = fprintf(fp, "1\n");
    }

    if (result > 0 && note->id != NOTE_NONE) {
        result = fprintf(fp, "id: %d\n", note->id);
    }

    if (result > 0 && note->is_checked != NOTE_NONE) {
        result = fprintf(fp, "is_checked: %d\n", note->is_checked);
    }

    if (result > 0 && 0 != strcmp("", note->created_by)) {
        result = fprintf(fp, "created_by: %s\n", note->created_by);
    }

    if (result > 0 && 0 != strcmp("", note->modified_by)) {
        result = fprintf(fp, "modified_by: %s\n", note->modified_by);
    }

    if (result > 0 && 0 != strcmp("", note->text)) {
        result = fprintf(fp, "text: %s\n", note->text);
    }

    fclose(fp);

    if (!result) {
        InformerERROR("%s: can't write datafile [%s]",
                      GATEWAY_STATUS_ERR, filepath);
        return FALSE;
    }

    InformerDEBUG("... write of note ok!\n");
    return TRUE;
}

int
InformerImportNotes(char *filepath, int index, int update_count)
{
    FILE *fp;
    int i = 0;
    int count = 0;
    int result = 1;
    InformerAppStruct *app = InformerGetApp();

    InformerDEBUG("ImportNotes: here we go. reading datafile [%s], index [%d], update? [%d]\n",
                  filepath, index, update_count);

    if ((fp=fopen(filepath, "r")) == NULL) {
        InformerERROR("%s: can't open datafile [%s]: %s",
                      GATEWAY_STATUS_ERR, filepath, strerror(errno));
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
        InformerERROR("%s: can't parse datafile [%s]", GATEWAY_STATUS_ERR, filepath);
        return FALSE;
    }

    InformerDEBUG("OK there are %d entries\n", count);

    for (i=0; i<count; i++) {
        /* skip the "key:" read the value */
        if (TRUE == InformerReadValAsInt(fp,    &(app->notes_data[index+i].id)) &&
            TRUE == InformerReadValAsString(fp, app->notes_data[index+i].text) &&
            TRUE == InformerReadValAsInt(fp,    &(app->notes_data[index+i].is_checked)) &&
            TRUE == InformerReadValAsString(fp, app->notes_data[index+i].created_by) &&
            TRUE == InformerReadValAsLong(fp,   &(app->notes_data[index+i].created_on)) &&
            TRUE == InformerReadValAsString(fp, app->notes_data[index+i].modified_by) &&
            TRUE == InformerReadValAsLong(fp,   &(app->notes_data[index+i].modified_on))) {
            /* looking good -- keep going */
            InformerDEBUG("... read note [%d] ok!\n", i);
        } else {
            InformerERROR("%s: can't parse datafile [%s]", GATEWAY_STATUS_ERR, filepath);
            return FALSE;
        }
    }

    if (TRUE == update_count) {
        app->notes_ui_table.cur_page = 1;
        app->notes_data_count = count;
    }

    InformerSortNoteData();
    InformerDEBUG("All notes read ok. Word up.\n");

    return TRUE;
}

/****************************************************************************
 *                      Informer UI Function Calls                          *
 ****************************************************************************/
int
InformerRowToIndex(int row_num)
{
    InformerAppStruct *app = InformerGetApp();
    return (app->notes_ui_table.cur_page - 1) * app->notes_ui_table.row_count + row_num - 1;
}

InformerNoteModeChoice
InformerGetNoteUIMode(void)
{
    InformerAppStruct *app = InformerGetApp();
    return (InformerNoteModeChoice) app->notes_ui_mode.ui->Value;
}

static unsigned long *
InformerNotesModeChanged(int CallbackArg, SparkInfoStruct SparkInfo)
{
    InformerAppStruct *app = InformerGetApp();
    InformerNoteModeChoice mode = InformerGetNoteUIMode();

    InformerDEBUG("Hey! The notes mode change got called with: value: %d\n", mode);

    if (INFORMER_NOTE_MODE_REFRESH == mode) {
        InformerGetNotesDB();
    }

    return NULL;
}

static unsigned long *
InformerNotesSortChanged(int CallbackArg, SparkInfoStruct SparkInfo)
{
    InformerAppStruct *app = InformerGetApp();
    InformerDEBUG("Hey! The notes sort change got called with: %d, value is: %d\n",
            CallbackArg, app->notes_ui_table.sort.ui->Value);
    InformerSortNoteData();
    InformerRefreshNotesUI();
    return NULL;
}


void
InformerHideAllNoteRows(void)
{
    int i;
    for (i=1; i<=UI_NUM_ROWS; i++) {
        InformerHideNoteRow(i);
    }
}

void
InformerHideNoteRow(int row_num)
{
    InformerDEBUG("++++ call to disable note row %d\n", row_num);
    InformerToggleNoteRow(row_num, 0);
}

void
InformerShowNoteRow(int row_num)
{
    InformerDEBUG("++++ call to enable note row %d\n", row_num);
    InformerToggleNoteRow(row_num, 1);
}

void
InformerToggleNoteRow(int row_num, int on_off)
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
        sparkToggle(SPARK_UI_CONTROL, app->notes_ui_table.row[row_num - 1].status.id);
        sparkToggle(SPARK_UI_CONTROL, app->notes_ui_table.row[row_num - 1].message.id);
        sparkToggle(SPARK_UI_CONTROL, app->notes_ui_table.row[row_num - 1].from.id);
    }
}

void
InformerRefreshNotesUI(void)
{
    InformerAppStruct *app = InformerGetApp();

    int row;
    int index;
    int start = 0;
    int end = 0;

    InformerHideAllNoteRows();
    app->notes_ui_mode.ui->Value = INFORMER_NOTE_MODE_REFRESH;
    sparkEnableParameter(SPARK_UI_CONTROL, app->notes_ui_table.sort.id);

    InformerDEBUG("****** Time to refresh the notes UI ********\n");
    InformerDEBUG("I think there are [%d] notes\n", app->notes_data_count);

    if (app->notes_data_count > 0) {
        if (1 == app->notes_data_count) {
            start = end = 1;
            sprintf(app->notes_ui_table.statusbar.ui->Value, "Displaying note 1 (of 1)");
        } else {
            start = (app->notes_ui_table.cur_page - 1) * app->notes_ui_table.row_count;
            if (app->notes_data_count - start < app->notes_ui_table.row_count) {
                /* if there is less than a page left, calculate partial */
                end = start - 1 + (app->notes_data_count - start) % app->notes_ui_table.row_count;
            } else {
                /* there is more than a full page left, max it out */
                end = start + app->notes_ui_table.row_count - 1;
            }

            sprintf(app->notes_ui_table.statusbar.ui->Value,
                    "Displaying notes %d - %d (of %d)",
                    start + 1, end + 1, app->notes_data_count);
        }

        for (row=1; row <= app->notes_ui_table.row_count; row++) {
            index = start + row - 1;
            if (index <= end) {
                InformerDEBUG("Going to display index [%d] at row [%d]\n", index, row);
                InformerUpdateNotesRowUI(app->notes_data[index], row);
            }
        }
    } else {
        sprintf(app->notes_ui_table.statusbar.ui->Value, "No notes to display");
    }
}

void
InformerUpdateNotesRowUI(InformerNoteData source, int row_num)
{
    char str[32];
    InformerAppStruct *app = InformerGetApp();
    InformerTableRowUI *note_ui;

    note_ui = &(app->notes_ui_table.row[row_num - 1]);

    InformerDEBUG("Truing to update row UI with row_num [%d]\n", row_num);
    note_ui->status.ui->Value = source.is_checked;
    if (1 == source.is_checked) {
        strftime(str, sizeof(str), "%m/%d", localtime(&source.modified_on));
        sprintf(note_ui->status.text, "by %s on %s", source.modified_by, str);
    } else {
        sprintf(note_ui->status.text, "TODO");
    }

    note_ui->status.ui->Title = note_ui->status.text;
    sprintf(note_ui->message.ui->Value, "%s", source.text);

    strftime(str, sizeof(str), "%D %I:%M%p", localtime(&source.created_on));
    sprintf(note_ui->from.ui->Value, "from %s at %s", source.created_by, str);

    InformerShowNoteRow(row_num);
}

static unsigned long *
InformerNotesCreateNoteCallback(int CallbackArg, SparkInfoStruct SparkInfo)
{
    char *input;
    InformerAppStruct *app = InformerGetApp();
    InformerNoteData note_data = InformerInitNoteData();

    input = app->notes_ui_create.ui->Value;

    if (strncmp("", input, SPARK_MAX_STRING_LENGTH) == 0) {
        // ignore -- nothing was entered
        InformerDEBUG("Ignoring empty input\n");
    } else {
        sparkMessage(CREATE_NOTE_WAIT);
        InformerDEBUG("New note text was entered\n");
        InformerDEBUG("The text value is: [%s]\n", input);

        if (TRUE != InformerGetCurrentUser(note_data.created_by, USERNAME_MAX)) {
            InformerERROR("Unable to determine the current user\n");
            return FALSE;
        }

        note_data.is_checked = 0;
        sprintf(note_data.text, "%s", input);
        sprintf(note_data.modified_by, "%s", note_data.created_by);

        if (TRUE == InformerExportNote("/tmp/trinity8", &note_data)) {
            if (TRUE == InformerCallGateway("new_note", "/tmp/trinity8", NULL)) {
                InformerGetNotesDB();
            }
        }
    }

    return NULL;
}

int
InformerSortNoteData(void)
{
    InformerAppStruct *app = InformerGetApp();
    InformerNoteSortChoice sort_by = app->notes_ui_table.sort.ui->Value;
    int (*compare)(const void *, const void *) = NULL;

    if (app->notes_data_count < 2) {
        InformerDEBUG("Less than 2 data elements -- can't sort\n");
        return TRUE;
    } else if (INFORMER_NOTE_SORT_DATE_CREATED == sort_by) {
        compare = InformerCompareNoteByDateCreated;
    } else if (INFORMER_NOTE_SORT_DATE_MODIFIED == sort_by) {
        compare = InformerCompareNoteByDateModified;
    } else if (INFORMER_NOTE_SORT_STATUS == sort_by) {
        compare = InformerCompareNoteByStatus;
    } else if (INFORMER_NOTE_SORT_ARTIST == sort_by) {
        compare = InformerCompareNoteByArtist;
    } else if (INFORMER_NOTE_SORT_AUTHOR == sort_by) {
        compare = InformerCompareNoteByAuthor;
    } else {
        InformerERROR("Unable to sort: unknown menu value [%d]\n", sort_by);
        return FALSE;
    }

    InformerDEBUG("Now going to call qsort\n");
    qsort(app->notes_data, app->notes_data_count, sizeof(InformerNoteData), compare);
    return TRUE;
}

int
InformerCompareNoteByDateCreated(const void *a, const void *b)
{
    InformerNoteData *note_a = (InformerNoteData*) a;
    InformerNoteData *note_b = (InformerNoteData*) b;

    InformerDEBUG("InformerCompareNoteByDateCreated called\n");

    if (note_a->created_on > note_b->created_on)
        return -1;
    else if (note_a->created_on < note_b->created_on)
        return 1;
    else
        return 0;
}

int
InformerCompareNoteByDateModified(const void *a, const void *b)
{
    InformerNoteData *note_a = (InformerNoteData*) a;
    InformerNoteData *note_b = (InformerNoteData*) b;

    InformerDEBUG("InformerCompareNoteByDateModified called\n");

    if (note_a->modified_on > note_b->modified_on)
        return -1;
    else if (note_a->modified_on < note_b->modified_on)
        return 1;
    else
        return 0;

}

int
InformerCompareNoteByStatus(const void *a, const void *b)
{
    InformerNoteData *note_a = (InformerNoteData*) a;
    InformerNoteData *note_b = (InformerNoteData*) b;

    InformerDEBUG("InformerCompareNoteByStatus called\n");

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
InformerCompareNoteByArtist(const void *a, const void *b)
{
    int result;

    InformerNoteData *note_a = (InformerNoteData*) a;
    InformerNoteData *note_b = (InformerNoteData*) b;

    InformerDEBUG("InformerCompareNoteByArtist called\n");

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
InformerCompareNoteByAuthor(const void *a, const void *b)
{
    int result;

    InformerNoteData *note_a = (InformerNoteData*) a;
    InformerNoteData *note_b = (InformerNoteData*) b;

    InformerDEBUG("InformerCompareNoteByAuthor called\n");

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
InformerReadValAsInt(FILE *fp, int *data)
{
    char line[4096];

    if (TRUE == InformerReadValAsString(fp, line)) {
        *data = atoi(line);
        return TRUE;
    } else {
        return FALSE;
    }
}

int
InformerReadValAsLong(FILE *fp, long int *data)
{
    char line[4096];

    if (TRUE == InformerReadValAsString(fp, line)) {
        *data = atol(line);
        return TRUE;
    } else {
        return FALSE;
    }
}

int
InformerReadValAsString(FILE *fp, char *data)
{
    int length;
    char *start;
    char *end;
    char line[4096];

    if (fgets(line, 4096, fp) != NULL) {
        printf("The line is [%s]\n", line);
        start = index(line, ':');
        if (start != NULL) {
            start += 2;     /* the space after the : */
            length = strlen(line);
            end = line + length - 1;    /* no null and no newline */
            strncpy(data, start, end - start);
            data[end - start] = '\0';
            printf("I read [%s]\n", data);
            return TRUE;
        } else {
            InformerERROR("Parse error reading from filehandle: no key found\n");
            return FALSE;
        }
    } else {
        InformerERROR("Unable to read from filehandle: %s\n", strerror(errno));
        return FALSE;
    }
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

    printf("------------------------------------------------\n");
    printf("app->setup_ui_setup_path---> [%s]\n", app->setup_ui_setup_path.ui->Value);
    printf("sparkGetLastSetupName --> [%s]\n", setup);
    printf("SparkInfoStruct.Name ---> [%s]\n", info.Name);
    printf("------------------------------------------------\n");
    return app->setup_ui_setup_path.ui->Value;
}

/*--------------------------------------------------------------------------*/
/* Returns path to the informer gateway script                              */
/*--------------------------------------------------------------------------*/
const char *
InformerGetGatewayPath(void)
{
    const char *path = "/usr/discreet/instinctual/informer/bin/gateway";
    return path;
}

/*--------------------------------------------------------------------------*/
/* Get the current spark user into the char* argument                       */
/* Returns TRUE on success, FALSE on faliure                                */
/*--------------------------------------------------------------------------*/
int
InformerGetCurrentUser(char *user, int max_length)
{
    FILE *fp;
    char line[1024];
    const char *program;
    const char *filepath;

    char *c;
    char *start;
    char *end;

    program = sparkProgramGetName();

    if (0 == strcmp("flame", program) ||
        0 == strcmp("flint", program) ||
        0 == strcmp("inferno", program)) {
        filepath = "/usr/discreet/user/effects/user.db";
    } else if (0 == strcmp("fire", program) ||
               0 == strcmp("smoke", program)) {
        filepath = "/usr/discreet/user/editing/user.db";
    } else {
        InformerERROR("%s: unknown program [%s]", CURRENT_USER_ERR, program);
        return FALSE;
    }

    if ((fp=fopen(filepath, "r")) == NULL) {
        InformerERROR("%s: can't open [%s]: %s", CURRENT_USER_ERR,
                      filepath, strerror(errno));
        return FALSE;
    }

    max_length -= 1; /* account for the null */

    do {
        c = fgets(line, 1024, fp);
        if (c != NULL) {
            InformerDEBUG("Checking line: %s\n", line);
            if (strstr(line, "UserGroupStatus:UsrGroup1={") != NULL) {
                InformerDEBUG("MATCH! -> %s\n", line);
                start = index(line, '{');
                end = rindex(line, '}');
                if (end != NULL && start != NULL) {
                    if (end - start < max_length - 1)
                        max_length = end - start;
                    InformerDEBUG("start [%c], end [%c], max_length [%d]\n", *start, *end, max_length);

                    strncpy(user, start + 1, max_length - 1);
                    user[max_length-1] = '\0';
                    InformerDEBUG("Going to return user [%s]\n", user);
                    return TRUE;
                }
            }
        }
    } while (c != NULL);


    return FALSE;
}


InformerAppModeChoice
InformerGetAppMode(InformerAppStruct *app)
{
    return app->app_current_mode;
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
    InformerAppStruct *app = InformerGetApp();

    int note_id;
    int is_checked;
    int index = InformerRowToIndex(row_num);
    InformerNoteModeChoice mode = InformerGetNoteUIMode();

    if (mode == INFORMER_NOTE_MODE_REFRESH && index < app->notes_data_count) {
        note_id = app->notes_data[index].id;
        is_checked = app->notes_ui_table.row[row_num-1].status.ui->Value;

        InformerDEBUG("It's note_id is: (%d)\n", note_id);
        InformerDEBUG("Row number (%d) was changed\n", row_num);
        InformerDEBUG("It's value is: %d\n", is_checked);

        InformerUpdateNoteDB(index, note_id, is_checked);
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
    int max_page;
    InformerAppStruct *app = InformerGetApp();
    InformerNoteModeChoice mode = InformerGetNoteUIMode();

    if (INFORMER_NOTE_MODE_REFRESH == mode) {
        if (INFORMER_TABLE_BUTTON_A == button) {
            InformerDEBUG("You clicked Notes Page Prev\n");
            if (app->notes_ui_table.cur_page > 1) {
                app->notes_ui_table.cur_page -= 1;
                InformerRefreshNotesUI();
            } else {
                InformerDEBUG("You can't go past page 1\n");
            }
        } else if (INFORMER_TABLE_BUTTON_B == button) {
            InformerDEBUG("You clicked Notes Page Next\n");
            max_page = (int) ceil((float)app->notes_data_count/app->notes_ui_table.row_count);
            if (app->notes_ui_table.cur_page < max_page) {
                app->notes_ui_table.cur_page += 1;
                InformerRefreshNotesUI();
            } else {
                InformerDEBUG("You can't go past the last page (%d)\n", max_page);
            }
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
    InformerDEBUG("--- interact event called x: %d, y: %d, pressure %f ---\n", x, y, pressure);
    /* return 1 for canvas display */
    return 1;
}

