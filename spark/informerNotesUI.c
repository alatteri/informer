#include <time.h>

#include "spark.h"
#include "informer.h"
#include "informerNotesUI.h"

static char CREATE_NOTE_UI[] = "                click here to create a new note";
static char CREATE_NOTE_WAIT[] = "Creating new note...";

SparkCanvasStruct SparkCanvas1 = { CanvasDraw, CanvasInteract };

SparkStringStruct SparkString13 = { "", CREATE_NOTE_UI, SPARK_FLAG_NONE, InformerNotesCreateNoteCallback };

SparkPupStruct SparkPup7 = {0, 5, InformerNotesSortChanged, {"Sort by date added",
                                                             "Sort by last modified",
                                                             "Sort by status",
                                                             "Sort by artist",
                                                             "Sort by author"}};

SparkStringStruct SparkString28 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };
SparkPushStruct SparkPush14 = { "<< Previous Page", InformerNotesButtonA };
SparkPushStruct SparkPush21 = { "Next Page >>", InformerNotesButtonB };

SparkPupStruct SparkPup34 = {0, 1, InformerNotesModeChanged, {"Refresh Notes"}};

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


void CanvasDraw(SparkCanvasInfo canvas_info)
{
    // InformerDEBUG("-- draw event called --\n");
}

int CanvasInteract(SparkCanvasInfo canvas_info, int x, int y, float pressure)
{
    // InformerDEBUG("--- interact event called x: %d, y: %d, pressure %f ---\n", x, y, pressure);
    return 1;
}

void InformerInitNotesUI(void)
{
    InformerAppStruct *app = InformerGetApp();

    /* Initialize the notes UI container */
    app->notes_ui_cur_page = 1;
    app->notes_ui_count = NOTES_UI_NUM_ROWS;

    app->notes_ui_mode = InformerInitPupUI(34, &SparkPup34);
    app->notes_ui_sort = InformerInitPupUI(7, &SparkPup7);
    app->notes_ui_create = InformerInitStringUI(13, &SparkString13);

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
}

static unsigned long *InformerNotesCreateNoteCallback(int CallbackArg, SparkInfoStruct SparkInfo)
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

        if (TRUE != InformerGetCurrentUser(note_data.created_by, NOTES_MAX_USER_LENGTH)) {
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

static unsigned long *InformerNotesSortChanged(int CallbackArg,
                                              SparkInfoStruct SparkInfo )
{
    InformerAppStruct *app = InformerGetApp();
    InformerDEBUG("Hey! The notes sort change got called with: %d, value is: %d\n",
            CallbackArg, app->notes_ui_sort.ui->Value);
    InformerSortNoteData();
    InformerRefreshNotesUI();
    return NULL;
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
            InformerDEBUG("You clicked Notes Page Prev\n");
            if (app->notes_ui_cur_page > 1) {
                app->notes_ui_cur_page -= 1;
                InformerRefreshNotesUI();
            } else {
                InformerDEBUG("You can't go past page 1\n");
            }
        } else if (INFORMER_NOTE_BUTTON_B == button) {
            InformerDEBUG("You clicked Notes Page Next\n");
            max_page = (int) ceil((float)app->notes_data_count/app->notes_ui_count);
            if (app->notes_ui_cur_page < max_page) {
                app->notes_ui_cur_page += 1;
                InformerRefreshNotesUI();
            } else {
                InformerDEBUG("You can't go past the last page (%d)\n", max_page);
            }
        }
    }
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

static unsigned long *InformerNotesModeChanged(int CallbackArg,
                                              SparkInfoStruct SparkInfo )
{
    InformerAppStruct *app = InformerGetApp();
    InformerNoteModeChoice mode = InformerGetNoteUIMode();

    InformerDEBUG("Hey! The notes mode change got called with: value: %d\n", mode);

    if (INFORMER_NOTE_MODE_REFRESH == mode) {
        InformerGetNotesDB();
    }

    return NULL;
}

InformerNoteModeChoice InformerGetNoteUIMode(void)
{
    InformerAppStruct *app = InformerGetApp();
    return (InformerNoteModeChoice) app->notes_ui_mode.ui->Value;
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

        InformerDEBUG("It's note_id is: (%d)\n", note_id);
        InformerDEBUG("Row number (%d) was changed\n", row_num);
        InformerDEBUG("It's value is: %d\n", is_checked);

        InformerUpdateNoteDB(index, note_id, is_checked);
    } else {
        InformerDEBUG("Whoa! can't toggle secret hidden notes! :)\n");
    }
}

void InformerHideAllNoteRows(void)
{
    int i;
    for (i=1; i<=NOTES_UI_NUM_ROWS; i++) {
        InformerHideNoteRow(i);
    }
}

void InformerHideNoteRow(int row_num)
{
    InformerDEBUG("++++ call to disable note row %d\n", row_num);
    InformerToggleNoteRow(row_num, 0);
}

void InformerShowNoteRow(int row_num)
{
    InformerDEBUG("++++ call to enable note row %d\n", row_num);
    InformerToggleNoteRow(row_num, 1);
}

void InformerToggleNoteRow(int row_num, int on_off)
{
    InformerAppStruct *app = InformerGetApp();
    void (*sparkToggle)(int, int) = NULL;

    if (1 <= row_num && NOTES_UI_NUM_ROWS >= row_num) {
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

    InformerDEBUG("****** Time to refresh the notes UI ********\n");
    InformerDEBUG("I think there are [%d] notes\n", app->notes_data_count);

    if (app->notes_data_count > 0) {
        if (1 == app->notes_data_count) {
            start = end = 0;
            sprintf(app->notes_ui_statusbar.ui->Value, "Displaying note 1 (of 1)");
        } else {
            start = (app->notes_ui_cur_page - 1) * app->notes_ui_count;
            if (app->notes_data_count - start < app->notes_ui_count) {
                /* if there is less than a page left, calculate partial */
                end = start - 1 + (app->notes_data_count - start) % app->notes_ui_count;
            } else {
                /* there is more than a full page left, max it out */
                end = start + app->notes_ui_count - 1;
            }

            sprintf(app->notes_ui_statusbar.ui->Value,
                    "Displaying notes %d - %d (of %d)",
                    start + 1, end + 1, app->notes_data_count);
        }

        InformerDEBUG("start [%d] end [%d] count [%d]\n", start, end, app->notes_ui_count);
        for (row=1; row <= app->notes_ui_count; row++) {
            index = start + row - 1;
            if (index <= end) {
                InformerDEBUG("Going to display index [%d] at row [%d]\n", index, row);
                InformerUpdateNotesRowUI(app->notes_data[index], row);
            }
        }
    } else {
        sprintf(app->notes_ui_statusbar.ui->Value, "No notes to display");
    }
}

void InformerUpdateNotesRowUI(InformerNoteData source, int row_num)
{
    char str[32];
    InformerAppStruct *app = InformerGetApp();
    InformerNoteRowUI *note_ui;

    note_ui = &(app->notes_ui[row_num - 1]);

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

