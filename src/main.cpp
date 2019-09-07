#include <vita2d.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/power.h> 
#include <psp2/io/fcntl.h>
#include <psp2/sqlite.h>
#include <psp2/apputil.h>
#include <psp2/display.h>
#include <psp2/ime_dialog.h>
#include <string>

#include "main.hpp"
#include "sqlite3.h"
#include "net/download.hpp"
#include "utils/filesystem.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/search.hpp"

#include "screens/screens.hpp"
#include "screens/list.hpp"
#include "screens/details.hpp"
#include "screens/popup.hpp"

extern unsigned int basicfont_size;
extern unsigned char basicfont[];

string taiConfig = "";
string taiConfigPath = "";
Screens screens;
json plugins;
json originalPlugins;
vector<AppInfo> appData;
vita2d_font *font = vita2d_load_font_mem(basicfont, basicfont_size);

static void initSceAppUtil()
{
	// Init SceAppUtil
	SceAppUtilInitParam init_param;
	SceAppUtilBootParam boot_param;
	memset(&init_param, 0, sizeof(SceAppUtilInitParam));
	memset(&boot_param, 0, sizeof(SceAppUtilBootParam));
	sceAppUtilInit(&init_param, &boot_param);

	// Set common dialog config
	SceCommonDialogConfigParam config;
	sceCommonDialogConfigParamInit(&config);
	sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_LANG, (int *)&config.language);
	sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_ENTER_BUTTON, (int *)&config.enterButtonAssign);
	sceCommonDialogSetConfigParam(&config);
}

AppInfo::AppInfo(string p_appID, string p_title, string fileLocation) {
    appID = p_appID;
    title = p_title;
    icon = fileLocation.find(".dds") == string::npos ? vita2d_load_PNG_file(fileLocation.c_str()) : NULL;
}

static int callback(void *data, int argc, char **argv, char **column_name) {
    vector<AppInfo> *ret = (vector<AppInfo>*) data;

	ret->emplace_back(string(argv[0]), string(argv[1]), string(argv[2]));
	return 0;
}

static int getAppData(vector<AppInfo> &ret) {
    SceIoDirent dirInfo;
    SceUID folder;

	ret.emplace_back("ALL", "All", "");
	ret.emplace_back("main", "Livearea (main)", "");

    sqlite3 *db = NULL;

    sceSysmoduleLoadModule(SCE_SYSMODULE_SQLITE);

    sqlite3_rw_init();

    int rc = sqlite3_open_v2("ur0:shell/db/app.db", &db, SQLITE_OPEN_READONLY, NULL);
    if(rc != SQLITE_OK) {
        return -1;
    }

    sqlite3_exec(db, "SELECT titleId, title, iconPath FROM tbl_appinfo_icon WHERE NOT titleId=\"NULL\" ORDER BY title ASC", callback, &ret, NULL);

	if (db != NULL)
		sqlite3_close(db);
	sqlite3_rw_exit();

    sceSysmoduleUnloadModule(SCE_SYSMODULE_SQLITE);

    return 0;
}

int main() {
    vita2d_init();
    initSceAppUtil();
    httpInit();
    netInit();

    vita2d_set_clear_color(RGBA8(255,255,255,255));

    vita2d_texture *bgIMG = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/bg.png");

    Filesystem::mkDir("ux0:data/Easy_Plugins");
    curlDownload("http://rinnegatamante.it/vitadb/list_plugins_json.php", "ux0:data/Easy_Plugins/plugins.json");

    if(doesDirExist("ux0:tai")) taiConfigPath = "ux0:tai/";
    else if(doesDirExist("ur0:tai")) taiConfigPath = "ur0:tai/";
    else return -1;

    taiConfig = Filesystem::readFile(taiConfigPath+"config.txt");

    plugins = json::parse(Filesystem::readFile("ux0:data/Easy_Plugins/plugins.json"));

    getAppData(appData);

    originalPlugins = plugins;

    Input input;

    List *list;
    sceClibPrintf("Main \n");

    screens.addScreen(list);

    while(1) {
        vita2d_start_drawing();
        vita2d_clear_screen();

        vita2d_draw_texture(bgIMG, 0, 0);
        
        input.updateLoop();
        screens.update(input);

        vita2d_end_drawing();
        vita2d_common_dialog_update();
        vita2d_swap_buffers();
        sceDisplayWaitVblankStart();

        if(input.newButtonsPressed(SCE_CTRL_SELECT)) {
            break;
        }
    }

    httpTerm();
    netTerm();
    vita2d_free_font(font);
    vita2d_free_texture(bgIMG);
    for(int i=0; i<appData.size();i++) {
        if(appData[i].icon != NULL)
        vita2d_free_texture(appData[i].icon);
    }
    vita2d_fini();
    
    sceKernelExitProcess(0);
    return 0;
}
