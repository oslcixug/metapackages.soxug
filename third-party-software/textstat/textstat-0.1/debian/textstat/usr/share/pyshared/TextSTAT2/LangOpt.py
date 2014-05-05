#!/usr/bin/python
# -*- coding: utf-8 -*-
#
# TextSTAT - Simples Text Analyse Tool
# 
# Copyright (c) 2001/2008, Matthias Hüning <mhuening@zedat.fu-berlin.de>
# All rights reserved.
#
# Sprachoptionen für TextSTAT
#

##############################
# ein paar globale Variablen
Version = "2.8e"
VersionDatum = "20/09/2009"
Copyright = "(c) Matthias Hüning 2000/2009"
eMail = "<matthias.huening@fu-berlin.de>"
ProgURL = "http://neon.niederlandistik.fu-berlin.de/textstat/"


##############################
########### Deutsch ##########
##############################

deutsch  = {
"Titel"         : "TextSTAT",
"Titel_lang"    : "TextSTAT (Simples Text Analyse Tool)",
"Version"       : Version,
"VersionDatum"  : "Version vom %s" % VersionDatum,
"Copyright"     : Copyright,
"eMail"         : eMail,
"ProgURL"       : ProgURL,
"ProgInfo"      : "TextSTAT ist freie Software.\nDer Autor übernimmt keinerlei Haftung.",

###### Menu #####
"Korpus"        : "Korpus",
"KorpusNew"     : "Neues Korpus",
"KorpusOpen"    : "Korpus öffnen",
"KorpusHinzu"   : "Korpus hinzufügen",
"KorpusSave"    : "Korpus speichern",
"KorpusSaveAs"  : "Speichern unter",
"KorpusDel"     : "Korpus löschen",
"LetzteKorpora" : "Letzte Korpora",

"DateiHinzu"    : "Datei von Festplatte hinzufügen",
"WebHinzu"      : "Web-Datei hinzufügen",
"NewsHinzu"     : "Newsgroup-Beiträge hinzufügen",
"DateiWeg"      : "Datei(en) entfernen",

"OptSave"       : "Optionen speichern",
"Exit"          : "Beenden",

"ImpEx"         : "Exportieren",
"Ex2csv"        : "Frequenzliste > CSV-Datei",
"Ex2txt"        : "Konkordanzen > TXT-Datei",
"Ex2excel"      : "Frequenzliste > MS Excel",
"Ex2word"       : "Konkordanzen > MS Word",
"ImpKorpus"     : "Korpus importieren (TextSTAT 1.x)",

"Sprache"       : "Sprache",
"CodePage"      : "Datei-Kodierung",
"Hilfe"         : " ? ",
"RegExpHelp"    : "Regular Expressions - Hilfe",
"History"       : "Änderungsprotokoll",
"License"       : "Lizenz",
"Willkommen"    : " Willkommen bei TextSTAT",


##### Toolbar #####
"KorpusNew2"    : "Neues Korpus anlegen",
"KorpusOpen2"   : "Ein bestehendes Korpus öffnen",
"KorpusHinzu2"  : "Bestehendes Korpus zu momentan geöffnetem Korpus hinzufügen",
"KorpusSave2"   : "Das momentane Korpus speichern",
"KorpusDel2"    : "Das Korpus wird definitiv gelöscht",
"WebHinzu2"     : "HTML-Datei(en) aus Internet zum Korpus hinzufügen",
"DateiHinzu2"   : "Datei(en) von Festplatte zum Korpus hinzufügen",
"DateiWeg2"     : "Datei(en) aus Korpus löschen",
"FreqZeigen"    : "Wortfrequenzen anzeigen",
"FreqZeigen2"   : "Frequenz der einzelnen Wortformen im Korpus anzeigen",
"Kopieren"      : "Kopieren (in Zwischenablage)",
"Kopieren2"     : "Auswahl in Zwischenablage kopieren",
"Ex2excel2"     : "Frequenzliste nach MS Excel exportieren",
"Ex2word2"      : "Konkordanzen nach MS Word exportieren",
"Info"          : "Info zu TextSTAT",
"Info2"         : "Informationen zum Programm",

##### Tabs #####
"KorpusTab"     : "Korpus", 
"FormenTab"     : "Wortformen",
"ConcTab"       : "Konkordanz",
"ZitatTab"      : "Zitat",

"KTinfo"        : "TextSTAT \n\nBitte füllen Sie Ihr Korpus mit einigen Files bzw. öffnen Sie ein bestehendes TextSTAT-Korpus.",
"KTkopieren"    : "Dateinamen kopieren",
"KTopen"        : "Datei öffnen",
"KTdatinfo"     : "Datei-Info",
"KTdatweg"      : "Datei(en) entfernen",

"FTinfo"        : "FREQUENZ \nder Wortformen (Token)",
"FTsortFreq"    : "nach Frequenz sortieren",
"FTsortAlpha"   : "alphabetisch sortieren",
"FTsortRetro"   : "retrograde sortieren",
"FTminFreq"     : "min. Frequenz",
"FTmaxFreq"     : "max. Frequenz",
"FTignGross"    : "Großschreibung ignorieren (A=a)",
"FTsuchFreq"    : "ODER such Frequenz für \nWörter mit dem String:",
"FTfreqliste"   : "Frequenzliste",
"FTwortform"    : "Wortform",
"FTfrequenz"    : "Frequenz",
"FTkopieren"    : "Kopieren",
"FTsuchKonk"    : "Konkordanzen suchen",
"FTzeigInfo"    : "mehr Informationen",
"FTformInfo"    : "Korpus-Informationen\n\nKorpusgröße: %s Wörter (Tokens)\n\n%s verschiedene Formen (Types, Großschreibung berücksichtigt) bzw. \n%s Types (Großschreibung ignoriert)\n\nDie Form '%s' kommt %s Mal vor (= %.2f %%)",
"FTOptionen"    : "Frequenz / Optionen",

"CTsuchen"      : "Suchen",
"CTganzWort"    : "nur ganze Wörter",
"CTignGross"    : "Großschreibung ignorieren (A=a)",
"CTcontextL"    : "Kontext links",
"CTcontextR"    : "Kontext rechts",
"CTmarkSuch"    : "Suchwort markieren",
"CTsortAlpha"   : "alphabetisch",
"CTsortR"       : "sortiert Kontext rechts",
"CTsortL"       : "sortiert Kontext links",
"CTaktualisier" : "Aktualisieren",
"CTmachQuery"   : "Suchanfragen-Editor",
"CTkopieren"    : "Kopieren",
"CTzeigZitat"   : "Mehr Kontext zeigen",
"CTOptionen"    : "Suchoptionen",

"ZTinfo"        : "Text der Belegstelle",
"ZTkopieren"    : "Kopieren",
"ZToeffnen"     : "Datei öffnen",
"ZToeffnen2"    : "\n(Doppelklick um Datei zu öffnen)\n",
"ZTInit"        : "\n Doppelklick auf eine Zeile im Konkordanz-Tab zeigt hier dieselbe Zeile mit mehr Kontext.",

##### Webdialog #####
"WDinfo"        : "Fügen sie HTML-Seiten direkt aus dem Internet hinzu.\nBitte geben Sie eine URL ein:",
"WDfrageSeiten" : "Wieviele Seiten möchten Sie in Ihr Korpus laden?",
"WDanzahl"      : "Anzahl der Seiten, die heruntergeladen werden sollen",
"WDinSubdir"    : "im angegebenen Subdirectory suchen",
"WDaufServer"   : "auf dem ganzen Server suchen",
"WDok"          : "Suchen",
"WDcancel"      : "Schließen",

##### NewsDialog #####
"NDinfo"        : "Newsgroup-Beiträge hinzufügen", 
"NDserver"      : "News-Server",  
"NDgruppe"      : "Newsgruppe", 
"NDanzahl"      : "Anzahl der Beiträge", 
"NDzitWeg"      : "Zitate entfernen", 
"NDok"          : "Suchen",
"NDcancel"      : "Schließen",

##### QueryDialog #####
"QDinfo"        : "Hilfe bei der Erstellung komplexer Suchanfragen \nSie können hier auch Wildcards verwenden (* und ?)",
"QDsuchFelder"  : "Suchbegriffe",
"QAusdruck1"    : "erster Suchbegriff",
"QAusdruck2"    : "zweiter Suchbegriff",
"QAbstand"      : "Anzahl der Wörter zwischen den Suchbegriffen",
"QAbstandMin"   : "mindestens und   ",
"QAbstandMax"   : "höchstens",
"QmachAusdruck" : "Suchanfrage erstellen",
"QDcancel"      : "Schließen",


##### Fehler #####
"Error"         : "Fehler",
"ErrKorpNew"    : "Korpusdatei konnte nicht angelegt werden!",
"ErrKorpOpen"   : "Korpus konnte nicht geöffnet werden!\nSind Sie sich sicher, dass es sich um eine TextSTAT-Korpusdatei handelt?",
"ErrKorpImp"    : "Korpus konnte nicht importiert werden!",
"ErrNoKorp"     : "Sie müssen zunächst ein neues Korpus anlegen bzw. ein bestehendes öffnen.",
"ErrKorpSave"   : "Korpusdatei konnte nicht gespeichert werden!",
"ErrFileOpen"   : "Fehler beim Öffnen der Datei!",
"ErrSelect"     : "Sie haben nichts ausgewählt!",
"ErrOptSave"    : "Optionen konnten nicht gespeichert werden",
"ErrExport"     : "Export-Fehler",
"ErrNoFreqList" : "Keine Wortfrequenzliste vorhanden",
"ErrNoKonkList" : "Keine Konkordanzliste vorhanden",
"ErrExcel"      : "MS Excel Fehler!\nSowohl die Python win32-Extensions als auch MS Excel müssen ordnungsgemäß installiert sein.",
"ErrExcel65K"   : "Fehler! MS Excel kann nur 65500 Zeilen verarbeiten. \nEs sind aber %s Zeilen vorhanden.",
"ErrWord"       : "MS Word Fehler!\nSowohl die Python win32-Extensions als auch MS Word müssen ordnungsgemäß installiert sein.",
"ErrExpFreq"    : "Beim Speichern der Wortfrequenzliste ist etwas schief gegangen.",
"ErrExpKonk"    : "Beim Speichern der Konkordanzliste ist etwas schief gegangen.",
"ErrWeb"        : "Beim Laden der HTML-Files aus dem Internet ist etwas schief gegangen.",
"ErrNews"       : "Beim Laden der Newsgroup-Berichte ist etwas schief gegangen.",
"ErrQuery"      : "Sie haben keinen Suchbegriff eingegeben.",


##### Mitteilungen und Fragen #####
"AskSure"       : "Sind Sie Sicher?",
"AskDelete"     : "Wollen Sie wirklich %s loeschen?",
"StatusWeb"     : "Es werden %s Files aus dem Internet hinzugeladen...",
"StatusNews"    : "Es werden %s Newsgroup-Beiträge geladen und hinzugefügt...",
"DateiInfo"     : "Datei-Informationen",
"DateiInfo2"    : "Datei: %s\n\nPfad: %s\n\nGröße: %s\n",
"InfoText"      : "TextSTAT\
\nNachdem Sie ein Korpus angelegt bzw. geöffnet haben, können Sie weitere Dateien hinzufügen \
oder auch wieder einige löschen. Markieren einer Datei und dann die rechte Maustaste drücken \
für weitere Optionen. \nSie können übrigens das Korpus auch als Ganzes speichern.\
\nZur Textanalyse haben Sie mehrere Möglichkeiten: \nUnter 'Wortformen' finden sie \
die Frequenzangaben zu einzelnen Wortformen. 'Konkordanz' zeigt KWIC-Konkordanzen \
(KeyWord in Context), 'Zitat' einen größeren Textzusammenhang.",

"StatusKorpImp" : "Korpus wurde erfolgreich importiert.",
"StatusKorpSave": "Korpus wurde gespeichert.",
"StatusFiles"   : " %s Dateien | %s Bytes ",
"StatusKorpDat" : " Korpus: %s  (Einstellung Dateikodierung: %s)",
"KodierungInfo" :  "Sie können jetzt Dateien hinzufügen.\n\nTextverarbeitungsdateien:\n- MS Word-Dateien ('.doc' bzw. '.docx')\n- OpenOffice-Dateien ('.odt bzw. .sxw')\n\nTextdateien:\n- ASCII/ANSI-Textdateien (meist '.txt')\n- HTML-Files (von der Festplatte oder direkt aus dem Internet)\n- Newsgroup-Beiträge (direkt aus dem Internet)\nBitte achten Sie bei den reinen Textdateien auf die richtige\nEinstellung der Dateikodierung.\nDie momentan eingestellte Dateikodierung ist: %s",
"StatusFormen"  : " %s Wortformen/Types (%s Wörter/Tokens im Korpus)",
"StatusNoKorp"  : " kein Korpus ",
"StatusSuchKonk": "Suche Konkordanzen...",
"StatusTreffer" : " %s Treffer ",
"StatusExpFreq" : "Frequenzliste wurde exportiert.",
"StatusExpKonk" : "Konkordanzliste wurde exportiert.",
"ExportHinweis" : "Am besten wählen Sie zur Darstellung eine nicht-proportionale Schriftart (wie Courier New)",
"StatusClip"    : "Auswahl wurde in Zwischenablage kopiert", 
"SpracheAendern": "Sprache ändern",
"SpracheInfo"   : "Sie müssen das Programm neu starten, um den Sprachwechsel zu aktivieren.\n\nYou have to restart TextSTAT to activate the language change.",
"ProgBeenden"   : "TextSTAT beenden",
"AbfrageEnde"   : "Das aktuelle Korpus wurde geändert.\nMöchten Sie es speichern, bevor TextSTAT beendet wird?",
"AbfrageSave"   : "Das aktuelle Korpus wurde geändert.\nMöchten Sie es speichern, bevor Sie ein neues Korpus laden?",
}


