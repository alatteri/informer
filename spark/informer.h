#ifndef _INFORMER_H
#define _INFORMER_H

#include "informerNotes.h"
#include "informerUI.h"
#include "informerNotesUI.h"

typedef struct {
    /* SETUP UI ELEMENTS */
    InformerStringUI            setup_ui_setup_path;    /* the path of the batch setup file */

    /* NOTE DATA */
    InformerNoteData            notes_data[100];        /* All of the note data */
    unsigned int                notes_data_count;       /* Total number of NoteData structs */
    int                         notes_data_been_read;   /* has any note data been read? */

    /* NOTE UI ELEMENTS */
                                                        /* Data to describe each UI row */
    InformerNoteRowUI           notes_ui[NOTES_UI_NUM_ROWS];
    unsigned int                notes_ui_count;         /* Total number of NoteUI structs */
    unsigned int                notes_ui_cur_page;      /* Current note page number */

    InformerPupUI               notes_ui_mode;          /* popup to select the note mode */
    InformerPupUI               notes_ui_sort;          /* the sort popup */
    InformerStringUI            notes_ui_create;        /* text field to input new notes */
    InformerPushUI              notes_ui_button_a;      /* the left button */
    InformerPushUI              notes_ui_button_b;      /* the right button */
    InformerStringUI            notes_ui_statusbar;     /* status bar for feedback/display */
} InformerAppStruct;

void InformerDEBUG(const char *format, ...);
void InformerERROR(const char *format, ...);

InformerAppStruct *InformerGetApp(void);
const char *InformerGetSetupPath(void);
const char *InformerGetGatewayPath(void);

int InformerGetCurrentUser(char *user, int max_length);
int InformerCallGateway(char *action, char *infile, char *outfile);
int InformerReadValAsInt(FILE *fp, int *data);
int InformerReadValAsLong(FILE *fp, long int *data);
int InformerReadValAsString(FILE *fp, char *data);


#endif /* _INFORMER_H */
