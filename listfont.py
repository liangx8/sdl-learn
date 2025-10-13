import os
import re

def createFinder(rootpath,func):
    exclude=(re.compile("Thai"),re.compile("Kannada")
             ,re.compile("Condensed"),re.compile("Kufi")
             ,re.compile("Telugu"),re.compile("Gujarati")
             ,re.compile("TaiLue"),re.compile("Hebrew")
             ,re.compile("Gurmukhi"),re.compile("Syriac")
             ,re.compile("Bengali"),re.compile("Georgian")
             ,re.compile("Brahmi"),re.compile("Malayalam")
             ,re.compile("Thaana"),re.compile("Arabian")
             ,re.compile("Balinese"),re.compile("Armenian")
             ,re.compile("Khmer"),re.compile("Myanmar")
             ,re.compile("Medefaidrin"),re.compile("Bhaiksuki")
             ,re.compile("Test"),re.compile("Toto")
             ,re.compile("Sinhala"),re.compile("Lao")
             ,re.compile("Limbu"),re.compile("Tamil")
             ,re.compile("Adlam"),re.compile("Wancho")
             ,re.compile("Symbols"),re.compile("TaiTham")
             ,re.compile("Carian"),re.compile("Meetei")
             ,re.compile("Rohingya"),re.compile("NKo")
             ,re.compile("Tifinagh"),re.compile("Tibetan")
             ,re.compile("Masaram"),re.compile("Sharada")
             ,re.compile("Inscriptional"),re.compile("Ahom")
             ,re.compile("Lisu"),re.compile("Yi")
             ,re.compile("Buginese"),re.compile("Devanagari")
             ,re.compile("Extra"),re.compile("Aboriginal")
             ,re.compile("Oriya"),re.compile("Dogra")
             ,re.compile("Vithkuqi"),re.compile("Meroitic")
             ,re.compile("Persian"),re.compile("Mende")
             ,re.compile("Bamum"),re.compile("Avestan")
             ,re.compile("Phoenician"),re.compile("Cham")
             ,re.compile("LinearA"),re.compile("Display")
             ,re.compile("Saurashtra")
             )
    def find(cp):
        '''cp 文件名最后的字母的列表'''
        for root,dirs,files in os.walk(rootpath):
            for f in files:
                bad=False
                for ex in exclude:
                    if ex.search(f):
                        bad=True
                        break
                if bad:
                    continue
                if f.endswith(cp):
                    fullname="{}/{}".format(root,f)
                    func(fullname)
    return find
class font:
    def __init__(self):
        self.__cnt=0
        self.__mono=[]
        self.__regular=[]
        self.__cjk=[]
        self.__re=re.compile("Mono")
        self.__recjk=re.compile("CJK-")
                        
    def run(self,x):
        self.__cnt=self.__cnt + 1
        if(self.__re.search(x)):
            self.__mono.append(x)
            return
        if(self.__recjk.search(x)):
            self.__cjk.append(x)
            return
        self.__regular.append(x)
    def gen(self):
        with open("src/listfont.c","w+") as lf:
            putx("monofont",lf,self.__mono)
            putx("cjkfont",lf,self.__cjk)
            putx("fontname",lf,self.__regular)
        print(len(self.__mono))
        print(len(self.__regular))
        print(len(self.__cjk))
def putx(name,out,lists):
    #print("const char *const {}[]={".format(name),file=out)
    print("\nconst char *const {}[]=".format(name),file=out,end="{\n")
    cnt=len(lists)
    for seq in range(cnt):
        endWith=",\n"
        if seq == cnt-1:
            endWith="};\n"
        print('"{}"'.format(lists[seq]),file=out,end=endWith)

if __name__=="__main__":
    aa=font()
    cf=createFinder("/usr/share/fonts",aa.run)
    cf((".ttc",".otf",".ttf"))
    aa.gen()
