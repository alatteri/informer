#include "informer.h"
#include "informerUI.h"

/******************************************************************************************/
/*** INFORMER SETUP ***********************************************************************/
SparkStringStruct SparkSetupString15 = { "", "%s", SPARK_FLAG_NO_INPUT, NULL };

void InformerInitSetupUI(void)
{
    InformerAppStruct *app = InformerGetApp();
    app->setup_ui_setup_path = InformerInitStringUI(15, &SparkSetupString15);
}

InformerPupUI InformerInitPupUI(unsigned int id, SparkPupStruct *ui)
{
    InformerPupUI result;
    result.id = id;
    result.ui = ui;
    return result;
}

InformerPushUI InformerInitPushUI(unsigned int id, SparkPushStruct *ui)
{
    InformerPushUI result;
    result.id = id;
    result.ui = ui;
    return result;
}

InformerStringUI InformerInitStringUI(unsigned int id, SparkStringStruct *ui)
{
    InformerStringUI result;
    result.id = id;
    result.ui = ui;
    return result;
}

InformerBooleanUI InformerInitBooleanUI(unsigned int id, SparkBooleanStruct *ui, char *text)
{
    InformerBooleanUI result;
    result.id = id;
    result.ui = ui;
    sprintf(result.text, "%s", text);
    return result;
}

