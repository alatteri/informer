#include <stdio.h>
#include <errno.h>
#include <stdarg.h>

#include "spark.h"
#include "informer.h"

extern int errno;

static char CURRENT_USER_ERR[] = "Unable to determine user";
static char GATEWAY_STATUS_ERR[] = "Unable to perform operation";

/*************************************
 * Informer globals
 *************************************/
InformerAppStruct    gApp;

/*--------------------------------------------------------------------------*/
/* Returns the main application data object                                 */
/*--------------------------------------------------------------------------*/
InformerAppStruct *InformerGetApp(void)
{
    return &gApp;
}

void InformerDEBUG(const char *format, ...)
{
    va_list args;
    char str[4096];

    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);

    fprintf(stderr, "DEBUG: %s", str);
}

void InformerERROR(const char *format, ...)
{
    va_list args;
    char str[4096];

    va_start(args, format);
    vsprintf(str, format, args);
    va_end(args);

    fprintf(stderr, "ERROR: %s", str);
    sparkError(str);
}

int InformerReadValAsInt(FILE *fp, int *data)
{
    char line[4096];

    if (TRUE == InformerReadValAsString(fp, line)) {
        *data = atoi(line);
        return TRUE;
    } else {
        return FALSE;
    }
}

int InformerReadValAsLong(FILE *fp, long int *data)
{
    char line[4096];

    if (TRUE == InformerReadValAsString(fp, line)) {
        *data = atol(line);
        return TRUE;
    } else {
        return FALSE;
    }
}

int InformerReadValAsString(FILE *fp, char *data)
{
    int length;
    char *start;
    char *end;
    char line[4096];

    if (fgets(line, 4096, fp) != NULL) {
        printf("The line is [%s]\n", line);
        start = index(line, ':');
        if (start != NULL) {
            start += 2;     /* the space after the : */
            length = strlen(line);
            end = line + length - 1;    /* no null and no newline */
            strncpy(data, start, end - start);
            data[end - start] = '\0';
            printf("I read [%s]\n", data);
            return TRUE;
        } else {
            InformerERROR("Parse error reading from filehandle: no key found\n");
            return FALSE;
        }
    } else {
        InformerERROR("Unable to read from filehandle: %s\n", strerror(errno));
        return FALSE;
    }
}

/*--------------------------------------------------------------------------*/
/* Returns the name of the current setup being worked on.                   */
/*--------------------------------------------------------------------------*/
const char *InformerGetSetupPath(void)
{
    SparkInfoStruct info;
    const char *setup;
    InformerAppStruct *app = InformerGetApp();

    setup = sparkGetLastSetupName();
    sparkGetInfo(&info);

    printf("------------------------------------------------\n");
    printf("app->setup_ui_setup_path---> [%s]\n", app->setup_ui_setup_path.ui->Value);
    printf("sparkGetLastSetupName --> [%s]\n", setup);
    printf("SparkInfoStruct.Name ---> [%s]\n", info.Name);
    printf("------------------------------------------------\n");
    return app->setup_ui_setup_path.ui->Value;
}

/*--------------------------------------------------------------------------*/
/* Returns path to the informer gateway script                              */
/*--------------------------------------------------------------------------*/
const char *InformerGetGatewayPath(void)
{
    const char *path = "/usr/discreet/instinctual/informer/bin/gateway";
    return path;
}

/*--------------------------------------------------------------------------*/
/* Get the current spark user into the char* argument                       */
/* Returns TRUE on success, FALSE on faliure                                */
/*--------------------------------------------------------------------------*/
int InformerGetCurrentUser(char *user, int max_length)
{
    FILE *fp;
    char line[1024];
    const char *program;
    const char *filepath;

    char *c;
    char *start;
    char *end;

    program = sparkProgramGetName();

    if (0 == strcmp("flame", program) ||
        0 == strcmp("flint", program) ||
        0 == strcmp("inferno", program)) {
        filepath = "/usr/discreet/user/effects/user.db";
    } else if (0 == strcmp("fire", program) ||
               0 == strcmp("smoke", program)) {
        filepath = "/usr/discreet/user/editing/user.db";
    } else {
        InformerERROR("%s: unknown program [%s]", CURRENT_USER_ERR, program);
        return FALSE;
    }

    if ((fp=fopen(filepath, "r")) == NULL) {
        InformerERROR("%s: can't open [%s]: %s", CURRENT_USER_ERR,
                      filepath, strerror(errno));
        return FALSE;
    }

    max_length -= 1; /* account for the null */

    do {
        c = fgets(line, 1024, fp);
        if (c != NULL) {
            InformerDEBUG("Checking line: %s\n", line);
            if (strstr(line, "UserGroupStatus:UsrGroup1={") != NULL) {
                InformerDEBUG("MATCH! -> %s\n", line);
                start = index(line, '{');
                end = rindex(line, '}');
                if (end != NULL && start != NULL) {
                    if (end - start < max_length - 1)
                        max_length = end - start;
                    InformerDEBUG("start [%c], end [%c], max_length [%d]\n", *start, *end, max_length);

                    strncpy(user, start + 1, max_length - 1);
                    user[max_length-1] = '\0';
                    InformerDEBUG("Going to return user [%s]\n", user);
                    return TRUE;
                }
            }
        }
    } while (c != NULL);


    return FALSE;
}

int InformerCallGateway(char *action, char *infile, char *outfile)
{
    int pid = 0;
    int index = 0;
    int status = 0;
    const char *setup;
    const char *gateway;
    const char *argv[10];
    const char **ptr = argv;

    setup = InformerGetSetupPath();
    gateway = InformerGetGatewayPath();

    InformerDEBUG("the gateway is [%s]\n", gateway);

    argv[index++] = gateway;
    argv[index++] = "-s";
    argv[index++] = setup;
    argv[index++] = "-a";
    argv[index++] = action;

    if (infile != NULL) {
        argv[index++] = "-i";
        argv[index++] = infile;
    }

    if (outfile != NULL) {
        argv[index++] = "-o";
        argv[index++] = outfile;
    }

    argv[index] = 0;    /* This is required to end the array */

    InformerDEBUG("---- calling gateway ----\n");
    while (*ptr) {
        InformerDEBUG("%s ", *ptr++);
    }
    InformerDEBUG("\n---- calling gateway ----\n");

    pid = sparkSystemNoSh(FALSE, argv[0], argv);
    sparkWaitpid(pid, &status, 0);
    InformerDEBUG("Gateway returned. status [%d]\n", status);

    if (0 != status) {
        // TODO: Map the status to a human readable string
        InformerERROR("%s: status [%d]", GATEWAY_STATUS_ERR, status);
        return FALSE;
    } else {
        return TRUE;
    }
}

