#include "python.h"
#include "spark.h"
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
    PyObject *app, *pResult;

    #if defined __XPY__
    return 0;
    #endif

    printf("-------------> CALLING GATEWAY CREATE NOTE ---------\n");

    PythonBeginCall();
    app = PythonGetApp();

    pResult = PyObject_CallMethod(app, "createNote", "siss",
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
    PyObject *app, *pNotes, *pItem;

    #if defined __XPY__
    return 0;
    #endif

    printf("-------------> CALLING GATEWAY UPDATE NOTE ---------\n");

    PythonBeginCall();
    app = PythonGetApp();

    pNotes = PyObject_CallMethod(app, "updateNote", "siis",
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
    PyObject *app, *pElems, *pItem;

    #if defined __XPY__
    return 0;
    #endif

    printf("-------------> CALLING GATEWAY UPDATE ELEM ---------\n");

    PythonBeginCall();
    app = PythonGetApp();

    pElems = PyObject_CallMethod(app, "updateElement", "sii",
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
    PyObject *app, *pNotes, *pItem, *pAttr;

    #if defined __XPY__
    return 0;
    #endif

    printf("---------__>>>.> CALLING GATEWAY GET NOTES M<<<,--_________b\n");

    count = -1;
    PythonBeginCall();
    app = PythonGetApp();

    printf("About to make the call for getNotes [%s]\n", setup);
    pNotes = PyObject_CallMethod(app, "getNotes", "s", setup);

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
    PyObject *app, *pElems, *pItem, *pAttr;

    #if defined __XPY__
    return 0;
    #endif

    printf("--------- CALLING GATEAY GET ELEMS -------------\n");

    count = -1;
    PythonBeginCall();
    app = PythonGetApp();

    printf("About to make the call for getElements [%s]\n", setup);
    pElems = PyObject_CallMethod(app, "getElements", "s", setup);

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

char *
GatewaySparkProcessFrame(const char *spark_name, SparkInfoStruct spark_info)
{
    int depth;
    char *str = NULL;
    PyObject *app, *spark, *pResult = NULL;

    printf("------ process frame called from %s -------\n", spark_name);
    #if defined __XPY__
    return NULL;
    #endif

    if (SPARKBUF_RGB_24_3x8 == spark_info.FrameDepth) {
        depth = 8;
    } else {
        printf("++++++++++ SOME OTHER DEPTH: %d ++++++++++\n", spark_info.FrameDepth);
        depth = spark_info.FrameDepth;
    }

    PythonBeginCall();
    app = PythonGetApp();
    spark = PythonAppGetSpark(app, spark_name);

    if (NULL != spark) {
        printf("OK. the spark was not null -- now calling processFrame\n");
        pResult = PyObject_CallMethod(spark, "processFrame", "iiii",
                                      spark_info.FrameWidth,
                                      spark_info.FrameHeight,
                                      depth,
                                      spark_info.FrameNo + 1);
        str = PyString_AsString(pResult);
        printf("Done. returning %s\n", str);
    }

    Py_XDECREF(spark);
    Py_XDECREF(pResult);
    PythonEndCall();

    printf("--- exiting processframe for %s ---\n", spark_name);
    return str;
}

char *
GatewaySparkRegister(const char *spark_name)
{
    char *name;
    PyObject *app, *pResult;

    printf("|||||||||||| GatewaySparkRegister called ||||||||||||||\n");
    PythonBeginCall();
    app = PythonGetApp();

    pResult = PyObject_CallMethod(app, "sparkRegister", "s", spark_name);
    name = PyString_AsString(pResult);

    Py_XDECREF(pResult);
    PythonEndCall();

    printf("|||||||||||| GatewaySparkRegister done ||||||||||||||\n");
    return name;
}

int
GatewaySparkRename(const char *old_name, const char *new_name)
{
    char *name;
    PyObject *app, *pResult;

    printf("|||||||||||| GatewaySparkRename called ||||||||||||||\n");
    PythonBeginCall();
    app = PythonGetApp();

    pResult = PyObject_CallMethod(app, "sparkRename", "ss", old_name, new_name);

    Py_XDECREF(pResult);
    PythonEndCall();
    printf("|||||||||||| GatewaySparkRename done ||||||||||||||\n");

    return 1;
}

