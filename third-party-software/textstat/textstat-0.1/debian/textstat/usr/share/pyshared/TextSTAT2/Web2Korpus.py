#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Hilfsklassen und -funktionen für Korpus.py


import htmllib, formatter, urllib, urlparse
import StringIO, re


# Klasse zum Entfernen von HTML und Formatieren der Textdatei
class HTMLStripper(htmllib.HTMLParser):
    def __init__(self):
        self.bodytext = StringIO.StringIO()
        writer = formatter.DumbWriter(self.bodytext)
        htmllib.HTMLParser.__init__(self,
            formatter.AbstractFormatter(writer))

    # dies überschreibt die Standardfunktion in htmllib.py,
    # die für jeden Link eine Art Fussnote im Text anlegt: [x]
    def anchor_end(self):
        if self.anchor:
            self.handle_data('')
            self.anchor = None
            
    # dies überschreibt die Standardfunktion in htmllib.py,
    # die für jedes Image den ALT-Text wiedergibt
    def handle_image(self, src, alt, *args):
        self.handle_data('')

    def gettext(self):
        return self.bodytext.getvalue()


#################

class Web2Korpus:
    def __init__(self, master=None, url='', anzahl=1, basis='', codepage='utf-8'):
        self.master = master
        self.url = url
        self.anzahl = anzahl
        self.basis = basis
        self.codepage = codepage
        
        if self.url[:7] not in ['http://', 'https:/']:
            self.url = 'http://' + self.url

        self.host = urlparse.urljoin(self.url, '/')
        self.subdir = urlparse.urljoin(self.url, ' ').strip()
        if basis == 'subdir':
            self.basis = self.subdir
        else:
            self.basis = self.host
        
        # Format: Dictionary mit key = url und value = [seite, [links]]
        self.data = {}
        self.links = []
        
        if self.anzahl == 1:
            self.getWebPage()
        else:
            self.getSomeWebPages()

           
    def checkLink(self, url):
        url_ok = 1
        # URL schon vorhanden
        if url in self.links:
            url_ok = 0
        # File nicht auf dem angegebenen Server
        elif not url.startswith(self.basis):
            url_ok = 0
        # falsches URL-Schema
        elif url[0:4] != 'http':
            url_ok = 0
        # falsches Dateiformat
        elif url.endswith(('.js', '.pdf', '.doc', '.docx', '.gif', '.bmp', '.png', '.jpg', '.jpeg', '.ico', '.wav', '.mp3', '.ogg')):
            url_ok = 0
        # Fragment: Sprungmarke innerhalb des Dokuments (also .../...#...)
        elif urlparse.urlparse(url)[5] != '':
            url_ok = 0
        return url_ok

           
    def getWebPage(self):
        try:
            verbindung = urllib.urlopen(self.url)
            # info = verbindung.info()
            seite = verbindung.read()
            
            # Kodierung aus Seite auslesen
            cod = re.search("text/html; charset=([ A-Za-z0-9-]+)", seite)
            if cod != None:
                cod = cod.group(1).strip()
            # ansonsten nehmen wir die Einstellung aus TextSTAT
            else:
                cod = self.codepage
            
            
            st = HTMLStripper()
            st.feed(seite)
            st.close()
            # Unicode-Text machen
            self.data[self.url] = unicode(st.gettext(), cod, 'replace')
            
            # Links überprüfen und in Liste
            for x in st.anchorlist:
                if x.find('/') == -1:
                    x = urlparse.urljoin(self.url, x)
                if x.find('/') == 0:
                    x = urlparse.urljoin(self.host, x)
                
                url_ok = self.checkLink(x)   
                if url_ok == 1:
                    self.links.append(x)
        except:
            self.data[self.url] = 'Fehler'
        
   
    def getSomeWebPages(self):
        self.getWebPage()
        i = 1
        while len(self.links) > 0:
            if i >= self.anzahl:
                break
            else:
                self.url = self.links[0]
                self.links.pop(0)
                if self.data.has_key(self.url) == 0:
                    self.getWebPage()
                    i = i+1
                    
                    # Fortschritt in Statusbar anzeigen
                    self.master.status.progress.updateProgress(newValue=i, newMax=self.anzahl)
                    
        # Progressbar zurücksetzen                
        self.master.status.progress.updateProgress(0)

        
    def getData(self):
        return self.data


    
if __name__ == '__main__':
    url = 'www.stern.de'
    k = Web2Korpus(url, anzahl=3, basis='server')
    x = k.getData()
    for i in x.keys():
        print i
        print x[i]