###################################
########### Nederlands ############
###################################

nederlands  = {
"Titel"         : "TextSTAT",
"Titel_lang"    : "TextSTAT (Simpel Text Analyse Tool)",
"Version"       : Version,
"VersionDatum"  : "Versie van %s" % VersionDatum,
"Copyright"     : Copyright,
"eMail"         : eMail,
"ProgURL"       : ProgURL,
"ProgInfo"      : "TextSTAT is vrije software.\nDe auteur is niet aansprakelijk voor eventuele gebreken en fouten.",

###### Menu #####
"Korpus"        : "Corpus",
"KorpusNew"     : "Nieuw corpus",
"KorpusOpen"    : "Corpus openen",
"KorpusHinzu"   : "Corpus toevoegen",
"KorpusSave"    : "Corpus opslaan",
"KorpusSaveAs"  : "Bewaren als",
"KorpusDel"     : "Corpus wissen",
"LetzteKorpora" : "Laatste corpora",

"DateiHinzu"    : "Bestand van harde schijf toevoegen",
"WebHinzu"      : "Web-bestand toevoegen",
"NewsHinzu"     : "Newsgroup-bijdragen toevoegen",
"DateiWeg"      : "Bestand(en) verwijderen",

"OptSave"       : "Voorkeuren opslaan",
"Exit"          : "Exit",

"ImpEx"         : "Export",
"Ex2csv"        : "Frequentielijst > CSV-file",
"Ex2txt"        : "Concordanties > TXT-file",
"Ex2excel"      : "Frequentielijst > MS Excel",
"Ex2word"       : "Concordanties > MS Word",
"ImpKorpus"     : "Corpus importeren (TextSTAT 1.x)",

"Sprache"       : "Taal",
"CodePage"      : "Codering",
"Hilfe"         : " ? ",
"RegExpHelp"    : "Regular Expressions - Hulp",
"History"       : "Geschiedenis (Duits)",
"License"       : "Licentie (Engels)",
"Willkommen"    : " Welkom bij TextSTAT",


##### Toolbar #####
"KorpusNew2"    : "Nieuw corpus maken",
"KorpusOpen2"   : "Bestaand corpus openen",
"KorpusHinzu2"  : "Bestaand corpus toevoegen aan corpus",
"KorpusSave2"   : "Actuele corpus opslaan",
"KorpusDel2"    : "Corpus bestand wordt gewist",
"WebHinzu2"     : "HTML-bestanden uit Internet toevoegen aan corpus",
"DateiHinzu2"   : "Bestand van harde schijf toevoegen aan corpus",
"DateiWeg2"     : "Bestand(en) uit corpus verwijderen",
"FreqZeigen"    : "Woordfrequenties tonen",
"FreqZeigen2"   : "Frequentie van woordvormen in het corpus berekenen",
"Kopieren"      : "Naar klembord kopiëren",
"Kopieren2"     : "Selectie naar klembord kopiëren",
"Ex2excel2"     : "Frequentielijst exporteren naar MS Excel",
"Ex2word2"      : "Concordanties exporteren naar MS Word",
"Info"          : "Informatie over TextSTAT",
"Info2"         : "Informatie over het programma",

##### Tabs #####
"KorpusTab"     : "Corpus", 
"FormenTab"     : "Woordvormen",
"ConcTab"       : "Concordantie",
"ZitatTab"      : "Citaat",

"KTinfo"        : "TextSTAT \n\nEerst een paar bestanden toevoegen aan een nieuw corpus resp. een bestaand corpus openen.",
"KTkopieren"    : "Bestandsnaam kopiëren",
"KTopen"        : "Bestand openen",
"KTdatinfo"     : "Informatie over bestand",
"KTdatweg"      : "Bestand(en) uit corpus verwijderen",

"FTinfo"        : "FREQUENTIE \nvan woordvormen (tokens)",
"FTsortFreq"    : "sorteren op frequentie",
"FTsortAlpha"   : "sorteren op alfabet",
"FTsortRetro"   : "retrograde sorteren",
"FTminFreq"     : "min. frequentie",
"FTmaxFreq"     : "max. frequentie",
"FTignGross"    : "hoofdletters negeren (A=a)",
"FTsuchFreq"    : "OF zoek frequentie voor\nwoorden met de string:",
"FTfreqliste"   : "Frequentielijst",
"FTwortform"    : "Woordvorm",
"FTfrequenz"    : "Frequentie",
"FTkopieren"    : "Kopiëren",
"FTsuchKonk"    : "Concordanties zoeken",
"FTzeigInfo"    : "meer informatie",
"FTformInfo"    : "Corpus informatie\n\nHet corpus bevat in totaal %s woorden (tokens)\n\n%s verschillende vormen (types, hoofdletters en kleine letter worden onderscheiden) resp. \n%s types (hoofdletters worden genegeerd)\n\nDe vorm '%s' komt %s keer voor (= %.2f %%)",
"FTOptionen"    : "Frequentie / opties",

"CTsuchen"      : "Zoeken",
"CTganzWort"    : "alleen hele woorden",
"CTignGross"    : "hoofdletters negeren",
"CTcontextL"    : "Context links",
"CTcontextR"    : "Context rechts",
"CTmarkSuch"    : "Zoekbegrip markeren",
"CTsortAlpha"   : "alfabetisch",
"CTsortR"       : "sorteren op context rechts",
"CTsortL"       : "sorteren op context links",
"CTaktualisier" : "Aktualiseren",
"CTmachQuery"   : "Query-editor",
"CTkopieren"    : "Kopiëren",
"CTzeigZitat"   : "Meer context tonen",
"CTOptionen"    : "Opties",

"ZTinfo"        : "Tekst van de bewijsplaats",
"ZTkopieren"    : "Kopiëren",
"ZToeffnen"     : "Bestand openen",
"ZToeffnen2"    : "\n(Dubbele klik om bestand te openen)\n",
"ZTInit"        : "\n Dubbele klik op een regel in de concordantie-tab toont hier diezelfde regel met meer context.",

##### WebDialog #####
"WDinfo"        : "Voeg HTML-bestanden toe direct van Internet.\nVoer hier een URL in a.u.b.:",
"WDfrageSeiten" : "Hoeveel pagina's wilt u toevoegen aan uw corpus?",
"WDanzahl"      : "aantal pagina's om te downloaden",
"WDinSubdir"    : "zoekdomein: deze map",
"WDaufServer"   : "zoekdomein: hele server ",
"WDok"          : "Zoeken",
"WDcancel"      : "Sluiten",

##### NewsDialog #####
"NDinfo"        : "Newsgroup-bijdragen toevoegen aan corpus", 
"NDserver"      : "News server",  
"NDgruppe"      : "Newsgroup", 
"NDanzahl"      : "aantal bijdragen", 
"NDzitWeg"      : "citaten uit bijdragen verwijderen", 
"NDok"          : "Zoeken",
"NDcancel"      : "Sluiten",

##### QueryDialog #####
"QDinfo"        : "Hulp bij het formuleren van complexe zoekopdrachten \nU kunt hier ook wildcards gebruiken (* en ?)",
"QDsuchFelder"  : "Zoekbegrippen",
"QAusdruck1"    : "eerste zoekbegrip",
"QAusdruck2"    : "tweede zoekbegrip",
"QAbstand"      : "Aantal woorden tussen beide zoekbegrippen",
"QAbstandMin"   : "minimaal en   ",
"QAbstandMax"   : "maximaal",
"QmachAusdruck" : "Zoekopdracht uitvoeren",
"QDcancel"      : "Sluiten",

##### Fehler #####
"Error"         : "Fout",
"ErrKorpNew"    : "Corpusbestand kon niet worden aangemaakt!",
"ErrKorpOpen"   : "Corpus kon niet worden geopend!\nIs dit wel een TextSTAT-corpus-bestand?",
"ErrKorpImp"    : "Corpus kon niet worden geïmporteerd!",
"ErrNoKorp"     : "U moet eerst een nieuw corpus aanmaken resp. een bestaand corpus openen.",
"ErrKorpSave"   : "Korpusbestand kon niet worden opgeslagen!",
"ErrFileOpen"   : "Fout bij het openen van het bestand!",
"ErrSelect"     : "U heeft niets geselecteerd!",
"ErrOptSave"    : "Opties konden niet worden opgeslagen!",
"ErrExport"     : "Export fout",
"ErrNoFreqList" : "Er is geen lijst met woordfrequenties",
"ErrNoKonkList" : "Er is geen concordantielijst",
"ErrExcel"      : "MS Excel fout!\n(Zijn de Python win32-extensies en MS Excel geïnstalleerd?)",
"ErrExcel65K"   : "Fout! MS Excel kan maar 65500 rijen verwerken. \nEr zijn echter %s vormen.",
"ErrWord"       : "MS Word fout!\n(Zijn de Python win32-extensies en MS Word geïnstalleerd?)",
"ErrExpFreq"    : "Bij het opslaan van de frequentielijst is iets misgegaan.",
"ErrExpKonk"    : "Bij het opslaan van de concordantielijst is iets misgegaan.",
"ErrWeb"        : "Bij het ophalen van HTML-files is iets misgegaan.",
"ErrNews"       : "Bij het ophalen van newsgroup-berichten is iets misgegaan.",
"ErrQuery"      : "Er is geen zoekterm ingevoerd.",

##### Mitteilungen und Fragen #####
"AskSure"       : "Weet u het zeker?",
"AskDelete"     : "Wilt u werkelijk %s verwijderen?",
"StatusWeb"     : "Er worden %s files van Internet gehaald en toegevoegd aan corpus...",
"StatusNews"    : "Er worden %s newsgroup-bijdragen gedownload en toegevoegd...",
"DateiInfo"     : "Informatie over bestand",
"DateiInfo2"    : "Bestand: %s\n\nPad: %s\n\nGrootte: %s\n",
"InfoText"      : "TextSTAT\
\nNadat u een corpus aangemaakt c.q. geopend hebt, kunt u bestanden toevoegen \
of ook weer verwijderen. Markeer een bestand en druk de rechter muistoets \
om meer opties te krijgen. U kunt ook het corpus als geheel opslaan. \
\nVoor de tekstanalyse zijn er meerdere mogelijkheden: \
\nOnder 'Woordvormen' kunt u de frequentie van de afzonderlijke vormen (tokens) laten \
berekenen. 'Concordantie' laat KWIC-concordanties zien (KeyWord in Context) en onder 'Citaat' \
vindt u meer context voor een bepaalde bewijsplaats.",

"StatusKorpImp" : "Corpus is met succes geïmporteerd.",
"StatusKorpSave": "Corpus is opgeslagen.",
"StatusFiles"   : " %s bestanden | %s bytes ",
"StatusKorpDat" : " Corpus: %s  (momentele codepage/codering: %s)",
"KodierungInfo" :  "U kunt nu bestanden toevoegen.\n\nTekstverwerkingsbestanden:\n- MS Word-bestanden ('.doc' bzw. '.docx'; alleen onder Windows)\n- OpenOffice-bestanden ('.odt of .sxw')\n\nTekstbestanden:\n- ASCII/ANSI-tekstbestanden (meestal '.txt')\n- HTML-bestanden (van uw harde schijf of direct van het Internet)\n- Newsgroup-bestanden (direct van het Internet)\nLet op: bij de platte-tekstbestanden moet u letten op de juiste codering.\nOp dit moment is de volgende codering geselecteerd: %s",
"StatusFormen"  : " %s woordvormen/types (%s woorden/tokens in corpus)",
"StatusNoKorp"  : " geen corpus ",
"StatusSuchKonk": "Zoek concordanties...",
"StatusTreffer" : " %s treffers ",
"StatusExpFreq" : "Frequentielijst is geëxporteerd.",
"StatusExpKonk" : "Concordantielijst is geëxporteerd.",
"ExportHinweis" : "Voor adequate presentatie kies een niet-proportioneel lettertype (bijv. Courier New)",
"StatusClip"    : "Selectie is naar het klembord gekopiëerd", 
"SpracheAendern": "Taal wijzigen",
"SpracheInfo"   : "U moet het programma opnieuw starten om de taalwijziging te activeren.",
"ProgBeenden"   : "TextSTAT afsluiten",
"AbfrageEnde"   : "Het actuele corpus is gewijzigd.\nWilt u het actuele corpus opslaan \nvoordat TextSTAT wordt beëindigd?",
"AbfrageSave"   : "Het actuele corpus is gewijzigd.\nWilt u het actuele corpus opslaan \nvoordat een nieuw corpus wordt geopend?",
}



