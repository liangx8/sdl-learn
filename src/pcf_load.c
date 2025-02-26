/*
pcf format
https://fontforge.org/docs/techref/pcf-format.html
*/

#include <stdint.h>
#include <stdio.h>
#include <malloc.h>

uint32_t msbyte4(unsigned char b[]);


struct PCF_TOC_ENTRY{
    uint32_t type;
    uint32_t format;
    uint32_t size;
    uint32_t offset;
};
// type field
#define PCF_PROPERTIES              (1<<0)
#define PCF_ACCELERATORS            (1<<1)
#define PCF_METRICS                 (1<<2)
#define PCF_BITMAPS                 (1<<3)
#define PCF_INK_METRICS             (1<<4)
#define PCF_BDF_ENCODINGS           (1<<5)
#define PCF_SWIDTHS                 (1<<6)
#define PCF_GLYPH_NAMES             (1<<7)
#define PCF_BDF_ACCELERATORS        (1<<8)

// format field
#define PCF_DEFAULT_FORMAT      0x00000000
#define PCF_INKBOUNDS           0x00000200
#define PCF_ACCEL_W_INKBOUNDS   0x00000100
#define PCF_COMPRESSED_METRICS  0x00000100

// format field may be modified by
#define PCF_GLYPH_PAD_MASK      (3<<0)            /* See the bitmap table for explanation */
#define PCF_BYTE_MASK           (1<<2)            /* If set then Most Sig Byte First */
#define PCF_BIT_MASK            (1<<3)            /* If set then Most Sig Bit First */
#define PCF_SCAN_UNIT_MASK      (3<<4)            /* See the bitmap table for explanation */
const char *toc_desc[]={
"PCF_PROPERTIES",
"PCF_ACCELERATORS",
"PCF_METRICS",
"PCF_BITMAPS",
"PCF_INK_METRICS",
"PCF_BDF_ENCODINGS",
"PCF_SWIDTHS",
"PCF_GLYPH_NAMES",
"PCF_BDF_ACCELERATORS"
};
// Properties Table
// struct ProperitsTable{
// uint32_t format;    　             /* Always stored with least significant byte first! */
// uint32_t nprops;                   /* Stored in whatever byte order is specified in the format */
// struct props {
//     uint32_t name_offset;          /* 属性名字的偏移 */
//     uint8_t isStringProp;
//     uint32_t value;                /* 属性的值，或者字符串值的偏移 */
// } props[nprops];
// char padding[(nprops&3)==0?0:(4-(nprops&3))];   /* pad to next int32 boundary */
// int string_size;                /* total size of all strings (including their terminating NULs) */
// char strings[string_size];
// char padding2[];
// };
void toc_comm(unsigned char *pcf,long size,long offset,uint32_t format,int seq){
    printf("%2d::type:%-21sformat:0x%04x, size:%8ld, offset:0x%06lx\n",seq,toc_desc[seq],format,size,offset);
    uint32_t *pu32=(uint32_t*)pcf;
    if(*pu32 != format){
        printf("warning : deference TOC format:%x,actual: %x",format,*pu32);
    }
}
void toc_properties(FILE *pcf,long size,long offset,uint32_t format)
{
    uint8_t *props=malloc(size);
    uint32_t nprop;
    fseek(pcf,offset,SEEK_SET);
    fread(props,1,size,pcf);
    toc_comm(props,size,offset,format,0);
    unsigned char *ptr=props+4;
    if(format & PCF_BYTE_MASK){
        nprop=msbyte4(ptr);
    } else {
        nprop=*((uint32_t *)ptr);
    }
    ptr +=4;
    int str_offset=(nprop * 9)/ 4 + 3;
    str_offset = str_offset * 4;
    int str_size;
    if(format & PCF_BYTE_MASK){
        str_size=msbyte4(props+str_offset);
    } else {
        str_size=*((uint32_t*)(props+str_offset));
    }
    printf("\t%d个属性,string占用内存%d字节\n",nprop,str_size);
    uint8_t *str_base=props+str_offset+4;
    for(int ix=0;ix<nprop;ix++){
        uint32_t name_offset,value;
        uint8_t isStringProp=*(ptr+ix*9+4);
        if(format & PCF_BYTE_MASK){
            name_offset=msbyte4(ptr+ix*9);
            value=msbyte4(ptr+ix*9+5);
        }else {
            name_offset=*((uint32_t *)(ptr+ix*9));
            value=*((uint32_t *)(ptr+ix*9+5));
        }
        //printf("\t%2d:name offset:%4d,isStringProp:%d,value:%9d/%8x\n",ix+1,name_offset,isStringProp,value,value);
        printf("\t%s=",str_base+name_offset);
        if(isStringProp){
            printf("%s\n",str_base + value);
        }else {
            printf("%d(integer)\n",value);
        }
    }
    free(props);
}
void toc_accelerators(FILE *pcf,long size,long offset,uint32_t format){
    uint8_t buf[4];
    fseek(pcf,offset,SEEK_SET);
    fread(&buf[0],1,4,pcf);
    toc_comm(buf,size,offset,format,1);
}
void toc_metrics(FILE *pcf,long size,long offset,uint32_t format){
    uint8_t buf[4];
    fseek(pcf,offset,SEEK_SET);
    fread(&buf[0],1,4,pcf);
    toc_comm(buf,size,offset,format,2);
    if(format & PCF_COMPRESSED_METRICS){
        uint8_t buf2[2];
        fread(&buf2[0],1,2,pcf);
        uint32_t cnt;
        if(format & PCF_BYTE_MASK){
            cnt = buf2[0] * 256 + buf2[1];
        } else {
            cnt = *((uint16_t *)&buf2[0]);
        }
        printf("\tmetrics count:%d\n",cnt);
    } else {
        printf("\t not implement\n");
    }
}

