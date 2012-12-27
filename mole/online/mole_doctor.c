
#include "proto.h"
#include "dbproxy.h"
#include "exclu_things.h"
#include "mole_doctor.h"
#define SICKBED_MAX 5
#define DOCTOR_WAIT_TIME 300
#define PATIENT_WAIT_TIME 300
#define CURE_REWARD     100
#define DUTY_REWARD     50
typedef struct _bed_info {
    sprite_t *doctor;
    timer_struct_t *doc_timer;
    sprite_t *patient;
    timer_struct_t *pat_timer;
}sickbed_info_t;

static sickbed_info_t sickbed[SICKBED_MAX];

static int check_doctor(sprite_t *p)
{
    if(!p) {
        return -1;
    }
    int i;
    int pos = -1;
    for(i = 0; i < SICKBED_MAX; i++) {
        if(p == sickbed[i].doctor) {
            pos = i;
            break;
        }
    }
    return pos;
}

static int check_patient(sprite_t *p)
{
    if(!p) {
        return -1;
    }
    int i;
    int pos = -1;
    for(i = 0; i < SICKBED_MAX; i++) {
        if(p == sickbed[i].patient) {
            pos = i;
            break;
        }
    }
    return pos;
}

int check_lahm_sick(pet_t *p, uint32_t uid) {
    int sick_info[3];
    sick_info[0] = p->sick_type & 0x3;
    sick_info[1] = (p->sick_type >> 2) & 0x3;
    sick_info[2] = (p->sick_type >> 4) & 0x3;
    int i;
    int sick_cnt = 0;
    for(i = 0; i < 3; i++) {
        if(sick_info[i] == 1)
            sick_cnt++;
    }
    if(sick_cnt >= 2) {
        ERROR_LOG("pet sick too many [%d %d %d %d]", uid, sick_info[0], sick_info[1], sick_info[2]);
    }

    if(sick_info[0] == 1) {
        DEBUG_LOG("LAHM FROSTBITE [%d %d]", uid, p->sick_type);
        return 1;
    }
    if(sick_info[1] == 1) {
        DEBUG_LOG("LAHM BURN [%d %d]", uid, p->sick_type);
        return 2;
    }
    if(sick_info[2] == 1) {
        DEBUG_LOG("LAHM ALLERGY [%d %d]", uid, p->sick_type);
        return 3;
    }
    return 0;
}

int clean_doctor(sprite_t *p)
{
    if(!p) {
        return -1;
    }
    int pos = check_doctor(p);
    if(pos != -1) {
        DEBUG_LOG("CLEAN DOCTOR [%d %d %d]", p->id, p->waitcmd, pos);
        sickbed[pos].doctor = NULL;
        if(sickbed[pos].doc_timer) {
            REMOVE_TIMER(sickbed[pos].doc_timer);
        }
        sickbed[pos].doc_timer = NULL;
    }
    return pos;
}

int clean_patient(sprite_t *p)
{
    if(!p) {
        return -1;
    }
    int pos = check_patient(p);
    if(pos != -1) {
        DEBUG_LOG("CLEAN PATIENT [%d %d %d]", p->id, p->waitcmd, pos);
        sickbed[pos].patient = NULL;
        if(sickbed[pos].pat_timer) {
            REMOVE_TIMER(sickbed[pos].pat_timer);
        }
        sickbed[pos].pat_timer = NULL;
    }
    return pos;
}

int remove_doctor(void* owner, void* data)
{
    sprite_t *p = owner;
    if(!p) {
        return -1;
    }
    int pos = check_doctor(p);
    if(pos != -1) {
        notify_doctor_quit(p, pos, 1);
        DEBUG_LOG("TIMER REMOVE DOCTOR [%d %d]", p->id, pos);
        sickbed[pos].doctor = NULL;
        sickbed[pos].doc_timer = NULL;
        if(sickbed[pos].patient) {
           clean_patient(sickbed[pos].patient);
        }
    }
    return 0;
}

