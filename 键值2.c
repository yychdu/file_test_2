#define _CRT_SECURE_NO_WARNINGS 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
int sec_flag = 0;
int key_flag = 0;

//.ini的节名有空格可以检测

 /*
 * @description         : 读取指定节名和键名ini配置文件的键值函数
 * @param – frame       :为ini文件名
 * @param - section     :为ini文件中的节名
 * @param - key         :为section节中间的键名
 * @param - value       :为存放键值的缓冲区
 * @param - size        :为value缓冲区尺寸   
 * @return : >=0 读取成功并返回实际键值尺寸
 *           -1 读取失败，原因是文件打开失败
 *           -2 读取失败，原因是节名不存在
 *           -3 读取失败，原因是键名不存在
*/
int IniReadStr(const char* fname , const char *section , const char *key , char *value , int size)
{  
    int result = -2,j = 0,i;
    char *begin, *k;
    char str[512]="";
    FILE *fp = fopen(fname, "r");
    if (fp == NULL)
    {
        result = -1;
        printf("open file err");
    }
    else
    {
        char *buf = (char *)malloc(sizeof(char) * size);    //buf[size]
        if (buf == NULL)
            printf("malloc err");
        else
        {
            while (fgets(buf, size, fp) != NULL)
            {
                k = strchr(buf, ';');
                if (k != NULL)          //如果是注释
                    continue;
                else
                {
                    begin = strchr(buf, '[');
                    if (begin == NULL)
                        continue;       //如果不是节 就获取下一个字符串
                    else
                    {
                        for (i = 1;  begin[i] != ']' ;i++)      //提取[]字符串
                        {
                            if(isalnum(*(begin+i)))
                                str[j++] = begin[i];
                        }
                        j = 0;
                        if(strnicmp(str,section,strlen(str)) == 0)
                        {
                            while (fgets(buf, size, fp) != NULL)
                            {
                                if (strchr(buf, '[') != NULL)       //到下一个节点还没找到
                                {
                                    result = -3;
                                    break;
                                }
                                else if ((strnicmp(buf,key,strlen(key)) == 0)  && buf[strlen(key)] == '=')
                                {
                                    for (i = 0; isprint(buf[strlen(key) + 1 + i]) ;i++)
                                        value[i] = buf[strlen(key) + 1 + i];
                                    result = strlen(value);
                                    // printf("%s\n", value);
                                    break;
                                }
                            }
                            break;
                        }
                        memset(str, 0, 512);
                    }
                }
               
            }
            free(buf);
        }
        
        fclose(fp);
    }
    return result;
}

int main()
{
    char filename[20] = "k.ini", section[20], key[20], value[512]="";
    printf("input section:\n");
    scanf("%s", section);
    printf("input key:\n");
    scanf("%s", key);
    int k=IniReadStr(filename, section, key, value, 512);
    printf("%d", k);
}