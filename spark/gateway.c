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

void
GatewaySetFrameRate(double frameRate)
{
    PyObject *app, *pResult;

    #if defined __XPY__
    return;
    #endif

    printf("-------------> CALLING GATEWAY SET FRAME RATE ---------\n");

    PythonBeginCall();
    app = PythonGetApp();

    pResult = PyObject_CallMethod(app, "setFrameRate", "d", frameRate);

    if (NULL == pResult) {
        PyErr_Print();
        InformerERROR("Unable to call setFrameRate(%d)\n", frameRate);
    }

    Py_XDECREF(pResult);
    PythonEndCall();
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

    if (pResult && PyObject_IsTrue(pResult)) {
        status = TRUE;
        InformerDEBUG("Made note OK!\n");
    } else {
        status = FALSE;
        PyErr_Print();
        InformerError("Unable to create note.\n");
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
        PyErr_Print();
        InformerERROR("Failed to update note.\n");
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

    InformerDEBUG("-------------> CALLING GATEWAY UPDATE ELEM ---------\n");

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
        PyErr_Print();
        InformerERROR("Failed to update element.\n");
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

    InformerDEBUG("---------__>>>.> CALLING GATEWAY GET NOTES M<<<,--_________b\n");

    count = -1;
    PythonBeginCall();
    app = PythonGetApp();

    InformerDEBUG("About to make the call for getNotes [%s]\n", setup);
    pNotes = PyObject_CallMethod(app, "getNotes", "s", setup);

    if (pNotes && PyList_Check(pNotes)) {
        count = PyList_Size(pNotes);
        InformerDEBUG("There were %d notes\n", count);

        for (i=0; i<count; i++) {
            // pItem is a borrowed ref.
            pItem = PyList_GetItem(pNotes, i);
            GatewayPyObjectToNoteData(pItem, &data[i]);
            printf("... read note [%d] ok!\n", i);
        }
    } else {
        PyErr_Print();
        InformerERROR("Failed to get notes.\n");
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

    InformerDEBUG("--------- CALLING GATEAY GET ELEMS -------------\n");

    count = -1;
    PythonBeginCall();
    app = PythonGetApp();

    InformerDEBUG("About to make the call for getElements [%s]\n", setup);
    pElems = PyObject_CallMethod(app, "getElements", "s", setup);

    if (pElems && PyList_Check(pElems)) {
        count = PyList_Size(pElems);
        InformerDEBUG("There were %d elements\n", count);

        for (i=0; i<count; i++) {
            // pItem is a borrowed ref.
            pItem = PyList_GetItem(pElems, i);
            GatewayPyObjectToElemData(pItem, &(data[i]));

            InformerDEBUG("... read element [%d] ok!\n", i);
        }
    } else {
        PyErr_Print();
        InformerERROR("Failed to get elements.\n");
    }

    Py_XDECREF(pElems);
    PythonEndCall();

    return count;
}

char *
GatewaySparkProcessFrameStart(const char *spark_name, SparkInfoStruct spark_info)
{
    int depth = 0;
    char *str = NULL;
    PyObject *app, *pResult = NULL;

    printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    printf("------ process frame called from %s -------\n", spark_name);
    printf("* current frame: %d\n", spark_info.FrameNo + 1);
    printf("* max frame: %d\n", spark_info.TotalFrameNo);
    printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    #if defined __XPY__
    return NULL;
    #endif

    PythonBeginCall();
    app = PythonGetApp();

    if (SPARKBUF_RGB_24_3x8 == spark_info.FrameDepth) {
        depth = 8;
    } else if (SPARKBUF_RGB_48_3x12 == spark_info.FrameDepth) {
        depth = 12;
    } else {
        InformerERROR("Unsupported frame depth (%d)\n", spark_info.FrameDepth);
        return NULL;
    }

    if (depth) {
        printf("OK. the spark was not null -- now calling processFrame\n");
        printf("* spark_name: %s\n", spark_name);
        printf("* spark_info.FrameWidth: %d\n", spark_info.FrameWidth);
        printf("* spark_info.FrameHeight: %d\n", spark_info.FrameHeight);
        printf("* depth: %d\n", depth);
        printf("* 1: 1\n");
        printf("* spark_info.FrameNo + 1: %d\n", spark_info.FrameNo + 1);
        printf("* spark_info.TotalFrameNo: %d\n", spark_info.TotalFrameNo);

        pResult = PyObject_CallMethod(app, "frameProcessStart", "siiiiii",
                                      spark_name,
                                      spark_info.FrameWidth,
                                      spark_info.FrameHeight,
                                      depth,
                                      1,
                                      spark_info.FrameNo + 1,
                                      spark_info.TotalFrameNo);
        if (NULL == pResult) {
            PyErr_Print();
            InformerERROR("Unable to call frameProcessStart for %s\n", spark_name);
            str = NULL;
        } else if (pResult == Py_None) {
            printf("NULL! frameProcessStart said to ignore the frame...\n");
            str = NULL;
        } else {
            printf("pResult was not None\n");
            str = PyString_AsString(pResult);
            printf("Done. returning %s\n", str);
        }

        Py_XDECREF(pResult);
    }

    PythonEndCall();

    printf("--- exiting process frame start for %s ---\n", spark_name);
    return str;
}

void
GatewaySparkProcessFrameEnd(const char *spark_name)
{
    PyObject *app, *pResult = NULL;

    #if defined __XPY__
    return;
    #endif

    printf("--- entering process frame end for %s ---\n", spark_name);

    PythonBeginCall();
    app = PythonGetApp();
    pResult = PyObject_CallMethod(app, "frameProcessEnd", "s", spark_name);

    if (NULL == pResult) {
        PyErr_Print();
        InformerERROR("Unable to call frameProcessEnd for %s\n", spark_name);
    }

    Py_XDECREF(pResult);
    PythonEndCall();

    printf("--- exiting process frame end for %s ---\n", spark_name);
}

void
GatewaySparkProcessStart(const char *spark_name)
{
    PyObject *app, *pResult;
    printf("|||||||||||| GatewaySparkProcessStart called with (%s) ||||||||||||||\n", spark_name);

    PythonBeginCall();
    app = PythonGetApp();
    pResult = PyObject_CallMethod(app, "sparkProcessStart", "s", spark_name);
    if (NULL == pResult) {
        PyErr_Print();
        InformerERROR("Unable to call sparkProcessStart for %s\n", spark_name);
    }
    printf("----- returned from sparkProcessStart in spark ----\n");
    Py_XDECREF(pResult);
    PythonEndCall();

    printf("|||||||||||| GatewaySparkProcessStart returning ||||||||||||||\n");
}

void
GatewaySparkProcessEnd(const char *spark_name)
{
    PyObject *app, *pResult;
    printf("|||||||||||| GatewaySparkProcessEnd called with (%s) ||||||||||||||\n", spark_name);

    PythonBeginCall();
    app = PythonGetApp();
    pResult = PyObject_CallMethod(app, "sparkProcessEnd", "s", spark_name);
    if (NULL == pResult) {
        PyErr_Print();
        InformerERROR("Unable to call sparkProcessEnd for %s\n", spark_name);
    }

    printf("----- returned from sparkProcessEnd in spark ----\n");
    Py_XDECREF(pResult);
    PythonEndCall();

    printf("|||||||||||| GatewaySparkProcessEnd returning ||||||||||||||\n");
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
    if (NULL == pResult) {
        PyErr_Print();
        InformerERROR("Unable to register spark (%s)\n", spark_name);
    } else {
        name = PyString_AsString(pResult);
    }

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
    if (NULL == pResult) {
        PyErr_Print();
        InformerERROR("Unable to rename spark (%s -> %s)\n", old_name, new_name);
    }

    Py_XDECREF(pResult);
    PythonEndCall();
    printf("|||||||||||| GatewaySparkRename done ||||||||||||||\n");

    return 1;
}

