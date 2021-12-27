# Modifications

## installation ubuntu 20.04 desktop
```
sudo apt-get update && sudo apt-get upgrade
sudo apt-get install build-essential
sudo apt-get install libsdl2-dev
sudo apt-get install libsdl2-ttf-dev
```

gcc src/main.c -o bin/prog -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2

## version 0.1

- Modification de la gestion du parametre -f (fullscreen)
- clarification du programme principal: isolation de la gestion des evenements clavier et de l'affichage
- gestion du jeu suivant un automate à états finis
- move_paddle: modification pour permettre par la suite une partie mulmtijoueur
- Ajout de la fermeture de la fenetre en cliquant sur la croix
- Textes par police TTF et plus par image
- adaptation de la taille du jeu avec le changement de résolution
- gestion du fullscreen
