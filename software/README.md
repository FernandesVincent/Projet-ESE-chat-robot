# Compte rendu partie software 

Ce compte rendu explique tous les codes du répertoire et rapporte ainsi des précisions sur les programmes du projet. 

## acc_test

Ce répertoire contient un projet STM32 avec pour cible le MCU STM32G431. 
Comme l'accéléromètre de notre carte ne fonctionnait pas, on a d'abord fait un programme qu'on a testé sur une ancienne carte.
Mais comme le driver est relativement bien écrit, je pense qu'il est facilement portable sur une autre cible STM.

Ce projet contient un programme qui configure l'accéléromètre ADXL350 en interruption sur SINGLE TAP, et permet ainsi de détecter les collisions avec les autres robots, pour ensuite changer de rôle chat-souris.

#### Configuration de l'accéleromètre 

<img src="https://www.plantuml.com/plantuml/png/bPBDZjCm483lUOhXj621L5Z4ORcijmaqKfkMDALxG8fCEkrQSfpWdnK2yIuSwP5N82_6sAM8LbpsKglZyJVVJERQEc6SRrNooDrXYz0h8NNYf5C8kPPE2YMjSBBJiF7GYDwXDy3Js-dboyia0RXAXxqLZPFtVx8Gr8NK3etnVOnyq23kiG769dnQ5-_pSPqIywF2l7pt-0bZhvEa9miPie_eEXI7hrI0Il20HmFar7OR-a7h3KOgqIbgS2Ujen1ZY_awA-jnDMKCKCCiM_BbginWDaypAXHvuqvlvOuG4RdjJ2iSYLlOT5eFUxH7uPHNl7upBsUyYhZa6PJKRgTyT6swjbV33uSKNoYnG_Zry_chtV_A2DYQOV_9esuGDXV3lhaJsacbg28zf1aDt-75Q0IJBuVYfrtThEkILnbZv-pZq2tG24LBCYRquM6P5s-dMLtnnP5d_m8jN94nOzMap9QJaCFOKx3efGfFdUZZTRegAKuHCiT_BzPvaSxNOIAE_ar9hAX29hKm2aDf4KXANw1rf6K1DQJprSr19nBehFbjjWolnETWRwIb0mz3TNZiSoRw7itvIfqtTpgilAZgMUY98AVxyr1ALd2oZg3aMzns-lmFNVPCd-pwDUgDRzLl" width="300">

#### Traitement de l'interruption 

La fonction `void ADXL343_IntCallback(h_ADXL343_t *h_ADXL343, uint16_t GPIO_Pin)` est a appelé dans la fonction de callback d'interruption `void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)`.
Dans cette fonction, une lecture dans le registre **`INT_SOURCE`** est nécessaire pour clear le flag d'interruption et ainsi permettre la réception de la prochaine interruption. 

## ancien_pcb

## lidar_nucleo

Ce répertoire contient un code permettant de réceptionner et traiter les données lidar et détecter la cible qui est la lidar d'un autre robot.

<img src="https://www.plantuml.com/plantuml/png/TP91ojim48NtSueXguHI29HsaKK9D9licf9TMyH4cXeHMNQaSNicdyCNwrYAIrpu5pRey_DxJneTir2IeGqWNW8Zd3mrYTgMqJ7cQNmC72rZcaRBlVWk8WjA8Y_SSfHPvReOfv4p1kyePM2bqoXiYrm5rjy20t1qM580UWtrrlUa3fjlRDd_vcjtTe7D-OgMGhYHlUEFM3tDGiolrBN6vxJTRP0oVdboq9e3u-PXXgpMM9P5SLbd2UMxgPxAiuyDpgX8lohW8Qr_i9niuEyVFkr_riIoXZTT7IyR_-TxAcXs1bWAu_lFUC439lg3ZeJWecHfyCfXOVgvPXzmrT_CikrEpwhYJ6qVb1T4mSzJOhJFsjL2pLQj87HTZz8DCazHdu_xFROPS57L_0F-jqSZb1gM6XMRmFWEdSz2qPRx4-Xbzd3Kbzwqlm00" width="800">

#### Reception et traitement de données lidar 

- Le LiDAR communique avec le microcontrôleur via une liaison UART.
La réception des données repose sur l’utilisation de l’interruption IDLE, déclenchée lorsque la ligne RX reste inactive pendant un certaine durée signifiant la fin d'une trame.
Cette interruption est exploitée pour détecter la fin d’une trame de données et initier le traitement des informations reçues.

