Il s'agit d'une implémentation du jeu classique Asteroids pour [Flipper Zero](https://flipperzero.one/). À l’intérieur, vous trouverez un moteur 2D simple qui peut être réutilisé pour implémenter d’autres jeux. Remarque : Celui-ci est le fork de SimplyMinimal de la version d'Antirez avec plusieurs modifications.

# Quoi de neuf
* Tir rapide automatique (moins d'usure des boutons de cette façon aussi)
* Le bouton Haut applique les propulseurs
* Retour haptique et effets LED
* Mise sous tension du système
* Système de score élevé
* Sauvegarde automatique et chargement des meilleurs scores
* Possibilité de faire une pause
* Quelques modifications de certains éléments de jeu

## Ce qui va suivre
* Écran des paramètres
* Activation des effets sonores (option marche/arrêt configurable)
* ~~Power Ups~~ Gestion améliorée de la mise sous tension

---

Ceci est une capture d'écran, mais le jeu est bien meilleur dans l'appareil lui-même :

![Capture d'écran d'Asteroids for Flipper Zero](/images/Asteroids-PowerUps.png "Capture d'écran dans le jeu")

![Écran Pause](images/Pause%20Screen.png "Écran Pause")

# Contrôles:
* Gauche/Droite : faites pivoter le navire dans les deux directions.
* Ok, appuyez brièvement : balles à rafale courte
* Ok, appuyez longuement : balles à tir automatique
* Haut : Accélérer
* Vers le bas : décélérer
* Retour (appui court) : mettre le jeu en pause
* Retour (appui long) : Quitter le jeu. Il enregistrera également automatiquement le meilleur score.

Vos meilleurs scores seront automatiquement enregistrés. Allez-y et concourez !

---
# Mises sous tension
* ![](assets/firepower_shifted_9x10.png "Munitions") - Tirs de mitrailleuses. Appuyez et maintenez le bouton OK pour tirer plus que les 5 balles par défaut
* ![](assets/heart_10x10.png "Lives") - Et tu, Brute ? Donne une vie bonus jusqu'à un maximum de 5 vies
* ![](assets/nuke_10x10.png "Nuke") - Nuke (travail en cours). Détruit tout ce qui est en vue (mais conserve les power-ups)
* ![](assets/split_shield_10x10.png "Shield") - Utilisez la force ! Fait tourner un bouclier qui peut être utilisé comme bélier. Ne subissez aucun dégât pendant son utilisation.

## Plus de power-ups à venir...

---
## Installation du fichier binaire (aucune build nécessaire)

Accédez aux versions et déposez le fichier `asteroids.fap` dans le
Emplacement Flipper Zero suivant :

     /ext/apps/Jeux

La partie `ext` signifie que nous sommes dans la carte SD. Alors si tu ne veux pas
d'utiliser l'application Android (ou autre) pour télécharger le fichier,
vous pouvez simplement retirer la carte SD, l'insérer dans votre ordinateur,
copiez l'amende dans « applications/Jeux », et c'est tout.

## Installation de l'application à partir des sources

* Téléchargez le kit de développement Flipper Zero et construisez-le :
```
mkdir -p ~/flipperZero/official/
cd ~/flipperZero/officiel/
git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git ./
./fbt
```
* Copiez ce dossier d'application dans `official/application_user`.
* Connectez votre Flipper via USB.
* Construisez et installez avec : `./fbt launch_app APPSRC=asteroids`.

## Licence

Licence BSD. Apprécier.