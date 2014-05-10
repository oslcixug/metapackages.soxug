#!/bin/bash

#***********************************************************************
#Script para actualizar o repositorio cada vez que se engade un paquete*
#                                                                     **
#NOTA: Lanzar como root (non sudo)            			********
#AUTOR: Marcos Fernandez Lopez para a Oficina de SW Libre do CIXUG *****
#***********************************************************************


##########################################
#Creacion das listas de paquetes Packages
##########################################

echo "Creando listas de paquetes ............."

apt-ftparchive packages pool/main/ >/home/packages.cixug.es/testing/dists/precise/main/binary-amd64/Packages

gzip < /home/packages.cixug.es/testing/dists/precise/main/binary-amd64/Packages > /home/packages.cixug.es/testing/dists/precise/main/binary-amd64/Packages.gz

apt-ftparchive packages pool/main/ >/home/packages.cixug.es/testing/dists/precise/main/binary-i386/Packages

gzip < /home/packages.cixug.es/testing/dists/precise/main/binary-i386/Packages > /home/packages.cixug.es/testing/dists/precise/main/binary-i386/Packages.gz

##########################################
#Creacion do arquivo coas sumas de control 
#das listas de paquetes e de fontes
##########################################

echo "Creando suma de control ............."
sleep 1

apt-ftparchive -c /etc/archive.config release /home/packages.cixug.es/testing/dists/precise/ > /home/packages.cixug.es/testing/dists/precise/Release

###########################################
#Firma GPG do arquivo coas sumas de control
###########################################

echo "Firmando o arquivo de sumas ..........." 
sleep 1

gpg --output /home/packages.cixug.es/testing/dists/precise/Release.gpg -ba /home/packages.cixug.es/testing/dists/precise/Release

echo "Repositorio actualizado correctamente"
