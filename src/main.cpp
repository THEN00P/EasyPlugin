#include <vita2d.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/power.h> 
#include <psp2/io/fcntl.h>
#include <string>

#include "main.hpp"
#include "net/download.hpp"
#include "utils/filesystem.hpp"
#include "utils/search.hpp"
#include "utils/format.hpp"

#include "screens/list.hpp"
#include "screens/details.hpp"
#include "screens/popup.hpp"

SceCtrlData pad;

vector<AppInfo> getAppData() {
    vector<AppInfo> ret;

    SceIoDirent dirInfo;
    SceUID folder;

    AppInfo app;

    if( (folder = sceIoDopen( "ur0:appmeta/" )) != NULL) {
        while (sceIoDread(folder, &dirInfo) != NULL) {
            app.appID = dirInfo.d_name;
            app.title = "name";
            app.icon = vita2d_load_PNG_file(("ur0:appmeta/"+app.appID+"/icon0.png").c_str());

            ret.push_back(app);
        }
    }

    return ret;
}

int main() {
    Filesystem fs;

    vita2d_init();
    vita2d_set_clear_color(RGBA8(255,255,255,255));

    vita2d_texture *bgIMG = vita2d_load_PNG_file("ux0:app/ESPL00009/resources/bg.png");
    
    httpInit();
    netInit();
    curlDownload("http://rinnegatamante.it/vitadb/list_plugins_json.php", "ux0:data/Easy_Plugins/plugins.json");

    SharedData sharedData;

    if(doesDirExist("ux0:tai")) sharedData.taiConfigPath = "ux0:tai/";
    else if(doesDirExist("ur0:tai")) sharedData.taiConfigPath = "ur0:tai/";
    else return -1;

    sharedData.taiConfig = fs.readFile(sharedData.taiConfigPath+"config.txt");

    sharedData.plugins = json::parse(fs.readFile("ux0:data/Easy_Plugins/plugins.json"));

    sharedData.appData = getAppData();

    sharedData.original = sharedData.plugins;

    List listView;
    Popup popupView;
    Details detailsView;

    while(1) {
        sceCtrlPeekBufferPositive(0, &pad, 1);
        vita2d_start_drawing();
        vita2d_clear_screen();

        vita2d_draw_texture(bgIMG, 0, 0);

        if(pad.buttons != SCE_CTRL_CROSS) sharedData.blockCross = false;
        
        if(sharedData.scene == 0) listView.draw(sharedData, pad.buttons);
        
        if(sharedData.scene == 1) detailsView.draw(sharedData, pad.buttons);

        if(sharedData.scene == 2) popupView.draw(sharedData, pad.buttons);

        vita2d_end_drawing();
        vita2d_swap_buffers();

        if(pad.buttons == SCE_CTRL_SELECT) {
            break;
        }
        if(pad.buttons == SCE_CTRL_START) {
            scePowerRequestColdReset();
        }
    }

    for(int i=0;i<sharedData.appData.size();i++) vita2d_free_texture(sharedData.appData[i].icon);

    httpTerm();
    netTerm();
    vita2d_free_font(sharedData.font);
    vita2d_free_texture(bgIMG);
    listView.free();
    detailsView.free();
    vita2d_fini();
    
    sceKernelExitProcess(0);
    return 0;
}
