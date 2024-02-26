# Souris IMC Air

## Bref

> "Vous pouvez transformer n'importe quoi en souris aérienne si vous êtes assez courageux"

  — Piper, alias Pez

Naturellement, la citation ci-dessus s'applique également à [Flipper](https://flipperzero.one/).

## Quoi?

L'application vous permet de transformer votre Flipper en une souris aérienne USB ou Bluetooth (vous avez besoin d'un module supplémentaire, voir la section Matériel ci-dessous)...

Son utilisation est très simple :
  * Connectez le Flipper via un câble USB et choisissez « USB », ou choisissez « Bluetooth » et associez-le à votre PC ;
  * Tenez le Flipper dans votre main avec les boutons pointés vers l'écran ;
  * Agitez votre Flipper comme si vous ne vouliez pas déplacer le curseur ;
  * Bouton haut pour un clic gauche de la souris ;
  * Bouton bas pour un clic droit de la souris ;
  * Bouton central pour un clic central de la souris ;
  * Boutons gauche et droit pour le défilement ;
  * Utilisez l'option du menu d'étalonnage si vous remarquez une dérive significative (placez votre Flipper sur une surface plane, assurez-vous qu'il ne bouge pas, exécutez cette option, attendez 2 secondes, c'est terminé).

Voir le premier prototype [en action](https://www.youtube.com/watch?v=DdxAmmsYfMA).

## Matériel

Le module personnalisé utilise une puce accéléromètre/gyroscope Bosch BMI160 connectée via I2C.

Jetez un œil au dossier [schematic](https://github.com/ginkage/FlippAirMouse/tree/main/schematic) pour les fichiers Gerber, BOM et CPL, afin de pouvoir commander directement auprès de JLCPCB.

Idée originale :

![À quoi je pensais que cela ressemblerait](https://github.com/ginkage/FlippAirMouse/blob/main/schematic/schematic.png)

Attente:

![À quoi EDA cela ressemblerait-il](https://github.com/ginkage/FlippAirMouse/blob/main/schematic/render.png)

Réalité:

![À quoi ça ressemble](https://github.com/ginkage/FlippAirMouse/blob/main/schematic/flipper.jpg)

## Logiciel

Le code est basé sur le [pilote] Bosch d'origine (https://github.com/BoschSensortec/BMI160_driver/) et sur une implémentation de suivi d'orientation de Google [Cardboard](https://github.com/googlevr/cardboard/tree /master/sdk/sensors)

## Licence

TL;DR : utilisez le code comme vous le souhaitez, accordez le crédit là où il est dû, aucune garantie d'aucune sorte n'est fournie.