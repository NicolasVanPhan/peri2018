
CR TP6 PERI
Phan Nicolas
Aggoun Bryan

Exécution multi-taches
----------------------

Une application sur carte Arduino a parfois besoin d'effectuer plusieurs taches
en même temps (ex : faire clignoter une LED, tourner un servomoteur, lire périodiquement
un capteur...), sauf que le microcontrolleur de la Arduino ne comporte qu'un seul coeur,
il faut donc employer une technique de pseudo-parallélisme pour que la carte Arduino
effectue les différentes taches en parallèle sur un seul coeur.

Pour cela, nous utiliserons la méthode de programmation suivante :
Chaque tache sera une fonction loop\_XXX() à exécuter en boucle, et qui est non-bloquante,
c'est-à-dire que la fonction n'attend pas qu'un évènement se produise avant de se terminer.

La fonction loop() sera :
```C
void loop ()
{
	loop_tache1();
	loop_tache2();
	loop_tache3();
}

Ainsi, à chaque tour de loop(), l'exécution de chaque tache avancera d'un laps de temps défini,
donnant ainsi l'impression que toutes les taches s'exécutent en parallèle.

### Que contient le tableau waitForTimer[]

waitForTimer[] est un tableau dont la case `i` contient le numéro de la période à laquelle
la fonction a été appelée pour la dernière fois avec timer = `i`.
Par exemple, si waitForTimer[3] = 2013, cela veut dire que le dernier appel à timer 3
s'est fait à la période 2013 (la période 0 serait l'allumage de la Arduino).

En comparant ce numéro avec le numéro de la période courante, la fonction peut déduire le
nombre de périodes écoulées depuis le dernier appel (avec timer = i).

### Dans quel cas la fonction waitFor() peut rendre 2 ?

La fonction waitFor() peut rendre 2 si 2 périodes se sont écoulées depuis son dernier appel.
Généralement les taches appellent waitFor() jusqu'à ce qu'elle renvoie 1 et passent à autre chose,
mais si une tache A appelle waitFor() puis une tache B s'exécute pendant 2 périodes,
puis on revient à la tache A, alors waitFor() renverra 2.


Ecran
-----

Il s'agit ici de créer une tache qui affiche à l'écran le nombre de secondes écoulées depuis
l'allumage de la carte.
Suivant le principe de programmation vu à l'exercice précédent,
nous avons crée une fonction loop\_Screen() qui s'exécute en boucle et qui utilise waitFor()
pour détecter quand une nouvelle seconde s'est écoulée, auquel cas elle incrémete un compteur
(compteur qui indique donc le nombre de secondes écoulées depuis l'allumage) et qui affiche cette
valeur à l'écran.


Communication inter-taches
--------------------------

Ici nous aurons deux taches :
 * Une tache émettrice (loop\_sender()) qui lit et envoie la valeur de la thermistance dans la boite
   aux lettres toutes les 0.5 secondes.
 * Une tache réceptrice (loop\_receiver()) qui change la valeur de la LED toutes les `blinkPeriod`
 secondes, et vérifie constamment la boite aux lettres. Si une nouvelle valeur est recu dans la boite
 aux lettres, la tache affecte cette valeur à blinkPeriod et ainsi modifie la fréqence de clignotement
 de la LED.


