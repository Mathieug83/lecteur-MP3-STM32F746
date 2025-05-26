# Utilisation de TouchGFX

TouchGFX ne fonctionne que sur windows, il s'installe depuis ce site "https://www.st.com/en/development-tools/touchgfxdesigner.html". Une fois installé, il faut aller dans le dossier "Utilities/PC_Software/TouchGFXDesigner/" est lancer l'executable. Cela installera TouchGFX Designer, c'est une application qui permet de créer une interface à l'aide de widget en drag and drop et de faire un minimum de logique, on peut ensuite générer et tout sera créé automatiquement. On peuut parmis les logiques possibles, il y a les appels de fonction que l'ont peut créer dans le dossier gui (c'est ce même dossier qui est disponible sur ce git). 

La piste posible pour qu'il y est des interactions entre des taches extern et TouchGFX serait de passer par des queues et d'utilisé la fonction "void MainView::handleTickEvent()" qui s'activer de façon périodique, je n'ai pas eu le temps d'implementer cette méthode
