# FindMy Flipper - Émulateur FindMy SmartTag

Cette application étend les fonctionnalités Bluetooth du FlipperZero, lui permettant d'agir comme un Apple AirTag ou un Samsung SmartTag, ou même les deux simultanément. Il utilise la balise BLE de FlipperZero pour diffuser un signal SmartTag qui sera capté par le réseau FindMy. J'ai fait cela pour servir d'outil polyvalent à des fins de suivi, offrant la possibilité de cloner des balises existantes, de générer des paires de clés OpenHaystack pour l'intégration avec le réseau FindMy d'Apple et d'ajuster les paramètres de diffusion des balises de l'appareil.

## Caractéristiques

1. Émulation de balise : clonez votre Apple AirTag ou Samsung SmartTag existant sur le FlipperZero, ou générez une paire de clés à utiliser avec le réseau FindMy sans posséder un véritable AirTag.
2. Personnalisation : les utilisateurs peuvent ajuster l'intervalle entre les diffusions des balises et modifier la puissance d'émission en fonction de leurs besoins, en optimisant à la fois la visibilité et la durée de vie de la batterie.
3. Fonctionnement efficace en arrière-plan : l'application est optimisée pour fonctionner en arrière-plan, garantissant que votre FlipperZero peut toujours être suivi avec une utilisation minimale de la batterie et sans arrêter l'utilisation normale.

## Guide d'utilisation

### Étape 1 : Installation
- **Option A :** Utilisez le micrologiciel publié/précompilé approprié (FAP) pour votre appareil.
- **Option B :** Créez vous-même le firmware en utilisant `fbt/ufbt`.
- Les deux options d'installation nécessitent que vous exécutiez une version de développement du micrologiciel. Lorsque la version aura accès à la balise BLE supplémentaire, cela changera, merci !
- Tous les firmwares devraient désormais fonctionner avec la branche principale, y compris les icônes
  
### Étape 2 : Obtention des données SmartTag

#### Option A : Clonage d'une balise existante (préféré et vous permet d'effectuer un suivi sans configuration supplémentaire)
1. **Associez un tag :** Tout d'abord, associez un AirTag ou un Samsung SmartTag à votre appareil.
2. **Entrez en mode « Perdu » :** Gardez l'étiquette éloignée de l'appareil sur lequel elle est enregistrée pendant environ 15 minutes.
3. **Téléchargez nrfConnect :** Installez nrfConnect depuis le Google Play Store. (La version Apple ne révèle pas les données brutes nécessaires, je recherche une solution de contournement)
4. **Filtrer et analyser :**
    - Ouvrez l'application, cliquez sur les filtres et excluez tout sauf la marque de votre tag (Apple/Samsung).
    - Réglez le RSSI au réglage le plus bas (-40 dBm).
    - Lancer une analyse. Attendez que votre SmartTag apparaisse comme un appareil « FindMy ».
5. **Capturer les données** : cliquez sur **Raw** ou **View Raw** pour capturer votre **charge utile** et notez l'**adresse MAC** de votre balise. Retirez immédiatement la batterie du tag pour empêcher la rotation clé/MAC.
6. **Entrez les données dans l'application FlipperZero :** Saisissez la **charge utile** et l'**adresse MAC** capturées dans l'application FlipperZero.

#### Option B : Méthode ouvrir Haystack
1. **Générer une balise :** Téléchargez le fichier `generate_keys.py` et exécutez-le dans votre terminal. (Vous aurez besoin de la cryptographie ```python3 -m pip install cryptography```)
2. **Suivez les invites :** Pendant l'exécution, vous serez invité à saisir des informations. À la fin, vous obtiendrez une **Clé privée**, une **Clé publique**, une **Charge utile** et une **Adresse MAC**.
    - **Clé privée** est nécessaire pour recevoir des rapports de localisation d'Apple.
    - **L'adresse MAC** doit être enregistrée dans l'application FlipperZero :
      1. Ouvrez l'application et accédez au menu de configuration.
      2. Choisissez « enregistrer la balise » et entrez l'adresse MAC lorsque vous y êtes invité.
      3. Une boîte de dialogue de charge utile apparaîtra ensuite. Saisissez votre **Charge utile** ici.
      4. Cliquez sur Enregistrer.
3. **Achèvement de la configuration :** Avec cette configuration, votre appareil est prêt pour Open Haystack. Suivez les étapes spécifiques pour Open Haystack ou MaclessHaystack en fonction de votre configuration.
    - Vous ne possédez pas de Mac : https://github.com/dchristl/macless-haystack ou https://github.com/Chapoly1305/FindMy
    - Posséder un Mac : https://github.com/seemoo-lab/openhaystack

## Configuration sur Mac avec l'application OpenHayStack (OHS) -- Si vous possédez un Mac, instructions

