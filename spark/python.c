/* Example of embedding Python in another program */

#include <Python.h>
#include <dlfcn.h>

void initxyzzy(void); /* Forward */

void* PyModule;
PyThreadState *tstate = NULL;

void PythonInitialize(void)
{
    int argc = 1;
    char *argv[1];

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

    // create a brand spankin new interpreter
    // XXX the current PyThreadState* is returned
    // tstate = Py_NewInterpreter();
    // if (NULL == tstate) {
    //     gUnloadPython = 0;
    //     Py_FatalError("Unable to initialize Python interpreter.");
    //     return;
    // }

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

    PyRun_SimpleString("import threading\n");
    PyRun_SimpleString("import sitecustomize\n");

    PyRun_SimpleString("import instinctual\n");
    PyRun_SimpleString("import instinctual.informer\n");
    PyRun_SimpleString("from instinctual.informer.spark import Spark\n");
    PyRun_SimpleString("SPARK = Spark()\n");
    PyRun_SimpleString("SPARK.start()\n");

    PyRun_SimpleString("print 'DONE LOADING PYTHON'\n");

    tstate = PyEval_SaveThread();
}

void PythonExit(void)
{
    printf("********** NOW GOING TO EXIT PYTHON *****************\n");

    PyEval_RestoreThread(tstate);

    PyRun_SimpleString("print 'NOW STOPPING THE SPARK'\n");
    PyRun_SimpleString("SPARK.stop()\n");
    PyRun_SimpleString("del SPARK\n");

    printf("Now finalizing python...\n");
    // Py_Finalize();
    printf("python was finalized\n");

    // dlclose(PyModule);
    printf("dlclose was called\n");

    printf("\nGoodbye, cruel world\n");
}

/* A static module */

/* 'self' is not used */
static PyObject *
xyzzy_foo(PyObject *self, PyObject* args)
{
    return PyInt_FromLong(42L);
}

static PyMethodDef xyzzy_methods[] = {
    {"foo",     xyzzy_foo,  METH_NOARGS,
     "Return the meaning of everything."},
    {NULL,      NULL}       /* sentinel */
};

void
initxyzzy(void)
{
    PyImport_AddModule("xyzzy");
    Py_InitModule("xyzzy", xyzzy_methods);
}

