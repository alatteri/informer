#include "python.h"
#include "gateway.h"
#include <stdlib.h>

int
InformerGatewayGetElems(void)
{
    return FALSE;
//    InformerAppStruct *app = InformerGetApp();
//    sparkMessage(GET_ELEMS_WAIT);
//    InformerTableHideAllRows();
//
//    if (INFORMER_APP_STATE_OK == app->app_state) {
//        if (TRUE == InformerGatewayCall("get_elements", NULL, "/tmp/trinity20")) {
//            if (TRUE == InformerGatewayImportElems("/tmp/trinity20", 0, TRUE)) {
//                app->elems_data_been_read = TRUE;
//                InformerTableRefreshUI();
//                return TRUE;
//            } else {
//                // TODO: What should happen here?
//                InformerERROR("Unable to get elements from the database\n");
//                return FALSE;
//            }
//        }
//    } else {
//        InformerDEBUG("Not calling GetElems: app state is not OK\n");
//    }
}

int
InformerGatewayUpdateNote(int index, int note_id, int is_checked)
{
//    InformerNoteData data = InformerCreateNoteData();
//
//    sparkMessage(UPDATE_NOTE_WAIT);
//
//    if (TRUE != InformerGetCurrentUser(data.modified_by, USERNAME_MAX)) {
//        InformerERROR("Unable to determine the current user\n");
//        return FALSE;
//    }
//
//    data.id = note_id;
//    data.is_checked = is_checked;
//
//    if (TRUE == InformerNoteDataExport(&data, "/tmp/trinity2")) {
//        InformerDEBUG("--------- update note ----------\n");
//        InformerDEBUG("is_checked [%d], user [%s], id [%d], index [%d]\n",
//                      data.is_checked, data.modified_by, data.id, index);
//        InformerDEBUG("--------- update note ----------\n");
//        if (TRUE == InformerGatewayCall("update_note", "/tmp/trinity2", "/tmp/trinity3")) {
//            if (TRUE == InformerGatewayImportNotes("/tmp/trinity3", index, FALSE)) {
//                InformerTableRefreshUI();
//                return TRUE;
//            }
//        }
//    }
//
    return FALSE;
}

/*--------------------------------------------------------------------------*/
/* Serialize the note and write it out to filepath                          */
/*--------------------------------------------------------------------------*/
int
InformerNoteDataExport(InformerNoteData *data, char *filepath)
{
    return FALSE;
//    FILE *fp;
//    int result = 1;
//
//    InformerDEBUG("here we go. writing datafile [%s]\n", filepath);
//
//    if ((fp=fopen(filepath, "w")) == NULL) {
//        InformerERROR("%s: can't write datafile [%s]",
//                      GATEWAY_STATUS_ERR, filepath);
//        return FALSE;
//    }
//
//    InformerDEBUG("-------- OK -------- opened the file for writing!\n");
//
//    if (result > 0) {
//        result = fprintf(fp, "1\n");
//    }
//
//    if (result > 0 && data->id != DATA_NONE) {
//        result = fprintf(fp, "id: %d\n", data->id);
//    }
//
//    if (result > 0 && data->is_checked != DATA_NONE) {
//        result = fprintf(fp, "is_checked: %d\n", data->is_checked);
//    }
//
//    if (result > 0 && 0 != strcmp("", data->created_by)) {
//        result = fprintf(fp, "created_by: %s\n", data->created_by);
//    }
//
//    if (result > 0 && 0 != strcmp("", data->modified_by)) {
//        result = fprintf(fp, "modified_by: %s\n", data->modified_by);
//    }
//
//    if (result > 0 && 0 != strcmp("", data->text)) {
//        result = fprintf(fp, "text: %s\n", data->text);
//    }
//
//    fclose(fp);
//
//    if (!result) {
//        InformerERROR("%s: can't write datafile [%s]",
//                      GATEWAY_STATUS_ERR, filepath);
//        return FALSE;
//    }
//
//    InformerDEBUG("... write of note ok!\n");
//    return TRUE;
}

/*--------------------------------------------------------------------------*/
/* Serialize the element and write it out to filepath                       */
/*--------------------------------------------------------------------------*/
int
InformerElemDataExport(InformerElemData *data, char *filepath)
{
    return FALSE;
//    FILE *fp;
//    int result = 1;
//
//    InformerDEBUG("here we go. writing datafile [%s]\n", filepath);
//
//    if ((fp=fopen(filepath, "w")) == NULL) {
//        InformerERROR("%s: can't write datafile [%s]",
//                      GATEWAY_STATUS_ERR, filepath);
//        return FALSE;
//    }
//
//    InformerDEBUG("-------- OK -------- opened the file for writing!\n");
//
//    if (result > 0) {
//        result = fprintf(fp, "1\n");
//    }
//
//    if (result > 0 && data->id != DATA_NONE) {
//        result = fprintf(fp, "id: %d\n", data->id);
//    }
//
//    if (result > 0 && 0 != strcmp("", data->kind)) {
//        result = fprintf(fp, "kind: %s\n", data->kind);
//    }
//
//    if (result > 0 && 0 != strcmp("", data->text)) {
//        result = fprintf(fp, "text: %s\n", data->text);
//    }
//
//    if (result > 0 && data->is_checked != DATA_NONE) {
//        result = fprintf(fp, "is_checked: %d\n", data->is_checked);
//    }
//
//    if (result > 0 && 0 != strcmp("", data->created_by)) {
//        result = fprintf(fp, "created_by: %s\n", data->created_by);
//    }
//
//    fclose(fp);
//
//    if (!result) {
//        InformerERROR("%s: can't write datafile [%s]",
//                      GATEWAY_STATUS_ERR, filepath);
//        return FALSE;
//    }
//
//    InformerDEBUG("... write of element ok!\n");
//    return TRUE;
}


