#include "CwxAppProcessMgr.h"
#include "CwxEchoClientApp.h"

int main(int argc, char** argv){
    CwxEchoClientApp* pApp = new CwxEchoClientApp();
    if (0 != CwxAppProcessMgr::init(pApp)) return 1;
    CwxAppProcessMgr::start(argc, argv, 200, 300);
    return 0;
}