int remove_patient(void* owner, void* data)
{
    sprite_t *p = owner;
    if(!p) {
        return -1;
    }
    int pos = check_patient(p);
    if(pos != -1) {
        notify_patient_quit(p, pos);
        DEBUG_LOG("TIMER REMOVE PATIENT [%d %d]", p->id, pos);
        sickbed[pos].patient = NULL;
        sickbed[pos].pat_timer = NULL;
    }
    return 0;
}


int get_sick_house_info_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, SICKBED_MAX, l);
    int i;
    for(i = 0; i < SICKBED_MAX; i++) {
        if(sickbed[i].doctor) {
            PKG_UINT32(msg, (sickbed[i].doctor)->id, l);
            DEBUG_LOG("SICK BED DOCTOR INFO [%d %d %d]", p->id, i, (sickbed[i].doctor)->id);
        } else {
            PKG_UINT32(msg, 0, l);
        }
        if(sickbed[i].patient) {
            DEBUG_LOG("SICK BED PATIENT INFO [%d %d %d]", p->id, i, (sickbed[i].patient)->id);
            PKG_UINT32(msg, (sickbed[i].patient)->id, l);
        } else {
            PKG_UINT32(msg, 0, l);
        }
    }
    init_proto_head(msg, p->waitcmd, l);
    return send_to_self(p, msg, l, 1);
}

int doctor_sit_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    uint32_t position;
    UNPKG_UINT32(body, position, i);
    if(position > (SICKBED_MAX - 1)) {
        ERROR_RETURN(("sickbed pos error [%d %d]", p->id, position), -1);
    }

    clean_doctor(p);
    if(sickbed[position].doctor) {
        return send_to_self_error(p, p->waitcmd, -ERR_sickbed_already_occupied, 1);
    }
    sickbed[position].doctor = p;
    sickbed[position].doc_timer = ADD_TIMER_EVENT(p, remove_doctor, 0, now.tv_sec + DOCTOR_WAIT_TIME);
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, position, l);
    PKG_UINT32(msg, p->id, l);
    init_proto_head(msg, PROTO_DOCTOR_SIT, l);
    DEBUG_LOG("DOCTOR SIT [%d %d]", p->id, position);
    send_to_map(p, msg, l, 1);
    return 0;
}

int handle_doctor_work(sprite_t *p)
{
    switch(p->waitcmd) {
        case PROTO_DOCTOR_DUTY:
            {
                uint32_t pos = *(uint32_t*)p->session;
                int l = sizeof(protocol_t);
                PKG_UINT32(msg, DUTY_REWARD, l);
                PKG_UINT32(msg, pos, l);
                PKG_UINT32(msg, p->id, l);
                init_proto_head(msg, p->waitcmd, l);
                send_to_map(p, msg, l, 1);
                if(sickbed[pos].patient) {
                    //ERROR_LOG("doctor duty but patient at sickbed [%d %d]", p->id, (sickbed[pos].patient)->id);
                    clean_patient(sickbed[pos].patient);
                }
                do_db_attr_op(NULL, p->id, DUTY_REWARD, 0, 0, 0, 0, ATTR_CHG_roll_back, 0);
                DEBUG_LOG("DOCTOR DUTY [%d %d]", p->id, pos);
            }
            break;
        case PROTO_DOCTOR_CURE:
            {
                uint32_t pos = *(uint32_t*)p->session;
                if(!sickbed[pos].patient){
                    ERROR_RETURN(("no patient [%d]", p->id), -1);
                }
                if(!(sickbed[pos].patient)->followed) {
                    ERROR_RETURN(("no pet [%d]", (sickbed[pos].patient)->id), -1);
                }
                int sick_type = check_lahm_sick((sickbed[pos].patient)->followed, (sickbed[pos].patient)->id);
                if(!sick_type) {
                    int l = sizeof(protocol_t);
                    PKG_UINT32(msg, CURE_REWARD, l);
                    PKG_UINT32(msg, pos, l);
                    PKG_UINT32(msg, p->id, l);
                    init_proto_head(msg, p->waitcmd, l);
                    send_to_map(p, msg, l, 1);
                    do_db_attr_op(NULL, p->id, CURE_REWARD, 0, 0, 0, 0, ATTR_CHG_roll_back, 0);
                    DEBUG_LOG("CURE LAHM NO SICK [%d %d %d ]", p->id, pos, (sickbed[pos].patient)->id);
                    clean_patient(sickbed[pos].patient);
                    return 0;
                }
                cure_pet((sickbed[pos].patient)->followed, (sick_type -1));

                uint32_t patient_id = (sickbed[pos].patient)->id;
                *(uint32_t*)p->session = pos;
                db_infect_pet(p, (sickbed[pos].patient)->followed, patient_id);
                DEBUG_LOG("CURE LAHM [%d %d %d %d]", p->id, pos, (sickbed[pos].patient)->id, (sickbed[pos].patient)->followed->sick_type);
                clean_patient(sickbed[pos].patient);
                return 0;
            }
            break;
        default:
            ERROR_RETURN(("unknown cmd [%d %d]", p->id, p->waitcmd), -1);
    }
    return 0;
}

