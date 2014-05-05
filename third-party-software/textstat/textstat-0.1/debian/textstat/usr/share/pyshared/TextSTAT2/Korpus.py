#!/usr/bin/python
# -*- coding: utf-8 -*-

import os.path, cPickle, re
import Web2Korpus, News2Korpus, ConvertText
import locale
locale.setlocale(locale.LC_CTYPE, '')     # LC_CTYPE beeinflusst String-Operationen
locale.setlocale(locale.LC_COLLATE, '')   # LC_COLLATE beeinflusst das Sortier-Verhalten


class Korpus:
    def __init__(self, master=None):
        self.master = master
        self.db = None
        self.dbFile = None
        self.wordFreq = None
        self.anzahlTokens = 0
        self.suchKonk = None
        self.konkResultat = None
        self.zitatDatei = None
        self.dbGeaendert = 0

    def newDb(self, ff):
        self.dbFile = ff
        self.db = {}
        f = open(self.dbFile, 'wb')
        cPickle.dump(self.db, f, 2)
        f.close() 

    def openDb(self, ff):
        """DB anlegen bzw. öffnen""" 
        try:
            ff = os.path.normpath(ff)
            f = open(ff, 'rb')
            self.db = cPickle.load(f)
            f.close()
            self.dbFile = ff
            # Testen, ob dies vielleicht ein altes TextSTAT 1.5-Korpus ist
            # versuchen zu importieren
            if isinstance(self.db, list):
                self.db = {}
                self.importOldKorpus(ff)
        except:
            pass
                                 
        
    def addDb(self, ff):
        """Korpus zu DB hinzufügen"""
        ff = os.path.normpath(ff)
        f = open(ff, 'rb')
        x = cPickle.load(f)
        f.close() 
        try:
            for k in x.keys():
                self.db[k] = x[k]
        except:
            # Wenn was schiefgeht ist es ja vielleicht ein altes Korpus
            if isinstance(x, list):
                del x
                self.importOldKorpus(ff)
                
        self.clearVars()
                     

    def saveDb(self):
        """DB schließen""" 
        # Wir speichern binär, Protokoll 2 (neu in Python 2.3)
        f = open(self.dbFile, 'wb')
        cPickle.dump(self.db, f, 2)
        f.close() 
        self.dbGeaendert = 0
        
    def getDbName(self):
        """Gibt Dateinamen des Korpus zurück"""
        return os.path.split(self.dbFile)[1]
        
    def getDbPath(self):
        """Gibt Dateipfad zurück"""
        return self.dbFile

    def getDbKeys(self):
        """Gibt sortierte Liste der Keys zurück (Strings)"""
        k = self.db.keys()
        k.sort()
        return k

    def getDbFileNames(self):
        """Gibt sortierte Liste der Filenamen zurück (Unicode)"""
        l = []
        for k in self.db.keys():
            l.append(self.db[k]['path'])
        l.sort()
        return l
        
    def getDbSize(self):
        """Gibt Summe der Dateigrößen zurück"""
        dbSize = 0
        for k in self.db.keys():
            dbSize = dbSize + len(self.db[k]['file'])
        return dbSize

    def getDbFile(self, k):
        """Lesbarerer Shortcut zum Ansprechen einer einzelnen Datei im Korpus"""
        return self.db[k]

    def countDbFiles(self):
        """Gibt Anzahl der Dateien im Korpus zurück"""
        return len(self.db.keys())       
        

    def addDbFiles(self, files, codepage):
        """Fügt Textfiles zum Korpus hinzu"""
        for f in files:
            # wegen Problemen beim Öffnen, jetzt erst einmal den Datipfad 'normalisieren'
            f = os.path.normpath(f)
            
            # Word-File?? sind wir unter Windows?
            # dann versuchen wir halt mal das zu lesen           
            if f[-4:] == '.doc' or f[-4:] == '.rtf' or f[-5:] == '.docx':
                try:
                    word = ConvertText.ReadWord(f)
                    ff = word.getData()
                except:
                    continue
            
            # OpenOffice-File??         
            elif f[-4:] == '.sxw' or f[-4:] == '.odt':

                oo = ConvertText.ReadOO(f)
                ff = oo.getData()
                ff = unicode(ff, 'utf-8', 'replace')
                    
            else:
                ff = open(f).read()
                
                # Haben wir es mit einem HTML-File zu tun? Dann HTML weg...
                if '.htm' in f:
                    st = Web2Korpus.HTMLStripper()
                    st.feed(ff)
                    st.close()
                    ff = st.gettext()
    
                # So, jetzt machen wir aus dem File ein Unicode-File
                ff = unicode(ff, codepage, 'replace')
                               
            element = {'path':f, 'file':ff}
 
            # Keys dürfen nur Strings sein, kein Unicode
            # ACHTUNG Was passiert eigentlich wenn der Dateiname Zeichen enthält
            # die nicht Latin-1 sind??
            key = f.encode('utf-8', 'replace')
            self.db[key] = element
            
        self.clearVars()


    def addHTMLFiles(self, url, anzahl, codepage, basis):
        d = Web2Korpus.Web2Korpus(master=self.master, url=url, anzahl=anzahl, basis=basis, codepage=codepage)
        data = d.getData()
        for f in data.keys():
            # Umwandlung in Unicode findet jetzt in Web2Korpus statt
            # ff = unicode(data[f], codepage, 'replace')
            ff = data[f]
            element = {'path':f, 'file':ff}
            self.db[f] = element
        self.clearVars()


    def addNewsFiles(self, server='', gruppe='', anzahl=50, zitate=1, codepage='latin-1'):
        d = News2Korpus.News2Korpus(master=self.master, server=server, gruppe=gruppe, anzahl=anzahl, zitate=zitate)
        data = d.getData()
        for f in data.keys():
            ff = unicode(data[f], codepage, 'replace')
            element = {'path':f, 'file':ff}
            self.db[f] = element
        self.clearVars()

        
    def importOldKorpus(self, korpName):
        korpFile = open(korpName)
        dateien = cPickle.load(korpFile)
        korpFile.close()
        for dat in dateien:
            f = dat[0]
            ff = dat[1]
            element = {'path':f, 'file':ff}
            key = f.encode('utf-8', 'replace')
            self.db[key] = element
        self.clearVars()     

        
    def delDbFiles(self, files):
        """Entfernt Dateien aus dem Korpus"""
        for f in files:
            key = f.encode('utf-8', 'replace')
            del self.db[key] 
        self.clearVars()

     
    def clearVars(self):
        # Frequenz- und Konkordanzliste löschen
        self.wordFreq = None
        self.anzahlTokens = 0
        self.konkResultat = None
        self.dbGeaendert = 1


    def dbWordFreq(self):
        """Hilfsfunktion für getWordFreq(): erstellt das Dict mit Wortfrequenzen"""
        self.wordFreq = {}
        self.wordFreqKlein = {}
        # woerter = re.compile(r"[\w-]+", re.U | re.M)
        woerter = re.compile(r"\b([\w'-]+)\b", re.U | re.M)
        dateien = self.getDbKeys()
        z = 0
        self.anzahlTokens = 0
        for dat in dateien:
            allewoerter = woerter.findall(self.db[dat]['file'])
            # Was jetzt kommt ist ein fürchterlicher Workaround; wir brauchen das,  
            # damit Unicode richtig alphabetisch sortiert wird (wie Locale es verlangt)
            # allewoerter = [x + ' ' for x in allewoerter]
            # >>> 08/2008: Workaround ist anscheinend nicht mehr nötig...
            
            self.anzahlTokens = self.anzahlTokens + len(allewoerter)
            for wort in allewoerter:
                self.wordFreq[wort] = self.wordFreq.get(wort, 0) + 1
                # und jetzt das ganze nochmal mit Kleinschreibung
                wort = wort.lower()
                self.wordFreqKlein[wort] = self.wordFreqKlein.get(wort, 0) + 1
                
            # Fortschritt in Statusbar anzeigen
            z = z + 1
            self.master.status.progress.updateProgress(newValue=z, newMax=len(dateien))


    def getWordFreq(self, sort='freq', grossklein=0, minFreq=0, maxFreq=0, suchFreq=''):
        """Gibt Wortfrequenzliste zurück; Basis dafür ist das Dict self.wordFreq """
        if self.wordFreq == None:
            self.dbWordFreq()

        freqListe = [] 

        if suchFreq != '':
        # wenn ein bestimmtes Suchwort angegeben ist.
            for wort in self.wordFreq.keys():
                if wort.lower().find(suchFreq.lower()) != -1:
                    freqListe.append([wort, self.wordFreq[wort]])
              
        else:
        # wenn kein Suchwort da ist (also normalerweise...)
            try:
                minFreq = int(minFreq)
            except:
                minFreq = 0
            try:
                maxFreq = int(maxFreq)
            except:
                maxFreq = 0
                
            if grossklein == 1:
                freqDict = self.wordFreqKlein
            else:
                freqDict = self.wordFreq
                
            if minFreq ==0 and maxFreq==0:
                for k, v in freqDict.items():
                    freqListe.append([k, v])
            else:
                for k, v in freqDict.items():
                    if (maxFreq == 0 and v >= minFreq) or (v >= minFreq and v <= maxFreq):
                        freqListe.append([k, v])

        # Wir sortieren zunächst mal (default) alphabetisch
        def sortDeutsch(a,b):
            return locale.strcoll(a[0], b[0])
        freqListe.sort(sortDeutsch) 
    
        if sort == 'alpha':
            # explizit (noch einmal) alphabetisch
            freqListe.sort(sortDeutsch)
           
        elif sort == 'retro':
            # retrograde
            def sortRetro(a, b):
                a, b = list(a[0]), list(b[0])
                a.reverse()
                b.reverse()
                a = ''.join(a)
                b = ''.join(b)
                return locale.strcoll(a,b)
                # return cmp(a, b)
            freqListe.sort(sortRetro)

        else:
            # Default: absteigend nach Frequenz
            def sortFrequenz(a, b):
                return cmp(b[1], a[1])
            freqListe.sort(sortFrequenz)

        # Progressbar zurücksetzen                
        self.master.status.progress.updateProgress(0)

        return freqListe



    def dbWordConc(self, pattern='', ganzWort=0, grossklein=0, cL=50, cR=50):     
      
        if ganzWort == 1:
            # nur ganze Wörter
            suchmuster = r"\b%s\b" % self.suchKonk
        else:
            suchmuster = self.suchKonk
            
        if grossklein == 1:
            # Großschreibung ignorieren
            muster = re.compile(suchmuster, re.U | re.S | re.I)
        else:
            muster = re.compile(suchmuster, re.U | re.S)
                    
        self.konkResultat = []
        dateien = self.getDbKeys()
        z=0
        for dat in dateien:
            datei = self.db[dat]['file']
            
            pos = 0
            while 1:
                res = muster.search(datei, pos)
                if res == None:
                    break
    
                # ab pos weitersuchen, also pos erhöhen
                pos = res.start() + 1
    
                # Kontext festlegen
                anfang = res.start() - cL
                ende = res.end() + cR

                if anfang < 0:
                    anfang = 0
                if ende > len(datei):
                    ende = len(datei)
    
                # Gefundenen String ins Resultat (besteht aus drei Teilen) und auch den Dateinamen
                konk = [datei[anfang:res.start()], datei[res.start():res.end()], datei[res.end():ende]]
                self.konkResultat.append(['', konk, dat])            
              
            # Fortschritt in Statusbar anzeigen
            z = z + 1
            self.master.status.progress.updateProgress(newValue=z, newMax=len(dateien))
        # Progressbar zurücksetzen                
        self.master.status.progress.updateProgress(0)


    def getWordConc(self, pattern='', ganzWort=0, grossklein=0, cL='50', cR='50', sort='', markKonk=0):
        if pattern != '':
            self.suchKonk = pattern  
            
        try:
            cL = int(cL)
            cR = int(cR)
        except:
            cL = 50
            cR = 50
            
        # Hier könnte man eine Bedingung einbauen, so dass nicht immer alle
        # Files durchsucht werden müssen...
        self.dbWordConc(pattern=pattern, ganzWort=ganzWort, grossklein=grossklein, cL=cL, cR=cR)

        z = 0
        for k in range(len(self.konkResultat)):
            konk = self.konkResultat[k][1]
            if markKonk == 1:
                x = konk[1].upper()
                anzeigeString = konk[0].rjust(cL) + x + konk[2]
            else:
                anzeigeString = konk[0].rjust(cL) + konk[1] + konk[2]
            # Newline rauswerfen
            anzeigeString = anzeigeString.replace('\n', ' ')
            # Format: [Konk für Anzeige, Original Konk (Tuple), Dateiname]
            self.konkResultat[k][0] = anzeigeString
                
            # Fortschritt in Statusbar anzeigen
            z = z + 1
            self.master.status.progress.updateProgress(newValue=z, newMax=len(self.konkResultat))

        # Sortieren  
        def sort_konk(a, b):
            # Wir nehmen das Original zum sortieren (also das Tuple)
            a = a[1]
            b = b[1]
            if sort == 'rechts':
                return locale.strcoll(a[2], b[2])
            elif sort == 'links':
                a, b = a[0].split(), b[0].split()
                if len(a) == 0: a.insert(0, ' ')
                if len(b) == 0: b.insert(0, ' ')
                return locale.strcoll(a[-1], b[-1])
            else:            
                return locale.strcoll(a[1], b[1])
      
        
        self.konkResultat.sort(sort_konk)
        # Progressbar zurücksetzen                
        self.master.status.progress.updateProgress(0)
        
        return self.konkResultat
            

    def getContext(self, i, contextL=100, contextR=100):
        konk = self.konkResultat[i]
        konkordanz = ''.join(konk[1])
        self.zitatDatei = konk[2]
        datei = self.db[self.zitatDatei]['file']
        stelleL = datei.find(konkordanz)
        stelleR = stelleL + len(konkordanz)
        
        anfang = stelleL - contextL
        ende = stelleR + contextR

        # Leerzeichen suchen, damit nur ganze Wörter gezeigt werden
        if anfang < 0: 
            anfang = 0
        while datei[anfang] != ' ':
            anfang = anfang - 1
            if anfang <= 0:
                anfang = 0
                break
        if ende > len(datei): 
            ende = len(datei)-1
        while datei[ende] != ' ':
            ende = ende + 1
            if ende >= len(datei):
                ende = len(datei)
                break

        res = [datei[anfang:stelleL], datei[stelleL:stelleR], datei[stelleR:ende]]
        return res

       