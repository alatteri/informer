/****************************************************************************
 *                                 Informer                                 *
 ****************************************************************************/
#ifndef _INFORMER_H
#define _INFORMER_H

#include "spark.h"

#include <time.h>

#define UI_NUM_ROWS 5
#define USERNAME_MAX 32
#define DATA_NONE -1
#define INFORMER_MAX_STR_LEN 4096


/*************************************
 * Informer enums
 *************************************/
typedef enum {
    INFORMER_NOTE_MODE_REFRESH
} InformerNoteModeChoice;

typedef enum {
    INFORMER_ELEM_MODE_REFRESH
} InformerElemModeChoice;

typedef enum {
    INFORMER_NOTE_SORT_DATE_CREATED,
    INFORMER_NOTE_SORT_DATE_MODIFIED,
    INFORMER_NOTE_SORT_STATUS,
    INFORMER_NOTE_SORT_ARTIST,
    INFORMER_NOTE_SORT_AUTHOR
} InformerNoteSortChoice;


typedef enum {
    INFORMER_ELEM_SORT_DATE_CREATED,
    INFORMER_ELEM_SORT_STATUS,
    INFORMER_ELEM_SORT_KIND,
    INFORMER_ELEM_SORT_AUTHOR
} InformerElemSortChoice;

typedef enum {
    INFORMER_TABLE_BUTTON_A,
    INFORMER_TABLE_BUTTON_B
} InformerTableButtonChoice;

typedef enum {
    INFORMER_APP_MODE_SETUP,
    INFORMER_APP_MODE_NOTES,
    INFORMER_APP_MODE_ELEMS
} InformerAppModeChoice;

typedef enum {
    INFORMER_APP_STATE_OK,
    INFORMER_APP_STATE_ERR
} InformerAppStateChoice;

/*************************************
 * Informer notes data structure
 *************************************/
typedef struct {
    int                 id;                             /* Primary key of Note */
    char                text[INFORMER_MAX_STR_LEN];     /* The actual note message */
    int                 is_checked;                     /* Boolean: is the note checked? */
    char                created_by[USERNAME_MAX];       /* The user who created the note */
    time_t              created_on;                     /* Date note was created */
    char                modified_by[USERNAME_MAX];      /* The user who last modified the note */
    time_t              modified_on;                    /* Date note was last modified */
} InformerNoteData;

typedef struct {
    int                 id;                             /* Primary key of the element */
    char                kind[SPARK_MAX_STRING_LENGTH];  /* the kind of the element */
    char                text[INFORMER_MAX_STR_LEN];     /* the element path/message */
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
    int                         app_state;              /* the app state: OK or ERR */
    InformerAppModeChoice       app_current_mode;       /* The current mode: setup, notes, elem */

    /* SETUP UI */
    InformerStringUI            setup_ui_setup_path;    /* the path of the batch setup file */
    InformerStringUI            setup_ui_spark_name;    /* the name of the spark */

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
const char * InformerGetAppModeStr(InformerAppStruct *app);
void InformerSetAppMode(InformerAppStruct *app, InformerAppModeChoice mode);
InformerTableUI * InformerGetTableUI(InformerAppStruct *app);
int InformerGetDataCount(InformerAppStruct *app);
int InformerTableGetCurPage(InformerTableUI *table);
void InformerTableSetCurPage(InformerTableUI *table, int cur_page);
int InformerTableGetPageOfIndex(InformerTableUI *table, int data_index);
int InformerIsAppStateOK(InformerAppStruct *app);
void InformerSetAppState(InformerAppStruct *app, InformerAppStateChoice state);

int InformerNoteDataSort(void);
int InformerNoteDataCompareByDateCreated(const void *a, const void *b);
int InformerNoteDataCompareByDateModified(const void *a, const void *b);
int InformerNoteDataCompareByStatus(const void *a, const void *b);
int InformerNoteDataCompareByArtist(const void *a, const void *b);
int InformerNoteDataCompareByAuthor(const void *a, const void *b);

int InformerElemDataSort(void);
int InformerElemDataCompareByKind(const void *a, const void *b);
int InformerElemDataCompareByAuthor(const void *a, const void *b);
int InformerElemDataCompareByStatus(const void *a, const void *b);
int InformerElemDataCompareByDateCreated(const void *a, const void *b);

InformerAppStruct *InformerGetApp(void);
InformerNoteData InformerCreateNoteData(void);
InformerElemData InformerCreateElemData(void);
InformerPupUI InformerCreatePupUI(unsigned int id, SparkPupStruct *ui);
InformerPushUI InformerCreatePushUI(unsigned int id, SparkPushStruct *ui);
InformerStringUI InformerCreateStringUI(unsigned int id, SparkStringStruct *ui, char *text);
InformerBooleanUI InformerCreateBooleanUI(unsigned int id, SparkBooleanStruct *ui, char *text);

const char *InformerGetSetupPath(void);
void InformerSetSetupPath(char *path);
const char *InformerGetSparkName(void);
void InformerSetSparkName(char *name);
int InformerGetCurrentUser(char *user, int max_length);

InformerNoteModeChoice InformerGetNoteUIMode(void);
void InformerTableButtonEvent(InformerTableButtonChoice button);

int InformerGetNotes(void);
int InformerGetElems(void);
int InformerUpdateNote(int index, int id, int is_checked);
int InformerUpdateElem(int index, int id, int is_checked);

int InformerTableRowToIndex(int row_num);
void InformerTableRowShow(InformerTableRowUI *row);
void InformerTableRowHide(InformerTableRowUI *row);
void InformerTableRowSetFromText(InformerTableRowUI *row, const char *format, ...);
void InformerTableRowSetMessageText(InformerTableRowUI *row, const char *format, ...);
void InformerTableRowSetStatusValue(InformerTableRowUI *row, int is_checked);
void InformerTableRowSetStatusText(InformerTableRowUI *row, const char *format, ...);

void InformerTableHideAllRows(void);
void InformerTableRefreshUI(void);

void InformerTableRowUpdateUI(int data_index, int row_num);
void InformerTableRowUpdateUIWithNotes(int data_index, int row_num);
void InformerTableRowUpdateUIWithElems(int data_index, int row_num);

int InformerFileReadValAsInt(FILE *fp, int *data);
int InformerFileReadValAsLong(FILE *fp, long int *data);
int InformerFileReadValAsString(FILE *fp, char *data);

static unsigned long *InformerNotesModeChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerNotesSortChanged(int CallbackArg, SparkInfoStruct SparkInfo);

static unsigned long *InformerElemsModeChanged(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerElemsSortChanged(int CallbackArg, SparkInfoStruct SparkInfo);

static unsigned long *InformerTableButtonA(int CallbackArg, SparkInfoStruct SparkInfo);
static unsigned long *InformerTableButtonB(int CallbackArg, SparkInfoStruct SparkInfo);
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

void InformerTableSetStatus(InformerTableUI *table, const char *format, ...);

#endif /*_INFORMER_H */
