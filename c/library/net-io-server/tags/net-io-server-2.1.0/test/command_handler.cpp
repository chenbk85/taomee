#include "command_handler.h"
#include <string.h>

int parse_command(const char *cmd, int length, char *name)
{
    int i = 0;

    while ((i < length)
           && (cmd[i] == ' ' || cmd[i] == '\t' || cmd[i] == '\r' || cmd[i] == '\n'))
    {
        ++ i;
    }

    if (i >= length)
    {
        return -1;
    }

    if ((length - i) < 4)
    {
        return -1;
    }

    if (strncmp(cmd + i, "get", 3) == 0)
    {
        if (cmd[i + 3] != ' ')
        {
            return -1;
        }

        i += 4;
        int j = i;

        while (j < length
               && (cmd[j] != ' ')
               && cmd[j]
               && (cmd[j] != '\r')
               && (cmd[j] != '\n'))
        {
            ++ j;
        }

        memcpy(name, cmd + i, j - i);
        name[j - i] = '\0';
        return 0;
    }
    else
    {
        return -2;
    }
}
