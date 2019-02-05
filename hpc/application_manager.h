/*
 * application_manager.h
 *
 * Created: 10/4/2018 1:36:20 PM
 *  Author: I17643
 */

#ifndef APPLICATION_MANAGER_H_
#define APPLICATION_MANAGER_H_

struct shared_networking_params {
	int amDisconnecting : 1;
	int haveAPConnection : 1;
	int haveERROR : 1;
};
extern struct shared_networking_params shared_networking_params;

#ifdef __cplusplus
extern "C"
{
#endif
	void application_init(void);
	void runScheduler(void);
#ifdef __cplusplus
}
#endif


#endif /* APPLICATION_MANAGER_H_ */
