# libmy_secmalloc

*XXXXXXXXXXXXXXXXXXXXXXXXXXX*

## Présentation

Ce projet de groupe s'inscrit dans le cadre du cursus de 1ere année à l'école 2600.

Développé uniquement en C.

## Architecture


- **Secmalloc**
- - ``src/`` *Répertoire des sources du secmalloc*
- - ``build/`` *Répertoire de build du secmalloc*

- **Debug** 
- - ``debug/gdb_test.gdb`` *Macro gdb en guidage au debug*
- - ``debug/`` *Fichiers .c aidant uniquement au debug de l'application / Indépendants des tests unitaires*

- **Tests**
- - ``tests/`` *Répertoire de tests unitaires*


## Compilation

### Secmalloc

Pour compiler le projet ``make clean & make dynamic`` 

Utiliser ensuite le ``build/libmy_secmalloc.so`` dans la variable d'environement ``LD_PRELOAD``

Ex:

  - ``LD_PRELOAD=build/libmy_secmalloc.so ls``

### Tests Unitaires

Pour compiler les tests ``cd tests && make clean && make``

Pour lancer les tests ``./test``


## Logs

Une variable d'environement est disponible pour le log.

> NOTE: Tous les logs sont envoyés sur stderr

Il y'a 3 niveaux de level:

- ``LOG_INFO`` 1

- ``LOG_WARNING`` 2

- ``LOG_ERROR`` 4
  

La variable d'environement permet de gérer l'affichage des logs, par défaut ``LOG_LVL`` est utilisé.

L'erreur est affichée à partir du moment où LOG_LVL&level = level

## Exemples

Pour afficher uniquement ``LOG_WARNING`` & ``LOG_ERROR`` et exécuter la commande ``ls -lah``

```
LOG_LVL=6 LD_PRELOAD=build/libmy_secmalloc.so ls -lah
```

Le programme possède une bonne stabilité et secmalloc permet de lancer de nombreux programmes.

Programmes testés et fonctionnels (sur ubuntu 22.04):

 - ``ssh``
 - ``curl``
 - ``python3``
 - ``tree``
 - ``sh``
 - ``bash``
 - ``ls``

Programmes rencontrants des erreurs:

 - Programmes en java / rust
 - ``vim``
  

## Notes 

> Le programme a une pool de donnée dédiée et **fixe**. Si le programme prend trop de mémoire, une erreur est levée et le programme s'arrète. 

> En raison de la fusion (merge) de plusieurs blocs, des blocs plus petit peuvent être stockées dans des nodes présentant une mémoire plus grande. Essayer de déclencher un heap overflow après avoir fait plusieurs opérations de free peut ne produire aucun résultat, dans le cas où il reste de l'espace disponibles dans le node contenant le bloc alloué.

