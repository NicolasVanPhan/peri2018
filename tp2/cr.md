
Dand ce TP2, nous allons de nouveau implementer un programme qui fait clignoter
deux LEDs jusqu'a l'appui sur un bouton, sauf que cette fois, nous creerons
un module noyau qui sera utilise par un script utilisateur pour faire
clignoter les leds.

Detail sur ce qu'est un module
==============================

un module est une partie du noyau deja compilee et qui peut etre chargee
dynamiquement en pleine execution du noyau, a la volee.
Avant Linux 2.6, les modules avaient pour extention .o par convention,
maintenant ils prennent par convention l'extension .ko (kernel object)
qui permet plus facilement de les distinguer des fichiers objets classiques.
En effet, en plus des fichiers objets classiques, les .ko contiennent
une section speciale ".modinfo" contenant des metadonnees sur le module.


Schema par couches du sujet de TP
=================================

Programme utilisateur

interface 1 : /dev/ledbp   read() write()

Module driver

interface 2 : Espace adressage GPIO

Materiel (ports GPIO connectes aux LEDs et au bouton)

Ici, nous allons creer un module contenant un driver permettant a l'user
d'allumer des leds et de lire un bouton en ecrivant/lisant dans un fichier
special. Pour allumer/eteindre les LEDS, l'utilisateur ecrira deux char
dans le fichier special /dev/ledbp et pour savoir si le bouton est appuye,
il lira un caractere depuis /dev/ledbp.

Difference en /dev/ledbp est un vrai fichier
============================================

Quand on lit/ecrit dans /dev/ledbp, on ecrit pas reellement dans un endroit
du disque, en realite on envoie/recoit un pointeur vers un tableau de char,
et derriere le module fait ce qu'il veut avec.
La convention utilisee pour quelle chaine ecrire pour quelle commmande
est completement arbitraire, c'est dans le module qu'est definit cela,
mais c'est mieux pour l'user de choisir une convention simple.


Composantes d'un module :
=========================

Un module minimal se compose de deux choses :
 * Fonction d'initialisation, appelee a chaque fois que le module est charge.
 * Fonction de terminaison, appelee quand le module est decharge.

On the Rasp 3 (with raspbian already installed in the SD card)


Creation et test d'un module noyau :
====================================

Nous partons d'un fichier source C,
contrairement a un programme quelconque, ce fichier source
contient les elements necessaires pour etre un module noyau,
c'est a dire une macro MODULE, une fonction d'initialisation
et de cleanup qui seront appelees au chargement/dechargement du module.

Nous compilons ce fichier qui deviendra non pas un .o mais un .ko,
comme detaille dans la partie 1. Pour compiler un module noyau, il est
necessaire de fournir les sources du noyau pour compiler un nouveau module
(ce nouveau module utilise typiquement des librairies .h provenant d'autres
morceaux de noyau, d'ou la necessite d'avoir les sources du noyau)

Creation et test d'un module noyau sur Raspberry Pi 3
=====================================================

Ayant reussi a compiler et charger un module kernel sur Raspberry Pi 3
depuis cette meme Raspberry Pi 3, nous avons trouve utile de detailler
les problemes rencontres et solutions apportees :

Nous compilons depuis la Raspberry 3, donc il n'y a pas de cross-compilation,
c'est de la compilation simple : il suffit d'executer le makefile suivant :

obj-m += mymodule.o
all :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

Or le make echouera car il ne trouvera pas le dossier /lib/modules/build
Ceci est du au fait que les sources du noyau raspbian ne sont pas incluses
par default, une maniere de les obtenir est en executant rpi-sources
(mettre a jour le firmware avec rpi-update avant)
En effet, nos kernel modules sont susceptibles d'inclure les headers d'autres
modules, donc il faut necessairement les sources du noyau pour compiler de
nouveaux modules.
Apres cela, il suffit d'appeler make, puis d'executer insmode mymodule.ko

N.B. NE PAS appeler son module "module.c" ou "kernel.c", ces noms generiques
posent des problemes au chargement du module, (erreur "Invalid arguments")

