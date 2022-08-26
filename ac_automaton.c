#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint64_t undefined8;
typedef uint32_t undefined4;
typedef uint16_t undefined2;
typedef uint8_t undefined;
typedef uint8_t byte;
typedef int16_t longlong;
typedef uint16_t ulonglong;

struct trie_tree_node
{
    unsigned char c;
    unsigned char child_num;
    unsigned short state_out;
    unsigned short out_distance;
    unsigned short state;
    unsigned short fail;
    unsigned short father;
    unsigned short child[256];
};
struct trie_tree_node *trie_tree;
unsigned p_free_node=1;
unsigned node_num=16384;
unsigned short output_state_con=0;

unsigned short new_node(unsigned short father,unsigned char c)
{
    unsigned i;
    void *new_tree;
    
    if (node_num<=p_free_node){
        node_num*=2;
        new_tree=realloc(trie_tree,node_num*sizeof(struct trie_tree_node));
        printf("realloc:%d\n");
        if (new_tree==NULL){
            node_num/=2;
            return 0;
        }
        trie_tree=(struct trie_tree_node*)new_tree;
    }
    trie_tree[p_free_node].c=c;
    trie_tree[p_free_node].child_num=0;
    trie_tree[p_free_node].state_out=0;
    trie_tree[p_free_node].out_distance=0;
    trie_tree[p_free_node].state=p_free_node;
    trie_tree[p_free_node].fail=0;
    trie_tree[p_free_node].father=father;
    for(i=0;i<256;i++){
        trie_tree[p_free_node].child[i]=0;
    }
    return p_free_node++;
}

int build_ac_automata(void)

{    
    unsigned i;
    unsigned j;
    unsigned short pfail;
    for(i=1;i<p_free_node;i++){
        if(trie_tree[i].father==0){
            continue;
        }
        pfail=trie_tree[trie_tree[i].father].fail;
        while(trie_tree[pfail].child[trie_tree[i].c]==0){
            if(pfail==0){
                break;
            }
            pfail=trie_tree[pfail].fail;
        }
        trie_tree[i].fail=trie_tree[pfail].child[trie_tree[i].c];
    }
    return 0;
}


int build_trie_tree(void)

{
    void *new_tree;
    int i;

    new_tree=malloc(node_num*sizeof(struct trie_tree_node));
    if (new_tree==NULL){
        printf("ERR:No free memory\n");
        return -1;
    }
    trie_tree=(struct trie_tree_node*)new_tree;
    trie_tree[0].c=0;
    trie_tree[0].child_num=0;
    trie_tree[0].state_out=0;
    trie_tree[0].out_distance=0;
    trie_tree[0].state=0;
    trie_tree[0].father=0;
    trie_tree[0].fail=0;
    for(i=0;i<256;i++){
        trie_tree[0].child[i]=0;
    }
    return 0;
}


int escape_char_chaege(unsigned char *buf1,unsigned char *buf2)
{
    unsigned i=0,j=0,k;
    while(buf1[i]!='\0'){
        if(buf1[i]!='\\'){
            buf2[j++]=buf1[i++];
            continue;
        }
        i++;
        switch(buf1[i]){
        case 'a':
            buf2[j++]='\a';
            break;
        case 'b':
            buf2[j++]='\b';
            break;
        case 'f':
            buf2[j++]='\f';
            break;
        case 'n':
            buf2[j++]='\n';
            break;
        case 'r':
            buf2[j++]='\r';
            break;
        case 't':
            buf2[j++]='\t';
            break;
        case 'v':
            buf2[j++]='\v';
            break;
        case '\\':
            buf2[j++]='\\';
            break;
        case '\'':
            buf2[j++]='\'';
            break;
        case '\"':
            buf2[j++]='\"';
            break;
        case '?':
            buf2[j++]='\?';
            break;
        case '0':
            buf2[j++]='\0';
            break;
        case 'x':
            i++;
            buf2[j]=0;
            for(k=0;k<2;k++){
                if(buf1[i+k]>='0' && buf1[i+k]<='9'){
                    buf2[j]+=(buf1[i+k]-'0')<<(4*(1-k));
                }
                else if(buf1[i+k]>='a' && buf1[i+k]<='f'){
                    buf2[j]+=(buf1[i+k]-'a'+10)<<(4*(1-k));
                }
                else{
                    return -i;
                }
            }
            i++;
            j++;
            break;
        case 'X':
            i++;
            buf2[j]=0;
            for(k=0;k<2;k++){
                if(buf1[i+k]>='0' && buf1[i+k]<='9'){
                    buf2[j]+=(buf1[i+k]-'0')<<(4*1-k);
                }
                else if(buf1[i+k]>='A' && buf1[i+k]<='F'){
                    buf2[j]+=(buf1[i+k]-'A'+10)<<(4*1-k);
                }
                else{
                    return -i;
                }
            }
            i++;
            j++;
            break;
        default:
            return -i;
            break;
        }
        i++;
    }
    return j;
}
int add_word(unsigned char *word,int size)

