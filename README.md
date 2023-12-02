# source-engine-metaballs Beta
Simple source engine metaballs implementation. Its in beta.

Based on [Paul Baker's project from 2002](https://www.paulsprojects.net/opengl/metaballs/metaballs.html).

*THIS IS FOR SOURCE CODE!*

## How to install
- Copy `src/game` folder to the `src` folder of your mod/game.
- (OPTIONAL) Copy `game/mod_hl2/materials` in your mod.
- Copy `game/mod_hl2/fgd/blobs.fgd` in your mod.
- Go to `src/game/client` of your mod/game, open `client_base.vpc` and add `$include "metaballs\metaballs.vpc"` somewhere.
- Go to `src/game/server` of your mod/game, open `server_base.vpc` and add `$include "metaballs\metaballs.vpc"` somewhere.

Basically thats it.

## How to use
- Open Options of the hammer. Add this fgd to the `Game Data Files`
- Use `point_blob_container` and `point_blob_element` in hammer.
