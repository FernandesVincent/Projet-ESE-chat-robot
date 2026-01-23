# Projet ESE – Chat Robot  

Le projet de Chat Robot développé dans le cadre de la spécialité ESE dirigée par M.Fiack et supervisée par lui et M. Papazogou s'est achevée le vendredi 16 Janvier 2026. 

Nous allons donc revenir ici sur les différents aspects notables du projet, notamment ceux où notre projet s'est détaché des consignes initiales.

Nous allons donc passer en revue les différentes parties du projet en commençant par une partie sur l'organisation du projet et des tâches attribuées aux différents membres de l'équipe puis nous aborderons la partie Hardware séparée en deux sous parties pour Onshape et Kicad suivie de la partie Software avec les différents composants importants du projet. 

Par soucis de lisibilité de l'ensemble du document nous n'allons pas détaillé en profondeur toutes les parties et sous parties liées à la mise en place de chaque composant sur le pcb ou à sa programmation. À la place, nous expliquerons brièvement l'ensemble des logiques hardware et software pour ensuite analyser en profondeurs seulement certains aspects de ces parties.

## Organisation du Projet

Le projet a été porté par une équipe de quatres étudiants de 3ème année à l'ENSEA faisant partit de la spécialité ESE, cette équipe est composée de : 

- Vincent FERNANDES
- Paul CAILLAUD
- Kelly LUO
- Yimei Xia

Après diverses discussions au cours du semestre, voici comment ont été attribués les rôles concernant le projet : 

- Vincent : PCB, modélisation 3D, Soudure CMS & THT TOFs, mise en commun du code sous FreeRTOS
- Paul : Modélisation 3D, cablâge, Soudure CMS & THT, Accéléromètre, Moteurs
- Kelly : Lidar
- Yimei : Accéléromètre

Par ailleurs un README relatant l'avancée à mi-parcours (13 Novembre) est disponible dans le dossier Gestion.

# Hardware

## Modélisation 3D

Pour notre projet, nous avons décidés d'utiliser un pack de batteries Li-ion 2S au lieu du pack de 5 batteries NIMH car cela nous apporte une stabilité accrue de maintient de la tension aux bornes de la batterie ainsi qu'une simplicité de la recharge. En effet, pour recharger les batteries que nous utilisions il nous fallait simplement les connecter à n'importe quelle alimentation stabilisée à 8.4V.

Cela étant, dû au fait d'avoir modifier les batteries nous avons du adapter notre modélisation 3D. Nous avons donc décider de l'inclure directement dans la base comme présenté ci-dessous afin d'augmenter la stabilité de l'ensemble du robot. Cependant, cela a eu pour effet d'augmenter le diamètre de la base de notre robot passant d'une quinzaine de centrimètre à une vingtaine. De plus, nous avons ajouté un cache sur le dessus de la base pour bloquer la batterie en cas de chute ou autre accident. Finalement nous avons également équipé la base d'un arceau afin d'accueillir un bandeau de LED Neopixel ws2812b. 
Contrairement aux autres robots, nous n'avons fait qu'un unique étage, sur lequel sont placé les 6 TOFs et le LiDAR, afin d'avoir la place entre le cache de la batterie et le sol de cet étage de placer le PCB suspendu par des vis M3 et bloqué par les écrous correspondants.

De plus, nous avons également modifier les roues afin d'y inclure non pas un mais deux joints toriques par roue afin, encore une fois, de garantir une meilleur stabilité du robot au vu de sa taille. Or, cela complexifie légerment les virages que peut effectuer le robot là où les lignes droites sont cependant meilleures.

## PCB

Pour la conception de PCB nous sommes passés par le logiciel KiCAD et nous en avons crée 4 différents pour certaines raisons particulières que j'expliquerais au fur et à mesure avec l'explication générale de chaque PCB.

# PCB v1

Ce PCB est le premier qui a été designé pour le projet de robot chat. Il inclut notamment les composants de base au fonctionnement du robot tel que les drivers de moteurs, l'accéléromètre et les connecteurs pour les TOFs ainsi que celui pour le LiDAR. En effet, pour ce projet nous avons choisis d'utiliser une solution à base de 6 TOFs afin de détecter le vide autour de la table sur laquelle se déplace le robot. Le choix de 6 TOFs a été réfléchis afin d'avoir suffisament de TOFs pour pouvoir repérer le vide peut importe l'orientation du robot au moment d'approcher le bord de table, sans non plus utiliser une quantité non raisonnable de capteurs.

De plus, ce PCB inclut trois nouveaux éléments que l'on a décidé d'ajouter à notre robot. Le premier est un connecteur vers un module MP3 dans le but de jouer un son, défini en amont, à certains moments clés de la présentation. On peut prendre comme exemple le moment où du vide est repéré par un TOF ou bien au moment d'une collision avec un autre robot. 

Le deuxième élément est un bandeau de LED Neopixels commandé par une PWM avec DMA. Le but ici est de venir, tout comme le module MP3, indiquer la réalisation de certaines actions du robot. Par exemple, l'idée principale était d'illuminer une des 96 LEDs du bandeau dès lors que le LiDAR détecte une cible. Certaines LED pourraient s'allumer en rouge en direction de cibles non valides alors qu'une LED unique serait allumée en direction de l'objet valide : l'autre robot.

