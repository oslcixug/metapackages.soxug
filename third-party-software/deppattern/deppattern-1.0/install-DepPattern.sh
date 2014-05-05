#!/bin/sh


for file in tree-tagger/cmd/*
do
    awk '$0=="BIN=./bin"{print "BIN='`pwd`'/tree-tagger/bin";next}\
         $0=="CMD=./cmd"{print "CMD='`pwd`'/tree-tagger/cmd";next}\
         $0=="LIB=./lib"{print "LIB='`pwd`'/tree-tagger/lib";next}\
         {print}' $file > $file.tmp;
    mv $file.tmp $file;
done

for file in dp.sh*
do
    awk '$0=="TREETAGGER=\"./tree-tagger\""{print "TREETAGGER=\"'`pwd`'/tree-tagger/cmd/tree-tagger\"";next}\
         $0=="DEPPATTERN_DIR=\"./DepPattern\""{print "DEPPATTERN_DIR=\"'`pwd`'\"";next}\
        {print}' $file > $file.tmp;
    mv $file.tmp $file;
done


echo 'Path variables modified in tagging scripts.'

chmod 0755 dp.sh
chmod 0755 scripts/*
chmod 0755 tree-tagger/cmd/*


echo ''
echo 'You should add  '`pwd`', '`pwd`'/cmd and '`pwd`'/bin to the command search path.'
echo ''

echo "Don't forget that you can also install and use Freeling."
echo "If you use Freeling, it should be installed in /usr/local/."
