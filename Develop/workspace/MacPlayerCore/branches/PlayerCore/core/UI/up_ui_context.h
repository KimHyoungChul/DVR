#ifndef _UP_UI_CONTEXT_HEADER
#define _UP_UI_CONTEXT_HEADER

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

typedef void (*process_cb)(void* param, void* data, int len, int dropFrame, int fps);

typedef struct _UPUI_VideoCtx
{
	SRV_Identify type;
	/*network address*/
	UPSint8* addr;
	UPSint32 port;

	/*login request for some protocol*/
	UPSint8* username;
	UPSint8* password;

	UPUint videoChannel;
	UPUint videoWidth;
	UPUint videoHeight;

	UpThreadHandle processData;
	UpMutexHandle processMutex;
	UpEventHandle processEvent;
	SRV_Thread_Status processStatus;
	process_cb dataProcess;
	void* paramData;

	SRV_Instance *srvInstance;

	struct _UPUI_VideoCtx *next;

}UPUI_VideoCtx;

UPUI_VideoCtx *upui_ctx_create(const UPSint8* addr, UPSint32 port);
void upui_ctx_destroy(UPUI_VideoCtx* ctx);
UPSint upui_ctx_initSrv(UPUI_VideoCtx* ctx);
void upui_ctx_finiSrv(UPUI_VideoCtx* ctx);
void upui_ctx_setHandleCB(UPUI_VideoCtx* ctx, process_cb handlecb, void* param);
UPSint upui_ctx_start(UPUI_VideoCtx *ctx);
UPSint upui_ctx_stop(UPUI_VideoCtx *ctx);
UPSint upui_ctx_notify(UPUI_VideoCtx *ctx);	

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_UP_UI_CONTEXT_HEADER*/