int
InformerGatewayUpdateElem(int index, int note_id, int is_checked)
{
//    InformerElemData data = InformerCreateElemData();
//
//    sparkMessage(UPDATE_ELEM_WAIT);
//
//    data.id = note_id;
//    data.is_checked = is_checked;
//
//    if (TRUE == InformerElemDataExport(&data, "/tmp/trinity22")) {
//        InformerDEBUG("--------- update element ----------\n");
//        InformerDEBUG("is_checked [%d], id [%d], index [%d]\n",
//                      data.is_checked, data.id, index);
//        InformerDEBUG("--------- update element ----------\n");
//        if (TRUE == InformerGatewayCall("update_element", "/tmp/trinity22", "/tmp/trinity32")) {
//            if (TRUE == InformerGatewayImportElems("/tmp/trinity32", index, FALSE)) {
//                InformerTableRefreshUI();
//                return TRUE;
//            }
//        }
//    }
//
//    return FALSE;
}

int
InformerGatewayCall(char *action, char *infile, char *outfile)
{
//    int pid = 0;
//    int index = 0;
//    int status = 0;
//    const char *setup;
//    const char *gateway;
//    const char *argv[10];
//    const char **ptr = argv;
//
//    setup = InformerGetSetupPath();
//    gateway = InformerGetGatewayPath();
//
//    InformerDEBUG("the gateway is [%s]\n", gateway);
//
//    argv[index++] = gateway;
//    argv[index++] = "-s";
//    argv[index++] = setup;
//    argv[index++] = "-a";
//    argv[index++] = action;
//
//    if (infile != NULL) {
//        argv[index++] = "-i";
//        argv[index++] = infile;
//    }
//
//    if (outfile != NULL) {
//        argv[index++] = "-o";
//        argv[index++] = outfile;
//    }
//
//    argv[index] = 0;    /* This is required to end the array */
//
//    InformerDEBUG("---- calling gateway ----\n");
//    while (*ptr) {
//        InformerDEBUG("%s ", *ptr++);
//    }
//    InformerDEBUG("\n---- calling gateway ----\n");
//
//    pid = sparkSystemNoSh(FALSE, argv[0], argv);
//    sparkWaitpid(pid, &status, 0);
//    InformerDEBUG("Gateway returned. status [%d]\n", status);
//
//    if (0 != status) {
//        // TODO: Map the status to a human readable string
//        InformerERROR("%s: status [%d]", GATEWAY_STATUS_ERR, status);
//        return FALSE;
//    } else {
//        return TRUE;
//    }
}

