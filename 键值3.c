#define _CRT_SECURE_NO_WARNINGS 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

//.ini的节名有空格可以检测

int chricmp(const int a, const int b)    //中文问题？？？ 
{
    int result = 0;  //0 代表不等 1 代表相等 
    if (a == b)
        result = 1;
    else if (a + 'A' - 'a' == b || a - ('A' - 'a') == b)        //有问题
        result = 1;
    return result;
}
#if 0
    [ asdas =  asdw  ]     //]末尾是结束符 或者换行符 
   [dwqd] = qwe
 ddas = qwe =sdqe
#endif
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
int IniReadStr(const char* fname , const char *section , const char *key , char *value , int size)  //节 键为空情况 未考虑
{  
    int result = -2,j = 0,i = 0,sec_size = 0,judge_flag = 1,key_size = 0;
    int ch;                         //必须 int 不能 char 
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
        while (ch != EOF)       //  有错误 自己思考  
        {
            if(ch <= ' ')   //排除行首空格  负数没考虑!!!
                ch = fgetc(fp);
            if (sec_size == strlen(section))    //查找到节点    size 用完之后清0  
            {
                //搜索键
                result = -3; //键不存在
                while (ch != '[' && ch != EOF)
                {
                    if(isalnum(ch))     //中文
                    {
                        judge_flag = 1;
                        while (ch != '=' && ch != '\n')
                        {
                            if (judge_flag == 1 && chricmp(ch, key[i++]) != 0 )
                                key_size++;
                            else
                                judge_flag = 0;
                            ch = fgetc(fp); 
                        }
                        i = 0;
                    }
                    if (key_size == strlen(key) && ch == '=')
                    {
                        ch = fgetc(fp); // '=' 往后移
                        while (ch != '\n')
                        {
                            value[i++] = ch;
                            ch = fgetc(fp);
                        }
                        result = strlen(value);
                    }
                    key_size = 0;
                    ch = fgetc(fp);
                }
                sec_size = 0;
            }
            else                    //查找节点 
            {
                if (ch == ';')  //如果是分号 跳过此行       没考虑死循环 最后一行 !!!
                {
                    while(ch != '\n')
                        ch = fgetc(fp);
                }
                else if (ch == '[')
                {
                    judge_flag = 1;
                    //开始搜索
                    while (ch != '\n')
                    {
                        ch = fgetc(fp);  
                        if(isalnum(ch))
                        {
                            while (ch != ' ' && ch != ']')      //判断简单   需要考虑很多情况
                            {
                                if(judge_flag == 1 && chricmp(ch, section[i++]) != 0 )
                                    sec_size++;
                                else
                                {
                                    judge_flag = 0;
                                    break;
                                }
                                ch = fgetc(fp);
                            }
                            i = 0;
                        }
                    }
                }
                else            //什么都不是
                    ch = fgetc(fp);
            }  
        }
    }
    fclose(fp);
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