##############################
########### English ##########
##############################

english  = {
"Titel"         : "TextSTAT",
"Titel_lang"    : "TextSTAT (Simple Text Analysis Tool)",
"Version"       : Version,
"VersionDatum"  : "Version date %s" % VersionDatum,
"Copyright"     : Copyright,
"eMail"         : eMail,
"ProgURL"       : ProgURL,
"ProgInfo"      : "TextSTAT is free software.\nUse at your own risk.\nThe author accepts no responsibility.",

###### Menu #####
"Korpus"        : "Corpus",
"KorpusNew"     : "New corpus",
"KorpusOpen"    : "Open corpus",
"KorpusHinzu"   : "Add corpus",
"KorpusSave"    : "Save corpus",
"KorpusSaveAs"  : "Save as",
"KorpusDel"     : "Delete corpus",
"LetzteKorpora" : "Recent corpora",

"DateiHinzu"    : "Add local file",
"WebHinzu"      : "Add file from web",
"NewsHinzu"     : "Add newsgroup postings",
"DateiWeg"      : "Remove file(s)",

"OptSave"       : "Save options",
"Exit"          : "Exit",

"ImpEx"         : "Export",
"Ex2csv"        : "Frequency list > CSV file",
"Ex2txt"        : "Concordance list > TXT file",
"Ex2excel"      : "Frequency list > MS Excel",
"Ex2word"       : "Concordance list > MS Word",
"ImpKorpus"     : "Import corpus (TextSTAT 1.x)",

"Sprache"       : "Language",
"CodePage"      : "Encoding",
"Hilfe"         : " ? ",
"RegExpHelp"    : "Regular Expressions - Help",
"History"       : "History (German)",
"License"       : "License",
"Willkommen"    : " Welcome to TextSTAT",


##### Toolbar #####
"KorpusNew2"    : "Create new corpus",
"KorpusOpen2"   : "Open existing corpus",
"KorpusHinzu2"  : "Add existing corpus to actual corpus",
"KorpusSave2"   : "Save corpus",
"KorpusDel2"    : "Delete the corpus file",
"WebHinzu2"     : "Add HTML file(s) from Internet to corpus",
"DateiHinzu2"   : "Add local file(s) to corpus",
"DateiWeg2"     : "Delete file(s) from corpus",
"FreqZeigen"    : "Show word frequencies",
"FreqZeigen2"   : "Show frequencies of word forms in corpus",
"Kopieren"      : "Copy to clipboard",
"Kopieren2"     : "Copy selection to clipboard",
"Ex2excel2"     : "Export frequency list to MS Excel",
"Ex2word2"      : "Export concordances to MS Word",
"Info"          : "Information",
"Info2"         : "Information about TextSTAT",

##### Tabs #####
"KorpusTab"     : "Corpus", 
"FormenTab"     : "Word forms",
"ConcTab"       : "Concordance",
"ZitatTab"      : "Citation",

"KTinfo"        : "TextSTAT \n\nPlease create a corpus and add some files (or open existing corpus).",
"KTkopieren"    : "Copy file name",
"KTopen"        : "Open file",
"KTdatinfo"     : "File information",
"KTdatweg"      : "Remove file(s)",

"FTinfo"        : "FREQUENCY \nof word forms (tokens)",
"FTsortFreq"    : "sort on frequency",
"FTsortAlpha"   : "sort alphabetically",
"FTsortRetro"   : "retrograde",
"FTminFreq"     : "min. frequency",
"FTmaxFreq"     : "max. frequency",
"FTignGross"    : "sort case insensitive",
"FTsuchFreq"    : "OR look up frequency for\nwords containing the string:",
"FTfreqliste"   : "Frequency list",
"FTwortform"    : "Word form",
"FTfrequenz"    : "Frequency",
"FTkopieren"    : "Copy",
"FTsuchKonk"    : "Look up concordances",
"FTzeigInfo"    : "More information",
"FTformInfo"    : "Corpus information\n\nThe corpus contains %s words (tokens)\n\n%s different forms (types, capital letters are distinctive) \n%s types (capital letters are ignored)\n\nThe form '%s' appears %s times (= %.2f %%)",
"FTOptionen"    : "Frequency / options",

"CTsuchen"      : "Search",
"CTganzWort"    : "search whole words only",
"CTignGross"    : "search case insensitive",
"CTcontextL"    : "context left",
"CTcontextR"    : "context right",
"CTmarkSuch"    : "mark search string",
"CTsortAlpha"   : "alphabetically",
"CTsortR"       : "sort context right",
"CTsortL"       : "sort context left",
"CTaktualisier" : "Refresh",
"CTmachQuery"   : "Query editor",
"CTkopieren"    : "Copy",
"CTzeigZitat"   : "Show more context",
"CTOptionen"    : "Options",

"ZTinfo"        : "Text of citation",
"ZTkopieren"    : "Copy",
"ZToeffnen"     : "Open file",
"ZToeffnen2"    : "\n(Double click to open file)\n",
"ZTInit"        : "\n Double click one of the hits in the concordance tab to show the same hit here with more context.",

##### Webdialog #####
"WDinfo"        : "Add HTML files directly from the internet.\nPlease enter URL:",
"WDfrageSeiten" : "How many pages do you want to add to your corpus?",
"WDanzahl"      : "number of pages to retrieve",
"WDinSubdir"    : "domain: only subdirectory",
"WDaufServer"   : "domain: whole server",
"WDok"          : "Search",
"WDcancel"      : "Close",

##### NewsDialog #####
"NDinfo"        : "Add newsgroup postings to corpus", 
"NDserver"      : "news server",  
"NDgruppe"      : "newsgroup", 
"NDanzahl"      : "number of postings", 
"NDzitWeg"      : "remove quotes from postings", 
"NDok"          : "Search",
"NDcancel"      : "Close",

##### QueryDialog #####
"QDinfo"        : "Help function for formulating complex queries \nWildcards are allowed here (* and ?)",
"QDsuchFelder"  : "Search term",
"QAusdruck1"    : "first search term",
"QAusdruck2"    : "second search term",
"QAbstand"      : "Number of words between the two search terms",
"QAbstandMin"   : "min. and   ",
"QAbstandMax"   : "max.",
"QmachAusdruck" : "Start search",
"QDcancel"      : "Close",

##### Fehler #####
"Error"         : "Error",
"ErrKorpNew"    : "Corpus file could not be created!",
"ErrKorpOpen"   : "Corpus file could not be opened!\nAre you sure this is a TextSTAT corpus file?",
"ErrKorpImp"    : "Corpus file could not be imported!!",
"ErrNoKorp"     : "You have to create a corpus first (or to open an existing corpus).",
"ErrKorpSave"   : "Corpus file could not be saved!",
"ErrFileOpen"   : "Error on opening file!",
"ErrSelect"     : "Nothing selected!",
"ErrOptSave"    : "Options could not be saved!",
"ErrExport"     : "Export error",
"ErrNoFreqList" : "No word frequency list available",
"ErrNoKonkList" : "No word concordance list available",
"ErrExcel"      : "MS Excel error!\n(Are the Python win32 extentions an MS Excel installed correctly?)",
"ErrExcel65K"   : "Error! MS Excel cannot handle more than 65500 rows. \nWe have %s forms.",
"ErrWord"       : "MS Word error!\n(Are the Python win32 extentions an MS Word installed correctly?)",
"ErrExpFreq"    : "Error on saving the exported word frequency list.",
"ErrExpKonk"    : "Error on saving the exported concordance list.",
"ErrWeb"        : "Error while fetching HTML-files from the internet.",
"ErrNews"       : "Error while fetching newsgroup postings.",
"ErrQuery"      : "No search term defined.",


##### Mitteilungen und Fragen #####
"AskSure"       : "Are you sure?",
"AskDelete"     : "Do you really want to delete %s ?",
"StatusWeb"     : "%s files are being retrieved from the internet...",
"StatusNews"    : "%s newsgroup postings are being retrieved...",
"DateiInfo"     : "File information",
"DateiInfo2"    : "File: %s\n\nPath: %s\n\nSize: %s\n",
"InfoText"      : "TextSTAT\
\nAfter creating (or opening) a corpus, you could add some files to it. \
You could also remove some files of course. The right mouse button will give you a context menu. \
The corpus can be saved as a whole. \
\nThere are some facilities for text analysis: under 'Word forms' you will find \
frequency numbers for the tokens in your texts. 'Concordance' will show you KWIC concordances \
(KeyWord in Context), 'Citation' presents more context.",

"StatusKorpImp" : "Corpus was imported successfully.",
"StatusKorpSave": "Corpus has been saved.",
"StatusFiles"   : " %s files | %s bytes ",
"StatusKorpDat" : " Corpus: %s  (actual CodePage setting: %s)",
"KodierungInfo" :  "You may now add some files to the corpus.\n\nWord processor files:\n- MS Word files ('.doc' or '.docx'; Windows only)\n- OpenOffice-files ('.odt or .sxw')\n\nText files:\n- ASCII/ANSI text files (often '.txt')\n- HTML files (from your hard disc or directly from Internet)\n- Newgroup postings (directly from Internet)\nWhen adding text files, make sure you are using the correct encoding.\nThe actual setting is: %s",
"StatusFormen"  : " %s word forms/types (%s words/tokens in corpus)",
"StatusNoKorp"  : " no corpus ",
"StatusSuchKonk": "Look up concordances...",
"StatusTreffer" : " %s hits ",
"StatusExpFreq" : "Frequency list has been exported.",
"StatusExpKonk" : "Concordance list has been exported",
"ExportHinweis" : "You should use a non-proportional font (like Courier New)",
"StatusClip"    : "Selection has been copied to clipboard", 
"SpracheAendern": "Change language",
"SpracheInfo"   : "You have to restart TextSTAT to activate the language change.",
"ProgBeenden"   : "Exit TextSTAT",
"AbfrageEnde"   : "The corpus has been changed. \nWould you like to save your corpus \nbefore closing TextSTAT?",
"AbfrageSave"   : "The corpus has been changed. \nWould you like to save your corpus \nbefore opening another corpus?",
}


##############################
########### Portuguese #######
##############################
# Thanks to Hugo Sunayama <hugocogeae@yahoo.com.br> for the translation

