#ifndef H_PEA_PET_GIFT_H_2012_05_16
#define H_PEA_PET_GIFT_H_2012_05_16


#include <libtaomee++/proto/proto_base.h>
#include "CtableRoute100x10.h"
#include "proto.hpp"
#include "benchapi.h"
#include "pea_common.hpp"



class pea_pet_gift : public CtableRoute100x10
{

    public:

        pea_pet_gift(mysql_interface * db);



        int update_pet_gift(db_user_id_t * db_user_id, uint32_t pet_no, db_update_pet_gift_t * p_info);

        int query_pet_gift(db_user_id_t * db_user_id, uint32_t pet_no, std::vector<db_pet_gift_info_t> & gift_vec);


};




#endif
