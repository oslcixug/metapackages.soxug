#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# Hilfsklassen und -funktionen für Korpus.py

import nntplib


class News2Korpus:
    def __init__(self, master=None, server='', gruppe='', anzahl=1, zitate='nein'):
        self.master = master
        self.server = server
        self.gruppe = gruppe
        self.anzahl = anzahl
        self.zitate = zitate
        self.data = {}
        
        self.machNewsKorp()

    def machNewsKorp(self):
        ng = nntplib.NNTP(self.server)
        resp, count, first, last, name = ng.group(self.gruppe)
        # resp, koepfe = ng.xover(first, last)
        resp, koepfe = ng.xhdr('subject', first + '-' + last)
        gelesen = 0


        for kopf in koepfe:
            if gelesen < self.anzahl:
                nummer = kopf[0]
                betreff = kopf[1]
                # absender = kopf[2]
                # datum = kopf[3]
                # beitrag = kopf[4]
           
                artikel = ng.body(nummer)
                beitrag = artikel[2]
                art = artikel[3]
    
                if self.zitate == 1:
                    art = [x for x in art if x == '' or x[0] != '>']
                    
                text = '\n'.join(art)
                # text = '%s\n%s\n%s\n\n%s' % (betreff, absender, datum, text)
                   
                name = "news:%s" % beitrag
                self.data[name] = text
                gelesen = gelesen + 1
                
                # Fortschritt in Statusbar anzeigen
                self.master.status.progress.updateProgress(newValue=gelesen, newMax=self.anzahl)
    
            else:
                break
    
        ng.quit()
        # Progressbar zurücksetzen                
        self.master.status.progress.updateProgress(0)

    def getData(self):
        return self.data


if __name__ == '__main__':
    server = 'news.dzug.org'
    gruppe = 'de.comp.dzug.cmf'
    k = News2Korpus(server=server, gruppe=gruppe, anzahl=25, zitate='nein')
    x = k.getData()
    for i in x.keys():
        print i
        print x[i]