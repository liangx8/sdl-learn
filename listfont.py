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
        for root,dirs,files in os.walk(rootpath):
            for f in files:
                bad=False
                for ex in exclude:
                    if ex.search(f):
                        bad=True
                        break
                if bad:
                    continue
                fullname="{}/{}".format(root,f)
                if cp.match(fullname):
                    func(fullname)
    return find
class font:
    def __init__(self):
        self.__cnt=0
        self.__mono=[]
        self.__regular=[]
        self.__re=re.compile("Mono")
                        
    def run(self,x):
        self.__cnt=self.__cnt + 1
        if(self.__re.search(x)):
            self.__mono.append(x)
        else:
            self.__regular.append(x)
    def gen(self):
        with open("src/listfont.c","w+") as lf:
            print("const char *const monofont[]={",file=lf)
            start=True
            for one in self.__mono:
                if start:
                    start=False
                else:
                    print(",",end="",file=lf)
                print('"{}"'.format(one),file=lf)
            print('};',file=lf)
            print("const char *const fontname[]={",file=lf)
            start=True
            for one in self.__regular:
                if start:
                    start=False
                else:
                    print(",",end="",file=lf)
                print('"{}"'.format(one),file=lf)
            print('};',file=lf)
        print(len(self.__mono))
        print(len(self.__regular))
if __name__=="__main__":
    aa=font()
    pt=re.compile(".*(ttc$|otf$|ttf$)")
    cf=createFinder("/usr/share/fonts",aa.run)
    cf(pt)
    aa.gen()
