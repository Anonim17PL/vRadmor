#include "config.h"
#include "ui_config.h"

#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
#pragma warning (disable : 4100)  /* Disable Unreferenced parameter warning */
#include <Windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/clientlib_publicdefinitions.h"
#include "ts3_functions.h"
#include "CSerialPort/CSerialPort.h"
#include "CSerialPort/CSerialPort.c"

static struct TS3Functions ts3Functions;

#ifdef _WIN32
#define _strcpy(dest, destSize, src) strcpy_s(dest, destSize, src)
#define snprintf sprintf_s
#else
#define _strcpy(dest, destSize, src) { strncpy(dest, src, destSize-1); (dest)[destSize-1] = '\0'; }
#endif

#define PLUGIN_API_VERSION 23

#define PATH_BUFSIZE 512
#define COMMAND_BUFSIZE 128
#define INFODATA_BUFSIZE 128
#define SERVERINFO_BUFSIZE 256
#define CHANNELINFO_BUFSIZE 512
#define RETURNCODE_BUFSIZE 128

static char* pluginID = NULL;

#ifdef _WIN32
/* Helper function to convert wchar_T to Utf-8 encoded strings on Windows */
static int wcharToUtf8(const wchar_t* str, char** result) {
    int outlen = WideCharToMultiByte(CP_UTF8, 0, str, -1, 0, 0, 0, 0);
    *result = (char*)malloc(outlen);
    if(WideCharToMultiByte(CP_UTF8, 0, str, -1, *result, outlen, 0, 0) == 0) {
        *result = NULL;
        return -1;
    }
    return 0;
}

#endif

/*********************************** Required functions ************************************/
/*
 * If any of these required functions is not implemented, TS3 will refuse to load the plugin
 */


using namespace std;
int portnoset,portno;
int portopened;
PORT port;
HANDLE mainth;

/* Unique name identifying this plugin */
const char* ts3plugin_name() {
#ifdef _WIN32
    /* TeamSpeak expects UTF-8 encoded characters. Following demonstrates a possibility how to convert UTF-16 wchar_t into UTF-8. */
    static char* result = NULL;  /* Static variable so it's allocated only once */
    if(!result) {
        const wchar_t* name = L"vRadmor";
        if(wcharToUtf8(name, &result) == -1) {  /* Convert name into UTF-8 encoded result */
            result = "vRadmor";  /* Conversion failed, fallback here */
        }
    }
    return result;
#else
    return "vRadmor";
#endif
}

/* Plugin version */
const char* ts3plugin_version() {
    return "0.1";
}

/* Plugin API version. Must be the same as the clients API major version, else the plugin fails to load. */
int ts3plugin_apiVersion() {
    return PLUGIN_API_VERSION;
}

/* Plugin author */
const char* ts3plugin_author() {
    /* If you want to use wchar_t, see ts3plugin_name() on how to use */
    return "Anonim17PL/A17PL/Anonim18PL/A18PL";
}

/* Plugin description */
const char* ts3plugin_description() {
    /* If you want to use wchar_t, see ts3plugin_name() on how to use */
    return "Bridge plugin for the serial port radio interface.\r\nMostek dla interfejsu radiotelefonu portu szeregowego.";
}

/* Set TeamSpeak 3 callback functions */
void ts3plugin_setFunctionPointers(const struct TS3Functions funcs) {
    ts3Functions = funcs;
}

bool abortthread = false;

DWORD WINAPI MainThread( LPVOID lpParam )
{
int init_done = 0;
while(!abortthread) //Infinite loop
{
    //const char init_response[80] = "HELLO_TERMINAL\n";
    const char norm_response[80] = "OK\n";

    //Uwaga! Użytkownik używa półautomatycznego systemu dołączania! (vRadmor)
    QString INFO_norm = "[i]Note! The user uses a semi-automatic device for joining channels! (vRadmor)\r\nUwaga! Użytkownik używa półautomatycznej centralki systemu dołączania! (vRadmor)[/i]";
    QString INFO_zew = "[i]Note! The user uses a semi-automatic device for joining channels [b]with radio call capability[/b]! (vRadmor)\r\nUwaga! Użytkownik używa półautomatycznej centralki systemu dołączania [b]z możliwością wywoływania[/b]! (vRadmor)[/i]";
    if (portopened == 1) {
        char recivestr[80];
        ReciveData(port, recivestr, 80);
        QString string(recivestr);
        char returnCode[RETURNCODE_BUFSIZE];
        ts3Functions.createReturnCode(pluginID, returnCode, RETURNCODE_BUFSIZE);
        init_done=2;
        if (init_done==2) {
        uint64 idch = atoi(recivestr);
        anyID serverConnectionHandlerID = ts3Functions.getCurrentServerConnectionHandlerID();
        anyID CliID;

        if (ts3Functions.getClientID(serverConnectionHandlerID, &CliID) == ERROR_ok) {
            if (idch > 0) {

                /* Get own clientID */


                /* Create return code for requestClientMove function call. If creation fails, returnCode will be NULL, which can be
                 * passed into the client functions meaning no return code is used.
                 * Note: To use return codes, the plugin needs to register a plugin ID using ts3plugin_registerPluginID */


                /* In a real world plugin, the returnCode should be remembered (e.g. in a dynamic STL vector, if it's a C++ plugin).
                 * onServerErrorEvent can then check the received returnCode, compare with the remembered ones and thus identify
                 * which function call has triggered the event and react accordingly. */

                /* Request joining specified channel using above created return code */
                if (ts3Functions.requestClientMove(serverConnectionHandlerID, CliID, idch, "", returnCode) == ERROR_ok) {
                 SendData(port, norm_response);
            }
            }
            if (string == "INFO") {
                uint64 ChID;
                if(ts3Functions.getChannelOfClient(serverConnectionHandlerID, CliID, &ChID) == ERROR_ok) {
                    ts3Functions.requestSendChannelTextMsg(serverConnectionHandlerID,INFO_norm.toStdString().c_str(),ChID,returnCode);
                  }

                }
            if (string == "INFO_ZEW") {
                uint64 ChID;
                if(ts3Functions.getChannelOfClient(serverConnectionHandlerID, CliID, &ChID) == ERROR_ok) {
                    ts3Functions.requestSendChannelTextMsg(serverConnectionHandlerID,INFO_zew.toStdString().c_str(),ChID,returnCode);
                  }

                }
        }
        }

    }
}
}

