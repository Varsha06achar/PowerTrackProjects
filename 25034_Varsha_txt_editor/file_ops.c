#include "file_ops.h"

Status openFile(TextEditor *editor, char *file_name)
{
    FILE *fp = fopen(file_name, "r");
    if(fp == NULL)
    {
        perror("fopen");
        return FAILURE;
    }

    char *buffer = malloc(sizeof(char) * MAX_BUFFER_SIZE);
    if(buffer == NULL)
    {
        fclose(fp);
        return FAILURE;
    }
        
    int ch;

    int count = 0;
    int capacity = MAX_BUFFER_SIZE;
    while((ch = fgetc(fp)) != EOF)
    {
        if(ch == '\n')
        {
            buffer[count] = '\0';
            addLine(editor, buffer);
            count = 0;      
        }
        else
        {
            if(count < capacity)
            {
                buffer[count++] = (char)ch;
            }
            else
            {
                capacity = 2 * capacity;
                char *temp = realloc(buffer, sizeof(char) * capacity);
                if(temp == NULL)
                {
                    perror("realloc");
                    free(buffer);
                    fclose(fp);
                    return FAILURE;
                }
                
                buffer = temp;
                buffer[count++] = (char)ch;
            }
        }
    }

    //Left over need to be flushed
    if(count > 0)
    {
        buffer[count] = '\0';
        addLine(editor, buffer);
    }

    free(buffer);
    fclose(fp);
    return SUCCESS;
}

Status saveFile(TextEditor *editor)
{
    if(editor->head == NULL)
        return FAILURE;

    FILE *fp = fopen("fileSave.txt", "w");
    if(fp == NULL)
    {
        perror("fopen");
        return FAILURE;
    }
    
    Node *temp = editor->head;
    while(temp)
    {
        fprintf(fp, "%s\n", temp->text);
        temp = temp->next;
    }

    fclose(fp);
    return SUCCESS;
}