int
GatewayGetNotes(const char *setup, InformerNoteData *data)
{
    int i, count;
    PyObject *spark, *pNotes, *pItem;

    printf("---------__>>>.> CALLING GATEWAY GET NOTES M<<<,--_________b\n");

    count = -1;
    PythonBeginCall();
    spark = SparkGetSpark();

    printf("About to make the call for getNotes [%s]\n", setup);
    pNotes = PyObject_CallMethod(spark, "getNotes", "s", setup);

    if (pNotes && PyList_Check(pNotes)) {
        count = PyList_Size(pNotes);
        printf("There were %d notes!\n", count);
        for (i=0; i<count; i++) {
            // pItem is a borrowed ref.
            pItem = PyList_GetItem(pNotes, i);
            printf("going to set id\n");
            data[i].id = atoi(PyString_AsString(PyObject_GetAttrString(pItem, "id")));
            printf("going to set text\n");
            strncpy(data[i].text, PyString_AsString(PyObject_GetAttrString(pItem, "text")), 4096);
            printf("going to set is_checked with [%s]\n", PyString_AsString(PyObject_GetAttrString(pItem, "is_checked")));
            data[i].is_checked = atoi(PyString_AsString(PyObject_GetAttrString(pItem, "is_checked")));
            printf("going to set created_by\n");
            strncpy(data[i].created_by, PyString_AsString(PyObject_GetAttrString(pItem, "created_by")), 32);
            printf("going to set created_on\n");
            data[i].created_on = atol(PyString_AsString(PyObject_GetAttrString(pItem, "created_on")));
            printf("going to set modified_on\n");
            strncpy(data[i].modified_by, PyString_AsString(PyObject_GetAttrString(pItem, "modified_by")), 32);
            printf("going to set modified_by\n");
            data[i].modified_on = atol(PyString_AsString(PyObject_GetAttrString(pItem, "modified_on")));
            printf("... read note [%d] ok!\n", i);
        }
    } else {
        printf("------- ERROR MAKING CALL TODO XXX ---------\n");
    }

    Py_XDECREF(pNotes);
    PythonEndCall();

    return count;

//    FILE *fp;
//    int i = 0;
//    int count = 0;
//    int result = 1;
//    InformerAppStruct *app = InformerGetApp();
//
//    InformerDEBUG("ImportNotes: here we go. reading datafile [%s], index [%d], update? [%d]\n",
//                  filepath, index, update_count);
//
//    if ((fp=fopen(filepath, "r")) == NULL) {
//        InformerERROR("%s: can't open datafile [%s]: %s",
//                      GATEWAY_STATUS_ERR, filepath, strerror(errno));
//        return FALSE;
//    }
//
//    /*
//        read the data file:
//        - first line is number of note objects (not lines...)
//        - entries are of form
//            key: value
//    */
//    result = fscanf(fp, "%d%*1[\n]", &count);
//    if (1 != result) {
//        InformerERROR("%s: can't parse datafile [%s]", GATEWAY_STATUS_ERR, filepath);
//        return FALSE;
//    }
//
//    InformerDEBUG("OK there are %d entries\n", count);
//
//    for (i=0; i<count; i++) {
//        /* skip the "key:" read the value */
//        if (TRUE == InformerFileReadValAsInt(fp,    &(app->notes_data[index+i].id)) &&
//            TRUE == InformerFileReadValAsString(fp, app->notes_data[index+i].text) &&
//            TRUE == InformerFileReadValAsInt(fp,    &(app->notes_data[index+i].is_checked)) &&
//            TRUE == InformerFileReadValAsString(fp, app->notes_data[index+i].created_by) &&
//            TRUE == InformerFileReadValAsLong(fp,   &(app->notes_data[index+i].created_on)) &&
//            TRUE == InformerFileReadValAsString(fp, app->notes_data[index+i].modified_by) &&
//            TRUE == InformerFileReadValAsLong(fp,   &(app->notes_data[index+i].modified_on))) {
//            /* looking good -- keep going */
//            InformerDEBUG("... read note [%d] ok!\n", i);
//        } else {
//            InformerERROR("%s: can't parse datafile [%s]", GATEWAY_STATUS_ERR, filepath);
//            return FALSE;
//        }
//    }
//
//    return TRUE;
}

int
InformerGatewayImportElems(char *filepath, int index, int update_count)
{
    return FALSE;
//    FILE *fp;
//    int i = 0;
//    int count = 0;
//    int result = 1;
//    InformerAppStruct *app = InformerGetApp();
//
//    InformerDEBUG("ImportElems: here we go. reading datafile [%s], index [%d], update? [%d]\n",
//                  filepath, index, update_count);
//
//    if ((fp=fopen(filepath, "r")) == NULL) {
//        InformerERROR("%s: can't open datafile [%s]: %s",
//                      GATEWAY_STATUS_ERR, filepath, strerror(errno));
//        return FALSE;
//    }
//
//    /*
//        read the data file:
//        - first line is number of note objects (not lines...)
//        - entries are of form
//            key: value
//    */
//    result = fscanf(fp, "%d%*1[\n]", &count);
//    if (1 != result) {
//        InformerERROR("%s: can't parse datafile [%s]", GATEWAY_STATUS_ERR, filepath);
//        return FALSE;
//    }
//
//    InformerDEBUG("OK there are %d entries\n", count);
//
//    for (i=0; i<count; i++) {
//        /* skip the "key:" read the value */
//        if (TRUE == InformerFileReadValAsInt(fp,    &(app->elems_data[index+i].id)) &&
//            TRUE == InformerFileReadValAsString(fp, app->elems_data[index+i].kind) &&
//            TRUE == InformerFileReadValAsString(fp, app->elems_data[index+i].text) &&
//            TRUE == InformerFileReadValAsInt(fp,    &(app->elems_data[index+i].is_checked)) &&
//            TRUE == InformerFileReadValAsString(fp, app->elems_data[index+i].created_by) &&
//            TRUE == InformerFileReadValAsLong(fp,   &(app->elems_data[index+i].created_on))) {
//            /* looking good -- keep going */
//            InformerDEBUG("... read elemens [%d] ok!\n", i);
//        } else {
//            InformerERROR("%s: can't parse datafile [%s]", GATEWAY_STATUS_ERR, filepath);
//            return FALSE;
//        }
//    }
//
//    if (TRUE == update_count) {
//        app->elems_ui_table.cur_page = 1;
//        app->elems_data_count = count;
//    }
//
//    InformerElemDataSort();
//    InformerDEBUG("All elements read ok. Word up.\n");
//
//    return TRUE;
}

