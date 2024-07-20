#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
char namebuf[256];
const char *random_file(long *pidx)
{
    FILE *flist=fopen("png-list.txt","r");
    if(flist==NULL){
        printf("open file error %d(%s)\n",errno,strerror(errno));
        return NULL;
    }
    long pos=*pidx;
    if(pos>0){
        pos--;
        fseek(flist,pos,SEEK_SET);
        while(1){
            int ch=fgetc(flist);
            if(ch==EOF){
                fclose(flist);
                *pidx=-1;
                return NULL;
            }
            if(ch=='\n'){
                pos++;
                break;
            }
            pos--;
            if(pos<=0){
                // pos 只应该是0
                rewind(flist);
                break;
            }
            fseek(flist,-2,SEEK_CUR);
        }
    }
    int idx=0;
    while(1){
        int ch=fgetc(flist);
        if(ch==EOF){
            *pidx=-1;
            if(idx){
                namebuf[idx]='\0';
                return &namebuf[0];
            } else {
                return NULL;
            }
        }
        if(ch=='\n'){
            namebuf[idx]='\0';
            break;
        } else {
            namebuf[idx]=ch;
        }
        idx++;
    }
    *pidx=pos+idx+1;
    fclose(flist);
    return &namebuf[0];
}