int cure_lahm_free(sprite_t *p)
{
    uint32_t pos = *(uint32_t*)p->session;
    if(!sickbed[pos].patient){
        ERROR_RETURN(("no patient [%d]", p->id), -1);
    }
    if(!(sickbed[pos].patient)->followed) {
        ERROR_RETURN(("no pet [%d]", (sickbed[pos].patient)->id), -1);
    }
    int sick_type = check_lahm_sick((sickbed[pos].patient)->followed, (sickbed[pos].patient)->id);
    if(!sick_type) {
        int l = sizeof(protocol_t);
        PKG_UINT32(msg, CURE_REWARD, l);
        PKG_UINT32(msg, pos, l);
        PKG_UINT32(msg, p->id, l);
        init_proto_head(msg, p->waitcmd, l);
        send_to_map_except_self(p, msg, l, 0);
        clean_patient(sickbed[pos].patient);
        return 0;
    }
    cure_pet((sickbed[pos].patient)->followed, (sick_type -1));

    uint32_t patient_id = (sickbed[pos].patient)->id;
    db_infect_pet(NULL, (sickbed[pos].patient)->followed, patient_id);
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, CURE_REWARD, l);
    PKG_UINT32(msg, pos, l);
    PKG_UINT32(msg, p->id, l);
    init_proto_head(msg, p->waitcmd, l);
    send_to_map_except_self(p, msg, l, 0);

    DEBUG_LOG("CURE LAHM FREE [%d %d %d %d]", p->id, pos, (sickbed[pos].patient)->id, (sickbed[pos].patient)->followed->sick_type);
    clean_patient(sickbed[pos].patient);
    return 0;
}

int patient_sit_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    if(!p->followed) {
        ERROR_RETURN(("no pet [%d]", p->id), -1);
    }
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    uint32_t position;
    UNPKG_UINT32(body, position, i);
    if(position > (SICKBED_MAX - 1)) {
        ERROR_RETURN(("sickbed pos error [%d %d]", p->id, position), -1);
    }
    if(check_patient(p) != -1) {
        return send_to_self_error(p, p->waitcmd, -ERR_sickbed_already_sit, 1);
    }
    if(sickbed[position].patient) {
        return send_to_self_error(p, p->waitcmd, -ERR_sickbed_already_occupied, 1);
    }
    sickbed[position].patient = p;
    sickbed[position].pat_timer = ADD_TIMER_EVENT(p, remove_patient, 0, now.tv_sec + PATIENT_WAIT_TIME);
    DEBUG_LOG("PATIENT SIT [%d %d]", p->id, position);
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, position, l);
    PKG_UINT32(msg, p->id, l);
    init_proto_head(msg, p->waitcmd, l);
    send_to_map(p, msg, l, 1);
    return 0;
}