portuguese  = {
"Titel"         : "TextSTAT",
"Titel_lang"    : "TextSTAT (Simple Text Analysis Tool)",
"Version"       : Version,
"VersionDatum"  : "Data da versão %s" % VersionDatum,
"Copyright"     : Copyright,
"eMail"         : eMail,
"ProgURL"       : ProgURL,
"ProgInfo"      : "TextSTAT é software livre.\nUse por sua conta e risco.\nO autor não aceita qualquer tipo de responsabilidade.\n\nPortuguese translation by Hugo Sunayama <hugocogeae@yahoo.com.br>",

###### Menu #####
"Korpus"        : "Corpus",
"KorpusNew"     : "Novo corpus",
"KorpusOpen"    : "Abrir corpus",
"KorpusHinzu"   : "Adicionar corpus",
"KorpusSave"    : "Salvar corpus",
"KorpusSaveAs"  : "Salvar como",
"KorpusDel"     : "Deletar corpus",
# has to be translated:
"LetzteKorpora" : "Recent corpora",

"DateiHinzu"    : "Adicionar arquivo local",
"WebHinzu"      : "Adicionar arquivo da web",
"NewsHinzu"     : "Adicionar publicações de um grupo de notícias",
"DateiWeg"      : "Remover arquivo(s)",

"OptSave"       : "Salvar opções",
"Exit"          : "Sair",

"ImpEx"         : "Exportar",
"Ex2csv"        : "Lista de freqüências > Arquivo CSV",
"Ex2txt"        : "Lista de concordâncias > Arquivo TXT",
"Ex2excel"      : "Lista de freqüências > MS Excel",
"Ex2word"       : "Lista de concordâncias > MS Word",
"ImpKorpus"     : "Importar corpus (TextSTAT 1.x)",

"Sprache"       : "Língua",
"CodePage"      : "Codificação",
"Hilfe"         : " ? ",
"RegExpHelp"    : "Expressões regulares - Ajuda",
"History"       : "Histórico (em Alemão)",
"License"       : "Licença",
"Willkommen"    : " Bem-vindo ao TextSTAT",


##### Toolbar #####
"KorpusNew2"    : "Criar novo corpus",
"KorpusOpen2"   : "Abrir corpus existente",
"KorpusHinzu2"  : "Adicionar corpus existente ao corpus atual",
"KorpusSave2"   : "Salvar corpus",
"KorpusDel2"    : "Deletar o arquivo do corpus",
"WebHinzu2"     : "Adicionar arquivo(s) HTML de um corpus da internet",
"DateiHinzu2"   : "Adicionar arquivo local ao corpus",
"DateiWeg2"     : "Deletar arquivo(s) do corpus",
"FreqZeigen"    : "Mostrar freqüência de palavras",
"FreqZeigen2"   : "Mostrar freqüência de formas (de palavras) no corpus",
"Kopieren"      : "Copiar para a área de transferência",
"Kopieren2"     : "Copiar seleção para a área de transferência",
"Ex2excel2"     : "Exportar lista de freqüência para o MS Excel",
"Ex2word2"      : "Exportar concordâncias para o MS Word",
"Info"          : "Informações",
"Info2"         : "Informações a respeito do TextSTAT",

##### Tabs #####
"KorpusTab"     : "Corpus", 
"FormenTab"     : "Formas",
"ConcTab"       : "Concordância",
"ZitatTab"      : "Citação",

"KTinfo"        : "TextSTAT \n\nFavor, crie um corpus e adicione alguns arquivos (ou abra um corpus existente).",
"KTkopieren"    : "Copie o nome do arquivo",
"KTopen"        : "Abrir arquivo",
"KTdatinfo"     : "Informações do arquivo",
"KTdatweg"      : "Remover arquivo(s)",

"FTinfo"        : "FREQÜÊNCIA \nde formas (tokens)",
"FTsortFreq"    : "organizar por freqüência",
"FTsortAlpha"   : "organizar alfabeticamente",
"FTsortRetro"   : "ordem reversa",
"FTminFreq"     : "freqüência min.",
"FTmaxFreq"     : "freqüência max.",
"FTignGross"    : "organizar sem distinção de tipo\n(letras maiúsculas ou minúsculas)",
"FTsuchFreq"    : "OU buscar a freqüência de\npalavras contendo os caracteres:",
"FTfreqliste"   : "Lista de freqüência",
"FTwortform"    : "Formas",
"FTfrequenz"    : "Freqüência",
"FTkopieren"    : "Copiar",
"FTsuchKonk"    : "Buscar concordâncias",
"FTzeigInfo"    : "Mais informações",
"FTformInfo"    : "Informação do corpus\n\nO corpus contém %s palavras (tokens)\n\n%s formas diferentes (tipos, letras maiúsculas são diferenciadas) \n%s tipos (letras maiúsculas são ignoradas)\n\nA forma '%s' aparece %s vezes (= %.2f %%)",
"FTOptionen"    : "Freqüência / opções",

"CTsuchen"      : "Pesquisar",
"CTganzWort"    : "coincindir somente com a palavra inteira",
"CTignGross"    : "não diferenciar letras maiúsculas e minúsculas",
"CTcontextL"    : "contexto à esquerda",
"CTcontextR"    : "contexto à direita",
"CTmarkSuch"    : "destacar padrão ou palavra consultada",
"CTsortAlpha"   : "alfabeticamente",
"CTsortR"       : "organizar o contexto à direita",
"CTsortL"       : "organizar o contexto à esquerda",
"CTaktualisier" : "Atualizar",
"CTmachQuery"   : "Editor de consultas",
"CTkopieren"    : "Copiar",
"CTzeigZitat"   : "Mostrar contexto mais amplo",
"CTOptionen"    : "Opções",

"ZTinfo"        : "Texto da citação",
"ZTkopieren"    : "Copiar",
"ZToeffnen"     : "Abrir arquivo",
"ZToeffnen2"    : "\n(Clique duas vezes para abrir o arquivo)\n",
# Has to be translated:
"ZTInit"        : "\n Double click one of the hits in the concordance tab to show the same hit here with more context.",

##### Webdialog #####
"WDinfo"        : "Adicionar arquivo(s) HTML diretamente da Internet.\nFavor digite o endereço (URL):",
"WDfrageSeiten" : "Quantas páginas você quer adicionar ao seu corpus?",
"WDanzahl"      : "número de páginas a recuperar",
"WDinSubdir"    : "domínio: somente subdiretório",
"WDaufServer"   : "domínio: servidor inteiro",
"WDok"          : "Buscar",
"WDcancel"      : "Fechar",

##### NewsDialog #####
"NDinfo"        : "Adicionar publicações de um grupo de notícias ao corpus", 
"NDserver"      : "servidor de notícias",  
"NDgruppe"      : "grupo de notícias", 
"NDanzahl"      : "número de publicações (postings)", 
"NDzitWeg"      : "remover citações das publicações", 
"NDok"          : "Buscar",
"NDcancel"      : "Fechar",

##### QueryDialog #####
"QDinfo"        : "Função de ajuda para formular consultas complexas\nCaracteres curinga (wildcards) são permitidos aqui (* e ?)",
"QDsuchFelder"  : "Consulta",
"QAusdruck1"    : "primeiro padrão (palavra)",
"QAusdruck2"    : "segundo padrão (palavra)",
"QAbstand"      : "Número de palavras entre os padrões (palavras) consultados",
"QAbstandMin"   : "min. e  ",
"QAbstandMax"   : "max.",
"QmachAusdruck" : "Iniciar consulta",
"QDcancel"      : "Fechar",

##### Fehler #####
"Error"         : "Erro",
"ErrKorpNew"    : "Arquivo do corpus não pôde ser criado!",
"ErrKorpOpen"   : "Arquivo do corpus não pôde ser aberto!\nVocê tem certeza de que este é um arquivo de corpus do TextSTAT?",
"ErrKorpImp"    : "Arquivo do corpus não pôde ser importado!",
"ErrNoKorp"     : "Você deve primeiramente criar um corpus (ou abrir um corpus existente).",
"ErrKorpSave"   : "Arquivo do corpus não pôde ser salvo!",
"ErrFileOpen"   : "Erro ao abrir o arquivo!",
"ErrSelect"     : "Nada selecionado!",
"ErrOptSave"    : "Opções não foram armazenadas!!",
"ErrExport"     : "Erro ao exportar",
"ErrNoFreqList" : "Nenhuma lista de freqüência disponível",
"ErrNoKonkList" : "Nenhuma lista de concordâncias disponível",
"ErrExcel"      : "Erro do MS Excel!\n(As extensões win32 do Python e o MS Excel estão instalados corretamente?)",
"ErrExcel65K"   : "Erro! O MS Excel não pode gerenciar mais de 65500 linhas. \nTemos %s formas.",
"ErrWord"       : "Erro do MS Word!\n(As extensões win32 do Python e o MS Word estão instalados corretamente?)",
"ErrExpFreq"    : "Erro ao salvar a lista de freqüências de palavras exportada.",
"ErrExpKonk"    : "Erro ao salvar a lista de concordâncias exportada.",
"ErrWeb"        : "Erro ao buscar arquivos HTML da Internet.",
"ErrNews"       : "Erro ao buscar publicações de grupos de notícias.",
"ErrQuery"      : "Nenhuma palavra (padrão) de busca definida.",


##### Mitteilungen und Fragen #####
"AskSure"       : "Você tem certeza?",
"AskDelete"     : "Você realmente quer deletar %s ?",
"StatusWeb"     : "%s arquivos estão sendo coletados da Internet...",
"StatusNews"    : "%s publicações de grupos de notícias estão sendo coletadas...",
"DateiInfo"     : "Informação do arquivo",
"DateiInfo2"    : "Arquivo: %s\n\nCaminho: %s\n\nTamanho: %s\n",
"InfoText"      : "TextSTAT\
\n\n\nApós criar (ou abrir) um corpus, você pode adicionar alguns arquivos a ele. \
Você pode, obviamente, remover alguns arquivos também. O botão direito do mouse mostrará um menu de opções. \
O corpus pode ser salvo como um todo. \
\n\nHá alguns utilitários para análise textual: \n\nEm 'Formas' você encontrará \
números de freqüências para tokens em seus textos. \nEm 'Concordâncias' você encontrará concordâncias KWIC \
(palavras - chave contextualizadas). \n'Citação' apresenta contextos amplificados.",

"StatusKorpImp" : "O corpus foi importado com sucesso.",
"StatusKorpSave": "O corpus foi salvo.",
"StatusFiles"   : " %s arquivos | %s bytes ",
"StatusKorpDat" : " Corpus: %s  (atual definição de codificação: %s)",
"KodierungInfo" :  "Agora você pode adicionar alguns arquivos ao corpus.\n\nArquivos de processadores de texto:\n- arquivos do MS Word ('.doc', '.docx'; somente Windows)\n- arquivos do OpenOffice ('.sxw', '.odt')\n\nArquivos de texto:\n- arquivos de texto ASCII/ANSI (frequentemente '.txt')\n- arquivos HTML (localizados em seu HD ou diretamente da Internet)\n- Publicações de grupos de notícias (diretamente da Internet)\nAo adicionar arquivos de texto, certifique-se de usar a codificação correta.\nA definição atual é: %s",
"StatusFormen"  : " %s formas/tipos (%s palavras/tokens no corpus)",
"StatusNoKorp"  : " nenhum corpus ",
"StatusSuchKonk": "Buscar concordâncias...",
"StatusTreffer" : " %s hits ",
"StatusExpFreq" : "A lista de freqüências foi exportada.",
"StatusExpKonk" : "A lista de concordâncias foi exportada",
"ExportHinweis" : "Você deve utilizar uma fonte não-proporcional (como Courier New)",
"StatusClip"    : "A seleção foi copiada para a área de transferência", 
"SpracheAendern": "Mudar a língua",
"SpracheInfo"   : "Você deve reiniciar o TextSTAT para ativar a mudança de língua.",
"ProgBeenden"   : "Sair do TextSTAT",
"AbfrageEnde"   : "O corpus foi alterado. \nVocê gostaria de salvar o seu corpus \nantes de fechar o TextSTAT?",
"AbfrageSave"   : "O corpus foi alterado. \nVocê gostaria de salvar o seu corpus \nantes de abrir outro corpus?",
}

##############################
########### Français #########
##############################
# Thanks to Robert Caron <robert.caron@wanadoo.fr> for the translation!

