
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
d'abord indiquer au noyau qu'il s'agit des fonctions de gestion du fichier
d'interface en donnant leurs adresses dans une struture `struct file_operations`
que nous appelerons `fops_ledbp`.
Ensuite nous créerons un nouveau device correspondant a l'interface grace
a `register_chrdev()` a laquelle nous donnerons `fops_ledbp` :
c'est comme ca que le noyau sait qu'il faudra appeler `open_ledbp()`,
`read_ledbp()`, etc lorsqu'un utilisateur effectue des opérations sur le device.

Compilation
-----------

Ce code C peut ensuite être compilé et chargé dans le noyau.
Il ne reste plus qu'à creer un point d'accès au device
(qui sera sous la forme de fichier) avec `mknod`.

Pour vérifier que le device a bien été crée, il suffit d'aller voir dans
le répertoire des devices : `ls /dev/ | grep ledbp` doit retourner
quelque chose, si cela ne retourne rien alors il y a eu une erreur.

Utilisation du device en tant qu'utilisateur
--------------------------------------------

Une fois notre périphérique de gestion LED & boutons accessible via
`/etc/devices/ledbp`, nous allons effecter une écriture et une lecture pour
vérifier le bon fonctionnement du module.

#### Test en écriture

La commande `echo "rien" > /dev/ledbp` écrit la chaine "rien" dans notre device.
Si tout s'est bien passé, le device a normalement été ouvert, a subi une
écriture, puis a été refermé. Et en effet, `dmesg | tail` affiche bien
trois messages `"open()"`, `"write()"` et `"close()"`, issu des `printk()`
des fonctions du module, témoignant que ces fonctions ont bien été appelées
dans et ordre.

#### Avant de passer à la lecture, un aparte sur `dd`:
`dd` est une commande permettant de faire des copies brutes de données d'un
fichier ou disque à un autre, il prend comme principaux arguments un fichier
`if=fichier_source`, `of=fichier_destination` et `count` le nombre de blocs à
lire, et `bs` pour block size, la taille d'un bloc en octets.

Ici `of` n'est pas spécifié, il s'agira donc de la sortie standard (le terminal)
et `if` est spécifié via le symbole de redirection `<`. Enfin, comme `bs` et
`count` valent 1, le nombre d'octet lu est 1 fois 1 = 1.

#### Test en lecture 
Il s'agit donc d'une commande qui lit un octet de /dev/ledbp et l'écrit sur la
sortie standard. Et effectivement, `dmesg` affiche bien un open, un read et
un close.

Etape 4 : Accès aux GPIO depuis les fonctions du pilote
=======================================================

Création du driver qui accède aux GPIO
--------------------------------------

En s'inspirant des macros d'accès GPIO du TME1 et des fonctions `gpio_fsel()`
et `gpio_write()`, nous avons vu que `GPIO_CONF_REG(0x1c)` correspond au champ
d'initiialisation et équivaut à `gpio_regs->gpfsel[]`, nous en avons déduit
que `GPIO_CONF_REG(0x34)` utilisée pour la macro de lecture, équivaut à
`gpio_regs->gplev[]`, nous avons donc effectué l'opération binaire de la macro
de lecture sur `gpio_regs->gplev[]`.

```C
static int gpio_read(int pin)
{
	return ((gpio_regs->gplev[pin / 32]) >> (pin % 32)) & 0x1;
}
```

Nous avons choisi une convention simple pour le controle des LEDS et du bouton:
L'utilisateur doit écrire deux caractères dans `/dev/ledbp` pour les LEDS,
le caractère `0` éteint la LED et un autre caractère l'allume.
Puis la valeur du bouton s'obtient en lisant un octet de `/dev/ledbp`,
`0` veut dire que le bouton est enfoncé et `1` relaché (ou `2` en cas d'erreur).

```C
static ssize_t 
read_ledbp(struct file *file, char *buf, size_t count, loff_t *ppos) {
	int read_value;
	printk(KERN_DEBUG "read()\n");

	if (count > 0) {
		read_value = gpio_read(gpio_btn[0]);
		if (read_value == 1) {
			buf[0] = '1';
		}
		else if (read_value == 0) {
			buf[0] = '0';
		}
		else {
			buf[0] = '2';
			printk(KERN_DEBUG "GPIO read %d\n", read_value);
		}
	}
	return count;
}
```
```C
static ssize_t 
write_ledbp(struct file *file, const char *buf, size_t count, loff_t *ppos) {
	printk(KERN_DEBUG "write()\n");

	if (count > 0) {  		/* If user wrote something */
		printk(KERN_DEBUG "char : %c\n", buf[0]);
		if (buf[0] == '0')	/* if first char is 0, turn off LED0*/
			gpio_write(gpio_led[0], 0);
		else		  	/* Otherwise turn on LED0 */
			gpio_write(gpio_led[0], 1);
	}
	return count;
}
```

Enfin, nous avons testé le fonctionnement du module avec un script qui
allume la LED quand le bouton est appuyé, le script fait une simple redirection
de la valeur du bouton vers la LED, en boucle.

```bash
#!/bin/bash

while true
do
	val=`dd bs=1 count=1 < /dev/ledbp`;
	if [ $val -eq '1' ]
	then
		echo '1' > /dev/ledbp;
	else
		echo '0' > /dev/ledbp;
	fi
done
```


Tentative de migration vers la Raspberry Pi 3
=====================================================

Ayant essayé de compiler et charger un module kernel sur Raspberry Pi 3
depuis cette même Raspberry Pi 3, nous avons trouvé intéressant de détailler
les problèmes rencontrés et solutions apportées :

Nous compilons depuis la Raspberry 3, donc il n'y a pas de cross-compilation,
c'est de la compilation simple : il suffit d'exécuter le makefile suivant :

```
obj-m += mymodule.o
all :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

Or le make échouera car il ne trouvera pas le dossier `/lib/modules/build`.
Ceci est dû au fait que les sources du noyau raspbian ne sont pas incluses
par défaut, une manière de les obtenir est en exécutant `rpi-source`
(mettre à jour le firmware avec `rpi-update` avant)
Nos kernel modules sont susceptibles d'inclure les headers d'autres
modules, donc il faut nécéssairement les sources du noyau pour compiler de
nouveaux modules.
Apres cela, il suffit d'appeler `make`, puis de charger le module. 

A l'état actuel, le noyau se charge sans erreurs mais une écritre dans
`/dev/ledbp` entraine un Kernel Oops, le samedi ne fut donc pas très fructueux.

*N.B. NE PAS appeler son module "module.c" ou "kernel.c", ces noms generiques
posent des problemes au chargement du module, (erreur "Invalid arguments")*