int doctor_quit_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    int pos = clean_doctor(p);
    if(pos == -1) {
        //ERROR_LOG("doctor no sit [%d]", p->id);
        return send_to_self_error(p, p->waitcmd, -ERR_sickbed_not_sit, 1);
    }
    clean_patient(sickbed[pos].patient);
    notify_doctor_quit(p, pos, 1);
    return 0;
}

int notify_doctor_quit(sprite_t* p, int pos, int completed)
{
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, pos, l);
    PKG_UINT32(msg, p->id, l);
    init_proto_head(msg, PROTO_DOCTOR_QUIT, l);
    DEBUG_LOG("DOCTOR QUIT [%d %d %d]", p->id, p->waitcmd, pos);
    send_to_map(p, msg, l, completed);
    return 0;
}

int notify_patient_quit(sprite_t* p, int pos)
{
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, pos, l);
    PKG_UINT32(msg, p->id, l);
    init_proto_head(msg, PROTO_PATIENT_QUIT, l);
    DEBUG_LOG("PATIENT QUIT [%d %d %d]", p->id, p->waitcmd, pos);
    send_to_map(p, msg, l, 1);
    return 0;
}

int clean_all(sprite_t *p, int has_clean, int completed)
{
    int doc_pos;
    int pat_pos;
    if(has_clean) {
        doc_pos = *(uint32_t*)p->session;
        pat_pos = -1;
    } else {
        doc_pos = check_doctor(p);
        pat_pos = check_patient(p);
    }
    if(doc_pos == -1 && pat_pos == -1) {
        return 0;
    }
    if(doc_pos != -1) {
        notify_doctor_quit(p, doc_pos, completed);
        if(!has_clean) {
            clean_doctor(p);
        }
        sprite_t* patient = sickbed[doc_pos].patient;
        if(patient) {
            clean_patient(patient);
        }
    }
    if(pat_pos != -1) {
        notify_patient_quit(p, pat_pos);
        clean_patient(p);
    }
    return 0;
}

int patient_quit_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    int pos = clean_patient(p);
    if(pos == -1) {
        ERROR_LOG("patient no sit [%d]", p->id);
        return send_to_self_error(p, p->waitcmd, -ERR_sickbed_not_sit, 1);
    }
    notify_patient_quit(p, pos);
    return 0;
}

int doctor_check_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    int pos = check_doctor(p);
    if(pos == -1) {
        ERROR_RETURN(("not doctor [%d]", p->id), -1);
    }
    if(!sickbed[pos].patient){
        ERROR_RETURN(("no patient [%d]", p->id), -1);
    }
    DEBUG_LOG("DOCTOR CHECK [%d %d %d]", p->id, (sickbed[pos].patient)->id, pos);
    mod_expire_time(sickbed[pos].doc_timer, now.tv_sec + DOCTOR_WAIT_TIME);
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, pos, l);
    PKG_UINT32(msg, p->id, l);
    init_proto_head(msg, p->waitcmd, l);
    send_to_map(p, msg, l, 1);
    return 0;
}

int doctor_duty_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    int pos = clean_doctor(p);
    if(pos == -1) {
        ERROR_RETURN(("not doctor [%d]", p->id), -1);
    }
    *(uint32_t*)p->session = pos;
    return db_set_sth_done(p, 127, 10, p->id);
}

int doctor_cure_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    int pos = clean_doctor(p);
    if(pos == -1) {
        ERROR_RETURN(("not doctor [%d]", p->id), -1);
    }
    if(!sickbed[pos].patient){
        ERROR_RETURN(("no patient [%d]", p->id), -1);
    }
    if(!(sickbed[pos].patient)->followed) {
        ERROR_RETURN(("no pet [%d]", (sickbed[pos].patient)->id), -1);
    }
    *(uint32_t*)p->session = pos;
    return db_set_sth_done(p, 127, 10, p->id);
}

