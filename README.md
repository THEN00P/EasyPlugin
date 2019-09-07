# EasyPlugin
EasyPlugin is an easy to use hombrew application for the PSVITA that installs taiHENkaku plugins from [VitaDB](https://vitadb.rinnegatamante.it/#/plugins).

## "Planned features"
- Ability to change the plugin database.
- Touch controls
- Installing Homebrew apps that come with plugins (for now they get copied to ux0:tai/VPKS/ so you can install them using VitaShell)
- Display PSP icons

## Building

Prerequisites:
- [VitaSDK](https://vitasdk.org/)
- [libsqlite](https://github.com/VitaSmith/libsqlite)

Compiling:
```
cmake .
make
```
