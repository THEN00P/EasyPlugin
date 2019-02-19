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

AppInfo::AppInfo(string p_appID, string p_title, string fileLocation) {
    appID = p_appID;
    title = p_title;
    icon = vita2d_load_PNG_file(fileLocation.c_str());
}

string logging = "";

int getAppData(vector<AppInfo> &ret) {
    SceIoDirent dirInfo;
    SceUID folder;

    //*works replace with sql later*
    if( (folder = sceIoDopen( "ux0:app/" )) != NULL) {
        while (sceIoDread(folder, &dirInfo) != NULL) {

            //--> redo this part to be more compact at some point
            void *sfo_buffer = NULL;
            int sfo_size = allocateReadFile(string("ux0:app/"+string(dirInfo.d_name)+"/sce_sys/param.sfo").c_str(), &sfo_buffer);
            if (sfo_size < 0)
                return sfo_size;

            char name[48];
            getSfoString(sfo_buffer, "TITLE", name, sizeof(name));
            //<--

            ret.emplace_back(string(dirInfo.d_name), string(name), ("ur0:appmeta/"+string(dirInfo.d_name)+"/icon0.png").c_str());
        }
    }

    sceIoDclose(folder);

    return 0;
}

int main() {
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

    sharedData.taiConfig = Filesystem::readFile(sharedData.taiConfigPath+"config.txt");

    sharedData.plugins = json::parse(Filesystem::readFile("ux0:data/Easy_Plugins/plugins.json"));

    getAppData(sharedData.appData);

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

    httpTerm();
    netTerm();
    vita2d_free_font(sharedData.font);
    vita2d_free_texture(bgIMG);
    vita2d_free_texture(sharedData.appData[0].icon);
    listView.free();
    detailsView.free();
    vita2d_fini();
    
    sceKernelExitProcess(0);
    return 0;
}