- Le traitement des données est fait en suivant le [manuel du protocole](https://github.com/YDLIDAR/YDLidar-SDK/blob/master/doc/YDLidar-SDK-Communication-Protocol.md) du constructeur. 
Après la réception d’une trame complète, celle-ci est d’abord validée en vérifiant la présence du header ainsi que l’intégrité des données à l’aide du checksum.
Une fois la trame jugée valide, les champs principaux sont extraits, notamment le nombre d’échantillons (LSN), l’angle de départ de mesure (FSA) et l’angle de fin de mesure (LSA).
Ces informations permettent ensuite de calculer la correspondance entre chaque échantillon de distance et son angle de mesure associé.

#### Algo de détection de cible

- L’algorithme de détection de cible repose sur la connaissance a priori des dimensions de la cible, constituée d’un cylindre de 6 cm de diamètre, correspondant à un second LiDAR.
- Les données de distance, stockées sous forme circulaire, sont parcourues afin d’analyser l’environnement sur 360°.
- Pour chaque mesure de distance, l’ouverture angulaire théorique correspondant à un cylindre de 6 cm est calculée en fonction de la distance mesurée.
- Cette ouverture angulaire permet d’identifier des ensembles d’échantillons contigus susceptibles de correspondre à une cible réelle.
Une liste de cibles potentielles est alors extraite, parmi laquelle seule la cible la plus proche est conservée, les objets éloignés présentant une probabilité plus élevée de faux positifs.

## tof_g747

#### Configuation des capteurs TOF

<img src="https://www.plantuml.com/plantuml/svg/VLFDRjD04BxxAKQz9BaH43s0a5eIjKIgrGW7r0EIjNZ7pY9xrvgTZK2exyArpv4NOxnreWq1ZfxvVhxP6LzvLiIXQpCst2Aih66ZMkCL6sT1JuA5IlMCWM3ZRc3AhYwDW6PPzkQwOhD3WXQFcEb3yU7J1jx1VFPM49_lNh-wU_5GBXMhGNfwKYrONAfZlI4NUbY8-rSQxSDH-8iTd2lLIlUv-JDHMYFLZhgFM1Uzqhcogi4EhUXcvv348UaNoFTrxP6JzX0xDnMv71bf2FbuY7bH_F2C7GY1ZMtyHSopSBOsJI0aODD97JhqyfcwvAYa5EDj8kPzq0roz2cmSbPxSJeLwvp6KGe48OuXbLpYpbIO2-YCURjUtOEnZ4IX7sPAURUzSS_Oo7iSg5jb6-4fJUWzmkhbujpfMiUkl5wEp8Im_mNVN211Lu8OsZ0ziAHGSeort3zrYYXUZ1_7yLbo3PG-JdeJuppZLI2qatGa40fu3AOTpyGZbqT0oLknsRfMJsT_B6aawSc_7cXrg6yE6iFGEzLYe267DfwLhU83o5vsI6m8qCAD-RvseZg_d5-MVBgJlwNN-wTUUHy77uHZiF-lAhj2guS_zpS0" width="300">

- Les capteurs TOF VL53L0X fonctionne avec une communication I2C donc les registres ne sont pas ouvertement indiquer dans la datasheet qui nous recommande d'utiliser le driver fourni par le constructeur.
- Comme nous prévoyons 6 capteurs sur un même bus I2C et que l'addresse par défaut des TOF est la même, nous utilisons le pin XSHUT du capteur pour activer puis configurer sans oublier de changer l'addresse device des TOF un à un, pour pouvoir ensuite manipuler les 6 TOFs correctement.
- Les étapes de configuation suivent les étapes indiquer dans le [manuel de référence](https://www.st.com/resource/en/user_manual/um2039-world-smallest-timeofflight-ranging-and-gesture-detection-sensor-application-programming-interface-stmicroelectronics.pdf)

#### Traitement des interruptions

- Le pin d'interruption GPIO détectant un front montant correspond au tof dont le dépassement de seuil est détecté, on utilise `VL53L0X_ClearInterruptMask(tof_devices[i].dev, VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_HIGH);` pour clear le flag d'interrupion du côté du TOF pour que celui-ci puisse envoyer les prochaines interruptions.



