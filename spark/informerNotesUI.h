#ifndef _INFORMER_NOTES_UI_H
#define _INFORMER_NOTES_UI_H


#include "informerNotes.h"
#include "informerUI.h"

#define NOTES_UI_NUM_ROWS 5

typedef struct {
    InformerBooleanUI   status;                         /* the status of the note */
    InformerStringUI    message;                        /* the note's message */
    InformerStringUI    from;                           /* who the note is from */
} InformerNoteRowUI;

typedef enum {
    INFORMER_NOTE_MODE_REFRESH = 0
} InformerNoteModeChoice;

typedef enum {
    INFORMER_NOTE_SORT_DATE_CREATED = 0,
    INFORMER_NOTE_SORT_DATE_MODIFIED = 1,
    INFORMER_NOTE_SORT_STATUS = 2,
    INFORMER_NOTE_SORT_ARTIST = 3,
    INFORMER_NOTE_SORT_AUTHOR = 4
} InformerNoteSortChoice;

typedef enum {
    INFORMER_NOTE_BUTTON_A = 0,
    INFORMER_NOTE_BUTTON_B = 1
} InformerNoteButtonChoice;

InformerNoteModeChoice InformerGetNoteUIMode(void);
void InformerNotesButtonEvent(InformerNoteButtonChoice button);
void InformerShowNoteRow(int row_num);
void InformerHideNoteRow(int row_num);
void InformerHideAllNoteRows(void);
void InformerToggleNoteRow(int row_num, int on_off);
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
static unsigned long *InformerNotesCreateNoteCallback(int CallbackArg, SparkInfoStruct SparkInfo);
void InformerNotesToggleRow(int row_num);

void CanvasDraw(SparkCanvasInfo);
int  CanvasInteract(SparkCanvasInfo Canvas,
                    int PointerX, int PointerY,
                    float PointerPressure); /* return 1 for canvas display */

void InformerInitNotesUI(void);

#endif /* _INFORMER_NOTES_UI_H */
