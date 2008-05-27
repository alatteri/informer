/* Example of embedding Python in another program */

#include <Python.h>
#include <stdio.h>
#include <dlfcn.h>
#include "python.h"
#include "informer.h"

void* PyModule;
PyThreadState *TSTATE = NULL;
PyObject *APP = NULL;

void PythonInitialize(const char *program)
{
    char *argv[1];

    #if defined __XPY__
    return;
    #endif

    PyObject *pName, *pModule, *pResult;

    #if defined _PLATFORM_64
    PyModule = dlopen("/usr/lib64/libpython2.3.so", RTLD_LAZY|RTLD_GLOBAL);
    #else
    PyModule = dlopen("/usr/lib/libpython2.3.so",   RTLD_LAZY|RTLD_GLOBAL);
    #endif

    if (!PyModule) {
        InformerDEBUG("----------------- !!!!!!!!!!!! PyMODULE WAS FALSE --------------\n");
        return;
    }

    argv[0] = "/usr/bin/python";
    argv[1] = NULL;

    InformerDEBUG("********** NOW GOING TO INIT PYTHON *****************\n");

    // Initialize the Python interpreter.  Required.
    InformerDEBUG("Py_Initialize...\n");
    Py_Initialize();

    // Create (and acquire) the interpreter lock
    InformerDEBUG("PyEval_InitThreads...\n");
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

    InformerDEBUG("Hello, brave new world\n\n");

    InformerDEBUG("----------- what is sys ------------\n");
    InformerDEBUG("----------- going to import sys ------------\n");
    PyRun_SimpleString("import sys\n");
    InformerDEBUG("----------- going to append ------------\n");
    PyRun_SimpleString("sys.path.append('/usr/discreet/sparks/instinctual/informer/lib')\n");
    PyRun_SimpleString("sys.path.append('/usr/discreet/sparks/instinctual/informer/third_party/lib')\n");

    PyRun_SimpleString("import sitecustomize\n");

    PyRun_SimpleString("import instinctual\n");
    PyRun_SimpleString("import instinctual.informer\n");


    // Load the App module
    InformerDEBUG("Now importing instinctual.informer.app...\n");
    pName = PyString_FromString("instinctual.informer.app");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule == NULL) {
        PyErr_Print();
        InformerERROR("Failed to load pModule\n");
        // TODO: this can hang the flame on load
        // need to find out what to call/do here
        return;
    }

    // Instantiate the App object
    InformerDEBUG("Going to instantiate App object with program: %s\n", program);
    // APP = PyObject_CallObject(pFunc, "s", program);
    APP = PyObject_CallMethod(pModule, "App", "s", program);
    if (APP == NULL) {
        PyErr_Print();
        InformerERROR("Could not create APP object\n");

        Py_DECREF(pModule);
        return;
    }

    Py_DECREF(pModule);

    // Kick off the APP
    pResult = PyObject_CallMethod(APP, "start", NULL);
    Py_XDECREF(pResult);

    PythonEndCall();
}

void PythonBeginCall(void)
{
    #if defined __XPY__
    return;
    #endif

    PyEval_RestoreThread(TSTATE);
}

void PythonEndCall(void)
{
    #if defined __XPY__
    return;
    #endif

    TSTATE = PyEval_SaveThread();
}

void PythonExit(void)
{
    PyObject *app, *pResult;

    #if defined __XPY__
    return;
    #endif

    InformerDEBUG("********** NOW GOING TO EXIT PYTHON *****************\n");

    PythonBeginCall();

    InformerDEBUG("NOW STOPPING THE APP\n");

    app = PythonGetApp();
    pResult = PyObject_CallMethod(app, "stop", NULL);

    Py_DECREF(pResult);
    Py_DECREF(app);

    // This crashes with:
    // Ending the interpreter...
    // Fatal Python error: Py_EndInterpreter: not the last thread
    // Application: flame 2008 x86_64
    // Error: abnormal termination, signal = 6
    // SIGABRT - Abort
    // Unknown user initiated signa
    // Killed
    //
    // InformerDEBUG("Ending the interpreter...\n");
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

    // InformerDEBUG("Now finalizing python...\n");
    // Py_Finalize();
    // InformerDEBUG("python was finalized\n");

    // dlclose(PyModule);
    InformerDEBUG("dlclose was called\n");

    InformerDEBUG("\nGoodbye, cruel world\n");
}

PyObject *
PythonGetApp(void)
{
    return APP;
}

PyObject *
PythonAppGetSpark(PyObject *app, const char *spark_name)
{
    PyObject *spark;
    spark = PyObject_CallMethod(app, "sparkGetByName", "s", spark_name);

    if (NULL == spark || Py_None == spark) {
        InformerDEBUG("CRAP. could not get spark named (%s)\n", spark_name);
        spark = NULL;
    }

    return spark;
}
