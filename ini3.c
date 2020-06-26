#define _CRT_SECURE_NO_WARNINGS 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
// ((unsigned)c <= ' ')
#define _IsBlank(c) ((c >= 0 && c <'\n') || (c > '\n' && c <=' '))
#define _isAlpha(a) ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z'))

int chricmp(int a,int b)    
{
    return (a == b) || ((a ^ b) == 0x20 && _isAlpha(a));
}

int NextLine(FILE* fp)
{
    int ch;
    do
    {
        ch = fgetc(fp);
    } while (ch != '\n' && ch != EOF);
    return ch;
}

int CleanBlank(FILE * fp)
{
    int ch;
    do
    {
        ch = fgetc(fp); 
    } while (_IsBlank(ch));  //排除空格   
    return ch;
}

int FindSec(char ch, FILE * fp,const char *section,int *line_status)
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
            if (search_flag != 0 && ch != ']')
            {
                judge_flag = 0;
                break;
            }
            if(search_flag == 0)
            {
                search_flag++;
                for (i = 0; section[i] != '\0';i++)
                {
                    if (chricmp(ch,section[i]))
                        sec_size++; //记得清零
                    else
                        break;
                    ch = fgetc(fp);
                }               
            }                       
        }
        else
        {   
            ch = CleanBlank(fp);
            if (ch == '\n' || ch == EOF)
                break;
            else // 键
            {
                judge_flag = 0;
                break;
            }

        }        
    }
    if (judge_flag == 1 && sec_size == strlen(section) && strlen(section) != 0)
        *line_status = 1;
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
    int  key_size = 0,line_status = 0;   
    int result = -2;
    int ch,j = 0;                         //必须 int 不能 char !!!!!!!!!!!
    FILE *fp = fopen(fname, "r");   //没有关闭
    if (fp == NULL)
    {
        result = -1;
        printf("open file err");
    }
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
                ch = NextLine(fp);
            else if (ch == '[' && line_status == 0) //状态2：查找节点 
                ch = FindSec(ch, fp, section, &line_status);
            else if (line_status == 1)              //状态3：查找到节 
            {
                //搜索键            '['结束不对  双换行结束  等号不能隔开 以key长度判断
                result = -3;
                if (key_size == 0)
                {
                    for (j = 0; key[j] != '\0';j++)
                    {
                        if (chricmp(ch,key[j]))
                            key_size++; //记得清零
                        else
                            break;
                        ch = fgetc(fp);
                    }
                }
                if ((key_size != strlen(key)))      //长度不匹配  直接下一行
                {
                    ch = NextLine(fp);
                    key_size = 0;
                    ch = fgetc(fp);
                    if (ch == '\n')         //第二个换行符  结束寻找
                        line_status = 0;
                    continue;
                }                   
                else                    //长度匹配            
                {
                    while (_IsBlank(ch))
                        ch = fgetc(fp);       //排除空字符
                    if (ch == '=')    //后面是等号 找到了
                    {
                        ch = CleanBlank(fp);  //排除‘=’后空格
                        for (j = 0;ch != EOF && ch != '\n';j++)
                        {                                                   
                            value[j] = ch;
                            ch = fgetc(fp); 
                        }
                        value[j] = '\0';
                        result = strlen(value);
                        break;
                    }
                    else if (ch != '=')     //不是等号 只有一部分   下一行
                    {
                        ch = NextLine(fp);
                        key_size = 0;
                        continue;
                    }
                }
                   
            }
            else            //什么都不是
                ch = fgetc(fp);
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