#ifndef __JNI_CARRUER_HADNDLER_H__
#define __JNI_CARRUER_HADNDLER_H__

#include <node_api.h>
#include "include/ela_carrier.h"

typedef struct HandlerContext {
    ElaCarrier* nativeCarrier;
    int interval;
    napi_env env;
    napi_ref object;
    napi_ref onIdleRef;
    napi_ref onConnectionRef;
    napi_ref onReadyRef;
    napi_ref onSelfInfoChangedRef;
    napi_ref onFriendConnectionRef;
    napi_ref onFriendInfoChangedRef;
    napi_ref onFriendPresenceRef;
    napi_ref onFriendRequestRef;
    napi_ref onFriendAddedRef;
    napi_ref onFriendRemovedRef;
    napi_ref onFriendMessageRef;
} HandlerContext;

void setCarrierCallbacks(napi_env env, napi_value jsCallbacks, ElaCallbacks *callbacks, HandlerContext *hc);

#endif //__JNI_CARRUER_HADNDLER_H__