#!/bin/bash

###################################################
# VOUS N'AVEZ PAS LE DROIT DE MODIFIER CE FICHIER #
###################################################

#
# compresseur
#

COMPRESS=gzip
OPTION_TAR=z
EXTENSION=gz

#COMPRESS=bzip2
#OPTION_TAR=j
#EXTENSION=bz2


#
# nombre d'arguments
#
if [ $# -eq 0 -o $# -gt 2 ]
then
    echo "[ERR]  nombre d'arguments incorrect"
    echo "       usage : " $0 "nom1 [nom2]"
    exit
fi


#
# ordre lexicographique pour les noms des auteurs (si binôme)
#
if [ $# -eq 2 ]
then
    if [[ "$1" > "$2" ]]
    then
	echo "[ERR]  les noms ne sont pas dans l'ordre alphabétique"
	exit
    fi
    filename=$1_$2_projet.tar.${EXTENSION}
else
    filename=$1_projet.tar.${EXTENSION}
fi



echo Analyse du fichier $filename

#
# existence du fichier
#
if [ ! -f $filename ]
then
    echo "[ERR]  fichier inexistant"
    exit
else
    echo "[ok]   fichier présent"
fi

#
# accès en lecture
#
if [ ! -r $filename ]
then
    echo "[ERR]  fichier non en lecture"
    exit
else
    echo "[ok]   fichier avec accès en lecture"
fi

#
# taille correcte ?
#
taille=`stat -L -c %s $filename`
if [ $taille -eq 0 ]
then
    echo "[ERR]  l'archive est vide"
    exit
else
    echo "[ok]   fichier non vide"
fi

if [ $taille -lt 10240 ]
then
    echo "[Warn] l'archive fait moins de 10 Ko, est-ce normal ?"
fi
if [ $taille -gt 1048576 ]
then
    echo "[Warn] l'archive dépasse 1 Mo, n'avez-vous pas laissé des fichiers inutiles ?"
fi

#
# compressé avec le bon archiveur-compresseur ?
#
let LG=16+${#COMPRESS}
type=`file -Lb $filename | cut -c -${LG}`
type_attendu="${COMPRESS} compressed data"
if [[ $type != $type_attendu ]]
then
    echo "[ERR]  le fichier n'a pas le bon type"
    echo "       voici le type de l'objet :"
    echo -n "       "
    file $filename
    exit
else
    echo "[ok]   compression fichier (${COMPRESS})"
fi

#
# est-ce bien une archive .tar ?
#
tar ${OPTION_TAR}vtf $filename 2> /dev/null > /dev/null
ok=$?
if [ $ok -ne 0 ]
then
    echo "[ERR]  lecture archive"
    exit
else
    echo "[ok]   lecture archive"
fi


#
# répertoire PROJET ?
#
reponse=`tar ${OPTION_TAR}vtf $filename --no-recursion PROJET 2> /dev/null`
if [ ! "$reponse" ]
then
    echo "[ERR]  pas d'entrée PROJET dans l'archive"
    exit
fi

if [ `echo $reponse | cut -c -1` != 'd' ]
then
    echo "[ERR]  l'entrée PROJET n'est pas un répertoire"
    exit
else
    echo "[ok]   répertoire PROJET"
fi

#
# rapport
#
reponse=`tar ${OPTION_TAR}vtf $filename --no-recursion PROJET/rapport.pdf 2> /dev/null`
if [ ! "$reponse" ]
then
    echo "[ERR]  pas de fichier PROJET/rapport.pdf dans l'archive"
    exit
else
    echo "[ok]   fichier PROJET/rapport.pdf"
fi

#
# code
#
REPS="PROJET/src"
for rep in $REPS
do
    reponse=`tar ${OPTION_TAR}vtf $filename --no-recursion $rep 2> /dev/null`
    if [ ! "$reponse" ]
    then
	echo "[ERR]  pas d'entrée $rep dans l'archive"
	exit
    else
	echo "[ok]   entrée $rep"
    fi

    if [ `echo $reponse | cut -c -1` != 'd' ]
    then
	echo "[ERR]  l'entrée $rep n'est pas un répertoire"
	exit
    else
	echo "[ok]   répertoire $rep"
    fi
done

# des warnigs pour ces répertoires
REPS="PROJET/src/.git"
for rep in $REPS
do
    reponse=`tar ${OPTION_TAR}vtf $filename --no-recursion $rep 2> /dev/null`
    if [ ! "$reponse" ]
    then
	echo "[Warn] pas d'entrée $rep dans l'archive"
    else
	echo "[ok]   entrée $rep"
	if [ `echo $reponse | cut -c -1` != 'd' ]
	then
	    echo "[Warn] l'entrée $rep n'est pas un répertoire"
	else
	    echo "[ok]   répertoire $rep"
	fi
    fi
done

echo "Analyse effectuée avec succès"
