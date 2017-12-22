#ifndef _SRV_RC_POOL_HEADER
#define _SRV_RC_POOL_HEADER

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void rc_pool_reset(Data_Pool *pool);
UPSint rc_pool_init(Data_Pool *pool, UPUint size);
UPSint rc_pool_fini(Data_Pool* pool);
PRC_data_entry_str rc_pool_listPop(Data_Pool* pool);
PRC_data_entry_str rc_pool_listPush(Data_Pool* pool, PRC_data_entry_str data);
PRC_data_entry_str rc_pool_poolPop(Data_Pool* pool);
PRC_data_entry_str rc_pool_poolPush(Data_Pool* pool, PRC_data_entry_str data);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*_SRV_RC_POOL_HEADER*/