int set_pet_sicktype_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, 0);
    switch(p->waitcmd) {
        case PROTO_DOCTOR_CURE:
            {
                int pos = *(uint32_t*)p->session;
                int l = sizeof(protocol_t);
                PKG_UINT32(msg, CURE_REWARD, l);
                PKG_UINT32(msg, pos, l);
                PKG_UINT32(msg, p->id, l);
                init_proto_head(msg, p->waitcmd, l);
                send_to_map(p, msg, l, 1);
                do_db_attr_op(NULL, p->id, CURE_REWARD, 0, 0, 0, 0, ATTR_CHG_roll_back, 0);
                break;
            }
        case PROTO_MEDICINE_CURE_LAHM:
            {
                uint32_t medicine_id = *(uint32_t*)p->session;
                uint32_t petid = *(uint32_t*)(p->session + 4);
                uint32_t uid = *(uint32_t*)(p->session + 8);

                item_t* pItm  = get_item_prop(medicine_id);
                pet_t*  pPet;
                if (!(pPet = get_pet (uid, petid))) {
                    ERROR_LOG("can't find pet=%u, id=%u", petid, p->id);
					return send_to_self_error(p, p->waitcmd, -ERR_lahm_no_sick, 1);
				}
                int i = sizeof(protocol_t);
                PKG_UINT32(msg, uid, i);
                PKG_UINT32(msg, pItm->id, i);
                PKG_UINT8(msg, pItm->u.feed_attr.type, i);
                PKG_UINT32(msg, pPet->flag, i);
                PKG_UINT32(msg, pPet->id, i);
                PKG_UINT8(msg, pPet->hungry, i);
                PKG_UINT8(msg, pPet->thirsty, i);
                PKG_UINT8(msg, pPet->sanitary, i);
                PKG_UINT8(msg, pPet->spirit, i);
                init_proto_head (msg, p->waitcmd, i);
                send_to_self(p, msg, i, 1);
                break;
            }
        default:
            ERROR_RETURN(("unknow cmd [%d %d]", p->id, p->waitcmd), -1);
    }
    return 0;
}

int medicine_cure_lahm_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 12);
    int i = 0;
    uint32_t medicine_id;
    uint32_t pet_id;
    uint32_t uid;

    UNPKG_UINT32(body, uid, i);
	UNPKG_UINT32(body, pet_id, i);
    UNPKG_UINT32(body, medicine_id, i);
    pet_t*  pPet;
    if (!(pPet = get_pet (uid, pet_id))) {
        ERROR_LOG("can't find pet=%u:%u, id=%u", uid,pet_id, p->id);
		return send_to_self_error(p, p->waitcmd, -ERR_lahm_no_sick, 1);
	}

    static int medicine[3] = {180050, 180051, 180052};
    int sick_type = check_lahm_sick(pPet, uid);
    if(sick_type == -1) {
        return send_to_self_error(p, p->waitcmd, -ERR_lahm_no_sick, 1);
    }
    if(medicine_id != medicine[sick_type -1]) {
        ERROR_LOG("medicine wrong [%d %d %d]", p->id, medicine_id, sick_type);
        return send_to_self_error(p, p->waitcmd, -ERR_medicine_wrong, 1);
    }
    cure_pet(pPet, (sick_type - 1));
    DEBUG_LOG("MEDICINE CURE LAHM [%d %d]", p->id, pPet->sick_type);
    *(uint32_t*)p->session = medicine_id;
    *(uint32_t*)(p->session + 4)= pet_id;
    *(uint32_t*)(p->session + 8)= uid;
    db_single_item_op(NULL, p->id, medicine_id, 1, 0);
    return db_infect_pet(p, pPet, uid);
}