francais  = {
"Titel"         : "TextSTAT",
"Titel_lang"    : "TextSTAT (Outil d'analyse de textes)",
"Version"       : Version,
"VersionDatum"  : "Version date %s" % VersionDatum,
"Copyright"     : Copyright,
"eMail"         : eMail,
"ProgURL"       : ProgURL,
"ProgInfo"      : "TextSTAT est un logiciel libre.\nL'auteur décline toute responsabilité.\npour d'éventuelles erreurs.\n\nFrench translation by Robert Caron <robert.caron@wanadoo.fr>",

###### Menu #####
"Korpus"        : "Corpus",
"KorpusNew"     : "Nouveau corpus",
"KorpusOpen"    : "Ouvrir un corpus",
"KorpusHinzu"   : "Ajouter un corpus",
"KorpusSave"    : "Enregistrer le corpus",
"KorpusSaveAs"  : "Enregistrer sous...",
"KorpusDel"     : "Supprimer le corpus",
"LetzteKorpora" : "Corpus récents",

"DateiHinzu"    : "Ajouter un fichier",
"WebHinzu"      : "Ajouter un document HTML",
"NewsHinzu"     : "Ajouter messages de liste de diffusion",
"DateiWeg"      : "Effacer fichier(s)",

"OptSave"       : "Sauvegarder les options",
"Exit"          : "Quitter",

"ImpEx"         : "Exporter",
"Ex2csv"        : "Liste de fréquences > Fichier CSV",
"Ex2txt"        : "Liste d'occurences > Fichier TXT",
"Ex2excel"      : "Liste de fréquences > MS Excel",
"Ex2word"       : "Liste d'occurences > MS Word",
"ImpKorpus"     : "Importer un corpus (TextSTAT 1.x)",

"Sprache"       : "Langage",
"CodePage"      : "Système de codage",
"Hilfe"         : " ? ",
"RegExpHelp"    : "Expressions régulières - Aide",
"History"       : "Historique (Allemand)",
"License"       : "Licence",
"Willkommen"    : " Bienvenue dans TextSTAT",


##### Toolbar #####
"KorpusNew2"    : "Créer nouveau corpus",
"KorpusOpen2"   : "Ouvrir un corpus",
"KorpusHinzu2"  : "Ajouter un corpus à l'actuel corpus",
"KorpusSave2"   : "Enregistrer corpus",
"KorpusDel2"    : "Effacer fichier du corpus",
"WebHinzu2"     : "Ajouter fichier(s) HTML depuis Internet dans le corpus",
"DateiHinzu2"   : "Ajouter fichier(s) locaux au corpus",
"DateiWeg2"     : "Effacer fichier(s) du corpus",
"FreqZeigen"    : "Montrer fréquences des mots",
"FreqZeigen2"   : "Montrer fréquences des formes de mots du corpus",
"Kopieren"      : "Copier dans presse-papier",
"Kopieren2"     : "Copier sélection dans presse-papier",
"Ex2excel2"     : "Exporter la liste des fréquences dans MS Excel",
"Ex2word2"      : "Exporter les occurences dans MS Word",
"Info"          : "Information",
"Info2"         : "Information à propos de TextSTAT",

##### Tabs #####
"KorpusTab"     : "Corpus", 
"FormenTab"     : "Formes de mots",
"ConcTab"       : "Occurences",
"ZitatTab"      : "Contexte",

"KTinfo"        : "TextSTAT \n\nCréer un corpus et ajouter quelques fichiers (ou ouvrir un corpus existant).",
"KTkopieren"    : "Copier nom du fichier",
"KTopen"        : "Ouvrir fichier",
"KTdatinfo"     : "Informations sur fichier",
"KTdatweg"      : "Supprimer fichier(s)",

"FTinfo"        : "FREQUENCES \ndes formes de mots (signes)",
"FTsortFreq"    : "Classer par fréquence",
"FTsortAlpha"   : "Classer par ordre alphabétique",
"FTsortRetro"   : "Classer par ordre alphabétique inverse",
"FTminFreq"     : "Fréquence min.",
"FTmaxFreq"     : "Fréquence max.",
"FTignGross"    : "Trier entre MAJUSCULES et minuscules",
"FTsuchFreq"    : "OU montrer les fréquences des\nmots contenant la chaîne de caractères :",
"FTfreqliste"   : "Liste de fréquences",
"FTwortform"    : "Formes de mots",
"FTfrequenz"    : "Fréquences",
"FTkopieren"    : "Copier",
"FTsuchKonk"    : "Montrer les occurences",
"FTzeigInfo"    : "Plus d'informations",
"FTformInfo"    : "Information sur le corpus\n\nLe corpus contient %s mots (signes)\n\n%s formes différentes (types, lettres majuscules prises en compte) \n%s types (lettres majuscules sont ignorées)\n\nLa forme '%s' apparaît %s fois (= %.2f %%)",
"FTOptionen"    : "Fréquences / options",

"CTsuchen"      : "Rechercher",
"CTganzWort"    : "Rechercher mots entiers seulement",
"CTignGross"    : "Rechercher sans distinction \nmajuscule/minuscule",
"CTcontextL"    : "Contexte gauche",
"CTcontextR"    : "Contexte droit",
"CTmarkSuch"    : "Marquer la chaîne recherchée",
"CTsortAlpha"   : "Alphabétique",
"CTsortR"       : "Classer par contexte droit",
"CTsortL"       : "Classer par contexte gauche",
"CTaktualisier" : "Actualiser",
"CTmachQuery"   : "Editeur de requête",
"CTkopieren"    : "Copier",
"CTzeigZitat"   : "Montrer plus de contextes",
"CTOptionen"    : "Options",

"ZTinfo"        : "Texte de la citation",
"ZTkopieren"    : "Copier",
"ZToeffnen"     : "Ouvrir fichier",
"ZToeffnen2"    : "\n(Double-clique pour ouvrir le fichier)\n",
"ZTInit"        : "\n Double-clique une de ces occurences pour l'afficher dans son contexte.",

##### Webdialog #####
"WDinfo"        : "Ajouter fichiers HTML directement depuis internet.\nEntrer son URL:",
"WDfrageSeiten" : "Combien de pages voulez-vous ajouter à votre corpus ?",
"WDanzahl"      : "Nombre de pages à trouver",
"WDinSubdir"    : "Domaine : seulement les sous-répertoires",
"WDaufServer"   : "Domaine : tout le site",
"WDok"          : "Chercher",
"WDcancel"      : "Quitter",

##### NewsDialog #####
"NDinfo"        : "Ajouter messages de listes de diffusion dans le corpus",
"NDserver"      : "Nouveau serveur",
"NDgruppe"      : "Liste de diffusion",
"NDanzahl"      : "Nombre de messages",
"NDzitWeg"      : "Supprimer les citations des messages",
"NDok"          : "Chercher",
"NDcancel"      : "Quitter",

##### QueryDialog #####
"QDinfo"        : "Aides pour formuler des requêtes complexes \nCaractères de remplacement sont attribués ici (* et ?)",
"QDsuchFelder"  : "Rechercher terme",
"QAusdruck1"    : "Premier terme recherché",
"QAusdruck2"    : "Second terme recherché",
"QAbstand"      : "Nombre de mots entre les deux termes recherchés",
"QAbstandMin"   : "min. et   ",
"QAbstandMax"   : "max.",
"QmachAusdruck" : "Démarrer recherche",
"QDcancel"      : "Quitter",

##### Fehler #####
"Error"         : "Erreur",
"ErrKorpNew"    : "Fichier de corpus ne peut être créé !",
"ErrKorpOpen"   : "Fichier de corpus ne peut pas être ouvert !\nEtes-vous sûr qu'il s'agit d'un fichier corpus de TextSTAT ?",
"ErrKorpImp"    : "Fichier corpus ne peut être importé !!",
"ErrNoKorp"     : "Vous devez d'abord créé un fichier de corpus (ou ouvrir un corpus existant).",
"ErrKorpSave"   : "Fichier de corpus ne peut pas être sauvegarder !",
"ErrFileOpen"   : "Erreur à l'ouverture du fichier !",
"ErrSelect"     : "Rien n'a été sélectionné !",
"ErrOptSave"    : "Les Options ne peuvent pas être sauvegardées !",
"ErrExport"     : "Erreur à l'exportation",
"ErrNoFreqList" : "Pas de liste de fréquence de mots disponible",
"ErrNoKonkList" : "Pas de liste d'occurence de mots disponible",
"ErrExcel"      : "MS Excel ereuor !\n(Est-ce que l'extension Python win32 et MS Excel sont installés correctement ?)",
"ErrExcel65K"   : "Erreur ! MS Excel ne peut traiter plus de 65500 lignes. \nNous avez %s formes.",
"ErrWord"       : "MS Word erreur !\n(Est-ce que l'extension Python win32 et MS Word sont installés correctement ??)",
"ErrExpFreq"    : "Erreur lors de la sauvegarde de l'export de la liste de fréquences.",
"ErrExpKonk"    : "Erreur lors de la sauvegarde de l'export de la liste d'occurences.",
"ErrWeb"        : "Erreur de chargement de fichiers HTML depuis internet.",
"ErrNews"       : "Erreur de chargement de messages de la liste de diffusion.",
"ErrQuery"      : "Pas de terme à rechercher.",


##### Mitteilungen und Fragen #####
"AskSure"       : "Etes-vous sûr ?",
"AskDelete"     : "Voulez-vous vraiment effacer %s ?",
"StatusWeb"     : "%s fichiers ont été trouvés sur internet...",
"StatusNews"    : "%s messages de la liste de diffusion ont été trouvés...",
"DateiInfo"     : "Information fichier",
"DateiInfo2"    : "Fichier : %s\n\nChemin : %s\n\nTaille : %s\n",
"InfoText"      : "TextSTAT\
\nAprès avoir créé (ou ouvert) un corpus, vous pourrez lui rajouter des fichiers. \
Vous pourrez, bien sûr, aussi supprimer des fichiers. Le clic-droit de la souris vous ouvrira un menu contextuel. \
Le corpus peut être sauvegarder dans son ensemble. \
\nIl y a quelques facilités pour l'analyse de textes : vous trouverez sous 'Formes de mots' \
Fréquences pour les signes dans votre texte. 'Occurence' vous montrera vos KWIC occurences \
(Mots-clés en contexte), 'Citation' donnera davantage de contexte.",

"StatusKorpImp" : "Corpus a été importé avec succès.",
"StatusKorpSave": "Corpus a été sauvegardé.",
"StatusFiles"   : " %s fichiers | %s bytes ",
"StatusKorpDat" : " Corpus: %s  (Réglage CodePage actuel : %s)",
"KodierungInfo" :  "Vous pouvez maintenant ajouter des fichiers à votre corpus.\n\nFichiers de Traitement de textes :\n- Fichiers MS Word ('.doc', '.docx'; Windows seulement)\n- Fichiers OpenOffice ('.sxw', '.odt')\n\nFichiers texte :\n- ASCII/ANSI fichiers texte (souvent '.txt')\n- Fichiers HTML (depuis votre disque dur ou directement depuis internet)\n- Messages de liste de diffusion (directement depuis internet)\nLors de l'ajout de fichiers texte, vérifier que l'encodage est correct.\nLe réglage actuel est : %s",
"StatusFormen"  : " %s mots formes/types (%s mots/signes dans le corpus)",
"StatusNoKorp"  : " pas de corpus ",
"StatusSuchKonk": "Mettre en évidence les occurences...",
"StatusTreffer" : " %s éléments ",
"StatusExpFreq" : "Liste de fréquences a été exportée.",
"StatusExpKonk" : "Liste des occurences a été exportée",
"ExportHinweis" : "Vous devez utilisé une police non-proportionnelle (comme Courier New)",
"StatusClip"    : "Sélection copiée dans le presse-papier",
"SpracheAendern": "Changer langage",
"SpracheInfo"   : "Vous devez redémarrer TextSTAT pour que le changement de langage soit pris en compte.",
"ProgBeenden"   : "Quitter TextSTAT",
"AbfrageEnde"   : "Le corpus a été modifié. \nVoulez-vous sauvegarder votre corpus \navant de quitter TextSTAT ?",
"AbfrageSave"   : "Le corpus a été modifié. \nVoulez-vous sauvegarder votre corpus \navant d'ouvrir un autre corpus ?",
}

##############################
########### Galician ##########
##############################
# Thanks to Gonçalo Cordeiro <gzcordeiro@gmail.com> for the translation!

