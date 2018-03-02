
COMMUNICATION PAR FIFO

Les tubes (pipes) sont un mécanisme de communication inter-processus (IPC)
suivant une politique first-in first-out (FIFO) et permettant
à différents processus de s'échanger des données.
Selon la norme POSIX il existe deux types types de tubes : les anonymes et les
nommés.

Les tubes anonymes permettent la communication entre processus liés par un
lien père/fils, et ne sont présents qu'en RAM, ils ne prennent donc pas de place
sur le disque.

A l'inverse, les tubes nommés prennent la forme d'un fichier (présence sur
disque donc) et permettent une communication entre n'importe quels processus.

	Dans quel répertoire est créée la fifo ?
	Quelle différence entre mkfifo et open ?
Ici, nous utiliserons un tube nommé pour faire communiquer le programme Py et
le programme C.
mkfifo() sert à créer ce tube (créer le fichier spécial de type tube nommé)
et open() servira à ouvrir le tube (en écriture pour l'émetteur, en lecture
pour le récepteur).
Comme le tube s'utilise comme un fichier, il est normal qu'on l'ouvre avec
la commande qui sert classiquement à ouvrir un fichier (aka open()).

	Pourquoi tester si la fifo existe ?
Le writer crée un pipe et écrit dedans, puis il s'arrête. Ensuite, le reader
lira le contenu du pipe, mais le writer ne sait pas quand est-ce que le
reader va lire.

Imaginons que le writer soit appelé deux fois avant que le reader soit exécuté,
si le writer crée à chaque exécution un nouveau pipe, alors au deuxième appel
il recréera le pipe et supprimera donc son ancien contenu alors qu'il n'a pas
été lu par le reader. C'est pour cela que le writer ne crée le pipe que s'il
n'existe pas déjà.

	A quoi sert flush ?
Lorsque le writer exécute la fonction write(), les données à écrire sont écrites
non pas dans le disque mais dans un buffer, et le reader ne lit que ce qu'il
y a sur le disque donc il ne voit pas les nouvelles données fraichement écrites.
Il faudra attendre que le buffer soit plein pour que ce qu'il y a dedans soit
enfin écrit sur le disque et soit donc disponible pour le reader, ce qui est
inutiliement long. Par conséquent, dès lors que le writer écrit, il flush
le buffer (il répercute cette écriture sur le disque) pour qu'elle soit
aussitot disponible par le reader.

	Pourquoi ne ferme-t-on pas la fifo ?


	Expliquez ce phénomène.
	D'après le man, si on ouvre une FIFO sans donner l'option O_NONBLOCK,
alors un writer ouvrant une fifo en écriture bloquera tant qu'un reader ne
l'aura pas ouvert en lecture.
"An open() for writing only will block the calling thread until a thread opens
the file for reading.". C'est pour cela que qu'il ne se passe rien tant que
le reader n'est pas appelé, en réalité le writer bloque sur open().

