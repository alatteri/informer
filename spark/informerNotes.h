#ifndef _INFORMER_NOTES_H
#define _INFORMER_NOTES_H


#include "spark.h"

#define NOTES_NONE -1
#define NOTES_MAX_USER_LENGTH 32

/*************************************
 * Informer notes data structure
 *************************************/
typedef struct {
    int             id;                          /* Primary key of Note */
    char            text[4096];                  /* The actual note message */
    int             is_checked;                  /* Boolean: is the note checked? */
                                                 /* The user who created the note */
    char            created_by[NOTES_MAX_USER_LENGTH];
    time_t          created_on;                  /* Date note was created */
                                                 /* The user who last modified the note */
    char            modified_by[NOTES_MAX_USER_LENGTH];
    time_t          modified_on;                 /* Date note was last modified */
} InformerNoteData;

void InformerInitNotesData(void);
InformerNoteData InformerInitNoteData(void);

int InformerSortNoteData(void);
int InformerCompareNoteByDateCreated(const void *a, const void *b);
int InformerCompareNoteByDateModified(const void *a, const void *b);
int InformerCompareNoteByStatus(const void *a, const void *b);
int InformerCompareNoteByArtist(const void *a, const void *b);
int InformerCompareNoteByAuthor(const void *a, const void *b);

int InformerExportNote(char *filepath, InformerNoteData *note);
int InformerImportNotes(char *filepath, int index, int update_count);
int InformerGetNotesDB(void);
int InformerUpdateNoteDB(int index, int note_id, int is_checked);

#endif /* _INFORMER_NOTES_H */