galician  = {
"Titel"         : "TextSTAT",
"Titel_lang"    : "TextSTAT (utilidade para a análise de textos)",
"Version"       : Version,
"VersionDatum"  : "Data da versión %s" % VersionDatum,
"Copyright"     : Copyright,
"eMail"         : eMail,
"ProgURL"       : ProgURL,
"ProgInfo"      : "O TextSTAT é software gratuíto.\nÚseo por súa conta e risco.\nO autor declina toda responsabilidade.\n\nTranslation into Galician by Gonçalo Cordeiro <gzcordeiro@gmail.com>",

###### Menu #####
"Korpus"        : "Corpus",
"KorpusNew"     : "Novo corpus",
"KorpusOpen"    : "Abrir un corpus",
"KorpusHinzu"   : "Engadir un corpus",
"KorpusSave"    : "Gardar o corpus",
"KorpusSaveAs"  : "Gardar como",
"KorpusDel"     : "Eliminar corpus",
"LetzteKorpora" : "Últimos corpora",

"DateiHinzu"    : "Engadir un ficheiro local",
"WebHinzu"      : "Engadir un ficheiro da Web",
"NewsHinzu"     : "Engadir mensaxes dun grupo de noticias",
"DateiWeg"      : "Eliminar ficheiro(s)",

"OptSave"       : "Gardar as opcións",
"Exit"          : "Saír",

"ImpEx"         : "Exportar",
"Ex2csv"        : "Lista de frecuencias > ficheiro CSV",
"Ex2txt"        : "Lista de concordancias > ficheiro TXT",
"Ex2excel"      : "Lista de frecuencias > MS Excel",
"Ex2word"       : "Lista de frecuencias > MS Word",
"ImpKorpus"     : "Importar corpus (TextSTAT 1.x)",

"Sprache"       : "Idioma",
"CodePage"      : "Codificación",
"Hilfe"         : " ? ",
"RegExpHelp"    : "Expresións regulares - Axuda",
"History"       : "Historia (Alemán)",
"License"       : "Licenza",
"Willkommen"    : " Benvido ao TextSTAT",


##### Toolbar #####
"KorpusNew2"    : "Crear un novo corpus",
"KorpusOpen2"   : "Abrir un corpus existente",
"KorpusHinzu2"  : "Engadir un corpus existente ao actual",
"KorpusSave2"   : "Gardar o corpus",
"KorpusDel2"    : "Eliminar o ficheiro de corpus",
"WebHinzu2"     : "Engadir ficheiros HTML desde Internet ao corpus",
"DateiHinzu2"   : "Engadir ficheiros locais ao corpus",
"DateiWeg2"     : "Eliminar ficheiros do corpus",
"FreqZeigen"    : "Mostrar as frecuencias de palabras",
"FreqZeigen2"   : "Mostra as frecuencias de formas de palabra no corpus",
"Kopieren"      : "Copiar no portapapeis",
"Kopieren2"     : "Copia a selección no portapapeis",
"Ex2excel2"     : "Exportar a lista de frecuencias para o MS Excel",
"Ex2word2"      : "Exportar as concordancias para o MS Word",
"Info"          : "Información",
"Info2"         : "Información sobre o TextSTAT",

##### Tabs #####
"KorpusTab"     : "Corpus", 
"FormenTab"     : "Formas de palabra",
"ConcTab"       : "Concordancias",
"ZitatTab"      : "Contexto",

"KTinfo"        : "TextSTAT \n\nCree un corpus e engada algún ficheiro (ou abra un corpus existente).",
"KTkopieren"    : "Copiar o nome do ficheiro",
"KTopen"        : "Abrir ficheiro",
"KTdatinfo"     : "Información de ficheiro",
"KTdatweg"      : "Eliminar ficheiro(s)",

"FTinfo"        : "FRECUENCIA \ndas formas de palabra (tokens)",
"FTsortFreq"    : "ordenar por frecuencia",
"FTsortAlpha"   : "ordenar alfabeticamente",
"FTsortRetro"   : "orde inversa (d-e)",
"FTminFreq"     : "frecuencia mín.",
"FTmaxFreq"     : "frecuencia máx.",
"FTignGross"    : "ordenar sen distinguir Maiús./min.",
"FTsuchFreq"    : "OU procurar a frecuencia\ndas palabras que conteñan a cadea:",
"FTfreqliste"   : "Lista de frecuencias",
"FTwortform"    : "Forma de palabra",
"FTfrequenz"    : "Frecuencia",
"FTkopieren"    : "Copiar",
"FTsuchKonk"    : "Procurar concordancias",
"FTzeigInfo"    : "Máis información",
"FTformInfo"    : "Información sobre o corpus\n\nO corpus contén %s palabras (tokens)\n\n%s formas diferentes (tipos, as maiúsculas son tidas en conta) \n%s tipos (ignorando as maiúsculas)\n\nA forma '%s' aparece %s veces (= %.2f %%)",
"FTOptionen"    : "Frecuencia / opcións",

"CTsuchen"      : "Procurar",
"CTganzWort"    : "procurar só palabras completas",
"CTignGross"    : "procurar sen distinguir Maiús./min.",
"CTcontextL"    : "contexto á esquerda",
"CTcontextR"    : "contexto á dereita",
"CTmarkSuch"    : "marcar a cadea procurada",
"CTsortAlpha"   : "alfabeticamente",
"CTsortR"       : "ordenar o contexto á dereita",
"CTsortL"       : "ordenar o contexto á esquerda",
"CTaktualisier" : "Recargar",
"CTmachQuery"   : "Editor de consultas",
"CTkopieren"    : "Copiar",
"CTzeigZitat"   : "Mostrar máis contexto",
"CTOptionen"    : "Opcións",

"ZTinfo"        : "Texto da citación",
"ZTkopieren"    : "Copiar",
"ZToeffnen"     : "Abrir ficheiro",
"ZToeffnen2"    : "\n(Prema dúas veces para abrir o ficheiro)\n",
"ZTInit"        : "\n Prema dúas veces nunha das ocorrencias do separador de concordancias para mostrar esta mesma ocorrencia con máis contexto.",

##### Webdialog #####
"WDinfo"        : "Engadir ficheiros HTML directamente desde a Internet.\nIntroduza o URL:",
"WDfrageSeiten" : "Cantas páxinas quere engadir ao seu corpus?",
"WDanzahl"      : "número de páxinas para recuperar",
"WDinSubdir"    : "dominio: só o subdirectorio",
"WDaufServer"   : "dominio: todo o servidor",
"WDok"          : "Procurar",
"WDcancel"      : "Pechar",

##### NewsDialog #####
"NDinfo"        : "Engadir mensaxes dun grupo de noticias ao corpus", 
"NDserver"      : "servidor de noticias",  
"NDgruppe"      : "grupo de noticias", 
"NDanzahl"      : "número de mensaxes", 
"NDzitWeg"      : "eliminar as citas das mensaxes", 
"NDok"          : "Procurar",
"NDcancel"      : "Pechar",

##### QueryDialog #####
"QDinfo"        : "Función de axuda para formular consultas complexas \nAquí permítense os comodíns (* e ?)",
"QDsuchFelder"  : "Termo de procura",
"QAusdruck1"    : "primero termo de procura",
"QAusdruck2"    : "segundo termo de procura",
"QAbstand"      : "Número de palabras entre ambos os termos de procura",
"QAbstandMin"   : "mín. e   ",
"QAbstandMax"   : "máx.",
"QmachAusdruck" : "Comezar a procura",
"QDcancel"      : "Pechar",

##### Fehler #####
"Error"         : "Erro",
"ErrKorpNew"    : "Non se puido crear o ficheiro de corpus!",
"ErrKorpOpen"   : "Non se puido abrir o ficheiro de corpus!\nEstá seguro de que se trata dun ficheiro de corpus do TextSTAT?",
"ErrKorpImp"    : "Non se puido importar o ficheiro de corpus!!",
"ErrNoKorp"     : "Primeiro hai que crear un corpus (ou abrir un corpus existente).",
"ErrKorpSave"   : "Non se puido gardar o ficheiro de corpus!",
"ErrFileOpen"   : "Erro ao abrir o ficheiro!",
"ErrSelect"     : "Non se seleccionou nada!",
"ErrOptSave"    : "Non se puido gardar as opcións!",
"ErrExport"     : "Erro na exportación",
"ErrNoFreqList" : "Non hai unha lista de frecuencias dispoñíbel",
"ErrNoKonkList" : "Non hai unha lista concordancias dispoñíbel",
"ErrExcel"      : "Erro do MS Excel!\n(Están instalados correctamente as extensións win32 do Python e o MS Excel?)",
"ErrExcel65K"   : "Erro! O MS Excel non pode manipular máis de 65500 filas. \nHai %s formas.",
"ErrWord"       : "Erro do MS Word!\n(Están instalados correctamente as extensións win32 do Python e o MS Excel?)",
"ErrExpFreq"    : "Erro ao gardar a lista de frecuencias exportada.",
"ErrExpKonk"    : "Erro ao gardar a lista de concordancias exportada.",
"ErrWeb"        : "Erro ao obter os ficheiros HTML da Internet.",
"ErrNews"       : "Erro ao obter as mensaxes do grupo de noticias.",
"ErrQuery"      : "Non se definiu un termo de procura.",


##### Mitteilungen und Fragen #####
"AskSure"       : "Está seguro?",
"AskDelete"     : "Quere eliminar %s ?",
"StatusWeb"     : "estase a obter %s ficheiros da Internet...",
"StatusNews"    : "estase a obter %s mensaxes de grupo de noticias...",
"DateiInfo"     : "Información de ficheiro",
"DateiInfo2"    : "Ficheiro: %s\n\nCamiño: %s\n\nTamaño: %s\n",
"InfoText"      : "TextSTAT\
\nDespois de crear (ou abrir) un corpus, pode engadirlle algúns ficheiros. \
Pode tamén eliminar ficheiros. O botón dereito do rato proporciona un menú de contexto. \
Pódese gardar o corpus como un conxunto. \
\nHai algunhas utilidades para a análise de textos: debaixo de 'Formas de palabra' encontrará \
os números de frecuencias dos tokens dos seus textos. 'Concordancia' mostra as concordancias KWIC \
(KeyWord in Context), 'Contexto' presenta os segmentos de texto onde aparece a palabra.",

"StatusKorpImp" : "O corpus importouse correctamente.",
"StatusKorpSave": "Gardouse o corpus.",
"StatusFiles"   : " %s ficheiros | %s bytes ",
"StatusKorpDat" : " Corpus: %s  (Codificación de caracteres actual: %s)",
"KodierungInfo" :  "Agora pode engadir algúns ficheiros ao corpus.\n\nFicheiros de procesador de palabras:\n- ficheiros do MS Word ('.doc' ou '.docx'; só no Windows)\n- ficheiros do OpenOffice.org ('.odt ou .sxw')\n\nFicheiros de texto:\n- ficheiros ASCII/ANSI (normalmente '.txt')\n- ficheiros HTML  (do seu disco ríxido ou directamente da Internet)\n- Mensaxes de grupos de noticias (directamente da Internet)\nAo engadir ficheiros de texto, asegúrese de que emprega a codificación correcta.\nA configuración actual é: %s",
"StatusFormen"  : " %s formas/tipos de palabra (%s palabras/tokens no corpus)",
"StatusNoKorp"  : " sen corpus ",
"StatusSuchKonk": "Procurar concordancias...",
"StatusTreffer" : " %s ocorrencias ",
"StatusExpFreq" : "Exportouse a lista de frecuencias.",
"StatusExpKonk" : "Exportouse a lista de concordancias",
"ExportHinweis" : "Debe utilizar un tipo de letra non proporcional (como o Courier New)",
"StatusClip"    : "Copiouse a selección no portapapeis", 
"SpracheAendern": "Cambiar o idioma",
"SpracheInfo"   : "É preciso reiniciar o TextSTAT para activar o cambio de idioma.",
"ProgBeenden"   : "Saír do TextSTAT",
"AbfrageEnde"   : "Modificouse o corpus. \nQuere gardar o corpus antes de \npechar o TextSTAT?",
"AbfrageSave"   : "Modificouse o corpus. \nQuere gardar o corpus antes de \nabrir un outro corpus?",
}

##############################
########### Finnish ##########
##############################

#Translator: Lasse Ehrnrooth
#Date: 15/4/2009
#Contact: <lasse.ehrnrooth@helsinki.fi>
#Other translators/proofreaders: Please send email to me if you make changes to the Finnish translations below.