Suivez ces étapes pour que tout fonctionne sur un Mac en utilisant la dernière version de l'application OpenHayStack.
Merci à Wr3nch pour l'aide

### Étape 1 : Créer un nouvel appareil
- Commencez par créer un nouvel appareil dans l'application OpenHayStack, mais **ne le déployez pas** immédiatement après sa création.

### Étape 2 : Exporter la configuration
- Choisissez d'**EXPORTER** la configuration en sélectionnant "tous les accessoires sous forme de fichier". Pour simplifier, assurez-vous de n’avoir qu’une seule entrée dans la liste avant d’exporter.
- Il est crucial que le format d'export soit en JSON.

### Étape 3 : Modifier le fichier JSON
Ouvrez le fichier JSON exporté dans un éditeur de texte et apportez les modifications suivantes :
- **Touches OHS gauche, droite de mon script ```generate_keys.py``` :**
     - `symmetricKey` doit être défini sur la `Hashed adv key`.
     - `privateKey` doit être remplacé par votre `Private Key`.
     - `oldestRelevantSymmetricKey` doit également utiliser la `Hashed adv key`.
- De plus, mettez à jour les attributs suivants sur « true  » :
- **Touches OHS gauche, droite de mon script ```generate_keys.py``` :**
     - `symmetricKey` doit être défini sur la `Hashed adv key`.
     - `privateKey` doit être remplacé par votre `Private Key`.
     - `oldestRelevantSymmetricKey` doit également utiliser la `Hashed adv key`.
- De plus, mettez à jour les attributs suivants sur « true  » :
    - `"isDeployed": true`
    - `"isActive": true`

### Étape 4 : Réimporter la configuration
- Après avoir enregistré vos modifications dans le fichier JSON, réimportez-le dans OpenHayStack.

### Étape 5 : Ajustez les paramètres dans l'application OHS
- Dans l'application Mac OpenHayStack, accédez à la barre supérieure et modifiez le paramètre d'heure de « 1 jour » à « 30 minutes ».
Donnez-lui le temps de traiter et d'appliquer les nouveaux paramètres.

En suivant ces étapes, votre appareil devrait être configuré et prêt à utiliser OpenHayStack sur un Mac.
****

### Étape 3 : Configuration sur le FlipperZero
- Au lancement de l'application, ouvrez le menu de configuration et cliquez sur ```Importer un fichier Tag``` ou ```Enregistrer Tag manuellement```. Placez votre fichier .keys généré sur la carte SD FlipperZero dans le dossier AppsData/FindMyFlipper pour l'importer à partir du fichier. Ou vous pouvez saisir manuellement les informations de la balise. Lorsque vous utilisez la méthode de clonage, vous pouvez exporter un fichier .txt depuis nrfConnect (cliquez sur le bouton Enregistrer) et le placer dans le même dossier afin de l'importer.

### Étape 4 : Suivi
- Une fois l'application configurée, votre FlipperZero peut être suivi à l'aide du service de suivi de la plateforme concernée (application FindMy pour les appareils Apple, SmartThings pour les appareils Samsung et navigateurs Web respectifs). Si vous utilisez des clés générées et OpenHaystack, vous pouvez effectuer le suivi sur l'application OHS ou via la configuration Macless Haystack. Les liens vers les deux sont ci-dessus


Personnalisation

- Intervalle de balise : Ajustez la fréquence à laquelle votre FlipperZero diffuse sa présence.
- Puissance de transmission : augmentez ou diminuez la force du signal pour équilibrer la portée de suivi et la durée de vie de la batterie.

Utilisation en arrière-plan

L'application est conçue pour avoir un impact négligeable sur la durée de vie de la batterie, même lorsqu'elle s'exécute en arrière-plan. Cela permet un suivi continu sans avoir besoin de recharges fréquentes.

Compatibilité

- Appareils Apple pour le suivi AirTag via le réseau FindMy.
- Tout appareil prenant en charge le suivi Samsung SmartTag, y compris les navigateurs Web (anciennement FindMyMobile).

Merci

- Un grand merci à toutes les personnes qui ont contribué au projet OpenHaystack, aux projets de soutien et aux guides sur le sujet. Cela ne serait rien sans vous ! Un merci spécial à WillyJL pour avoir aidé à faire fonctionner la saisie de l'application et à la refonte globale des fonctions de l'application !

Mentions légales et confidentialité

Cette application est destinée à un usage personnel et éducatif. Les utilisateurs sont responsables du respect des lois et réglementations locales en matière de confidentialité concernant les dispositifs de suivi. Le clonage et l'émulation des balises de suivi doivent être effectués de manière responsable et dans le respect de la propriété des appareils d'origine.

Clause de non-responsabilité

Ce projet n'est pas affilié à Apple Inc. ou Samsung. Tous les noms de produits, logos et marques sont la propriété de leurs propriétaires respectifs. Utilisez cette application de manière responsable et éthique.