#Lecture des métadonnées

L'objectif est d'afficher le titre est l'artiste d'un fichier mp3. Il y a deux normes, l'IPV4 et l'IPV6. Le fichier mp3_storage.c contient les fonctions permettant la lecteur de fichier mp3. Il est adapté depuis les fonctions lisant les fichoer .bmp fournies dans le tp fatfs. 
Je veux afficher (sans TouchGFX) les métadonnées d'un fichier pourvoir défilier parmis ces derniers a l'aide d'un index


# Tache et Queue

task_Bouton : pour la gestion des boutons qui gère l'indexage
task_Affich_Pic : pour l'affichage des informations

QueueBPHandle : pour la communication entre les deux taches