// lsbint32 format;                /* Always stored with least significant byte first! */
// int32 glyph_count;              /* byte ordering depends on format, should be the same as the metrics count */
// int32 offsets[glyph_count];     /* byte offsets to bitmap data */
// int32 bitmapSizes[4];           /* the size the bitmap data will take up depending on various padding options */
//                                 /*  which one is actually used in the file is given by (format&3) */
// char bitmap_data[bitmapsizes[format&3]];    /* the bitmap data. format contains flags that indicate: */
//                                 /* the byte order (format&4 => LSByte first)*/
//                                 /* the bit order (format&8 => LSBit first) */
//                                 /* how each row in each glyph's bitmap is padded (format&3) */
//                                 /*  0=>bytes, 1=>shorts, 2=>ints */
//                                 /* what the bits are stored in (bytes, shorts, ints) (format>>4)&3 */
//                                 /*  0=>bytes, 1=>shorts, 2=>ints */

struct {
    FILE *pcf;
    int32_t count;
    long seek_offsets;
    long seek_data;
}table_bitmaps;
void toc_bitmaps(FILE *pcf,long size,long offset,uint32_t format){
    uint8_t buf[256];
    fseek(pcf,offset,SEEK_SET);
    fread(&buf[0],8,4,pcf);
    toc_comm(buf,size,offset,format,3);
    int32_t glyph_count;
    if(format & PCF_BYTE_MASK){
        glyph_count = msbyte4(&buf[4]);
    } else {
        glyph_count = *((int32_t*)(&buf[4]));
    }
    table_bitmaps.count=glyph_count;
    table_bitmaps.seek_offsets=offset+8;
    long pos=offset+8+glyph_count*4;
    printf("\tglyph count:%d\n",glyph_count);
    for(int ix=0;ix<4;ix++){
        printf("\t\t%d\n",msbyte4(&buf[8+ix*4]));
    }
    fseek(pcf,pos,SEEK_SET);
    fread(&buf[0],16,4,pcf);
    for(int ix=0;ix<4;ix++){
        printf("\t\t%8x\n",msbyte4(&buf[ix*4]));
    }
}
void toc_ink_metrics(FILE *pcf,long size,long offset,uint32_t format){
    uint8_t buf[4];
    fseek(pcf,offset,SEEK_SET);
    fread(&buf[0],1,4,pcf);
    toc_comm(buf,size,offset,format,4);
}
void toc_bdf_encodings(FILE *pcf,long size,long offset,uint32_t format){
    uint8_t buf[4];
    fseek(pcf,offset,SEEK_SET);
    fread(&buf[0],1,4,pcf);
    toc_comm(buf,size,offset,format,5);
}
void toc_swidths(FILE *pcf,long size,long offset,uint32_t format){
    uint8_t buf[4];
    fseek(pcf,offset,SEEK_SET);
    fread(&buf[0],1,4,pcf);
    toc_comm(buf,size,offset,format,6);
}
// lsbint32 format;                 /* Always stored with least significant byte first! */
// int32 glyph_count;              /* byte ordering depends on format, should be the same as the metrics count */
// int32 offsets[glyph_count];     /* byte offsets to string data */
// int32 string_size;
// char string[string_size];

void toc_glyph_names(FILE *pcf,long size,long offset,uint32_t format){
    uint8_t buf[256];
    fseek(pcf,offset,SEEK_SET);
    fread(&buf[0],3,4,pcf);
    toc_comm(buf,size,offset,format,7);
    uint32_t cnt=0;
    if(format &PCF_BYTE_MASK){
        cnt=msbyte4(&buf[4]);
    } else {
        printf("\tnot implement\n");
        return;
    }
    uint32_t strings=cnt + 1 + 1 + 1; // format,glyph_count,string_size
    long str_offset=strings * 4 + offset;
    uint32_t str=msbyte4(&buf[8]);
    fseek(pcf,str_offset+str,SEEK_SET);
    fread(&buf[0],1,256,pcf);

    printf("\tglyph name count:%d first GLYPH NAME:%s\n",cnt,&buf[0]);
}
void toc_bdf_accelerators(FILE *pcf,long size,long offset,uint32_t format){
    uint8_t buf[4];
    fseek(pcf,offset,SEEK_SET);
    fread(&buf[0],1,4,pcf);
    toc_comm(buf,size,offset,format,8);
}

const void (*toc_handler[])(FILE *,long,long,uint32_t)={
    toc_properties,
    toc_accelerators,
    toc_metrics,
    toc_bitmaps,
    toc_ink_metrics,
    toc_bdf_encodings,
    toc_swidths,
    toc_glyph_names,
    toc_bdf_accelerators
    };
void test_pcf(const char *pcfname)
{
    FILE *pcf=fopen(pcfname,"r");
    if(pcf==NULL){
        printf("open %s error\n",pcfname);
        return;
    }
    char head[8];
    fread(&head[0],1,8,pcf);
    int cnt=*((uint32_t *)&head[4]);
    int size=sizeof(struct PCF_TOC_ENTRY)*cnt;
    printf("toc total:%d\n",cnt);
    struct PCF_TOC_ENTRY *toc=malloc(size);
    fread(toc,size,1,pcf);
    for(int ix=0;ix<cnt;ix++){
        for(int ty=0;ty<9;ty++){
            if(toc[ix].type== (1 << ty)){
                toc_handler[ty](pcf,toc[ix].size,toc[ix].offset,toc[ix].format);
            }
        }
        
    }
    printf("pos:%lx\n",ftell(pcf));
    free(toc);
    fclose(pcf);

}