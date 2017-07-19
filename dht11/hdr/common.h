#ifndef __COMMON_H__
#define __COMMON_H__


#define CMDSIZE     50
#define HISTSIZE    20

void clearScreen();
void setCursor(int x, int y);
void specialKey(char c);
void func(char *cmd);


#endif // COMMON_H_INCLUDED