/*
 * Custom code called right after loading the plugin. Returns 0 on success, 1 on failure.
 * If the function returns 1 on failure, the plugin will be unloaded again.
 */
int ts3plugin_init() {
    char appPath[PATH_BUFSIZE];
    char resourcesPath[PATH_BUFSIZE];
    char configPath[PATH_BUFSIZE];
    char pluginPath[PATH_BUFSIZE];

    /* Your plugin init code here */
    printf("PLUGIN: init\n");
    mainth = CreateThread(NULL, 0, MainThread, NULL, 0, NULL);

    /* Example on how to query application, resources and configuration paths from client */
    /* Note: Console client returns empty string for app and resources path */
    ts3Functions.getAppPath(appPath, PATH_BUFSIZE);
    ts3Functions.getResourcesPath(resourcesPath, PATH_BUFSIZE);
    ts3Functions.getConfigPath(configPath, PATH_BUFSIZE);
    ts3Functions.getPluginPath(pluginPath, PATH_BUFSIZE, pluginID);

    printf("PLUGIN: App path: %s\nResources path: %s\nConfig path: %s\nPlugin path: %s\n", appPath, resourcesPath, configPath, pluginPath);

    return 0;  /* 0 = success, 1 = failure, -2 = failure but client will not show a "failed to load" warning */
    /* -2 is a very special case and should only be used if a plugin displays a dialog (e.g. overlay) asking the user to disable
     * the plugin again, avoiding the show another dialog by the client telling the user the plugin failed to load.
     * For normal case, if a plugin really failed to load because of an error, the correct return value is 1. */
}



/* Custom code called right before the plugin is unloaded */
void ts3plugin_shutdown() {
    /* Your plugin cleanup code here */
    printf("PLUGIN: shutdown\n");
    abortthread = true;
    WaitForSingleObject(mainth, 2000);
    CloseHandle(mainth);
    ClosePort(port);
    /*
     * Note:
     * If your plugin implements a settings dialog, it must be closed and deleted here, else the
     * TeamSpeak client will most likely crash (DLL removed but dialog from DLL code still open).
     */

    /* Free pluginID if we registered it */
    if(pluginID) {
        free(pluginID);
        pluginID = NULL;
    }
}

/****************************** Optional functions ********************************/
/*
 * Following functions are optional, if not needed you don't need to implement them.
 */

/* Tell client if plugin offers a configuration window. If this function is not implemented, it's an assumed "does not offer" (PLUGIN_OFFERS_NO_CONFIGURE). */
int ts3plugin_offersConfigure() {
    printf("PLUGIN: offersConfigure\n");
    /*
     * Return values:
     * PLUGIN_OFFERS_NO_CONFIGURE         - Plugin does not implement ts3plugin_configure
     * PLUGIN_OFFERS_CONFIGURE_NEW_THREAD - Plugin does implement ts3plugin_configure and requests to run this function in an own thread
     * PLUGIN_OFFERS_CONFIGURE_QT_THREAD  - Plugin does implement ts3plugin_configure and requests to run this function in the Qt GUI thread
     */
    return PLUGIN_OFFERS_CONFIGURE_QT_THREAD;  /* In this case ts3plugin_configure does not need to be implemented */
}

/* Plugin might offer a configuration window. If ts3plugin_offersConfigure returns 0, this function does not need to be implemented. */
void ts3plugin_configure(void* handle, void* qParentWidget) {
    printf("PLUGIN: configure\n");
    config* pConf = new config(static_cast<QWidget*>(qParentWidget));

    if (pConf->isVisible()) {
            // Window is already displayed somewhere, bring it to the top and give it focus
            pConf->raise();
            pConf->activateWindow();
        } else {
            pConf->show();
        }
}

/*
 * If the plugin wants to use error return codes, plugin commands, hotkeys or menu items, it needs to register a command ID. This function will be
 * automatically called after the plugin was initialized. This function is optional. If you don't use these features, this function can be omitted.
 * Note the passed pluginID parameter is no longer valid after calling this function, so you must copy it and store it in the plugin.
 */
void ts3plugin_registerPluginID(const char* id) {
    const size_t sz = strlen(id) + 1;
    pluginID = (char*)malloc(sz * sizeof(char));
    _strcpy(pluginID, sz, id);  /* The id buffer will invalidate after exiting this function */
    printf("PLUGIN: registerPluginID: %s\n", pluginID);
}

config::config(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::config)
{
    ui->setupUi(this);
    ui->spinBox->setValue(portnoset);
}

config::~config()
{
    delete ui;
}


void config::on_spinBox_valueChanged(int arg1)
{
    portnoset = arg1;
}


void config::on_pushButton_clicked()
{
    if (portnoset <= 0) {
        ClosePort(port);
        portopened = 0;
    } else {
        ClosePort(port);
        port = OpenPort(portnoset);
        SetPortBoudRate(port, 9600);
        portopened = 1;
        const char init_response[80] = "HELLO_TERMINAL\n";
        SendData(port, init_response);
    }

    // To avoid having two threads running simultaneously, the previous thread is aborted.
    close();
}

