#include "python.h"
#include "gateway.h"
#include <stdlib.h>

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

void
GatewayPyStringToInt(PyObject *obj, int *data)
{
    *data = atoi(PyString_AsString(obj));
}

void
GatewayPyStringToLong(PyObject *obj, long int *data)
{
    *data = atol(PyString_AsString(obj));
}

void
GatewayPyStringToString(PyObject *obj, char *data, int max_length)
{
    strncpy(data, PyString_AsString(obj), max_length);
}

int
GatewayGetNotes(const char *setup, InformerNoteData *data)
{
    int i, count;
    PyObject *spark, *pNotes, *pItem, *pAttr;

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
            pAttr = PyObject_GetAttrString(pItem, "id");
            GatewayPyStringToInt(pAttr, &(data[i].id));
            Py_XDECREF(pAttr);

            printf("going to set text\n");
            pAttr = PyObject_GetAttrString(pItem, "text");
            GatewayPyStringToString(pAttr, data[i].text, 4096);
            Py_XDECREF(pAttr);

            printf("going to set is_checked\n");
            pAttr = PyObject_GetAttrString(pItem, "is_checked");
            GatewayPyStringToInt(pAttr, &(data[i].is_checked));
            Py_XDECREF(pAttr);

            printf("going to set created_by\n");
            pAttr = PyObject_GetAttrString(pItem, "created_by");
            GatewayPyStringToString(pAttr, data[i].created_by, 32);
            Py_XDECREF(pAttr);

            printf("going to set created_on\n");
            pAttr = PyObject_GetAttrString(pItem, "created_on");
            GatewayPyStringToLong(pAttr, &(data[i].created_on));
            Py_XDECREF(pAttr);

            printf("going to set modified_by\n");
            pAttr = PyObject_GetAttrString(pItem, "modified_by");
            GatewayPyStringToString(pAttr, data[i].modified_by, 32);
            Py_XDECREF(pAttr);

            printf("going to set modified_on\n");
            pAttr = PyObject_GetAttrString(pItem, "modified_on");
            GatewayPyStringToLong(pAttr, &(data[i].modified_on));
            Py_XDECREF(pAttr);

            printf("... read note [%d] ok!\n", i);
        }
    } else {
        printf("------- ERROR MAKING CALL TODO XXX ---------\n");
    }

    Py_XDECREF(pNotes);
    PythonEndCall();

    return count;
}

int
GatewayGetElems(const char *setup, InformerElemData *data)
{
    int i, count;
    PyObject *spark, *pElems, *pItem, *pAttr;

    printf("--------- CALLING GATEAY GET ELEMS -------------\n");

    count = -1;
    PythonBeginCall();
    spark = SparkGetSpark();

    printf("About to make the call for getElements [%s]\n", setup);
    pElems = PyObject_CallMethod(spark, "getElements", "s", setup);

    if (pElems && PyList_Check(pElems)) {
        count = PyList_Size(pElems);
        printf("There were %d elements!\n", count);

        for (i=0; i<count; i++) {
            // pItem is a borrowed ref.
            pItem = PyList_GetItem(pElems, i);

            printf("going to set id\n");
            pAttr = PyObject_GetAttrString(pItem, "id");
            GatewayPyStringToInt(pAttr, &(data[i].id));
            Py_XDECREF(pAttr);

            printf("going to set kind\n");
            pAttr = PyObject_GetAttrString(pItem, "kind");
            GatewayPyStringToString(pAttr, data[i].kind, 4096);
            Py_XDECREF(pAttr);

            printf("going to set text\n");
            pAttr = PyObject_GetAttrString(pItem, "text");
            GatewayPyStringToString(pAttr, data[i].text, 4096);
            Py_XDECREF(pAttr);

            printf("going to set is_checked\n");
            pAttr = PyObject_GetAttrString(pItem, "is_checked");
            GatewayPyStringToInt(pAttr, &(data[i].is_checked));
            Py_XDECREF(pAttr);

            printf("going to set created_by\n");
            pAttr = PyObject_GetAttrString(pItem, "created_by");
            GatewayPyStringToString(pAttr, data[i].created_by, 32);
            Py_XDECREF(pAttr);

            printf("going to set created_on\n");
            pAttr = PyObject_GetAttrString(pItem, "created_on");
            GatewayPyStringToLong(pAttr, &(data[i].created_on));
            Py_XDECREF(pAttr);

            printf("... read element [%d] ok!\n", i);
        }
    } else {
        printf("------- ERROR MAKING CALL TODO XXX ---------\n");
    }

    Py_XDECREF(pElems);
    PythonEndCall();

    return count;
}

