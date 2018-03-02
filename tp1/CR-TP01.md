Contexte et objectif
===========================

Dans ce TP, nous allons développer une application user faisant clignoter, à l'aide d'un bouton poussoir, deux LEDs qui seront connectés (les 2 LEDs et le BP) au port GPIO d'une carte Raspberry Pi1.

Détail sur notre environnement de travail 
------------------------------------------

Pour les TPs à venir, nous allons donc utiliser la carte Rasperry Pi1.
La rasperry est un mini-ordi, on doit donc s’y connecte comme on sur  ordi distant, un serveur : classiquement via SSH.

Les ordis sont sur des processeurs Intel, les compilateurs installés dessus compilent donc en du code pour architecture Intel, or la Raspberry Pi est basée sur un processeur ARM, il faut donc pouvoir compiler du code pour architecture ARM avec un PC sur architecture Intel, d’où la nécessité d’un compilateur croisé.

Durant ce TP, nous allons nous entrainer à faire clignoter deux LEDs connectées sur une broche de port GPIO chacune. Le GPIO est un type de périphérique présent sur la Rasperry.
C'est un périphérique très simple d'entrée/sortie, ne pouvant traiter que des signaux numériques 1 ou 0.

Contrôle des GPIO
------------------

Avant toutes choses, il est nécessaire de configurer correctement le périphérique utilisé (GPIO ici) avant de l'utiliser. C’est dans la datasheet de la raspberry que l’on va avoir toutes les informations nécessaires pour initialiser nos ports GPIO. On va récupérer l’adresse de base des registres GPIO dans l’espace d’adressage physique. Cela nous permettra ensuite de sélectionner les ports voulus et de “setup” les broches en :
  - **INPUT**
  - **OUTPUT**

Pour une broche initialisée en **OUTPUT**, il est possible d’écrire une valeur dessus, soit de mettre la sortie à 1 ou 0 manuellement.
Ce qui sera pratique pour contrôler manuellement (en logiciel) les LEDs.

### Clignotement des LEDs

Pour faire clignoter une LED, une première solution est d’allumer la LED, mettre en pause le programme pendant X millisecondes, eteindre la LED, remettre en pause pendant X ms et ce en boucle.
C'est grâce à une fonction `Delay()` que l'on va mettre le programme en pause. En effet, cette fonction va faire tourner le programme durant X ms et ne fera rien de spéciale pendant ce temps. Cependant, l'inconvéniant de cette technique est que durant le delay, le programme est occupé et ne peut donc rien faire pendant l'attente. De plus, ce n'est pas une technique vraiment très précise. Cette technique est donc utile pour des programmes simples, mais peut s'avèrer peu optimale pour des programmes plus complexes.

Pour optimiser le clignotement des LEDs, on pourait utiliser un **Timer**, soit un compteur de ms qui nous préviendrait lorsque X ms se sont écouler (avec une interruption par exemple). 

### Lecture d'une valeur d'une entrée GPIO (le BP)

On a ensuite contrôlé le clignotement des LEDs grâce à un bouton poussoir. Il a donc fallu configurer un port GPIO en entrée. L'idée est de regarder dans une boucle infinie, l'état de la broche. Si celle si à changer de valeur, alors suivant la valeur pris, le bouton a été soit enfoncé soit relaché.

Voici le code que nous avons implémenté :

```c
while (1) {
      delay(20);
      //val_nouv = GPIO_VALUE( gpio_base, GPIO_BTN0 );
      gpio_read(LIBGPIO_BTN0, &val_nouv);

      if((val_prec != val_nouv) && (val_nouv == 0))
        BP_ON = 1;
      if((val_prec != val_nouv) && (val_nouv == 1))
        BP_OFF = 1;

      if(BP_ON)
      {
        BP_ON = 0;
        printf(":))\n");
      }
      if(BP_OFF)
      {
        BP_OFF = 0;
        printf("nicolas\n");
      }
      if (val_nouv == 1)
        gpio_write(LIBGPIO_LED0, 1);
      gpio_write(LIBGPIO_LED1, 0);
      delay ( half_period );
      val_prec = val_nouv;
      gpio_write(LIBGPIO_LED0, 0);
      gpio_write(LIBGPIO_LED1, 1);
      delay ( half_period );
    }
```

Réalisation d'une librairie
------------------------------
