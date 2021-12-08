#ifndef HIB_H
#define HIB_H

#define HIB_CTL_Wait_WC while (!(HIB_CTL_R&HIB_CTL_WRC)) {}
#define ChangeHIBRegSettings(x) x;\
HIB_CTL_Wait_WC;\

void Hibernate_Mode_Init(void);
void SetRTCTime(unsigned long seconds);
void Activate_Hibernate (void);

#endif

