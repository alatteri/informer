#include <stdlib.h>
#include <errno.h>

#include "informer.h"

extern int errno;

static char GET_NOTES_WAIT[] = "Getting notes from database...";
static char UPDATE_NOTE_WAIT[] = "Updating database...";


void InformerInitNotesData(void)
{
    InformerAppStruct *app = InformerGetApp();

    /* Initialize the notes data container */
    app->notes_data_count = 0;
    app->notes_data_been_read = FALSE;
}

InformerNoteData InformerInitNoteData(void)
{
    InformerNoteData result;
    result.id = NOTES_NONE;
    result.is_checked = NOTES_NONE;
    sprintf(result.text, "");
    sprintf(result.created_by, "");
    result.created_on = NOTES_NONE;
    sprintf(result.modified_by, "");
    result.modified_on = NOTES_NONE;
    return result;
}

int InformerGetNotesDB(void)
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

int InformerUpdateNoteDB(int index, int note_id, int is_checked)
{
    InformerNoteData note_data = InformerInitNoteData();

    sparkMessage(UPDATE_NOTE_WAIT);

    if (TRUE != InformerGetCurrentUser(note_data.modified_by, NOTES_MAX_USER_LENGTH)) {
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

int InformerExportNote(char *filepath, InformerNoteData *note)
{
    FILE *fp;
    int result = 1;

    InformerDEBUG("here we go. writing datafile [%s]\n", filepath);

    if ((fp=fopen(filepath, "w")) == NULL) {
        InformerERROR("Can't write datafile [%s]", filepath);
        return FALSE;
    }

    InformerDEBUG("-------- OK -------- opened the file for writing!\n");

    if (result > 0) {
        result = fprintf(fp, "1\n");
    }

    if (result > 0 && note->id != NOTES_NONE) {
        result = fprintf(fp, "id: %d\n", note->id);
    }

    if (result > 0 && note->is_checked != NOTES_NONE) {
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
        InformerERROR("Can't write datafile [%s]", filepath);
        return FALSE;
    }

    InformerDEBUG("... write of note ok!\n");
    return TRUE;
}

int InformerImportNotes(char *filepath, int index, int update_count)
{
    FILE *fp;
    int i = 0;
    int count = 0;
    int result = 1;
    InformerAppStruct *app = InformerGetApp();

    InformerDEBUG("ImportNotes: here we go. reading datafile [%s], index [%d], update? [%d]\n",
                  filepath, index, update_count);

    if ((fp=fopen(filepath, "r")) == NULL) {
        InformerERROR("Can't open datafile [%s]: %s", filepath, strerror(errno));
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
        InformerERROR("Can't parse datafile [%s]", filepath);
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
            InformerERROR("Can't parse datafile [%s]", filepath);
            return FALSE;
        }
    }

    if (TRUE == update_count) {
        app->notes_ui_cur_page = 1;
        app->notes_data_count = count;
    }

    InformerSortNoteData();
    InformerDEBUG("All notes read ok. Word up.\n");

    return TRUE;
}


int InformerSortNoteData(void)
{
    InformerAppStruct *app = InformerGetApp();
    InformerNoteSortChoice sort_by = app->notes_ui_sort.ui->Value;
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

int InformerCompareNoteByDateCreated(const void *a, const void *b)
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

int InformerCompareNoteByDateModified(const void *a, const void *b)
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

int InformerCompareNoteByStatus(const void *a, const void *b)
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

int InformerCompareNoteByArtist(const void *a, const void *b)
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
        result = strncmp(note_a->modified_by, note_b->modified_by, NOTES_MAX_USER_LENGTH);

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

int InformerCompareNoteByAuthor(const void *a, const void *b)
{
    int result;

    InformerNoteData *note_a = (InformerNoteData*) a;
    InformerNoteData *note_b = (InformerNoteData*) b;

    InformerDEBUG("InformerCompareNoteByAuthor called\n");

    result = strncmp(note_a->created_by, note_b->created_by, NOTES_MAX_USER_LENGTH);

    if (0 != result)
        return result;
    else if (note_a->created_on > note_b->created_on)
        return -1;
    else if (note_a->created_on < note_b->created_on)
        return 1;
    else
        return 0;
}
