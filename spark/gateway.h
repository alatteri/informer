#ifndef _GATEWAY_H
#define _GATEWAY_H

#include "informer.h"

int GatewayGetNotes(const char *setup, InformerNoteData *data);


int InformerGatewayImportNotes(char *filepath, int index, int update_count);
int InformerGatewayImportElems(char *filepath, int index, int update_count);
int InformerGatewayGetElems(void);
int InformerGatewayUpdateNote(int index, int note_id, int is_checked);
int InformerGatewayUpdateElem(int index, int note_id, int is_checked);
int InformerGatewayCall(char *action, char *infile, char *outfile);

#endif
