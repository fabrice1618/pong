# Pong

Code original : https://github.com/flightcrank/pong

## évolutions:
- Refactorisation du code
- Corrections de bugs
- Clarification de la game loop
- Ajout possibilité de changer de résolution
- Affichage du texte avec polices de caractères TTF
- Préparation d'un mode multi-joueurs

## installation environnement de développement ubuntu 20.04 desktop
```bash
sudo apt-get update && sudo apt-get upgrade
sudo apt-get install build-essential
sudo apt-get install libsdl2-dev
sudo apt-get install libsdl2-ttf-dev
```
## Compilation et exécution

```bash
$ make
$ ./pong
$ ./pong -f
```

# Modifications

## version 0.1

- Modification de la gestion du parametre -f (fullscreen)
- clarification du programme principal: isolation de la gestion des evenements clavier et de l'affichage
- gestion du jeu suivant un automate à états finis
- move_paddle: modification pour permettre par la suite une partie mulmtijoueur
- Ajout de la fermeture de la fenetre en cliquant sur la croix
- Textes par police TTF et plus par image
- adaptation de la taille du jeu avec le changement de résolution
- gestion du fullscreen

## version 0.2
- modification algorithme move_paddle_ai()