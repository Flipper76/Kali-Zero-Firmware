<h1 align="center">KZFW - <code>Firmware Kali Zero</code> pour Flipper Zero</h1>

Ce firmware est une tentative de traduction en français du firmware [Xtreme](https://github.com/Flipper-XFW/Xtreme-Firmware).
----
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

----
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


