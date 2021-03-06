
COMMUNICATION PAR FIFO
----------------------

Les tubes (pipes) sont un mécanisme de communication inter-processus (IPC)
suivant une politique first-in first-out (FIFO) et permettant à différents
processus de s'échanger des données.
Selon la norme POSIX il existe deux types types de tubes : les anonymes et les
nommés.

Les tubes anonymes permettent la communication entre processus liés par un
lien père/fils, et ne sont présents qu'en RAM, ils ne prennent donc pas de place
sur le disque.

A l'inverse, les tubes nommés prennent la forme d'un fichier (présence sur
disque donc) et permettent une communication entre n'importe quels processus.

### Le writer

#### Dans quel répertoire est créée la fifo ?

Ici, la fifo est crée dans le répertoire `/tmp/`.

#### Quelle différence entre mkfifo et open ?

Ici, nous utiliserons un tube nommé pour faire communiquer le programme Py et
le programme C.
`mkfifo()` sert à créer ce tube (créer le fichier spécial de type tube nommé)
et `open()` servira à ouvrir le tube (en écriture pour l'émetteur, en lecture
pour le récepteur).
Comme le tube s'utilise comme un fichier, il est normal qu'on l'ouvre avec
la commande qui sert classiquement à ouvrir un fichier (aka `open()`).

#### Pourquoi tester si la fifo existe ?

Le writer crée un pipe et écrit dedans, puis il s'arrête. Ensuite, le reader
lira le contenu du pipe, mais le writer ne sait pas quand est-ce que le
reader va lire.

Imaginons que le writer soit appelé deux fois avant que le reader soit exécuté,
si le writer crée à chaque exécution un nouveau pipe, alors au deuxième appel
il recréera le pipe et supprimera donc son ancien contenu alors qu'il n'a pas
été lu par le reader. C'est pour cela que le writer ne crée le pipe que s'il
n'existe pas déjà.

#### A quoi sert flush ?

Lorsque le writer exécute la fonction `write()`, les données à écrire sont
écrites non pas dans le disque mais dans un buffer, et le reader ne lit que ce
qu'il y a sur le disque donc il ne voit pas les nouvelles données fraichement
écrites. Il faudra attendre que le buffer soit plein pour que ce qu'il y a
dedans soit enfin écrit sur le disque et soit donc disponible pour le reader,
ce qui est inutiliement long. Par conséquent, dès lors que le writer écrit,
il flush le buffer (il répercute cette écriture sur le disque) pour qu'elle soit
aussitot disponible par le reader.

#### Pourquoi ne ferme-t-on pas la fifo ?

### Le reader


