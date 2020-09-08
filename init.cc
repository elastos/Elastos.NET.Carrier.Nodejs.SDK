#include <node_api.h>

extern int registerCarrierMethods(napi_env env, napi_value *exports);

napi_value Init(napi_env env, napi_value exports) {

    registerCarrierMethods(env, &exports);
    return exports;

}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)


