/* Example of embedding Python in another program */

#include <Python.h>
#include <stdio.h>
#include <dlfcn.h>
#include "python.h"

void* PyModule;
PyThreadState *TSTATE = NULL;
PyObject *SPARK = NULL;

void PythonInitialize(void)
{
    int argc = 1;
    char *argv[1];

    PyObject *pName, *pModule, *pFunc, *pResult;

    PyModule = dlopen("/usr/lib64/libpython2.3.so", RTLD_LAZY|RTLD_GLOBAL);
    if (!PyModule) {
        printf("----------------- !!!!!!!!!!!! PyMODULE WAS FALSE --------------\n");
        return;
    }

    argv[0] = "/usr/bin/python";
    argv[1] = NULL;

    printf("********** NOW GOING TO INIT PYTHON *****************\n");

    // Initialize the Python interpreter.  Required.
    printf("Py_Initialize...\n");
    Py_Initialize();

    // Create (and acquire) the interpreter lock
    printf("PyEval_InitThreads...\n");
    PyEval_InitThreads();

    // XXX the current PyThreadState* is returned
    // create a brand spankin new interpreter
    TSTATE = Py_NewInterpreter();
    if (NULL == TSTATE) {
        Py_FatalError("Unable to initialize Python interpreter.");
        return;
    }

    // save a pointer to the main PyThreadState object
    // mainThreadState = PyThreadState_Get();

    // get a reference to the PyInterpreterState
    // mainInterpreterState = mainThreadState->interp;

    // create a thread state object for this thread
    // PyThreadState_New(mainInterpreterState);

    // XXX Do I care about
    // PyThreadState * myThreadState = PyThreadState_New(mainInterpreterState);

    printf("Hello, brave new world\n\n");

    printf("----------- what is sys ------------\n");
    PyRun_SimpleString("print 'sys is:', sys\n");
    printf("----------- going to import sys ------------\n");
    PyRun_SimpleString("import sys\n");
    printf("----------- going to append ------------\n");
    PyRun_SimpleString("sys.path.append('/usr/discreet/sparks/instinctual/informer/lib')\n");
    PyRun_SimpleString("sys.path.append('/usr/discreet/sparks/instinctual/informer/third_party/lib')\n");
    PyRun_SimpleString("print 'after:', sys.path\n");

    PyRun_SimpleString("import sitecustomize\n");

    PyRun_SimpleString("import instinctual\n");
    PyRun_SimpleString("import instinctual.informer\n");
    // PyRun_SimpleString("import instinctual.informer.spark\n");


    // Load the Spark module
    printf("Now importing instinctual.informer.spark...\n");
    pName = PyString_FromString("instinctual.informer.spark");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule == NULL) {
        PyErr_Print();
        fprintf(stderr, "Failed to load pModule\n");
        return;
    }

    // Access the Spark's constructor
    printf("pModule was not NULL\n");
    pFunc = PyObject_GetAttrString(pModule, "Spark");

    if (!pFunc || !PyCallable_Check(pFunc)) {
        PyErr_Print();
        fprintf(stderr, "Cannot find function Spark\n");

        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
        return;
    }

    // Instantiate the Spark object
    SPARK = PyObject_CallObject(pFunc, NULL);
    if (SPARK == NULL) {
        PyErr_Print();
        fprintf(stderr,"Could not create SPARK object\n");

        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
        return;
    }

    Py_XDECREF(pFunc);
    Py_DECREF(pModule);

    // Kick off the spark
    SparkStart(SPARK);

    PyRun_SimpleString("print 'DONE LOADING PYTHON'\n");
    PythonEndCall();
}

void PythonBeginCall(void)
{
    PyEval_RestoreThread(TSTATE);
}

void PythonEndCall(void)
{
    TSTATE = PyEval_SaveThread();
}

void PythonExit(void)
{
    PyObject *spark;

    printf("********** NOW GOING TO EXIT PYTHON *****************\n");

    PythonBeginCall();

    PyRun_SimpleString("print 'NOW STOPPING THE SPARK'\n");
    spark = SparkGetSpark();
    SparkStop(spark);
    Py_DECREF(spark);

    // This crashes with:
    // Ending the interpreter...
    // Fatal Python error: Py_EndInterpreter: not the last thread
    // Application: flame 2008 x86_64
    // Error: abnormal termination, signal = 6
    // SIGABRT - Abort
    // Unknown user initiated signa
    // Killed
    //
    // printf("Ending the interpreter...\n");
    // Py_EndInterpreter(TSTATE);


    // This crashes with
    // flame_LINUX_2.6_x86_64: Python/ceval.c:765: eval_frame:
    //      Assertion `(stack_pointer - f->f_valuestack) <= f->f_stacksize' failed.
    //
    // #0  0x00000032f7f2e37d in raise () from /lib64/tls/libc.so.6
    // #1  0x00000032f7f2faae in abort () from /lib64/tls/libc.so.6
    // #2  0x00000032f7f27c31 in __assert_fail () from /lib64/tls/libc.so.6
    // #3  0x0000002a9f5e7952 in _PyEval_SliceIndex () from /usr/lib64/libpython2.3.so.1.0
    // #4  0x0000002a9f5e8ba1 in _PyEval_SliceIndex () from /usr/lib64/libpython2.3.so.1.0
    // #5  0x0000002a9f5ea250 in PyEval_EvalCodeEx () from /usr/lib64/libpython2.3.so.1.0
    // #6  0x0000002a9f5a6d8d in PyFunction_SetClosure () from /usr/lib64/libpython2.3.so.1.0
    // #7  0x0000002a9f594380 in PyObject_Call () from /usr/lib64/libpython2.3.so.1.0
    // #8  0x0000002a9f59b9ab in PyMethod_New () from /usr/lib64/libpython2.3.so.1.0
    // #9  0x0000002a9f594380 in PyObject_Call () from /usr/lib64/libpython2.3.so.1.0
    // #10 0x0000002a9f5e3b29 in PyEval_CallObjectWithKeywords () from /usr/lib64/libpython2.3.so.1.0
    // #11 0x0000002a9f60aebd in _PyObject_GC_Del () from /usr/lib64/libpython2.3.so.1.0
    // #12 0x0000002a97ba20aa in start_thread () from /lib64/tls/libpthread.so.0
    // #13 0x00000032f7fc5b43 in clone () from /lib64/tls/libc.so.6
    // #14 0x0000000000000000 in ?? ()

    // printf("Now finalizing python...\n");
    // Py_Finalize();
    // printf("python was finalized\n");

    // dlclose(PyModule);
    printf("dlclose was called\n");

    printf("\nGoodbye, cruel world\n");
}

int
SparkStart(PyObject *spark)
{
    PyObject *pResult;
    pResult = PyObject_CallMethod(spark, "start", NULL);

    if (NULL == pResult)
        return FALSE;

    Py_XDECREF(pResult);
    return TRUE;
}

int
SparkStop(PyObject *spark)
{
    PyObject *pResult;
    pResult = PyObject_CallMethod(spark, "stop", NULL);

    if (NULL == pResult)
        return FALSE;

    Py_XDECREF(pResult);
    return TRUE;
}

PyObject *
SparkGetSpark(void)
{
    return SPARK;
}
