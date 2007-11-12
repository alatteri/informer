#ifndef _INFORMER_PYTHON_H
#define _INFORMER_PYTHON_H

#include <Python.h>
#include <stdio.h>

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

void PythonInitialize(void);
void PythonExit(void);
void PythonBeginCall(void);
void PythonEndCall(void);

PyObject *PythonGetApp(void);
PyObject *PythonAppGetSpark(PyObject *app, const char *spark_name);

#endif
