#!/usr/bin/python
# -*- coding: utf-8 -*-

from Tkinter import *

class StatusBar(Frame):
    def __init__(self, parent, ProgressBar=1):
        Frame.__init__(self, parent)
        
        # Zweitgeteilt: links der Statustext, rechts der Inikatortext
        # und dann evtl. noch der ProgressBar
        self.status1 = Label(self, bd=1, relief=SUNKEN, anchor=W)
        self.status1.pack(side=LEFT, fill=X, expand=YES, padx=2, pady=1)

        if ProgressBar==1:
            import ProgressBar
            self.progress = ProgressBar.ProgressBar(self, bd=1)
            self.progress.updateProgress(0)
            self.progress.frame.pack(side=RIGHT, padx=2, pady=1)

        self.status2 = Label(self, bd=1, relief=SUNKEN, anchor=E)
        self.status2.pack(side=RIGHT, padx=2, pady=1)

     
    def set(self, text=''):
        self.status1.config(text=text)
        self.status1.update_idletasks()
        
    def indicator(self, text):
        self.status2.config(text=text)
        self.status2.update_idletasks()
