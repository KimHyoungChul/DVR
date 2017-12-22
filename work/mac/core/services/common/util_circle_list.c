/***************************************************************************
 * Include Files                 	
 ***************************************************************************/
#include "up_sys_config.h"

#include "up_types.h"
#include "ucm.h"
#include "util_circle_list.h"

putilCircleListStr util_cl_create(int len)
{
	putilCircleListStr m_gcs_cycle_list = UP_NULL;

	//allocate the circle list struct
	m_gcs_cycle_list = (putilCircleListStr)memset(ucm_malloc(UP_NULL, sizeof(utilCircleListStr)),
		0, sizeof(utilCircleListStr));

	m_gcs_cycle_list->entry_number = len;
	m_gcs_cycle_list->high_index   = len -1;
	m_gcs_cycle_list->list = (void**)ucm_malloc(UP_NULL, len * sizeof(void*));

	return m_gcs_cycle_list;
}

void util_cl_destroy(putilCircleListStr m_gcs_cycle_list)
{
	if(m_gcs_cycle_list)
	{
		ucm_free(UP_NULL, m_gcs_cycle_list->list);
		ucm_free(UP_NULL, m_gcs_cycle_list);
	}
}

void* util_cl_list_push(putilCircleListStr m_gcs_cycle_list, void* pentry)
{
	if(m_gcs_cycle_list->circle_push_count - m_gcs_cycle_list->circle_pop_count < m_gcs_cycle_list->entry_number)   
	{
	  m_gcs_cycle_list->list[m_gcs_cycle_list->circle_push_index] = pentry;
	  m_gcs_cycle_list->circle_push_count++;
	  
	  if(m_gcs_cycle_list->circle_push_index == m_gcs_cycle_list->high_index) m_gcs_cycle_list->circle_push_index = 0;
	  else m_gcs_cycle_list->circle_push_index++;
	  
	  return pentry;
	}
	else
	  return UP_NULL;
}

void* util_cl_list_pop(putilCircleListStr m_gcs_cycle_list)
{
	void* result = UP_NULL;
	
	if(m_gcs_cycle_list && m_gcs_cycle_list->circle_push_count != m_gcs_cycle_list->circle_pop_count)
	{
		result = m_gcs_cycle_list->list[m_gcs_cycle_list->circle_pop_index];
		m_gcs_cycle_list->circle_pop_count++;

		if(m_gcs_cycle_list->circle_pop_index == m_gcs_cycle_list->high_index) m_gcs_cycle_list->circle_pop_index = 0;   
		else m_gcs_cycle_list->circle_pop_index++;
		return result;
	}

	return UP_NULL;
}

