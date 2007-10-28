#ifndef _GATEWAY_H
#define _GATEWAY_H

#include "informer.h"

void GatewayPyStringToInt(PyObject *obj, int *data);
void GatewayPyStringToLong(PyObject *obj, long int *data);
void GatewayPyStringToString(PyObject *obj, char *data, int max_length);


int GatewayGetNotes(const char *setup, InformerNoteData *data);
int GatewayGetElems(const char *setup, InformerElemData *data);


int InformerGatewayImportNotes(char *filepath, int index, int update_count);
int InformerGatewayImportElems(char *filepath, int index, int update_count);
int InformerGatewayUpdateNote(int index, int note_id, int is_checked);
int InformerGatewayUpdateElem(int index, int note_id, int is_checked);
int InformerGatewayCall(char *action, char *infile, char *outfile);

#endif