Finalement, le troisième élément est une Raspberry Pi zéro 2W. Nous avons choisis de l'incorporer pour, d'une part, remplacer la connexion bluetooth demandée mais également d'une autre part pour y développer une interface graphique intéractive. Cette dernière permettra de récupérer les données captées par le robot en temps réel mais également de lui envoyer des instructions, également en temps réel, via une lisaion UART cadencée à 1Mo de baud rate.

L'ajout de ces composants nous a obligé à changer le microcontroleur fournis de base pour un STM32G474RET6 possédant, au contraire de l'initial, le nombre suffisant d'HUART accessibles simultanément pour nos besoins.

Cependant, plusieurs problèmes datant de la conception sous KiCAD sont apparus une fois le PCB soudé et testé. Parmi celles-ci, les plus notables et plus importante sont les suivantes : 

- Oubli de connection de la pin ADDR de l'accéléromètre à GND (ou VCC en fonctiond de l'adresse voulue).
- Inversion des connexions SDA/SCL pour l'I2C de l'accéléromètre.
- Mauvaise connexion du pin Tx du LiDAR à un GPIO simple.
- Mauvaises connexions des pins GPIO des TOFs empêchant leur utilisation en interruption.

Même si, à la fin nous avons pu corriger ces erreurs de différentes façons il nous semblait essentiel de les faire apparaître dans ce compte rendu.

# PCB v1.5

Cette version du PCB a été créée suite à la découverte des erreurs présentes sur la v1. Cette version a été designée dans l'idée d'avoir un pcb purement fonctionnel corrigeant les erreurs du PCB précédent tout en enlevant les éléments non nécessaires ajoutés dans la v1. Nous avons tout de même gardé le même microcontrôleur bien que le nombre de pins soit bien plus important que nécessaire. Cela nous a cependant fait gagné du temps de conception et le pcb a donc pu être réalisé en moins d'une journée.

# PCB v2

Cette

# PCB audio

Ce PCB très simple utilise uniquement un amplificateur audio afin d'amplifier le signal en sortie du module MP3. Ce dernier utilise un câble mini-jack fonctionnant parfaitement pour une utilisation en connexion mini-jack directe comme avec un casque audio. Cependant cette sortie de module ne fournit pas assez de puissance pour qu'un haut parleur directement connecté produise un son audible. Il faut donc entre le haut-parleur et le module MP3 ajouter un amplificateur audio. 

Cet amplificateur audio a été choisi, tout d'abord pour ses spécificités techniques de 1W/8ohm correspondant excatement à celles de notre haut-parleur. De plus, quand la liste des choix possibles a été suffisament réduite nous avons pris le parti de choisir celui de la marque Texas Instrument car leur documentation est très lisible et compréhensible comparé à celle de leur concurrent. Cela nous permet, par exemple, de réduire le temps passé, tant sur la partie hardware, que sur la partie software de ce pcb. De plus, La communication I2C de l'amplificateur permet de modifier le volume en sortie sur 28 niveaux différents, ce qui est encore un autre avantage de ce dernier.

# Software 

# Lidar

# Moteurs

Les moteurs sont un élement essentiel du projet puisqu'il faut que le robot puisse se déplacer sur la table. Nous avons à notre disposition des moteurs FIT0520 qui seront chacuns pilotés par un driver. Nous avons fait le choix de faire une structure par moteur, cette dernière contenant le timer qui correspond à la PWM qui sera appliquée, la direction (Forward ou Reverse), la vitesse ainsi que le channel pour le timer. Un deuxième type de structure a ensuite été créé, celle du PID qui contient les valeurs des coefficients du correcteur afin de bien asservir le moteur. Il faut cependant noter que nous n'avons pas eu le temps de bien mettre en place le correcteur, les valeures ne sont donc pas les bonnes. Nous avons ensuite eu l'idée d'une autre structure appelée Robot qui comprend les deux moteurs et les PID, nous ne nous en sommes au final pas servis car nous controlions chaque moteur séparément.

Dans le .c, on commence par initialiser chaque moteur, en fournissant le timer ainsi qu'un channel pour "Forward" et un autre pour "Reverse", une vitesse nulle et la direction Forward. (On retrouve aussi les initialisations des autres structures).
Il a ensuite fallu réaliser les différentes fonctions des moteurs, start, stop, ainsi que les changements de direction et les changements de vitesse.

En exemple, pour la fonction moteur_forward, le code commence par vérifier quel moteur appelle la fonction, ensuite la PWM correspondante au bon moteur Forward est démarrée. La direction Reverse est éteinte, et ce même si le moteur était arrété précédement.

# TOF

# Accéléromètre

L'accéléromètre est l'élément du robot qui va nous permettre de mettre à jour le rôle du robot, souris ou chat. C'est un module où l'on va directement écrire et lire dans les registres, accessibles grâce à leur adresse.

Il a donc fallu commencer par énumérer tous les registres qui nous intéressaient 

# LEDs

# Audio

# Raspi