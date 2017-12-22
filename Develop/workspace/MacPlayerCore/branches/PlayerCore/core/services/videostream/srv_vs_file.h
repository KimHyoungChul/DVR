#ifndef _SRV_VS_FILE_HEADER
#define _SRV_VS_FILE_HEADER

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

UPSint vs_file_init();
void vs_file_fini();
void vs_file_main(void* data);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_SRV_VS_FILE_HEADER*/