suomi  = {
"Titel"         : "TextSTAT",
"Titel_lang"    : "TextSTAT (yksinkertainen ohjelma tekstianalyyseihin)",
"Version"       : Version,
"VersionDatum"  : "Version päiväys %s" % VersionDatum,
"Copyright"     : Copyright,
"eMail"         : eMail,
"ProgURL"       : ProgURL,
"ProgInfo"      : "TextSTAT on ilmainen ohjelma.\nKäyttö omalla vastuulla.\nOhjelman tekijä ei vastaa mahdollisista haitoista.\n\nTranslation into Finnish by Lasse Ehrnrooth <lasse.ehrnrooth@helsinki.fi>",

###### Menu #####
"Korpus"        : "Korpus",
"KorpusNew"     : "Uusi korpus",
"KorpusOpen"    : "Avaa korpus",
"KorpusHinzu"   : "Lisää korpus",
"KorpusSave"    : "Tallenna korpus",
"KorpusSaveAs"  : "Tallenna nimellä",
"KorpusDel"     : "Poista korpus",
"LetzteKorpora" : "Viimeksi käytetyt korpukset",

"DateiHinzu"    : "Lisää paikallinen tiedosto",
"WebHinzu"      : "Lisää tiedosto internetistä",
"NewsHinzu"     : "Lisää uutisryhmäviesti",
"DateiWeg"      : "Poista tiedosto(ja)",

"OptSave"       : "Tallenna asetukset",
"Exit"          : "Sulje",

"ImpEx"         : "Vie tiedosto",
"Ex2csv"        : "Frekvenssilista > CSV tiedosto",
"Ex2txt"        : "Konkordanssilista > TXT tiedosto",
"Ex2excel"      : "Frekvenssilista > MS Excel",
"Ex2word"       : "Konkordanssilista > MS Word",
"ImpKorpus"     : "Tuo korpus (TextSTAT 1.x)",

"Sprache"       : "Kieli",
"CodePage"      : "Merkistökoodaus",
"Hilfe"         : " ? ",
"RegExpHelp"    : "Säännölliset lausekkeet - Ohje",
"History"       : "Historia (saksaksi)",
"License"       : "Lisenssi",
"Willkommen"    : " Tervetuloa TextSTAT:in pariin",


##### Toolbar #####
"KorpusNew2"    : "Luo uusi korpus",
"KorpusOpen2"   : "Avaa aiemmin luotu korpus",
"KorpusHinzu2"  : "Lisää aiempi korpus nykyiseen",
"KorpusSave2"   : "Tallenna korpus",
"KorpusDel2"    : "Poista korpus tiedosto",
"WebHinzu2"     : "Lisää HTML tiedosto(ja) internetistä korpukseen",
"DateiHinzu2"   : "Lisää paikallisia tiedostoja korpukseen",
"DateiWeg2"     : "Poista tiedosto(ja) korpuksesta",
"FreqZeigen"    : "Näytä sanojen frekvenssit",
"FreqZeigen2"   : "Näytä sananmuotojen frekvenssit korpuksessa",
"Kopieren"      : "Kopioi leikepöydälle",
"Kopieren2"     : "Kopioi valinta leikepöydälle",
"Ex2excel2"     : "Vie frekvenssilista MS Exceliin",
"Ex2word2"      : "Vie konkordanssihaut MS Wordiin",
"Info"          : "Tietoja",
"Info2"         : "Tietoja TextSTAT:sta",

##### Tabs #####
"KorpusTab"     : "Korpus", 
"FormenTab"     : "Sananmuodot",
"ConcTab"       : "Konkordanssi",
"ZitatTab"      : "Sitaatti",

"KTinfo"        : "TextSTAT \n\nLuo korpus ja lisää siihen tiedostoja (tai avaa aiempi korpus).",
"KTkopieren"    : "Kopioi tiedostonimi",
"KTopen"        : "Avaa tiedosto",
"KTdatinfo"     : "Tiedoston ominaisuudet",
"KTdatweg"      : "Poista tiedosto(ja)",

"FTinfo"        : "FREKVENSSI \nsaneiden lukumäärä (saneet)",
"FTsortFreq"    : "järjestä frekvenssittäin",
"FTsortAlpha"   : "järjestä aakkosittain",
"FTsortRetro"   : "järjestä sananmuodot viimeisten kirjainten mukaan",
"FTminFreq"     : "pienin frekvenssi",
"FTmaxFreq"     : "suurin frekvenssi",
"FTignGross"    : "laske ja järjestä kirjainkoosta riippumatta",
"FTsuchFreq"    : "TAI \n\nEtsi frekvenssi\nsanalle tai sanoille merkkijonossa:",
"FTfreqliste"   : "Luo frekvenssilista",
"FTwortform"    : "Sananmuoto",
"FTfrequenz"    : "Frekvenssi",
"FTkopieren"    : "Kopioi",
"FTsuchKonk"    : "Etsi konkordansseja",
"FTzeigInfo"    : "Lisätietoja",
"FTformInfo"    : "Tietoja korpuksesta\n\nKorpus sisältää sanoja (saneita)\n\n%s erilaisia muotoja (tyyppejä, isot kirjaimet huomioidaan) \n%s tyyppejä (isoja kirjaimia ei huomioida)\n\nMuoto '%s' esiintyy %s kertaa (= %.2f %%)",
"FTOptionen"    : "Frekvenssiasetukset",

"CTsuchen"      : "Etsi",
"CTganzWort"    : "etsi vain kokonaisia sanoja",
"CTignGross"    : "etsi kirjainkoosta riippumatta",
"CTcontextL"    : "vasen konteksti",
"CTcontextR"    : "oikea konteksti",
"CTmarkSuch"    : "korosta etsittävä merkkijono",
"CTsortAlpha"   : "esiintymisjärjestyksessä",
"CTsortR"       : "järjestä oikea konteksti",
"CTsortL"       : "järjestä vasen konteksti",
"CTaktualisier" : "Päivitä",
"CTmachQuery"   : "Hakueditori",
"CTkopieren"    : "Kopioi",
"CTzeigZitat"   : "Näytä enemmän kontekstia",
"CTOptionen"    : "Konkordanssiasetukset",

"ZTinfo"        : "Sitaatin teksti",
"ZTkopieren"    : "Kopioi",
"ZToeffnen"     : "Avaa tiedosto",
"ZToeffnen2"    : "\n(kaksoisnapsauta tiedostoa)\n",
"ZTInit"        : "\n Kaksoisnapsauta yhtä osumista konkordanssivälilehdessä nähdäksesi saman osuman laajemmalla kontekstilla.",

##### Webdialog #####
"WDinfo"        : "Lisää HTML tiedostoja suoraan internetistä\nSyötä sivun osoite:",
"WDfrageSeiten" : "Kuinka monta sivua haluat lisätä korpukseesi?",
"WDanzahl"      : "Haettavien sivujen lukumäärä",
"WDinSubdir"    : "haettava alue: vain alihakemisto",
"WDaufServer"   : "haettava alue: koko palvelin",
"WDok"          : "Etsi",
"WDcancel"      : "Sulje",

##### NewsDialog #####
"NDinfo"        : "Lisää uutisryhmän viestit korpukseen",
"NDserver"      : "uutispalvelin",  
"NDgruppe"      : "uutisryhmä", 
"NDanzahl"      : "viestien lukumäärä", 
"NDzitWeg"      : "poista lainausmerkit viesteistä", 
"NDok"          : "Etsi",
"NDcancel"      : "Sulje",

##### QueryDialog #####
"QDinfo"        : "Aputoiminto monimutkaisempia hakuja varten. \nKorvausmerkit ovat sallittuja (* ja ?).",
"QDsuchFelder"  : "Hakutermit",
"QAusdruck1"    : "ensimmäinen hakutermi",
"QAusdruck2"    : "toinen hakutermi",
"QAbstand"      : "Sanojen lukumäärä hakutermien välillä",
"QAbstandMin"   : "min ja   ",
"QAbstandMax"   : "max",
"QmachAusdruck" : "Käynnistä etsintä",
"QDcancel"      : "Sulje",

##### Fehler #####
"Error"         : "Virhe",
"ErrKorpNew"    : "Korpustiedostoa ei voitu luoda.",
"ErrKorpOpen"   : "Korpustiedostoa ei voitu avata.\nOnko tiedosto TextSTAT korpus tiedosto?",
"ErrKorpImp"    : "Korpustiedostoa ei voitu tuoda.",
"ErrNoKorp"     : "Sinun täytyy ensiksi luoda korpus (tai avata aiemmin luotu korpus).",
"ErrKorpSave"   : "Korpustiedostoa ei voitu tallentaa.",
"ErrFileOpen"   : "Virhe avattaessa tiedostoa.",
"ErrSelect"     : "Mitään ei ole valittuna.",
"ErrOptSave"    : "Asetuksia ei voitu tallentaa.",
"ErrExport"     : "Tiedoston vientivirhe",
"ErrNoFreqList" : "Sanafrekvenssilistaa ei saatavilla",
"ErrNoKonkList" : "Sanakonkordanssilistaa ei saatavilla",
"ErrExcel"      : "MS Excel virhe.\n(Ovatko Python win32 laajennukset ja MS Excel asennettu oikein?)",
"ErrExcel65K"   : "Virhe. MS Excel ei pysty käsittelemään kuin 65500 riviä. \nKäsiteltävänä on %s muotoa.",
"ErrWord"       : "MS Word virhe.\n(Ovatko Python win32 laajennukset ja MS Word asennettu oikein?)",
"ErrExpFreq"    : "Virhe tallennettaessa vietyä sanafrekvenssilistaa.",
"ErrExpKonk"    : "Virhe tallennettaessa vietyä konkordanssilistaa.",
"ErrWeb"        : "Virhe haettaessa HTML tiedostoja internetistä.",
"ErrNews"       : "Virhe haettaessa uutisryhmän viestejä.",
"ErrQuery"      : "Hakutermiä ei ole määritelty.",

##### Mitteilungen und Fragen #####
"AskSure"       : "Oletko varma?",
"AskDelete"     : "Haluatko varmasti poistaa tiedoston %s ?",
"StatusWeb"     : "%s tiedosto(a) haetaan internetistä...",
"StatusNews"    : "%s uutisryhmäviestiä haetaan...",
"DateiInfo"     : "Tietoja tiedostosta",
"DateiInfo2"    : "Tiedosto: %s\n\nPolku: %s\n\nKoko: %s\n",
"InfoText"      : "TextSTAT\n\nKorpuksen luomisen tai avaamisen jälkeen voit lisätä siihen tiedostoja. Voit myös poistaa tiedostoja. Oikea hiirenpainike avaa kontekstivalikon. Korpuksen voi tallentaa kokonaisuudessaan. \n\nTekstin analysointiin liittyvät ominaisuudet: \n\n'Sananmuodot' välilehden alta löytyvät frekvenssimäärät tekstin saneille. 'Konkordanssi' näyttää KWIC konkordanssit (KeyWord in Context) ja 'Sitaatti' näyttää laajemmin konkordanssiosumaan liittyvää kontekstia.",

"StatusKorpImp" : "Korpuksen tuonti onnistui.",
"StatusKorpSave": "Korpus on tallennettu.",
"StatusFiles"   : " %s tiedosto(a) | %s tavua ",
"StatusKorpDat" : " Korpus: %s  (nykyinen CodePage asetus: %s)",
"KodierungInfo" : "Voit nyt lisätä tiedostoja korpukseen.\n\nTekstinkäsittelytiedostoja:\n- MS Word tiedostoja (.doc tai .docx (.docx vain Windowsissa))\n- OpenOffice tiedostoja (.odt tai .sxw)\n\nTekstitiedostoja:\n- ASCII/ANSI tekstitiedostoja (yleensä .txt)\n- HTML tiedostoja (levyasemalta tai suoraan internetistä)\n- Uutisryhmän viestejä  (suoraan internetistä)\n\nKun lisäät tekstitiedoston, tarkista että käytät oikeaa koodausta.\nMerkistökoodaus on nyt: %s",
"StatusFormen"  : " %s sananmuotoja/tyyppejä (%s sanoja/saneita korpuksessa)",
"StatusNoKorp"  : " ei korpusta ",
"StatusSuchKonk": "Hae konkordanssit...",
"StatusTreffer" : " %s osumat ",
"StatusExpFreq" : "Frekvenssilista on viety.",
"StatusExpKonk" : "Konkordanssilista on viety.",
"ExportHinweis" : "Käytä suhteuttamatonta kirjasintyyppiä (esim. Courier New)",
"StatusClip"    : "Valinta on kopioitu leikepöydälle", 
"SpracheAendern": "Vaihda kieli",
"SpracheInfo"   : "Käynnistä TextSTAT uudelleen aktivoidaksesi kielen vaihto.",
"ProgBeenden"   : "Poistu TextSTAT:sta",
"AbfrageEnde"   : "Korpusta on muutettu. \nTallennetaanko muutokset \nennen TextSTAT:n sulkemista?",
"AbfrageSave"   : "Korpusta on muutettu. \nTallennetaanko muutokset \nennen kuin uusi korpus avataan?",
}

##############################
########### Polski ##########
##############################
# Thanks to Erik-Jan Kuipers <erik.kuipers@uj.edu.pl> for the translation into Polish!

