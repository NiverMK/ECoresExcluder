# Description
On processors with e-cores (especially on notebooks or low TDP systems) when a game is trying to utilize all available cores, it may cause stutters and fps drops.
Utility automatically detects amount of p- and e-cores and sets correct process affinity mask to allow it use only p-cores.

# How to use
Use any dll plugin loader for required game like [Native Mod Loader](https://www.nexusmods.com/baldursgate3/mods/944?tab=description.) for Baldur's Gate 3.
Tested with Baldur's Gate 3 on Intel Core-i9 12900H