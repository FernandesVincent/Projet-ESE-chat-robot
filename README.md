# Projet ESE – Chat Robot  
**Point à mi-parcours – 13 novembre 2025**

Je propose ici de faire un point sur l’ensemble du projet tel qu’il a été défini à la date d’aujourd’hui.

---

## Partie hardware

Commençons par la partie hardware du projet, en détaillant l’ensemble des périphériques présents sur le PCB :

### TOFs  
Le robot est équipé de six capteurs TOF, disposés en cercle et orientés en biais (environ 45° selon la conception actuelle). Cette disposition permet d’obtenir une “vision” plus étendue (environ une dizaine de centimètres) de l’environnement autour du robot, plutôt que de se limiter à une détection verticale à la base.

### Module MP3  
Le connecteur est relié à un module MP3 communiquant en UART. Ce module permet d’insérer une carte microSD contenant des fichiers audio. Via l’UART, il est possible de sélectionner le fichier à lire et de régler le volume de sortie.  
La sortie audio (mini-jack) est connectée à un PCB annexe — actuellement en commande — qui amplifie le signal avant de l’envoyer à un haut-parleur.  
Ce système sera utilisé lors de moments clés de la démonstration, notamment lorsque le robot changera de rôle après une collision, afin d’indiquer s’il est “devenu” un chat ou une souris.

### LEDs  
Nous utilisons un ruban de LEDs Neopixel qui viendra en soutien au LiDAR pour indiquer la direction dans laquelle le robot adverse a été détecté.  
Elles serviront également, à l’instar du haut-parleur, à signaler les changements de rôle du robot.

### Raspberry Pi Zero 2W  
Un Raspberry Pi Zero 2W est ajouté au PCB afin d’héberger une interface graphique utilisateur (GUI) accessible en ligne.  
Cette interface permettra d’obtenir une “vision” du robot grâce à une caméra embarquée, ainsi que de visualiser diverses constantes et mesures telles que la vitesse, l’accélération ou encore le niveau de batterie du robot.

---

## Partie software

Concernant la partie **logicielle**, voici l’état d’avancement et l’organisation du travail à ce jour :

### Drivers développés
À l’heure actuelle, plusieurs drivers ont déjà été créés pour les périphériques mentionnés ci-dessus :

- **LEDs** : fonctionnel  
- **MP3** : fonctionnel  
- **TOFs** : améliorable  
- **Moteurs** : en cours de développement  
- **LiDAR** : en cours de développement  

La prochaine étape consistera à finaliser ces drivers, puis à travailler sur l’aspect noyau temps réel du projet à l’aide de FreeRTOS.  
Ce dernier nous permettra de faire fonctionner simultanément l’ensemble des modules pour aboutir à un robot pleinement opérationnel.

### Évolutions envisagées
Dans la perspective d’un système complet, plusieurs idées ont émergé, notamment :  
- la connexion d’une manette Bluetooth (type console) à la Raspberry Pi pour piloter le robot ;  
- l’intégration de commandes vocales pour une interaction plus naturelle.

---

## Partie mécanique

Enfin, la modélisation 3D du robot est pratiquement terminée.  
Quelques ajustements mineurs restent à effectuer, notamment au niveau de la base.  
Le principal point de réflexion concerne les roues, qui ont été modifiées pour être élargies par l’ajout d’un second joint torique sur chacune d’elles.  
Cependant, cette modification pourrait compliquer le contrôle du robot, car elle transforme le point de contact unique en aire de contact, ce qui pourrait affecter la maniabilité.