#!/usr/bin/python
# -*- coding: utf-8 -*-
#
#
# http://aspn.activestate.com/ASPN/Cookbook/Python/Recipe/302633
# angepasst und ergänzt von MH

import zipfile, re, sys
try:
    from win32com.client import Dispatch
except:
    pass

# OpenOffice-Texte lesen
class ReadOO:
    def __init__(self, filename):
        zf = zipfile.ZipFile(filename, "r")
        self.data = zf.read("content.xml")
        zf.close()

    def getXML(self):
        return self.data

    def getData(self, collapse=1):
        stripxml = re.compile("<[^>]*?>", re.DOTALL|re.MULTILINE)
        self.data = self.data.replace('&apos;', "\'")
        return " ".join(stripxml.sub(" ", self.data).split())

        
# MS Word-Texte lesen       
class ReadWord:
    def __init__(self, filename):
        self.w = Dispatch("Word.Application")
        # self.w.Visible = 1
        self.w.Documents.Open(filename)
        self.data = self.w.ActiveDocument.Content.Text
        self.data = ' '.join(self.data.split())
        self.w.ActiveDocument.Close()
 
        # nicht optimal: es bleibt immer eine Instanz von Word
        # im Speicher - eigentlich müsst noch ein self.w.Quit() kommen,
        # aber das schliesst Word dann komplett ab, also auch eventuelle
        # Dokumente, die vorher schon geöffnet waren...
        
    def getData(self):
        return self.data
        
        

if __name__=="__main__":

    if len(sys.argv)>1:
        oo = ReadOO(sys.argv[1])
        print oo.getXML()
        print oo.getData()
    else:
        pass
        # print __doc__.strip()

        # oo = ReadOO('C:\Dokumente und Einstellungen\Matthias\Desktop\Abrechnung Amsterdam 2007.odt')
        # print oo.getXML()
        # print oo.getData()
        # print type( oo.getData() )