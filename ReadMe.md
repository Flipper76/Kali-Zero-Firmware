<h1 align="center">KZFW - <code>Firmware Kali Zero</code> pour Flipper Zero</h1>

Ce firmware est une tentative de traduction en français du firmware [Xtreme](https://github.com/Flipper-XFW/Xtreme-Firmware).
-----
<br>
<h2 align="center">Paramètres KaliZero:</h2>

Nous avons écrit une application puissante mais facile à utiliser spécifiquement pour notre micrologiciel, qui vous donne un accès facile à toutes les fonctionnalités sophistiquées que nous avons implémentées:

<img src="https://github.com/Flipper76/Kali-Zero-Firmware/assets/798505/8a96def5-b35a-48ed-be42-566d94b23e7a" align="left" height="160vh"/>
<img align="left" height="180vh" width="10" src="https://upload.wikimedia.org/wikipedia/commons/3/3d/1_120_transparent.png">

- <ins><b>Interface:</b></ins> Personnalisez chaque élément de votre Flipper, des animations du bureau aux applications du menu principal, en passant par le style de l'écran de verrouillage, etc.

- <ins><b>Protocoles:</b></ins> Ici, vous pouvez basculer entre les modes USB et Bluetooth pour BadKB, et gérer les fréquences Subghz personnalisées.

- <ins><b>Divers:</b></ins> Toutes les autres options qui ne trouvent leur place nulle part ailleurs. Changez le nom de votre Flipper, le niveau XP et configurez le <a href="https://github.com/Z3BRO/Flipper-Zero-RGB-Backlight">rétroéclairage RVB</a>.

<br>

<br>
<h2 align="center">Paramètres KaliZero:</h2>

Nous avons écrit une application puissante mais facile à utiliser spécifiquement pour notre micrologiciel, qui vous donne un accès facile à toutes les fonctionnalités sophistiquées que nous avons implémentées:

<img src="https://github.com/Flipper76/Kali-Zero-Firmware/assets/798505/8a96def5-b35a-48ed-be42-566d94b23e7a" align="left" height="160vh"/>
<img align="left" height="180vh" width="10" src="https://upload.wikimedia.org/wikipedia/commons/3/3d/1_120_transparent.png">

- <ins><b>Interface:</b></ins> Personnalisez chaque élément de votre Flipper, des animations du bureau aux applications du menu principal, en passant par le style de l'écran de verrouillage, etc.

- <ins><b>Protocoles:</b></ins> Ici, vous pouvez basculer entre les modes USB et Bluetooth pour BadKB, et gérer les fréquences Subghz personnalisées.

- <ins><b>Divers:</b></ins> Toutes les autres options qui ne trouvent leur place nulle part ailleurs. Changez le nom de votre Flipper, le niveau XP et configurez le <a href="https://github.com/Z3BRO/Flipper-Zero-RGB-Backlight">rétroéclairage RVB</a>.

<br>

<br>
<h2 align="center">Installation:</h2>
<br>
Il existe plusieurs méthodes pour installer Kali Zero, choisissez celle que vous préférez :

<br>

> <details><summary><code>Package qFlipper (.tgz)</code></summary><ul>
> <li>Téléchargez le package qFlipper (.tgz) depuis la <a href="https://github.com/Flipper76/Kali-Zero-Firmware/releases/latest">page de la dernière version</a></li >
> <li>Ouvrez <a href="https://flipperzero.one/update">qFlipper</a> et connectez votre Flipper</li>
> <li>Cliquez sur <code>Install from file</code></li>
> <li>Sélectionnez le .tgz que vous avez téléchargé et attendez la fin de la mise à jour</li>
> </ul></détails>

> <details><summary><code>Archive Zip (.zip)</code></summary><ul>
> <li>Téléchargez l'archive compressée (.zip) depuis la <a href="https://github.com/Flipper76/Kali-Zero-Firmware/releases/latest">page de la dernière version</a></li >
> <li>Extraire l'archive. Ceci est maintenant votre nouveau dossier Firmware</li>
> <li>Ouvrez <a href="https://flipperzero.one/update">qFlipper</a>, allez dans <code>SD/update</code> et déplacez simplement le dossier du firmware à cet endroit</li>
> <li>Sur le Flipper, appuyez sur le bouton <code>Flèche vers le bas</code>, cela vous amènera au menu Fichier. Recherchez simplement votre dossier de mises à jour</li>
> <li>Dans ce dossier, sélectionnez le micrologiciel que vous venez de déplacer et exécutez le fichier simplement appelé <code>Update</code></li>
> </ul></détails>

<br>

-----
<br>
<h2 align="center">Construisez-le vous-même :</h2>

> **Avertissement**
> Nous ne fournirons pas de support de base pour la compilation sur notre serveur. Ceci est destiné aux personnes qui *savent* déjà ce qu'elles font !

```bash
Pour télécharger les outils nécessaires :
$ git clone --recursive --jobs 8 https://github.com/Flipper76/Kali-Zero-Firmware.git
$ cd Xtreme-Firmware/

Pour flasher directement sur le Flipper (Doit être connecté via USB, qFlipper fermé)
$ ./fbt flash_usb_full

Pour compiler un package TGZ
$ ./fbt updater_package

Pour créer et lancer une seule application :
$ ./fbt lancer APPSRC=votre_appid
```


