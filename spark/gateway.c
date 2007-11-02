#include "python.h"
#include "gateway.h"
#include <stdlib.h>

void
GatewayPyStringToInt(PyObject *pObj, int *data)
{
    *data = atoi(PyString_AsString(pObj));
}

void
GatewayPyStringToLong(PyObject *pObj, long int *data)
{
    *data = atol(PyString_AsString(pObj));
}

void
GatewayPyStringToString(PyObject *pObj, char *data, int max_length)
{
    strncpy(data, PyString_AsString(pObj), max_length);
}

void
GatewayPyObjectToNoteData(PyObject *pObj, InformerNoteData *data)
{
    PyObject *pAttr;

    printf("going to set id\n");
    pAttr = PyObject_GetAttrString(pObj, "id");
    GatewayPyStringToInt(pAttr, &(data->id));
    Py_XDECREF(pAttr);

    printf("going to set text\n");
    pAttr = PyObject_GetAttrString(pObj, "text");
    GatewayPyStringToString(pAttr, data->text, 4096);
    Py_XDECREF(pAttr);

    printf("going to set is_checked\n");
    pAttr = PyObject_GetAttrString(pObj, "is_checked");
    GatewayPyStringToInt(pAttr, &(data->is_checked));
    Py_XDECREF(pAttr);

    printf("going to set created_by\n");
    pAttr = PyObject_GetAttrString(pObj, "created_by");
    GatewayPyStringToString(pAttr, data->created_by, 32);
    Py_XDECREF(pAttr);

    printf("going to set created_on\n");
    pAttr = PyObject_GetAttrString(pObj, "created_on");
    GatewayPyStringToLong(pAttr, &(data->created_on));
    Py_XDECREF(pAttr);

    printf("going to set modified_by\n");
    pAttr = PyObject_GetAttrString(pObj, "modified_by");
    GatewayPyStringToString(pAttr, data->modified_by, 32);
    Py_XDECREF(pAttr);

    printf("going to set modified_on\n");
    pAttr = PyObject_GetAttrString(pObj, "modified_on");
    GatewayPyStringToLong(pAttr, &(data->modified_on));
    Py_XDECREF(pAttr);
}

void
GatewayPyObjectToElemData(PyObject *pObj, InformerElemData *data)
{
    PyObject *pAttr;

    printf("going to set id\n");
    pAttr = PyObject_GetAttrString(pObj, "id");
    GatewayPyStringToInt(pAttr, &(data->id));
    Py_XDECREF(pAttr);

    printf("going to set kind\n");
    pAttr = PyObject_GetAttrString(pObj, "kind");
    GatewayPyStringToString(pAttr, data->kind, 4096);
    Py_XDECREF(pAttr);

    printf("going to set text\n");
    pAttr = PyObject_GetAttrString(pObj, "text");
    GatewayPyStringToString(pAttr, data->text, 4096);
    Py_XDECREF(pAttr);

    printf("going to set is_checked\n");
    pAttr = PyObject_GetAttrString(pObj, "is_checked");
    GatewayPyStringToInt(pAttr, &(data->is_checked));
    Py_XDECREF(pAttr);

    printf("going to set created_by\n");
    pAttr = PyObject_GetAttrString(pObj, "created_by");
    GatewayPyStringToString(pAttr, data->created_by, 32);
    Py_XDECREF(pAttr);

    printf("going to set created_on\n");
    pAttr = PyObject_GetAttrString(pObj, "created_on");
    GatewayPyStringToLong(pAttr, &(data->created_on));
    Py_XDECREF(pAttr);
}

int
GatewayCreateNote(const char *setup, int is_checked, char *text, char *created_by)
{
    int status;
    PyObject *spark, *pResult;

    printf("-------------> CALLING GATEWAY CREATE NOTE ---------\n");

    PythonBeginCall();
    spark = SparkGetSpark();

    pResult = PyObject_CallMethod(spark, "createNote", "siss",
                                  setup, is_checked, text, created_by);

    if (pResult && PyBool_Check(pResult)) {
        status = TRUE;
        printf("Made note OK!\n");
    } else {
        status = FALSE;
        printf("Something bad happened -- couldn't make note\n");
        PyErr_Print();
    }

    Py_XDECREF(pResult);
    PythonEndCall();
    return status;
}

int
GatewayUpdateNote(const char *setup, InformerNoteData *data, int id,
                  int is_checked, char *modified_by)
{
    int status = FALSE;
    PyObject *spark, *pNotes, *pItem;
    printf("-------------> CALLING GATEWAY UPDATE NOTE ---------\n");

    PythonBeginCall();
    spark = SparkGetSpark();

    pNotes = PyObject_CallMethod(spark, "updateNote", "siis",
                                 setup, id, is_checked, modified_by);

    if (pNotes && PyList_Check(pNotes) && 1 == PyList_Size(pNotes)) {
        // pItem is a borrowed ref.
        pItem = PyList_GetItem(pNotes, 0);
        GatewayPyObjectToNoteData(pItem, data);
        status = TRUE;
    } else {
        // TODO
        status = FALSE;
        printf("Something bad happened -- couldn't update note\n");
        PyErr_Print();
    }

    Py_XDECREF(pNotes);
    PythonEndCall();
    return status;
}

int
GatewayUpdateElem(const char *setup, InformerElemData *data, int id,
                  int is_checked)
{
    int status = FALSE;
    PyObject *spark, *pElems, *pItem;
    printf("-------------> CALLING GATEWAY UPDATE ELEM ---------\n");

    PythonBeginCall();
    spark = SparkGetSpark();

    pElems = PyObject_CallMethod(spark, "updateElement", "sii",
                                 setup, id, is_checked);

    if (pElems && PyList_Check(pElems) && 1 == PyList_Size(pElems)) {
        // pItem is a borrowed ref.
        pItem = PyList_GetItem(pElems, 0);
        GatewayPyObjectToElemData(pItem, data);
        status = TRUE;
    } else {
        // TODO
        status = FALSE;
        printf("Something bad happened -- couldn't update element\n");
        PyErr_Print();
    }

    Py_XDECREF(pElems);
    PythonEndCall();
    return status;
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
            GatewayPyObjectToNoteData(pItem, &data[i]);
            printf("... read note [%d] ok!\n", i);
        }
    } else {
        printf("------- ERROR MAKING CALL TODO XXX ---------\n");
        PyErr_Print();
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
            GatewayPyObjectToElemData(pItem, &(data[i]));

            printf("... read element [%d] ok!\n", i);
        }
    } else {
        printf("------- ERROR MAKING CALL TODO XXX ---------\n");
        PyErr_Print();
    }

    Py_XDECREF(pElems);
    PythonEndCall();

    return count;
}

int
GatewayIsBatchProcessing(void)
{
    int status;
    PyObject *spark, *pResult;

    PythonBeginCall();
    spark = SparkGetSpark();

    pResult = PyObject_CallMethod(spark, "isBatchProcessing", NULL);
    GatewayPyStringToInt(pResult, &status);

    Py_XDECREF(pResult);
    PythonEndCall();

    return status;
}