polski  = {
"Titel"         : "TextSTAT",
"Titel_lang"    : "TextSTAT (Proste Narzedzie Analizy Tekstu)",
"Version"       : Version,
"VersionDatum"  : "Data wersji %s" % VersionDatum,
"Copyright"     : Copyright,
"eMail"         : eMail,
"ProgURL"       : ProgURL,
"ProgInfo"      : "TextSTAT jest darmowym oprogramowaniem. Uzywaj na wlasne ryzyko.\nAutor programu nie przyjmuje zadnej odpowiedzialnosci za funkcjonowanie i skutki uzywania programu.\n\nTranslation into Polish by Erik-Jan Kuipers <erik.kuipers@uj.edu.pl>",

###### Menu #####
"Korpus"        : "Korpus",
"KorpusNew"     : "Nowy korpus",
"KorpusOpen"    : "Otworz korpus",
"KorpusHinzu"   : "Dodaj korpus",
"KorpusSave"    : "Zapisz korpus",
"KorpusSaveAs"  : " Zapisz jako",
"KorpusDel"     : "Usun korpus",
"LetzteKorpora" : "Ostatnie korpusy",

"DateiHinzu"    : "Dodaj plik z dysku lokalnego",
"WebHinzu"      : "Dodaj plik z internetu",
"NewsHinzu"     : "Dodaj posty z grupy dyskusyjnej",
"DateiWeg"      : "Usun plik(i)",

"OptSave"       : "Zapisz opcje",
"Exit"          : "Zakoncz",

"ImpEx"         : "Eksportuj",
"Ex2csv"        : "Lista frekwencyjna > plik CSV",
"Ex2txt"        : "Lista konkordancyjna > plik TXT",
"Ex2excel"      : "Lista frekwencyjna > MS Excel",
"Ex2word"       : "Lista konkordancji > MS Word",
"ImpKorpus"     : "Importuj korpus (TextSTAT 1.x)",

"Sprache"       : "Jezyk",
"CodePage"      : "Kodowanie plikow",
"Hilfe"         : " ? ",
"RegExpHelp"    : "Wyrazenia regularne - Pomoc",
"History"       : "Historia (w j. niemieckim)",
"License"       : "Licencja",
"Willkommen"    : " Witamy w programie TextSTAT",


##### Toolbar #####
"KorpusNew2"    : "Utworz nowy korpus",
"KorpusOpen2"   : "Otworz istniejacy korpus",
"KorpusHinzu2"  : "Dodaj istniejacy korpus do otwartego korpusu",
"KorpusSave2"   : "Zapisz korpus",
"KorpusDel2"    : "Usun korpus",
"WebHinzu2"     : "Dodaj plik(i) HTML z internetu do korpusu",
"DateiHinzu2"   : "Dodaj plik(i) z dysku lokalnego do korpusu",
"DateiWeg2"     : "Usun plik(i) z korpusu",
"FreqZeigen"    : "Pokaz frekwencji slow",
"FreqZeigen2"   : "Pokaz frekwencji formy slow w korpusie",
"Kopieren"      : "Kopiuj do schowka",
"Kopieren2"     : "Kopiuj wybrane elementy do schowka",
"Ex2excel2"     : "Eksportuj liste frekwencyjna do MS Excel",
"Ex2word2"      : "Eksportuj liste konkordancji do MS Word",
"Info"          : "TextSTAT - informacje",
"Info2"         : "Informacje o programie",

##### Tabs #####
"KorpusTab"     : "Korpus", 
"FormenTab"     : "Formy slow",
"ConcTab"       : "Konkordancja",
"ZitatTab"      : "Cytat",

"KTinfo"        : "TextSTAT \n\nUtworz korpus i dodaj kilka plikow wzglednie otworz istniejacy korpus TextSTAT.",
"KTkopieren"    : "Kopiuj nazwa pliku",
"KTopen"        : "Otworz plik",
"KTdatinfo"     : "Informacje o pliku",
"KTdatweg"      : "Usun plik(i)",

"FTinfo"        : "FREKWENCJA \nformy slow (tokenow)",
"FTsortFreq"    : "sortuj wedlug frekwencji",
"FTsortAlpha"   : "sortuj alfabetycznie",
"FTsortRetro"   : "sortuj odwrotnie",
"FTminFreq"     : "frekwencja minimalna",
"FTmaxFreq"     : "frekwencja maksymalna",
"FTignGross"    : "ignoruj duze litery (A=a)",
"FTsuchFreq"    : "LUB szukaj frekwencja \nslow zawierajace ciag znakow:",
"FTfreqliste"   : "Lista frekwencyjna",
"FTwortform"    : "Forma slowa",
"FTfrequenz"    : "Frekwencja",
"FTkopieren"    : "Kopiuj",
"FTsuchKonk"    : "Szukaj konkordancje",
"FTzeigInfo"    : "Wiecej informacji",
"FTformInfo"    : "Informacje o korpusie\n\nKorpus zawiera %s slow (tokeny)\n\n%s rozne formy (typy, z duzymi literami) wzglednie \n%s typy (duze litery sa zignorowane)\n\nForma '%s' ukazuje sie %s razy (= %.2f %%)",
"FTOptionen"    : "Frekwencja / opcje",

"CTsuchen"      : "Szukaj",
"CTganzWort"    : "tylko calych wyrazow",
"CTignGross"    : "Ignoruj duze litery (A=a)",
"CTcontextL"    : "Kontekst po lewej stronie",
"CTcontextR"    : "Kontekst po prawej stronie",
"CTmarkSuch"    : "zaznac wyszukiwany ciag znakow",
"CTsortAlpha"   : "alfabetycznie",
"CTsortR"       : "sortuj kontekst z prawej strony",
"CTsortL"       : "sortuj kontekst z lewej strony",
"CTaktualisier" : "Odswiez",
"CTmachQuery"   : "Edytor zapytan",
"CTkopieren"    : "Kopiuj",
"CTzeigZitat"   : "Pokaz wiecej kontekstu",
"CTOptionen"    : "Opcje",

"ZTinfo"        : "Tekst cytatu",
"ZTkopieren"    : "Kopiuj",
"ZToeffnen"     : "Otworz plik",
"ZToeffnen2"    : "\n(Kliknij dwukrotnie, aby otworzyc plik)\n",
"ZTInit"        : "\n Kliknij dwukrotnie na dowolne zdanie w zakladce konkordancje, by zobaczyc to zdanie w rozszerzonym.",

##### Webdialog #####
"WDinfo"        : "Dodaj stron HTML bezposrednio z internetu.\nWprowadz adres URL:",
"WDfrageSeiten" : "Ile stron chcesz dodac do swojego korpusu?",
"WDanzahl"      : "Liczba stron do sciagania",
"WDinSubdir"    : "Szukaj w tym folderze",
"WDaufServer"   : "Szukaj na calym serwerze",
"WDok"          : "Szukaj",
"WDcancel"      : "Zamknij",

##### NewsDialog #####
"NDinfo"        : "Dodaj postow grupy dyskusyjnej do korpusu", 
"NDserver"      : "Serwer grup dyskusyjnych",  
"NDgruppe"      : "Grupa dyskusyjna", 
"NDanzahl"      : "Liczba postow", 
"NDzitWeg"      : "Usun cytaty z postow", 
"NDok"          : "Szukaj",
"NDcancel"      : "Zamknij",

##### QueryDialog #####
"QDinfo"        : "Pomoc w formulowaniu zlozonych pytan \nZnaki wieloznaczne (* i ?) sa dopuszczone",
"QDsuchFelder"  : "Terminy wyszukiwawcze",
"QAusdruck1"    : "Pierwszy termin wyszukiwawczy",
"QAusdruck2"    : "Drugi termin wyszukiwawczy",
"QAbstand"      : "Ilosc slow miedzy dwoma terminami",
"QAbstandMin"   : "minimalnie i   ",
"QAbstandMax"   : "maksymalnie",
"QmachAusdruck" : "Zacznij wyszukiwanie",
"QDcancel"      : "Zamknij",

##### Fehler #####
"Blad"         : "Blad",
"ErrKorpNew"    : "Nie udalo sie utworzyc pliku korpusowego!",
"ErrKorpOpen"   : "Nie udalo sie utworzyc pliku korpusowego!\nCzy to na pewno plik korpusu TextSTAT?",
"ErrKorpImp"    : "Nie udalo sie zaimportowac pliku korpusowego!!",
"ErrNoKorp"     : "Nalezy najpierw utworzyc nowy korpus (albo otworzyc istniejacy korpus).",
"ErrKorpSave"   : "Nie udalo sie zapisac pliku korpusowego!",
"ErrFileOpen"   : "Blad przy otwarciu pliku!",
"ErrSelect"     : "Nie wybrano nic!",
"ErrOptSave"    : "Nie udalo sie zapisac opcje!",
"ErrExport"     : "Blad eksportu",
"ErrNoFreqList" : "Lista frekwencyjna slow niedostepna",
"ErrNoKonkList" : "Lista konkordancji niedostepna",
"ErrExcel"      : "Blad MS Excel!\nZarowno rozszerzenie win32 w programie Pythona jak i MS Excel nalezy zainstalowac poprawnie.",
"ErrExcel65K"   : "Blad! MS Excel nie moze zawierac wiecej niz 65500 wierszy. \nIlosc wierszy to obecnie: %s.",
"ErrWord"       : "Blad MS Word!\nZarowno rozszerzenie win32 w programie Pythona jak i MS Excel nalezy zainstalowac poprawnie.",
"ErrExpFreq"    : "Blad przy zapisaniu listy frekwencyjnej slow.",
"ErrExpKonk"    : "Blad przy zapisaniu listy konkordancji.",
"ErrWeb"        : "Blad przy sciaganiu plikow HTML z internetu.",
"ErrNews"       : "Blad przy sciaganiu postow z grupy dyskusyjnej.",
"ErrQuery"      : "Brak terminu wyszukiwawczego.",

##### Mitteilungen und Fragen #####
"AskSure"       : "Czy jestes pewny?",
"AskDelete"     : "Czy naprawde chcesz usunac %s ?",
"StatusWeb"     : "ladowanie %s plikow z internetu...",
"StatusNews"    : "ladowanie %s posty z grupy dyskusyjnej...",
"DateiInfo"     : "Informacje o pliku",
"DateiInfo2"    : "Plik: %s\n\nsciezka: %s\n\nRozmiar: %s\n",
"InfoText"      : "TextSTAT\
\nPo utworzenia (lub otworzenia) korpusu, mozna dodawac nastepne pliki \
lub mozna tez usunac kilka plikow. Zaznac jakies plik i kliknij prawym przyciskiem myszy \
dla dalszych opcji. Mozna rowniez zapisac korpus w calosci. \
\nAnaliza tekstu mozna dokonac w roznych sposobach: \
\npod 'Formy slowa' znajdziesz \
frekwencje wystapien pojedynczych form slow. 'Konkordancja' pokazuje konkordancje typu KWIC \
(slowo kluczowe w kontekscie), 'Cytat' zaprezentuje wiecej kontekstu.",

"StatusKorpImp" : "Korpus zaimportowany pomyslnie.",
"StatusKorpSave": "Korpus zostal zapisany.",
"StatusFiles"   : " %s plikow | %s bajtow ",
"StatusKorpDat" : " Korpus: %s  (ustawienia kodowania pliku: %s)",
"KodierungInfo" :  "Teraz mozna dodac plikow do korpusu.\n\nDokumenty programu Word:\n- Pliki programu MS Word ('.doc' lub '.docx'\n- Pliki w formacie OpenOffice ('.odt' lub '.sxw')\n\nPliki tekstowe:\n- pliki tekstowe ASCII/ANSI (czesto '.txt')\n- Pliki HTML (z twardego dysku lub bezposrednio z internetu)\n- Posty grupy dyskusyjnej (bezposrednio z internetu)\nPrzy dodawanie plikow tekstowych, wybieraj poprawny \nkodowanie plikow.\nObecny kodowanie plikow to: %s",
"StatusFormen"  : " %s formy slow/typy (%s slow/tokenow w korpusie)",
"StatusNoKorp"  : " nie ma korpusu ",
"StatusSuchKonk": "Szukaj konkordancje...",
"StatusTreffer" : " %s wynikow ",
"StatusExpFreq" : "Lista frekwencyjna zostal wyeksportowane.",
"StatusExpKonk" : "Lista konkordancji zostal wyeksportowane",
"ExportHinweis" : "Należy używać czcionki nieproporcjonalnej (jak np. Courier New)",
"StatusClip"    : "Wybrany tekst został skopiowany do schowka",
"SpracheAendern": "Zmień język",
"SpracheInfo"   : "Aby aktywować zmiana języka, należy ponownie uruchomić TextSTAT.",
"ProgBeenden"   : "Zakończ TextSTAT",
"AbfrageEnde"   : "Korpus się zmienił. \nChcesz zapisać zmiany \nprzed zamknięciem TextSTAT?",
"AbfrageSave"   : "Korpus się zmienił. \nChcesz zapisać zmiany w korpusu \nzanim otworzysz nowy korpus?",
}

