<h1 align="center">KZFW - <code>Firmware Kali Zero</code> pour Flipper Zero</h1>

Voici le premier firmware en Français pour Flipper Zero.
-----
<br>
<h2 align="center">Paramètres KaliZero:</h2>

Nous avons écrit une application puissante mais facile à utiliser spécifiquement pour notre micrologiciel, qui vous donne un accès facile à toutes les fonctionnalités sophistiquées que nous avons implémentées:

<img src="https://github.com/Flipper76/Kali-Zero-Firmware/assets/798505/8a96def5-b35a-48ed-be42-566d94b23e7a" align="left" height="160vh"/>
<img align="left" height="180vh" width="10" src="https://upload.wikimedia.org/wikipedia/commons/3/3d/1_120_transparent.png">

- <ins><b>Interface:</b></ins> Personnalisez chaque élément de votre Flipper, des animations du bureau aux applications du menu principal, en passant par le style de l'écran de verrouillage, etc.

- <ins><b>Protocoles:</b></ins> Ici, vous pouvez basculer entre les modes USB et Bluetooth pour BadKB, et gérer les fréquences Subghz personnalisées.

- <ins><b>Divers:</b></ins> Toutes les autres options qui ne trouvent leur place nulle part ailleurs. Changez le nom de votre Flipper, le niveau XP et configurez le <a href="https://github.com/Z3BRO/Flipper-Zero-RGB-Backlight">rétroéclairage RVB</a>.

-----
<br>
<h2 align="center">Animations / Packs de ressources :
   <h3 align="center">Vous voulez essayer quelques packs de ressources ? Vérifiez <a href="https://flipper-xtre.me/asset-packs">ici</a>
   </h3>
</h2>

Nous avons créé notre propre système d'animation/actifs, nouveau et amélioré, que nous pouvons enfin révéler. Il vous permet de créer et de parcourir vos propres « packs d'actifs » en appuyant seulement sur quelques boutons, vous permettant de charger facilement des animations et des icônes personnalisées comme jamais auparavant.

<img src="https://github.com/Flipper76/Kali-Zero-Firmware/assets/798505/b6e34132-ee0a-43b6-9d18-a7c411207499" align="left" width="200px"/>
Vous pouvez facilement créer votre propre pack ou en trouver des créés par les utilisateurs. Essentiellement, nous avons nos propres dossiers <code>Anims</code> et <code>Icônes</code>, à l'intérieur de chaque <code>Asset Pack</code>.

<br clear="left"/>

<br>
<img src="https://github.com/Flipper76/Kali-Zero-Firmware/assets/798505/d6bf9a52-cd73-480e-a4e6-985c20f8c914" align="left" width="200px"/>
Une fois que vous avez quelques packs, téléchargez-les sur votre Flipper dans <code>SD/asset_packs</code> (si vous avez bien fait cela, vous devriez voir <code>SD/asset_packs/PackName/Anims</code> et/ou <code >SD/asset_packs/PackName/Icons</code>).

<br clear="left"/>

<br><img src="https://github.com/Flipper76/Kali-Zero-Firmware/assets/798505/396bfec4-30a7-46f2-a33c-c1bf03ac9420" align="left" width="200px"/>
<img align="left" height="180vh" width="10" src="https://upload.wikimedia.org/wikipedia/commons/3/3d/1_120_transparent.png">
Après avoir installé les packs sur Flipper, appuyez sur le bouton <code>Flèche HAUT</code> dans le menu principal et accédez à <code>Paramètres Kali Zero</code>. 
Choisissez ici le pack que vous souhaitez et modifiez les autres paramètres comme vous préférez, puis appuyez sur Retour pour redémarrer et profiter de vos nouveaux actifs pour toutes les applications (par exemple, l'actif d'analyse Subghz) et les animations !



<br clear="left"/>

-----
<br>
<h2 align="center">Mauvais clavier :</h2>
<img src="https://github.com/Flipper76/Kali-Zero-Firmware/assets/798505/0ebd008f-e54e-404e-90fe-252f176e698c" align="left" width="250px"/>
<! -- Cette connerie a besoin d'une image capturée, mais à cause du blocage, je n'arrive pas à en obtenir une. que quelqu'un fasse de la magie s'il vous plait -- !>
BadUSB est une application merveilleuse, mais elle ne dispose pas de fonctionnalités Bluetooth. Maintenant, certains pourraient affirmer que cela ne sert à rien car vous aurez toujours besoin d’une authentification des deux côtés, mais et si je vous disais que nous avons trouvé une solution à ce problème ?
<br><br>
Le Mauvais clavier vous permet de basculer entre les modes USB et Bluetooth pour vos attaques. En mode Bluetooth, il vous permet d'usurper le nom et le MAC de l'appareil comme vous le souhaitez. Être un haut-parleur JBL ou un clavier Razer sans fil est facilement réalisable, vous permettant de tromper les gens afin que vous puissiez exécuter vos charges utiles sans avoir besoin d'un câble à portée de main.

-----
<br>
<h2 align="center">Niveaux :</h2>

Ce firmware a 30 niveaux, pas seulement les 3 de base du niveau officiel.

Avec ce nouveau système en place, cela permet des choses intéressantes comme verrouiller les animations derrière un certain niveau. Cela peut être fait assez facilement : les ralentis_animations sont liés au système de niveaux. Plus précisément, la variable « Niveau minimum » de votre fichier manifeste est utilisée ici. Chaque niveau atteint débloque une nouvelle animation. Plus votre niveau est élevé, plus on peut voir d'animations.

-----
<br>
​
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

-----
<br>
<h2 align="center">Construisez-le vous-même :</h2>

> **Avertissement**
> Nous ne fournirons pas de support de base pour la compilation sur notre serveur. Ceci est destiné aux personnes qui *savent* déjà ce qu'elles font !

```bash
Pour télécharger les outils nécessaires :
$ git clone --recursive --jobs 8 https://github.com/Flipper76/Kali-Zero-Firmware.git
$ cd Kali-Zero-Firmware/

Pour flasher directement sur le Flipper (Doit être connecté via USB, qFlipper fermé) :
$ ./fbt flash_usb_full

Pour compiler un package TGZ 
$ ./fbt updater_package

Pour créer et lancer une seule application :
$ ./fbt lancer APPSRC=votre_appid
```


