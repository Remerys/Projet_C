#!/bin/bash

nb=10
min=95
max=105    # non inclus

echo '== insertion des '$nb' valeur(s), interval ['$min','$max'['
./client insertmany $nb $min $max
./client insert 100
./client insert 100
echo

echo "== tests d'existence"
let deb=min-1
let fin=max+1
for i in `seq $deb $fin`
do
    ./client exist $i
done
echo

echo "== affichage"
./client print
echo "== cardinalité"
./client howmany
echo "== min et mas"
./client min
./client max
echo "== somme"
./client sum
echo "== stop"
./client stop
echo

loc_nbThreads=3
loc_elt=5
loc_nb=20
loc_min=0
loc_max=10 # non inclus
echo '== calcul multi-thread (nbThreads='$loc_nbThreads', elt='$loc_elt', nb='$loc_nb', intervalle=['$loc_min','$loc_max'[)'
./client local $loc_nbThreads $loc_elt $loc_nb $loc_min $loc_max
