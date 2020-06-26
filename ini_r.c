
#define _CRT_SECURE_NO_WARNINGS 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
int IniReadStr(const char* fname, const char* section, const char* key, char* value, int size)
{
    int result = -2;
    if (fname == NULL)
        result = -1;
    else if (section == NULL)
        result = -2;
    else if (key == NULL)
        result = -3;
    else if (value == NULL || size <= 0)
        result = -4;
    else
    {
        FILE* fp = fopen(fname, "r");
        if (fp != NULL)
        {
            int sec_len = strlen(section);
            int key_len = strlen(key);
            long count;
            long fp_begin, fp_next;
            bool is_found_sec = false;
            char* begin_ptr;
            char* end_ptr;
            int line_size = 256;
            char* row = (char*)malloc(line_size);
            
            if (row != NULL)
            {
                do
                {
                    fp_begin = ftell(fp);
                    count = GetRowSize(fp);
                    if (line_size < count)
                    {
                        line_size = count;
                        free(row);
                        row = (char*)malloc(line_size + 1);
                        if (row == NULL)
                            break;
                    }
                    if (count < 2)
                        ;
                    else if (GetRow(fp, row, count, fp_begin))
                    {
                        end_ptr = row + count;
                        begin_ptr = LeftTrim(row, end_ptr);
                        if (*begin_ptr == ';')
                            ;
                        else if (!is_found_sec )
                        {
                            if (*begin_ptr == '[' && end_ptr - begin_ptr >= sec_len + 2
                                && IsTrueSection(begin_ptr + 1, section, end_ptr - begin_ptr - 1, sec_len))
                                is_found_sec = true;
                        }
                        else if (end_ptr - begin_ptr >= 2)
                        {
                            if (*begin_ptr == '[' && IsSection(begin_ptr + 1, end_ptr - begin_ptr - 1))//没找到键，而且找到了下一个节
                            {
                                result = -3;
                                break;
                            }
                            else if (end_ptr - begin_ptr > key_len && GetTrueKeyValue(begin_ptr, end_ptr - begin_ptr, key, key_len, value, &size))
                            {
                                result = size;
                                break;
                            }
                        }
                    }
                    else
                        break;
                                      
                } while (!feof(fp));
                if (row != NULL)
                    free(row);
            }
            else
                printf("Error malloc failed");

            fclose(fp);
        }
        else
            result = -1;
        
    }
    return result;
}