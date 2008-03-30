#ifndef _GATEWAY_H
#define _GATEWAY_H

#include "informer.h"
#include "spark.h"

void GatewayPyStringToInt(PyObject *pObj, int *data);
void GatewayPyStringToLong(PyObject *pObj, long int *data);
void GatewayPyStringToString(PyObject *pObj, char *data, int max_length);

void GatewayPyObjectToElemData(PyObject *pObj, InformerElemData *data);
void GatewayPyObjectToNoteData(PyObject *pObj, InformerNoteData *data);

void GatewaySetPixelAspectRatio(float pixel_aspect_ratio);
void GatewaySetFrameRate(double frame_rate);
int GatewayGetElems(InformerElemData *data);
int GatewayGetNotes(InformerNoteData *data);

int GatewayCreateNote(int is_checked, char *text);

int GatewayUpdateElem(InformerElemData *data, int id, int is_checked);
int GatewayUpdateNote(InformerNoteData *data, int id, int is_checked);

void GatewaySparkProcessStart(const char *spark_name);
char *GatewaySparkProcessFrameStart(const char *spark_name, SparkInfoStruct spark_info);
void GatewaySparkProcessFrameEnd(const char *spark_name);
void GatewaySparkProcessEnd(const char *spark_name);

char *GatewaySparkRegister(const char *spark_name);
int GatewaySparkRename(const char *old_name, const char *new_name);

#endif
