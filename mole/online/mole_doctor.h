#ifndef MOLE_DOCTOR_H
#define MOLE_DOCTOR_H

int get_sick_house_info_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int patient_sit_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int patient_quit_cmd(sprite_t* p, const uint8_t *body, int bodylen);

int doctor_sit_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int doctor_quit_cmd(sprite_t* p, const uint8_t *body, int bodylen);

int doctor_check_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int doctor_cure_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int doctor_duty_cmd(sprite_t* p, const uint8_t *body, int bodylen);

int medicine_cure_lahm_cmd(sprite_t* p, const uint8_t *body, int bodylen);
int set_pet_sicktype_callback(sprite_t *p, uint32_t id, char *buf, int len);

int check_lahm_sick(pet_t * p,uint32_t uid);
int handle_doctor_work(sprite_t *p);
int clean_doctor(sprite_t *p);
int clean_patient(sprite_t *p);
int clean_all(sprite_t *p, int has_clean, int completed);
int cure_lahm_free(sprite_t *p);

int notify_doctor_quit(sprite_t* p, int pos, int completed);
int notify_patient_quit(sprite_t* p, int pos);
int check_lahm_sick(pet_t *p, uint32_t uid);
#endif
