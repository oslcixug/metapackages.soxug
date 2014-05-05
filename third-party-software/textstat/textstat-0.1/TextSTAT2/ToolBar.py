#!/usr/bin/python
# -*- coding: utf-8 -*-


from Tkinter import *

# Enthaelt 2 Klassen: ToolBar und ToolTip

class ToolBar( Frame ):
    "Repraesentiert eine Werkzeugleiste"

    def __init__( self, parent, statusbar = None ):
        "Konstruieren und Initialisieren"
        Frame.__init__( self, parent )
        self.config( relief=RIDGE, bd=1 )
        self.statusBar = statusbar
        self.afterID = 0
        self.balloonhelp = None
        self.tools= {}

    def add( self, callbackname, text, icon, command, help = None ):
        "Ein Werkzeug hinzufuegen"
        img = PhotoImage( file=icon )
        tool = Button( self, image=img, relief=FLAT, bd=2, command=command )
        tool.config( height=20, width=20 )
        tool.pack( side=LEFT, padx=1, pady=2 )
        tool.bind( "<Enter>", lambda ev,self=self,tt=text,ht=help:self.on_enter(ev,tt,ht) )
        tool.bind( "<Leave>", self.on_leave )
        self.tools[callbackname] = tool, img

    def add_separator( self ):
        "Einen Separator hinzufuegen"
        separator = Frame( self, relief=GROOVE, bd=2, width=2 )
        separator.pack( side=LEFT, fill=Y, padx=8, pady=2 )

    def set_statusbar( self, statusbar ):
        "Die Statusleiste setzen"
        self.statusBar = statusbar

    def on_after( self, text, x, y ):
        self.balloonhelp = ToolTip( text, x, y )

    def on_enter( self, event, tooltext, helptext ):
        "Mauscursor auf Werkzeug"
        event.widget["relief"] = RAISED
        if self.statusBar:
            self.statusBar.set( helptext )
        self.afterID = self.after( 500, self.on_after, tooltext, event.x_root, event.y_root )

    def on_leave( self, event ):
        "Mauscursor verlaesst Werkzeug"
        event.widget["relief"] = FLAT
        if self.statusBar:
            self.statusBar.set()
        if self.balloonhelp:
            self.balloonhelp.destroy()
            self.balloonhelp = None
        if self.afterID: self.after_cancel( self.afterID )

    def set_enabled( self, tool, state ):
        if state == 0:
            self.tools[tool][0]["state"] = DISABLED
        else:
            self.tools[tool][0]["state"] = NORMAL



class ToolTip( Toplevel ):
    "Repraesentiert einen Tool Tip"
    
    def __init__( self, text, x, y ):
        "Konstruieren, initialisieren und anzeigen"
        Toplevel.__init__( self )
        self.overrideredirect( 1 )
        self.geometry( "+%d+%d" % (x-15,y+15) )
        label = Label( self, text=text, fg="#000000", bg="#ffffaa",bd=2,relief=RAISED )
        label.pack( fill=BOTH, expand=YES)