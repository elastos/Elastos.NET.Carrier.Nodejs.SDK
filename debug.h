#ifndef __JNI_DEBUG_H__
#define __JNI_DEBUG_H__

#include <iostream>
#include <node_api.h>
using namespace std;


static void debug(char *text) {
    cout << text << endl;
}

static void statusCheck(napi_status status, char *text) {
    if (status == napi_ok) cout << text << " - success" << endl;
    else cout << text << " - failure" << endl;
}

#endif // __JNI_DEBUG_H__