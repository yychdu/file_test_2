#ifndef __INI_READ
#define __INI_READ


static int NextLine(int ch, FILE *fp);
static int CleanBlank(FILE *fp);
static int FindSec(int ch, FILE *fp, const char *section, int *line_status);
static int KeyCompare(int ch, FILE *fp, int *key_size, const char *key);
static int BracketsCheck(int ch, FILE *fp);
static int ValueGet(int ch, FILE *fp, int size, char *value, int brackets_flag, int *result, int *check_fin);
int IniReadStr(const char *fname, const char *section, const char *key, char *value, int size);

#endif // !