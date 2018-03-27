#ifndef __WDG_H__
#define __WDG_H__

/** enable watch dog */
void wdg_activate(unsigned short num_of_ms);
/** inactive watch dog */
void wdg_reactivate(void);

#endif //__WDG_H__
