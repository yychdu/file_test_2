#define _CRT_SECURE_NO_WARNINGS 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
int sec_flag = 0;
int key_flag = 0;
int IniReadStr(const char* fname , const char *section , const char *key , char *value , int size )
{
    int retsize = 0,truelen = 0,i = 0;
    FILE *fp = fopen(fname, "r");
    char buf[512],temp_sec[512],str[512];//超过512怎么办
    if (fp == NULL)
    {
        printf("open file err");
        return -1;
    }
    sprintf(temp_sec, "[%s]", section); //加空格问题
    while(fgets(buf, 512, fp)!=NULL)
    {
        if(strnicmp(buf,temp_sec,strlen(temp_sec)) == 0)
        {
            sec_flag = 1;
            while(fgets(str, 512, fp) != NULL)
            {
                if(str[0] == '[')
                    return -3;//返回的时候要关闭文件
                if((strnicmp(str,key,strlen(key)) == 0) && str[strlen(key)] == '=')
                   {

                       for (; ;i++)
                       {
                           if (str[i] == '\0' || str[i-1] == ';')
                            {
                                truelen = i -1;
                                break;
                            }
                       }
                       for (i = 0;;i++)
                       {
                           if (str[strlen(key) + 1 + i] == '\0' || str[strlen(key)  +1+ i] == ';')
                                break;
                           value[i] = str[strlen(key) + 1 + i];
                       }
                           //    printf("len%d\n", truelen);
                    //    printf("%s\n", value);
                       return truelen - strlen(key) - 1;
                   }
            }
        }
    }
    fclose(fp);
    if(sec_flag==0)
        return -2;

}


int main()
{
    char filename[20], section[20], key[20], value[512];
    printf("input filename:\n");
    scanf("%s", filename);
    printf("input section:\n");
    scanf("%s", section);
    printf("input key:\n");
    scanf("%s", key);
    int k=IniReadStr(filename, section, key, value, 512);
    printf("%d", k);
}