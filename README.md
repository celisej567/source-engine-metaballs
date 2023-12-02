# source-engine-metaballs Beta
Simple source engine metaballs implementation. Its in beta.

Based on [Paul Baker's project from 2002](https://www.paulsprojects.net/opengl/metaballs/metaballs.html).

*THIS IS FOR SOURCE CODE!*

You can make pull requests or issues to give me some feedback about problems and their fixes.

## How to install
- Copy `src/game` folder to the `src` folder of your mod/game.
- (OPTIONAL) Copy `game/mod_hl2/materials` in your mod.
- Copy `game/mod_hl2/fgd/blobs.fgd` in your mod.
- Go to `src/game/client` of your mod/game 's source code, open `client_base.vpc` and add `$include "metaballs\metaballs.vpc"` somewhere.
- Go to `src/game/server` of your mod/game 's source code, open `server_base.vpc` and add `$include "metaballs\metaballs.vpc"` somewhere.
- Compile the code.

Basically thats it.

## How to use
- Open Options of the hammer. Add this fgd to the `Game Data Files`
- Use `point_blob_container` and `point_blob_element` in hammer.

### Remember
- Do not set Grid size bigger then 20. `CMeshBuilder` cant make more then 32k verticies.
- Its ok to have issues with lighting with this implementation. `CMeshBuilder` is peace of shit.

## Why is it an entity?
I wanted to make it work on SDK 2013 as well as on any other source game. SDK 2013 dont have particles source code. I whould like to make it to be a particle, but then  wont make it for SDK 2013 and even wont be able to make it opensource.
