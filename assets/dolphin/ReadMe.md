# Assets Dolphin

Les actifs de Dolphin sont divisés en 3 parties :

- blocking  - Animations essentielles utilisées pour bloquer les notifications du système. Ils sont regroupés dans `assets_dolphin_blocking.[h,c]`.
- internal - Animations internes utilisées pour l'animation des dauphins inactifs. Converti en `assets_dolphin_internal.[h,c]`.
- external - Animations externes utilisées pour l'animation des dauphins inactifs. Emballé dans le dossier de ressources et placé sur la carte SD.

# Les Fichiers

- `manifest.txt` - contient une énumération d'animations utilisée pour la sélection aléatoire d'animations. Point de départ pour Dolphin.
- `meta.txt` - contient des données qui décrivent comment l'animation est dessinée.
- `frame_X.png` - cadre d'animation.

## Fichier manifest.txt

Fichier au format Flipper avec clés ordonnées.

Entête:

```
Filetype: Flipper Animation Manifest
Version 1
```

- `Name` - nom de l'animation. Doit être le nom exact du répertoire d'animation.
- `Min butthurt`, `Max butthurt` - gamme de fesses de dauphin pour cette animation.
- `Min level`, `Max level` - plage de niveaux du dauphin pour cette animation. Si 0, cette animation ne participe pas à la sélection aléatoire des animations inactives et ne peut être sélectionnée que par son nom exact.
- `Weight` - chance que cette animation soit choisie lors d'une sélection d'animation aléatoire.

Certaines animations peuvent être exclues de la participation à la sélection aléatoire d'animations, comme « L1_NoSd_128x49 ».

## Fichier meta.txt

Fichier au format Flipper avec clés ordonnées.

Entête:

```
Filetype: Flipper Animation
Version: 1
```

- `Width` - largeur de l'animation en px (<= 128)
- `Height` - hauteur de l'animation en px (<= 64)
- `Passive frames` - nombre d'images bitmap pour l'état d'animation passive
- `Active frames` - nombre d'images bitmap pour l'état d'animation actif (peut être 0)
- `Frames order` - ordre des images bitmap où les N premières images sont passives et les M suivantes sont actives. Chaque numéro X dans l'ordre fait référence à une image bitmap, avec le nom frame\_X.bm. Ce fichier doit exister. N'importe quel nombre X peut être répété pour faire référence à la même image dans l'animation.
- `Active cycles` - cycles pour répéter N images actives pendant une période active complète. Par exemple. si les images pour les cycles actifs sont 6 et 7, et les cycles actifs sont 3, donc la période active complète joue 6 7 6 7 6 7. Les périodes complètes de période passive + active sont appelées *période totale*.
- `Frame rate` - nombre d'images à lire pendant 1 seconde.
- `Duration` - nombre total de secondes pour lire 1 animation.
- `Active cooldown` - nombre de secondes (après le mode passif) à passer avant d'entrer dans le prochain mode actif.

- `Bubble slots` - nombre de séquences de bulles.
- Toute séquence de bulles joue toute la séquence en mode actif. Il peut y avoir de nombreuses séquences de bulles et bulles à l'intérieur. Les bulles d’une séquence de bulles doivent résider dans 1 emplacement. L’ordre des bulles dans une séquence de bulles est déterminé par leur occurrence dans le fichier. Dès que l'index d'image sort de l'index EndFrame de la bulle, la bulle d'animation suivante est choisie. Il peut également y avoir des cadres sans bulles entre 2 bulles.

- `Slot` - numéro pour unir les bulles pour la même séquence.
- `X`, `Y` - sont les coordonnées du coin supérieur gauche de la bulle.
- `Text` - texte dans une bulle. La nouvelle ligne est `\n`
- `AlignH` - emplacement horizontal du coin de la bulle (Gauche, Centre, Droite)
- `AlignV` - emplacement vertical du coin de la bulle (Haut, Centre, Bas)
- `StartFrame`, `EndFrame` - plage d'index de trame dans toute la période pour afficher la bulle.

### Compréhension des index de trame

Par exemple nous avons

```
Passive frames: 6
Active frames: 2
Frames order: 0 1 2 3 4 5 6 7
Active cycles: 4
```

Ensuite, nous avons des index

```
                        passive(6)            active (2 * 4)
Real frames order:   0  1  2  3  4  5     6  7  6  7  6  7  6  7
Frames indexes:      0  1  2  3  4  5     6  7  8  9  10 11 12 13
```