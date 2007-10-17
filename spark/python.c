/* Example of embedding Python in another program */

#include "Python.h"
#include <dlfcn.h>

void initxyzzy(void); /* Forward */

void* PyModule;

void PythonInitialize(void)
{
    int argc = 1;
    char *argv[1];
    PyThreadState *myThreadState;
    PyThreadState *mainThreadState;
    PyInterpreterState *mainInterpreterState;

    PyModule = dlopen("/usr/lib64/libpython2.3.so", RTLD_LAZY|RTLD_GLOBAL);
    if (!PyModule) {
        printf("----------------- !!!!!!!!!!!! PyMODULE WAS FALSE --------------\n");
        return;
    }

    argv[0] = "/usr/bin/python";
    argv[1] = NULL;

    printf("********** NOW GOING TO INIT PYTHON *****************\n");

    printf("************ never before initialized! ************\n");

    /* Initialize the Python interpreter.  Required. */
    Py_Initialize();
    printf("1\n");

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(argv[0]);
    printf("2\n");

    // Initialize thread support
    PyEval_InitThreads();
    printf("3\n");

    // save a pointer to the main PyThreadState object
    mainThreadState = PyThreadState_Get();

    /* Add a static module */
    // initxyzzy();

    /* Define sys.argv.  It is up to the application if you
       want this; you can also let it undefined (since the Python 
       code is generally not a main program it has no business
       touching sys.argv...) */
    // PySys_SetArgv(argc, argv);
    printf("4\n");

    printf("))))))) now going to mainThreadState->interp ))))))))))))\n");

    // get a reference to the PyInterpreterState
    // mainInterpreterState = mainThreadState->interp;

    printf(")))))))) now going to create a thread state object for this thread ))))))))\n");

    // create a thread state object for this thread
    // myThreadState = PyThreadState_New(mainInterpreterState);

    // swap in my thread state
    // PyThreadState_Swap(myThreadState);

    /* Do some application specific code */
    printf("Hello, brave new world\n\n");

    /* Execute some Python statements (in module __main__) */
    // PyRun_SimpleString("import sys\n");
    // PyRun_SimpleString("print sys.builtin_module_names\n");
    // PyRun_SimpleString("print sys.modules.keys()\n");
    // PyRun_SimpleString("print sys.executable\n");
    // PyRun_SimpleString("print sys.argv\n");

    /* Note that you can call any public function of the Python
       interpreter here, e.g. call_object(). */
    printf("----------- what is sys ------------\n");
    PyRun_SimpleString("print 'sys is:', sys\n");
    printf("----------- going to import sys ------------\n");
    PyRun_SimpleString("import sys\n");
    printf("----------- going to append ------------\n");
    PyRun_SimpleString("sys.path.append('/usr/discreet/sparks/instinctual/informer/lib')\n");
    PyRun_SimpleString("sys.path.append('/usr/discreet/sparks/instinctual/informer/third_party/lib')\n");
    PyRun_SimpleString("print 'after:', sys.path\n");

    printf("----------- going to import instinctual ------------\n");
    PyRun_SimpleString("import instinctual\n");
    PyRun_SimpleString("print instinctual\n");

    printf("----------- going to import instinctual settings ------------\n");
    PyRun_SimpleString("from instinctual import settings\n");
    PyRun_SimpleString("print settings\n");

    printf("----------- going to import instinctual client ------------\n");
    PyRun_SimpleString("from instinctual.informer.client import Client, ClientConnectionError\n");
    PyRun_SimpleString("print Client, ClientConnectionError\n");
    PyRun_SimpleString("from instinctual.informer.spark import Spark\n");
    PyRun_SimpleString("Spark().start()\n");

    PyRun_SimpleString("print 'DONE LOADING PYTHON'\n");

    // clear the thread state
    // PyThreadState_Swap(NULL);

    // release the lock
    PyEval_ReleaseLock();
}

void PythonExit(void)
{
    printf("********** NOW GOING TO EXIT PYTHON *****************\n");
    /* Some more application specific code */
    printf("\nGoodbye, cruel world\n");
    /* Exit, cleaning up the interpreter */
    PyEval_AcquireLock();
    Py_Finalize();

    dlclose(PyModule);
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

