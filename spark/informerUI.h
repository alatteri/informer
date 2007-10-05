#ifndef _INFORMER_UI_H
#define _INFORMER_UI_H

#include "spark.h"

typedef struct {
    unsigned int        id;                             /* the id of the boolean element */
    SparkBooleanStruct  *ui;                            /* ptr to the ui element */
    char                text[SPARK_MAX_STRING_LENGTH];  /* the text to display with the checkbox */
} InformerBooleanUI;

typedef struct {
    unsigned int        id;                             /* the id of the string element */
    SparkStringStruct   *ui;                            /* ptr to the ui element */
} InformerStringUI;

typedef struct {
    unsigned int        id;                             /* the id of the popup element */
    SparkPupStruct      *ui;                            /* ptr to the ui element */
} InformerPupUI;

typedef struct {
    unsigned int        id;                             /* the id of the button element */
    SparkPushStruct     *ui;                            /* prt to the ui element */
} InformerPushUI;

InformerPupUI InformerInitPupUI(unsigned int id, SparkPupStruct *ui);
InformerPushUI InformerInitPushUI(unsigned int id, SparkPushStruct *ui);
InformerStringUI InformerInitStringUI(unsigned int id, SparkStringStruct *ui);
InformerBooleanUI InformerInitBooleanUI(unsigned int id, SparkBooleanStruct *ui, char *text);

void InformerInitSetupUI(void);

#endif /* _INFORMER_UI_H */