{
    unsigned i;
    unsigned short p_node;
    unsigned short out_distance;
    unsigned short last_state_out;
    if(size>0){
        p_node=0;
        out_distance=0;
        last_state_out=0;
        for(i=0;i<size;i++){
            if(trie_tree[p_node].child[word[i]]==0){
                trie_tree[p_node].child_num++;
                trie_tree[p_node].child[word[i]]=new_node(p_node,word[i]);
                if(trie_tree[p_node].child[word[i]]==0){
                    return -1;
                }
            }
            p_node=trie_tree[p_node].child[word[i]];
            if(trie_tree[p_node].state_out!=last_state_out && trie_tree[p_node].state_out!=0){
                last_state_out=trie_tree[p_node].state_out;
                out_distance=1;
            }
            else if(out_distance!=0){
                trie_tree[p_node].out_distance=out_distance;
                out_distance++;
            }
        }
        output_state_con++;
        trie_tree[p_node].state_out=output_state_con;
        trie_tree[p_node].out_distance=0;
    }
    return 0;
}




int main(int argc,char **argv)

{
    char *input_file="rule.ac";
    char *output_file="match.c";
    char *fun_name="match";
    unsigned char buf1[256];
    unsigned char buf2[256];
    unsigned len;
    unsigned line=1;
    unsigned mode;
    int i;
    int j;
    FILE *fp_in;
    FILE *fp_out;
    if(argc>1){
        if(sscanf(argv[1],"%u",&mode)!=1){
            printf("ac_automation mode [input file] [output file] [function name]\nmode:\n-0 normal mode\n-1 strict mode\n");
            return 0;
        }
    }
    else{
        printf("ac_automation mode [input file] [output file] [function name]\nmode:\n-0 normal mode\n-1 strict mode\n");
        return 0;
    }
    if (argc>2){
        input_file=argv[2];
    }
    fp_in=fopen(input_file,"r");
    if (argc>3) {
        output_file = argv[3];
    }
    fp_out=fopen(output_file,"w");
    if (argc>4) {
        fun_name=argv[4];
    }
    if ((fp_in==NULL) || (fp_out==NULL)){
        perror("ERR:");
        return -1;
    }
    build_trie_tree();
    fputs("/**\n",fp_out);
    if(mode==1){
        fputs(" * @brief 有限自动机,程序自动生成\n",fp_out);
    }
    else{
        fputs(" * @brief AC自动机,程序自动生成\n",fp_out);
    }
    fputs(" * @param str 要匹配的字符串\n * @param length 字符串长度\n * @param state 状态\n * @param state_out 状态输出:\n",fp_out);
    while(fgets(buf1,256,fp_in)!=NULL){
        len=strlen(buf1);
        if(buf1[len-1]!='\n'){
            printf("ERROR:Line %d:String is Too Long!\n",line);
            return -1;
        }
        buf1[len-1]='\0';
        if((len=escape_char_chaege(buf1,buf2))<=0){
            printf("ERROR:Line %d:Escape Char Error!\n%s\n",line,buf1);
            while(len<0){
                putchar(' ');
                len++;
            }
            puts("^\n");
            return -1;
        }
        line++;
        puts(buf1);
        add_word(buf2,len);
        fprintf(fp_out," * %d:%s\n",output_state_con,buf1);
        fflush(fp_out);
    }
    fputs(" * @return unsigned 匹配过的长度\n */\n",fp_out);
    build_ac_automata();
    fprintf(fp_out,"unsigned %s(char *str,unsigned length,unsigned short *state,unsigned short *state_out)\n{\n",fun_name);
    fputs("    int i;\n    *state_out=0;\n",fp_out);
    fputs("    for(i=0;i<length;i++){\n",fp_out);
    fputs("        switch(*state){\n",fp_out);
    for(i=0;i<p_free_node;i++){
        if(trie_tree[i].child_num==0){
            continue;
        }
        fprintf(fp_out,"        case %d:\n",trie_tree[i].state);
        for(j=0;j<256;j++){
            if(trie_tree[i].child[j]!=0){
                if(trie_tree[trie_tree[i].child[j]].state_out!=0){
                    if(trie_tree[trie_tree[i].child[j]].child_num==0)
                        fprintf(fp_out,"            if(str[i]=='\\x%.2x'){*state_out=%d;return i+1;}\n",j,trie_tree[trie_tree[i].child[j]].state_out);
                    else
                        fprintf(fp_out,"            if(str[i]=='\\x%.2x'){*state=%d;*state_out=%d;break;}\n",j,trie_tree[i].child[j],trie_tree[trie_tree[i].child[j]].state_out);
                }
                else{
                    fprintf(fp_out,"            if(str[i]=='\\x%.2x'){*state=%d;break;}\n",j,trie_tree[i].child[j]);
                }
            }
        }
        if(trie_tree[i].state_out!=0 || trie_tree[i].out_distance!=0){
            fprintf(fp_out,"            return i-%d;\n",trie_tree[i].out_distance);
        }
        else{
            if(mode==1){
                fputs("            *state=0;\n",fp_out);
                fputs("            return i+1;\n",fp_out);
            }
            else{
                fprintf(fp_out,"            *state=%d;\n",trie_tree[i].fail);
                fputs("            break;\n",fp_out);
            }
        }
    }
    fputs("        default:\n            break;\n        }\n    }\n",fp_out);
    fputs("    return i+1;\n}\n",fp_out);
    fclose(fp_in);
    fclose(fp_out);
    return 0;
}