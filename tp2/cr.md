
Contexte et objectif
====================

Dand ce TP2, nous allons de nouveau implémenter un programme qui fait clignoter
deux LEDs jusqu'à l'appui sur un bouton, sauf que cette fois, nous créerons
un module noyau qui sera utilisé par un script utilisateur pour faire
clignoter les LEDs.

Détail sur ce qu'est un module
------------------------------

Un module est une partie du noyau déjà compileé et qui peut etre chargée
dynamiquement en pleine exécution du noyau, a la volée.
Avant Linux 2.6, les modules avaient pour extention .o,
maintenant ils prennent par convention l'extension .ko (kernel object)
qui permet plus facilement de les distinguer des fichiers objets classiques.
En effet, en plus des fichiers objets classiques, les .ko contiennent
une section spéciale ".modinfo" contenant des métadonnees sur le module.

Schema par couches du sujet de TP
---------------------------------

| Programme utilisateur				|
| --------------------------------------------- |
| interface 1 : /dev/ledbp   read() write()	|
| Kernel module contenant le driver des LEDS	|
| interface 2 : Espace adressage GPIO		|
| Materiel (ports GPIO : LEDs et bouton)	| 

Ici, nous allons créer un module contenant un driver permettant à l'user
d'allumer des leds et de lire un bouton en écrivant/lisant dans un fichier
spécial. Pour allumer/éteindre les LEDS, l'utilisateur écrira deux char
dans le fichier spécial /dev/ledbp et pour savoir si le bouton est appuyé,
il lira un caractère depuis `/dev/ledbp`.

Différence en /dev/ledbp est un "vrai" fichier
----------------------------------------------

Quand on lit/écrit dans `/dev/ledbp`, on écrit pas réellement dans un endroit
du disque, en réalité on envoie/recoit un pointeur vers un tableau de char,
et ensuite le module fait ce qu'il veut avec.
La convention utilisée (quelle chaine ecrire pour quelle commmande)
est donc complètement arbitraire, c'est dans le module qu'est défini cela,
mais c'est mieux pour l'user de choisir une convention simple.

Etape 1 : Création et test d'un module noyau
============================================

Code du module
--------------

Nous partons d'un fichier source C,
contrairement à un programme quelconque, ce fichier source
contient les éléments nécessaires pour être un module noyau, c'est à dire :
 * une macro `MODULE`
 * une fonction d'initialisation
 * une fonction de cleanup 
 * pas de fonction `main()`

Ici, les fonctions `module_init()` et `module_exit()` prennent en argument
des pointeurs de fonctions et servent à indiquer au noyau que les fonctions
de chargement/déchargement de ce module sont respectivement `mon_module_init()`
et `mon_module_cleanup()`.

Compilation du module
---------------------

Nous compilons ce fichier qui deviendra non pas un `.o` mais un `.ko`,
(convention de nommage existante depuis Linux 2.6 et facilitant la distinction
entre un object file classique et un kernel module)
Pour compiler un module noyau, il est nécessaire de fournir les sources du
noyau en question (ce nouveau module utilise typiquement des librairies `.h`
provenant d'autres morceaux de noyau, donc il faut que ces `.h` soit presents). 
Ces sources sont dans le dossier indique par `KERNEDIR`.

Lorsque nous lancons la commmande `sudo insmod ./module.ko`, a priori
rien n'est retourné, nous ne voyons pas le chargement de notre module,
mais un appel a `lsmod` renvoie la liste des modules charges, et `module` y est
bien présent. De plus, le chargement du module entraine l'appel de la fonction
`mon_module_init()` qui est censé afficher dans les messages noyau un
`Hello World!` et lorsque nous exécutons `dmesg | tail`, nous avons bien
ce message.

De la même manière, `sudo rmmod module` décharge le module,
`lsmod | grep module` ne renvoie rien et `dmesg | tail` affiche dans les
derniers message `"Goodbye World!"`, ce qui confirme le déchargement du module.

Etape 2 : Ajout des parametres au module 
========================================

En ajoutant des paramètres, notre module devient plus souple car on peut
configurer son comportement en le chargeant, ainsi on peut préciser ici
le nombre de LEDs que le module devra allumer.

Pour voir qu'un paramètre à bien été lu, il suffit d'afficher cette valeur
au chargement avec `printk()` et de comparer le nombre donné au chargement
avec le nombre lu dans `dmesg | grep tail`

Etape 3 : Création d'un driver qui ne fait rien, mais qui le fait dans le noyau
===============================================================================

Création du driver
------------------

Nous copions notre module vide `module.c` en `ledbp.c` mais allons y mettre
quelque chose d'utile cette fois, c'est à dire les fonctions :
 * `open_ledbp()`
 * `read_ledbp()`
 * `write_ledbp()`
 * `release_ledbp()`

Ces fonctions permettrons à l'utilisateur de controller les LEDS via une
interface qui se présente *sous forme de fichier*.

Pour faire le lien entre ces fonctions et le fichier d'interface, il faut
d'abord indiquer qu'il s'agit des fonctions de gestion du fichier d'interface
en donnant leur adresse dans une struture `struct file_operations` que nous
appelerons `fops_ledbp`.
Ensuite nous créer un nouveau device correspondant a l'interface grace
a `register_chrdev()` a laquelle nous donnerons `fops_ledbp` :
c'est comme ca que le noyau sait qu'il faudra appeler `open_ledbp()`,
`read_ledbp()`, etc.

Compilation
-----------

Ce code C peut ensuite être compilé et chargé dans le noyau.
Il ne reste plus qu'à creer un point d'acèes au device
(qui sera sous la forme de fichier) avec `mknod`.

Pour vérifier que le device a bien été crée, il suffit d'aller voir dans
le répertoire des devices : `ls /etc/devices | grep ledbp` doit retourner
quelque chose, si cela ne retourne rien alors il y a eu une erreur.

Etape 4 : Accès aux GPIO depuis les fonctions du pilote
=======================================================



Création et test d'un module noyau sur Raspberry Pi 3
=====================================================

Ayant essayé de compiler et charger un module kernel sur Raspberry Pi 3
depuis cette même Raspberry Pi 3, nous avons trouvé utile de détailler
les problèmes rencontrés et solutions apportées :

Nous compilons depuis la Raspberry 3, donc il n'y a pas de cross-compilation,
c'est de la compilation simple : il suffit d'exécuter le makefile suivant :

obj-m += mymodule.o
all :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

Or le make échouera car il ne trouvera pas le dossier /lib/modules/build
Ceci est dû au fait que les sources du noyau raspbian ne sont pas incluses
par défaut, une manière de les obtenir est en exécutant rpi-sources
(mettre à jour le firmware avec rpi-update avant)
En effet, nos kernel modules sont susceptibles d'inclure les headers d'autres
modules, donc il faut nécéssairement les sources du noyau pour compiler de
nouveaux modules.
Apres cela, il suffit d'appeler make, puis d'exécuter insmode mymodule.ko

N.B. NE PAS appeler son module "module.c" ou "kernel.c", ces noms generiques
posent des problemes au chargement du module, (erreur "Invalid arguments")

