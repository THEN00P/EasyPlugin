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

## Credits
- [Electric1447](https://github.com/Electric1447) help and inspiration
- [Rinnegatamante](https://github.com/Rinnegatamante) for creating VitaDB
- [devnoname120](https://github.com/devnoname120) for a zip class I lifted from vhbb
- The music is [apollon - soap](https://modarchive.org/index.php?request=view_by_moduleid&query=158029)