#ifndef _GATEWAY_H
#define _GATEWAY_H

#include "informer.h"
#include "spark.h"

void GatewayPyStringToInt(PyObject *pObj, int *data);
void GatewayPyStringToLong(PyObject *pObj, long int *data);
void GatewayPyStringToString(PyObject *pObj, char *data, int max_length);

void GatewayPyObjectToElemData(PyObject *pObj, InformerElemData *data);
void GatewayPyObjectToNoteData(PyObject *pObj, InformerNoteData *data);

void GatewaySetFrameRate(double frameRate);
int GatewayGetElems(const char *setup, InformerElemData *data);
int GatewayGetNotes(const char *setup, InformerNoteData *data);

int GatewayCreateNote(const char *setup, int is_checked, char *text, char *created_by);

int GatewayUpdateElem(const char *setup, InformerElemData *data, int id, int is_checked);
int GatewayUpdateNote(const char *setup, InformerNoteData *data, int id, int is_checked, char *modified_by);

void GatewaySparkProcessStart(const char *spark_name);
char *GatewaySparkProcessFrameStart(const char *spark_name, SparkInfoStruct spark_info);
void GatewaySparkProcessFrameEnd(const char *spark_name);
void GatewaySparkProcessEnd(const char *spark_name);

char *GatewaySparkRegister(const char *spark_name);
int GatewaySparkRename(const char *old_name, const char *new_name);

#endif
