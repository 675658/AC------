# AC_Automata

AC自动机生成器,可以自动生成用于匹配给定字符串集的AC自动机

使用方式:
```shell
ac_automata mode [input file path [output file path [function name]]]
```
参数:

- mode:匹配模式

  0:AC自动机   出现预期外的字符后继续匹配,直到找到跟定字符串或到达缓冲区末尾
  
  1:有限自动机 出现预期外的字符之后立即返回
  
- input file path:输入的文件路径

  文件中是以换行分割的要匹配的字符串,支持c格式的转义字符
  
  文件末尾需要有一个空行
  
  若没有该项默认输入的文件为当前目录下的"rule.ac"文件
  
- output file path:输出的文件路径

  输出的文件内包含一个匹配函数
  
- function name:匹配函数的名称


例如 输入文件:
```
123
12345
1234567
23
2345
234567

```
最终生成的匹配函数格式如下:
```c
/**
 * @brief 有限自动机,程序自动生成
 * @param str 要匹配的字符串
 * @param length 字符串长度
 * @param state 状态
 * @param state_out 状态输出:
 * 1:123
 * 2:12345
 * 3:1234567
 * 4:23
 * 5:2345
 * 6:234567
 * @return unsigned 匹配过的长度
 */
unsigned match(char *str,unsigned length,unsigned short *state,unsigned short *state_out)
{
    int i;
    *state_out=0;
    for(i=0;i<length;i++){
        switch(*state){
        case 0:
            if(str[i]=='\x31'){*state=1;break;}
            if(str[i]=='\x32'){*state=8;break;}
            *state=0;
            return i+1;
        case 1:
            if(str[i]=='\x32'){*state=2;break;}
            *state=0;
            return i+1;
        case 2:
            if(str[i]=='\x33'){*state=3;break;}
            *state=0;
            return i+1;
        case 3:
            if(str[i]=='\x34'){*state=4;break;}
            *state_out=1;
            return i;
        case 4:
            if(str[i]=='\x35'){*state=5;break;}
            *state=0;
            return i+1;
        case 5:
            if(str[i]=='\x36'){*state=6;break;}
            *state_out=2;
            return i;
        case 6:
            if(str[i]=='\x37'){*state_out=3;return i+1;}
            *state=0;
            return i+1;
        case 8:
            if(str[i]=='\x33'){*state=9;break;}
            *state=0;
            return i+1;
        case 9:
            if(str[i]=='\x34'){*state=10;break;}
            *state_out=4;
            return i;
        case 10:
            if(str[i]=='\x35'){*state=11;break;}
            *state=0;
            return i+1;
        case 11:
            if(str[i]=='\x36'){*state=12;break;}
            *state_out=5;
            return i;
        case 12:
            if(str[i]=='\x37'){*state_out=6;return i+1;}
            *state=0;
            return i+1;
        default:
            break;
        }
    }
    return i+1;
}
```
注意:

调用时,需要传入state和state_out两个指针.

每次调用前,调用者需要将state清零.

调用后,函数返回值为已经匹配过的字符数量.

调用者需要通过state_out判断是否匹配成功.

示例:
```c
#include <stdio.h>
#include <string.h>
int main()
{
    unsigned short state,state_out;
    unsigned i;
    unsigned size;
    char buf[100];
    gets(buf);
    size=strlen(buf);
    for(i=0;i<size;){
        state=0;
        i+=match(&buf[i],size-i,&state,&state_out);
        if(state_out!=0){
            printf("%u\n",state_out);
        }
    }
    return 0;
}
```
