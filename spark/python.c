/* Example of embedding Python in another program */

#include <Python.h>
#include <dlfcn.h>

void initxyzzy(void); /* Forward */

void* PyModule;

int gUnloadPython = 0;

PyThreadState *myThreadState = NULL;
PyThreadState *mainThreadState = NULL;
PyInterpreterState *mainInterpreterState = NULL;

void PythonInitialize(void)
{
    int argc = 1;
    char *argv[1];
    PyThreadState *tempState = NULL;

    PyModule = dlopen("/usr/lib64/libpython2.3.so", RTLD_LAZY|RTLD_GLOBAL);
    if (!PyModule) {
        printf("----------------- !!!!!!!!!!!! PyMODULE WAS FALSE --------------\n");
        return;
    }

    argv[0] = "/usr/bin/python";
    argv[1] = NULL;

    printf("********** NOW GOING TO INIT PYTHON *****************\n");

    if (Py_IsInitialized()) {
        printf("************ Python was already loaded ************\n");
        gUnloadPython = 0;
    } else {
        printf("************ never before initialized! ************\n");
        gUnloadPython = 1;

        // Initialize the Python interpreter.  Required.
        printf("Py_Initialize...\n");
        Py_Initialize();

        // Initialize thread support -- OK to call multiple times
        //printf("PyEval_InitThreads...\n");
        PyEval_InitThreads();

        // Save a pointer to the main PyThreadState object
        //printf("PyThreadState_Get...\n");
        //mainThreadState = PyThreadState_Get();

        //if (mainThreadState == NULL) {
        //    printf("*********** FUCKING SHIT MAIN THREAD IS NULL ***************\n");
        //}

        // Get a reference to the PyInterpreterState
        // mainInterpreterState = mainThreadState->interp;

        // Create a thread state object for this thread
        // myThreadState = PyThreadState_New(mainInterpreterState);

        // Release global lock
        // PyEval_ReleaseLock();

        // Acquire global lock
        // PyEval_AcquireLock();

        // Swap in main thread state
        // tempState = PyThreadState_Swap(mainThreadState);

        // printf(")))))))) now going to create a thread state object for this thread ))))))))\n");

        /* Do some application specific code */
        printf("Hello, brave new world\n\n");

        /* Execute some Python statements (in module __main__) */
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
        PyRun_SimpleString("from instinctual.informer.spark import Spark\n");
        PyRun_SimpleString("SPARK = Spark()\n");
        PyRun_SimpleString("SPARK.start()\n");

        PyRun_SimpleString("print 'DONE LOADING PYTHON'\n");

        mainThreadState = PyThreadState_Get();

        // PyRun_SimpleString("print 'NOW STOPPING THE SPARK'\n");
        // PyRun_SimpleString("SPARK.stop()\n");

        // Swap out the current thread
        // PyThreadState_Swap(tempState);

        // release the lock
        PyEval_ReleaseLock();
    }
}

void PythonExit(void)
{
    PyThreadState *tempState = NULL;

    printf("********** NOW GOING TO EXIT PYTHON *****************\n");
    /* Some more application specific code */
    printf("\nGoodbye, cruel world\n");

    if (gUnloadPython == 1) {
        //printf("(((( i loaded it. now unloading python ))))\n");

        printf("PyEval_AcquireLock...\n");
        PyEval_AcquireLock();
        printf("lock was acquired\n");

        //// Swap in main thread state
        tempState = PyThreadState_Swap(mainThreadState);

        //PyRun_SimpleString("print 'NOW STOPPING THE SPARK'\n");
        //PyRun_SimpleString("SPARK.stop()\n");

        Py_Finalize();

        //// Swap out the current thread
        PyThreadState_Swap(tempState);

        // Release global lock
        PyEval_ReleaseLock();

        //// Clean up thread state
        //// PyThreadState_Clear(myThreadState);
        //// PyThreadState_Delete(myThreadState);


        printf("python was finalized\n");

        dlclose(PyModule);
        printf("dlclose was called\n");
    } else {
        printf("---> gUnloadPython was false -- not doing shit\n");
    }
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