#### Que fait `readline()`
La fonction  `readline()` lit une ligne de texte (donc jusqu'à un '\n')
depuis la FIFO et renvoie cette ligne, qui sera ensuite stockée dans `str`.

#### Le writer ne fait rien tant que le reader n'est pas appelé, expliquez

D'après le man, si on ouvre une FIFO sans donner l'option `O_NONBLOCK`,
alors un writer ouvrant une fifo en écriture bloquera tant qu'un reader ne
l'aura pas ouvert en lecture.
"An open() for writing only will block the calling thread until a thread opens
the file for reading.". C'est pour cela que qu'il ne se passe rien tant que
le reader n'est pas appelé, en réalité le writer bloque sur open().


CREATION D'UN SERVEUR FAKE
--------------------------

La fonction `select()` permet de surveiller si des évènements se produisent sur
des fichiers ouverts, et l'intérêt est qu'elle peut surveiller plusieurs
fichiers en même temps.

Pour l'utiliser, il faut passer par des structures du
type `fd_set` qui sont des variables pouvant contenir plusieurs descripteurs
de fichiers. `select()` demande 3 `fd_set` :
	- un contenant tous les fd des fichiers ouverts en lecture
	- un deuxième `fd_set` pour les fichiers en écriture et
	- un troisième `fd_set` pour les exceptions.
Ici tous les fichiers qu'on veut surveiller sont des fichiers qu'on lit
(le pipe côté récepteur et l'entrée standard) donc nous placerons leurs fd
dans le premier `fd_set`.

La fonction FD_SET réalise justement cela, elle ajoute un fd à un `fd_set`,
ici nous avons deux fichiers en lecture d'où :

```C
FD_ZERO(&rfds);                                     // erase all flags
FD_SET(s2f, &rfds);                                 // wait for s2f
FD_SET(STDIN_FILENO, &rfds);                        // wait for stdin
```
Le `FD_ZERO()` sert à réinitialiser le `fd_set`, elle enlève tous les fd qui
y étaient présent.

Lorsque fake recoit un nombre depuis l'entrée standard, il le stocke dans
une variable (ici le buffer `buff[]`), d'où :

```C
// If there's something coming from the standard input
if (FD_ISSET(STDIN_FILENO, &rfds)) {
	read(STDIN_FILENO, &buff, 100);
}
```

Et lorsque fake recoit un message depuis le pipe `f2s`, il affiche ce message
(sur la sortie standard) et renvoie sur le pipe `s2f` la dernière valeur lue
sur l'entrée standard (autrement dit le contenu de `buff[]`), d'où :

```C
// If there's something coming from the pipe
if (FD_ISSET(s2f, &rfds)) {                     // something to read
	int nbchar;
	if ((nbchar = read(s2f, serverRequest, MAXServerResquest)) == 0) break;
	serverRequest[nbchar]=0;
	fprintf(stderr,"%s", serverRequest);
	printf("Receive : %s\n", serverRequest);
	write(f2s, buff, 100);
}
```

CREATION D'UN SERVEUR WEB
-------------------------

### Du client vers les LED/bouton

Lorsqu'un client va sur la page web, entre le nombre (42 par exemple) et clique
sur "Entrer", son navigateur web émet une requête HTTP vers le serveur sur
la Raspberry Pi, cette requete contient la valeur entrée (ici 42).

Le script server.py écoute en permanence sur le port 8024 les requêtes
entrantes et lorsqu'elle en recoit une, elle détecte que l'utilisateur
a rempli le formulaire HTML. Cela déclenche l'exécution du script
`led.py` comme spécifié dans le code HTML du formulaire
(à la ligne `<form method="POST" action="led.py">` de main.py)

Ce script va lire la donnée entrée par l'utilisateur dans le champ `val` du
formulaire via `form.getValue('val')`, et renvoie cette valeur vers le serveur
fake via le tube.

Le serveur fake va ensuite allumer et éteindre les deux LEDs en fonction de la
valeur des deux LSBs de la valeur recue.

### Des LED/bouton vers le client

Après avoir mis à jour l'état des LEDs, le serveur fake lit l'état du bouton
et renvoie 1 ou 0 (pour bouton pressé ou relaché) au serveur via le tube,
et le serveur renvoie cette valeur au client dans la page HTML générée.

### Principales modifications

La principale modification de code effectuée par rapport à l'exercice 2 est
ce que fait le serveur fake lorsqu'il recoit un message depuis le tube :
L'état de la led 0 (resp. 1) sera l'état du bit 0 (resp. 1) du nombre recu.

*Principale modification dans fake.c (maintenant ledbp.c) :*
```C
// If there's something coming from the pipe                         
if (FD_ISSET(s2f, &rfds)) {                     // something to read 
	int nbchar;                                                      
	if ((nbchar = read(s2f, serverRequest, MAXServerResquest)) == 0) break;
	serverRequest[nbchar]=0;                                         
	printf("Receive : %s\n", serverRequest);                         

	// Set the LED state                          
	if (((int)serverRequest[0] - '0') & 0x1) // ASCII -> DEC conversion
		gpio_write(LIBGPIO_LED0, 1);                             
	else                                                             
		gpio_write(LIBGPIO_LED0, 0);                             

	if (((int)serverRequest[0] - '0') & 0x2) // ASCII -> DEC conversion
		gpio_write(LIBGPIO_LED1, 1); 
	else
		gpio_write(LIBGPIO_LED1, 0);

	// Return the button state to the client
	gpio_read(LIBGPIO_BTN0, &btnValue);
	serverResponse[0] = btnValue + '0';
	write(f2s, serverResponse, 50);
} 
```
