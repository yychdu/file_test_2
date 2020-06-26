#define _CRT_SECURE_NO_WARNINGS 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
// ((unsigned)c <= ' ')
#define _IsBlank(c) ((unsigned)c <= ' ' && c != '\n')
#define _isAlpha(a) ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z'))
#define chricmp(a,b) ((a == b) || ((a ^ b) == 0x20 && _isAlpha(a)))

int NextLine(int ch, FILE* fp)
{
    while (ch != '\n' && ch != EOF)
        ch = fgetc(fp);
    return ch;
}

int CleanBlank(FILE* fp)
{
    int ch;
    do
    {
        ch = fgetc(fp); 
    } while (_IsBlank(ch));  //排除空格   
    return ch;
}

int FindSec(int ch, FILE* fp, const char *section, int *line_status)
{
    int i = 0;
    int sec_size = 0;           
    int judge_flag = 1;
    int search_flag = 0;
    
    //开始搜索      只处理一行
    while (1)
    {                       
        if (ch != ']')
        {
            ch = CleanBlank(fp);
            if (search_flag == 0)
            {
                search_flag++;
                for (i = 0; section[i] != '\0'; i++)
                {
                    if (chricmp(ch, section[i]))
                        sec_size++; //记得清零
                    else
                        break;
                    ch = fgetc(fp);
                }               
            } 
            else if (ch != ']')
            {
                judge_flag = 0;
                break;
            }                      
        }
        else
        {   
            ch = CleanBlank(fp);
            if (ch != '\n' && ch != EOF)
                judge_flag = 0;
            break;
        }        
    }

    if (judge_flag == 1 && section[sec_size] == '\0')
        *line_status = 1;
    else 
        ch = NextLine(ch, fp);

    return ch;
}

int KeyCompare(int ch, FILE* fp, int *key_size, const char *key)
{
    *key_size = 0;
    for (int j = 0; key[j] != '\0' && ch != '\n' && ch != EOF; j++)
    {   
        if (chricmp(ch, key[j]))
            (*key_size)++; //记得清零
        else
            break;
        ch = fgetc(fp);
    }
    return ch;
}

int BracketsCheck(int ch, FILE* fp)            
{
    int result = 0;
    while (ch != '\n' && ch != EOF)
    {
        if (ch == ']')
            result = 1;
        else if ((unsigned)ch > ' ')
            result = 0;
        ch = fgetc(fp);
    }
    return result;
}

int ValueGet(char ch, FILE* fp, int size, char *value, int brackets_flag, int *result, int *check_fin) 
{
    int j = 0; 
    *check_fin = 0;
    while (_IsBlank(ch))
        ch = fgetc(fp);       //排除空字符      换行符？
    if (ch == '=')
    {   
        ch = CleanBlank(fp);
        while (ch != '\n' && ch != EOF)
        {    
            if (j < size)
                value[j++] = ch;
            if (ch == ']' && brackets_flag == 1)
            {
                ch = CleanBlank(fp);
                if (ch == '\n' || ch == EOF)        //找到节
                {
                    *check_fin = 1;
                    break;
                }
                else
                    continue;
            } 
            ch = fgetc(fp);
        }
        if (*check_fin)
            *result = -3;
        else
            *result = j;
    }
    else 
        ch = NextLine(ch, fp);
    return ch;
}

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
    int  key_size = 0,line_status = 0, brackets_flag = 0, check_fin;   
    int result = -2;
    int ch, j = 0;                         //必须 int 不能 char !!!!!!!!!!!
    FILE *fp = fopen(fname, "r");   //没有关闭
    if (fp == NULL)
    {
        result = -1;
        printf("open file err");
    }
    else if (value == NULL || size <= 0)
        result = -1;
    else if (section == NULL)
            ;
    else if (key == NULL)
        result = -3;
    else
    {
        ch = fgetc(fp);
        while (ch != EOF)   //开始           行 ：状态判断
        {          
            if ((unsigned)ch <= ' ')   //排除行首空格 
            {
                ch = fgetc(fp);
                continue;
            }
            else if (ch == ';')                     //状态1：分号开头 跳过此行
                ch = NextLine(ch, fp);
            else if (line_status == 1)              //状态3：查找KEY
            {
                brackets_flag = (ch == '[');
                //comparekey
                ch = KeyCompare(ch, fp, &key_size, key);

                if (key[key_size] == '\0')
                {
                    ch = ValueGet(ch, fp, size, value, brackets_flag, &result, &check_fin);
                    if (check_fin)
                        break;
                }
                else if (brackets_flag)
                {
                    if(BracketsCheck(ch, fp))
                        break;
                }
                else 
                    ch = NextLine(ch, fp); 
            }
            else if (ch == '[') //状态2：查找节点  
            {
                ch = FindSec(ch, fp, section, &line_status);
                if (line_status)
                    result = -3;
            }                      
            else            //什么都不是
                ch = NextLine(ch, fp);
        }       
    }
    fclose(fp);     
    return result;
}

int main()
{
    char filename[20] = "k.ini", section[20], key[20], value[512]="";
    printf("input section:\n");
    // scanf("%s", section);
    gets(section);
    printf("input key:\n");
    scanf("%s", key);
    int k=IniReadStr(filename, section, key, value, 512);
    if (k>=0)
        printf("finded!size:%d\n", k);
    else if (k==-1)
        puts("file err");
    else if (k==-2)
        puts("section not found");
    else if (k == -3)
        puts("key not found");
    return k;
}