# RC-500 Loop Station Editor

![Main Screen](../images/screenshot.png)

Ce logiciel permet de créer, d'éditer et de sauver un fichier de configuration
pour le looper Boss RC-500.

ATTENTION: Ce logiciel est en phase de test, en cours de développement, qui n'a aucun 
lien avec la société Boss/Roland, et contient probablement énormement de bogues. Donc,
avant de l'utiliser, n'oubliez pas de faire une copie de sauvegarde de vos fichiers.

## Téléchargement

Vous pouvez trouver les binaires pré-compilés pour Windows 8.1 et 10, Linux et
MacOs Catalina [ici](http://simplecpp.free.fr/boss-rc500/downloads/).

Actuellement, j'utilise le logiciel sous Linux Debian Bullseye.

## Utilisation de base

Quand le logiciel démarre, il charge le fichier de configuration par défaut, 
`MEMORY_DEFAULT.RC0` présent dans le répertoire des ressources.
Ainsi, vous pouvez directement créer votre fichier de configuration sans avoir à brancher
le RC-500 à votre PC.

If your looper is connected, you can directly open the MEMORY1.RC0 file
from the connected disk, in ROLAND/DATA directory.

The window title will show you the filename you are currently editing.

## Menu Principal

![Main Menu](../images/menu.png)

### New

Create a new database by reading the default ones provided in
resources/templates/MEMORY_DEFAULT.RC0 and resources/templates/SYSTEM_DEFAULT.RC0
A new database has no name, you will need to provide one when you
will save it to disk.

### Open

Open an existing directory which contains the memory and system
files (ROLAND/DATA for example).

### Save

Save the current databases into files.
For a new database, you will be prompted for a directory name on your
disk (or directly in your looper).

All files needed by the Boss RC500 looper will we writtent to disk
`MEMORO1.RC0`, `MEMORY2.RC0`, `SYSTEM1.RC0` et `SYSTEM2.RC0`.

It allows the looper to take in account the new files else it tries
to load one that contains the checksum (no computed by the tool)

### Memory

![Memory](../images/memory.png)

This list allows to change the current memory you are working on.
The list displays an extended version of the memory name. By default, only
the first 12 chars are displayed on the looper screen. Here, you can
write a longer description, only the first 12 will be seen on the looper
screen, but you can still see the longer form here.

Edit allows to change the current memory name.

### Copy

![Copy](../images/copy.png)

This option allow you to copy the current memory slot to range of memory  slots.
So you can easily duplicate your setting from a slot to some other quickly.

At the right of the Copy button, Advanced copy ('>') allow to copy the
selected parts to the destination.

![Advanced Copy](./images/copy_advanced.png)

### Assigns

![Assign](../images/assign.png)

This window allows you to edit the ASSIGN part of the looper.

#### Apply

Apply will apply the changes to the current memory selected in the main window.

#### Cancel

Cancel will revert any changes you have made.

### System

![System](../images/system.png)

This window allows you to edit the SYSTEM part of the looper.

#### Apply 

Apply will apply the changes to global system database.

#### Cancel

Cancel will revert any changes you have made.
