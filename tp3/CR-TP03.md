Notes du TP3 de PERI :
======================

## Configuration des GPIO pour le pilotage de l'écran

Les signaux RS, RW et E sont les entrées du LCD et donc vont être les sorties de nôtre Rasperry Pi.

Il est nécessaire de configurer le GPIOs en OUTPUT.

Pour envoyer un caractère on a besoin de 8 bits sauf qu'on utilise que 4 files. D'après la datasheet, notre commande est envoyé en deux temps:
	* Envoie des 4 bits de poids forts
	* Envoie des 4 bits de poids faibles

Afin d'envoyer des commandes au LCD il faut mettre l'entrée **RS** à '1' pour prevenir le LCD que l'on veut envoyer une instruction (commande), les signaux **DB4** à **DB7** vont nous permettre de préciser quelle instruction est demandée.

Si le signal **RS** est à '0', cela signifie que l'on envoie une donnée à écrire (car RW à '0' pour ce TP) au LCD. Les signaux **DB4** à **DB7** serviront à préciser le caractère à envoyer sachant que chaque caractère est codé sur 8 bit.

Les commandes sont définies par des maccros utilisant elles même la maccro HEX(h,1) qui prend en argument les bits de poids forts et faibles d'un entier sur 8 bits et renvoie cette entier, on peut alors envoyer une commande au LCD en ulisant la fonction `lcd_command()` qui prendra donc en argument les différentes maccro de commande du LCD.

Le mode 2 lignes du LCD fait en sorte que l'affiche LCD ne soit composé que de seulement 2 lignes. Si il est affiché 4 lignes sur le LCD alors alors ça signifie que 2 des 4 lignes représentent 1 ligne en réalité. Donc 1 ligne logiciel du LCD correspondra à 2 lignes d'affichage du LCD. Et les lignes ne suivent pas, la première ligne logiciel comprend les lignes 1 et 3 du LCD et la seconde ligne logiciel comprend les lignes 2 et 4 du LCD.

## Fonctionnement de l'écran et fonctions de base

La fonction lcd_strobe :

```c
void lcd_strobe(void)
{
  gpio_update(E, 1);
  usleep(1);
  gpio_update(E, 0);
  usleep(1);
}
```
La fonction lcd_write4bits : 

```c
void lcd_write4bits(int data)
{
  /* first 4 bits */
  gpio_update(D7, BIT(7,data));
  gpio_update(D6, BIT(6,data));
  gpio_update(D5, BIT(5,data));
  gpio_update(D4, BIT(4,data));
  lcd_strobe();

  /* second 4 bits */
  gpio_update(D7, BIT(3,data));
  gpio_update(D6, BIT(2,data));
  gpio_update(D5, BIT(1,data));
  gpio_update(D4, BIT(0,data));
  lcd_strobe();
  usleep(50); /* le délai minimum est de 37us */
}
```

La fonction lcd_set_cursor :

```c
void lcd_set_cursor(int x, int y)
{
	int	i;
	int	line_len;
	int	real_y[4] = {1, 3, 2, 4};

	lcd_command(LCD_RETURNHOME);
	line_len = 20;
	for (i = 0; i < line_len * (real_y[y - 1] - 1) + (x - 1); i++)
		lcd_command(LCD_CURSORSHIFT | LCD_CS_CURSORMOVE | LCD_CS_MOVERIGHT);
}
```

