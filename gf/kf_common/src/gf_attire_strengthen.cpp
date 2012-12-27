#include<stdlib.h>
#include<math.h>
#include "player_attr.hpp"
/**
 *
 white
 atk
 lv +1, +2
 (uselv + 5) / 6;
 lv +3
 (uselv + 5) / 2;

 green
 atk
 uselv > 10
 lv +1, +2
 uselv / 5;
 lv +3
 uselv * 3 / 5;

 blue
 atk
 uselv < 45
 lv +1, +2, +4, +5
 uselv / 4;
 lv + 3, +6
 uselv;

 uselv >= 45
 lv +1, +2, +4, +5
 sqrt((uselv - 28) / 18) * uselv / 4;
 lv + 3, +6
 sqrt((uselv - 28) / 18) * uselv;

 purple
 35 < uselv < 45
 lv +1, +2, +4, +5
 uselv / 4;
 lv + 3, +6
 uselv;
 lv +7, +8
 uselv * 3 / 8;
 lv +9
 uselv * 3 / 2;
 lv +10, +11
 uselv / 2;
 lv +12
 uselv * 2;

 uselv >= 45
 lv +1, +2, +4, +5
 sqrt((uselv - 28) / 18) * uselv / 4;
 lv + 3, +6
 sqrt((uselv - 28) / 18) * uselv;
 lv +7,+8
 sqrt((uselv - 28) / 18) * uselv * 3 / 8;
 lv +9
 sqrt((uselv - 28) / 18) * uselv * 3 / 2;
 lv +10, +11
 sqrt((uselv - 28) / 18) * uselv / 2;
 lv +12
 sqrt((uselv - 28) / 18) * uselv * 2;

 //======================================
 weapon
 blue / purple
 uselv < 45
 lv +4
 uselv * 2 / 5; (agility)
 uselv >= 45
 lv +4
 sqrt((uselv - 28) / 18) * uselv * 2 / 5; (agility)

 purple
 uselv < 45
 lv +8
 uselv * 3 / 5; (strength)
 userlv>=45
 lv +8
 sqrt((uselv - 28) / 18) * uselv * 3 / 5; (strength)

 uselv < 45
 lv +12
 uselv * 4 / 5; (hit)
 uselv >= 45
 lv +12
 sqrt((uselv - 28) / 18) * uselv * 4 / 5; (hit)
 //============================================
 clothes
 blue / purple
 uselv < 45
 lv +4
 uselv * 0.15; (body_quality)
 uselv >= 45
 lv +4
 sqrt((uselv - 28) / 18) * uselv * 0.15; (body_quality)

 purple
 uselv < 45
 lv +8
 uselv * 0.225; (hp)
 lv +8
 sqrt((uselv - 28) / 18) * uselv * 0.225; (hip)

 uselv < 45
 lv +12
 uselv * 0.3; (dodge)
 uselv >= 45
 lv +12
 sqrt((uselv - 28) / 18) * uselv * 0.3; (dodge)

 */

static float white_atk_factor[] = {1, 1, 3, 1, 1, 3, 1.5, 1.5, 4.5, 2, 2, 6};
int calc_white_weapon_add_atk(uint32_t uselv, uint32_t attirelv)
{
    float atk = 0.0;
    float lv = uselv;
    for (uint32_t i = 0; i < attirelv; i++) {
        atk += white_atk_factor[i];
    }
    return (lv / 6.0 * atk);
}

static float green_atk_factor[] = {0.2, 0.2, 0.6, 0.2, 0.2, 0.6, 0.3, 0.3, 0.9, 0.4, 0.4, 1.2};
int calc_green_weapon_add_atk(uint32_t uselv, uint32_t attirelv)
{
    float atk = 0.0;
    float lv = uselv;
    for (uint32_t i = 0; i < attirelv; i++) {
        atk += green_atk_factor[i];
    }
    return (lv * atk);
}
static float atk_factor[] = {0.25, 0.25, 1, 0.25, 0.25, 1, 0.375, 0.375, 1.5, 0.5, 0.5, 2};
int calc_blue_weapon_add_atk(uint32_t uselv, uint32_t attirelv)
{
    float atk = 0;
    float lv = uselv;
    for (uint32_t i = 0; i < attirelv; i++) {
        atk += atk_factor[i];
    }
    if (uselv < 45) {
        return  lv * atk;
    } else {
        return sqrt((lv - 28.0) / 18.0) * lv * atk;
    }
}
int calc_purple_weapon_add_atk(uint32_t uselv, uint32_t attirelv)
{
    float atk = 0;
    float lv = uselv + 5.0;
    for (uint32_t i = 0; i < attirelv; i ++) {
        atk += atk_factor[i];
    }
    if (uselv < 45) {
        return  lv * atk;
    } else {
        return sqrt((lv - 28.0) / 18.0) * lv * atk;
    }
}

/**
 * @brief calc weapon attack
 */
int calc_weapon_add_atk(uint32_t uselv, uint32_t attirelv, uint32_t quality)
{
    switch (quality) {
    case 1:
        return calc_white_weapon_add_atk(uselv, attirelv);
        break;
    case 2:
        return calc_green_weapon_add_atk(uselv, attirelv);
        break;
    case 3:
        return calc_blue_weapon_add_atk(uselv, attirelv);
        break;
    case 4:
    case 5:
        return calc_purple_weapon_add_atk(uselv, attirelv);
        break;
    default:
        return 0;
        break;
    }
}

/**
 * @brief calc clothes defense
 */
int calc_clothes_add_defense(uint32_t attirelv)
{
    return attirelv * 2;
}

/**
 * @brief calc blue purple add agility
 */
int blue_add_agility(uint32_t uselv, uint32_t attirelv)
{
    int ret = 0;
    float lv = uselv;
    if (attirelv >= 4) {
        if (uselv < 45) {
            ret = lv * 0.4;
        } else {
            ret = sqrt((lv - 28.0) / 18.0) * lv * 0.4;
        }
    }
    return ret;
}
/**
 * @brief calc weapon add agility
 */
int calc_weapon_add_agility(uint32_t uselv, uint32_t attirelv, uint32_t quality)
{
    int ret = 0;
    float lv = uselv;
    switch (quality) {
        case 1:
            ret = lv * 0.24;
            break;
        case 2:
            ret = lv * 0.32;
            break;
        case 3:
        case 4:
        case 5:
            ret = blue_add_agility(uselv, attirelv);
            break;
        default:
            break;
    }
    return ret;
}

/**
 * @brief calc blue purple add strength
 */
int blue_add_strength(uint32_t uselv, uint32_t attirelv)
{
    int ret = 0;
    float lv = uselv;
    if (attirelv >= 8) {
        if (uselv < 45) {
            ret = lv * 0.6;
        } else {
            ret = sqrt((lv - 28.0) / 18.0) * lv * 0.6;
        }
    }
    return ret;
}
/**
 * @brief calc weapon add strength
 */
int calc_weapon_add_strength(uint32_t uselv, uint32_t attirelv, uint32_t quality)
{
    int ret = 0;
    float lv = uselv;
    switch (quality) {
        case 1:
            ret = lv * 0.36;
            break;
        case 2:
            ret = lv * 0.48;
            break;
        case 3:
        case 4:
        case 5:
            ret = blue_add_strength(uselv, attirelv);
            break;
        default:
            break;
    }
    return ret;
}
/**
 * @brief calc blue purple add hit
 */
int blue_add_hit(uint32_t uselv, uint32_t attirelv)
{
    int ret = 0;
    float lv = uselv;
    if (attirelv == 12) {
        if (uselv < 45) {
            ret = lv * 0.8;
        } else {
            ret = sqrt((lv - 28.0) / 18.0) * lv * 0.8;
        }
    }
    return ret;
}
/**
 * @brief calc weapon add hit
 */
int calc_weapon_add_hit(uint32_t uselv, uint32_t attirelv, uint32_t quality)
{
    int ret = 0;
    float lv = uselv;
    switch (quality) {
        case 1:
            ret = lv * 0.48;
            break;
        case 2:
            ret = lv * 0.64;
            break;
        case 3:
        case 4:
        case 5:
            ret = blue_add_hit(uselv, attirelv);
            break;
        default:
            break;
    }
    return ret;
}
/**
 * @brief calc blue purple add body
 */
int blue_add_body(uint32_t uselv, uint32_t attirelv)
{
    int ret = 0;
    float lv = uselv;
    if (attirelv >= 4) {
        if (uselv < 45) {
            ret = lv * 0.15;
        } else {
            ret = sqrt((lv - 28.0) / 18.0) * lv * 0.15;
        }
    }
    return ret;

}
/**
 * @brief calc clothes add body
 */
int calc_clothes_add_body(uint32_t uselv, uint32_t attirelv, uint32_t quality)
{
    int ret = 0;
    float lv = uselv;
    switch (quality) {
        case 1:
            ret = lv * 0.09;
            break;
        case 2:
            ret = lv * 0.12;
            break;
        case 3:
        case 4:
        case 5:
            ret = blue_add_body(uselv, attirelv);
            break;
        default:
            break;
    }
    return ret;
}
/**
 * @brief calc blue purple add hp
 */
int blue_add_hpup(uint32_t uselv, uint32_t attirelv)
{
    int ret = 0;
    float lv = uselv;
    if (attirelv >= 8) {
        if (uselv < 45) {
            ret = lv * 0.225;
        } else {
            ret = sqrt((lv - 28.0) / 18.0) * lv * 0.225;
        }
    }
    return ret;
}
/**
 * @brief calc clothes add hp
 */
int calc_clothes_add_hp(uint32_t uselv, uint32_t attirelv, uint32_t quality)
{
    int ret = 0;
    float lv = uselv;
    switch (quality) {
        case 1:
            ret = lv * 0.135;
            break;
        case 2:
            ret = lv * 0.18;
            break;
        case 3:
        case 4:
        case 5:
            ret = blue_add_hpup(uselv, attirelv);
            break;
        default:
            break;
    }
    return ret;
}
/**
 * @brief calc clothes add dodge
 */
int blue_add_dodge(uint32_t uselv, uint32_t attirelv)
{
    int ret = 0;
    float lv = uselv;
    if (attirelv == 12) {
        if (uselv < 45) {
            ret = lv * 0.3;
        } else {
            ret = sqrt((lv - 28.0) / 18.0) * lv * 0.3;
        }
    }
    return ret;
}
/**
 * @brief calc clothes add dodge
 */
int calc_clothes_add_dodge(uint32_t uselv, uint32_t attirelv, uint32_t quality)
{
    int ret = 0;
    float lv = uselv;
    switch (quality) {
        case 1:
            ret = lv * 0.18;
            break;
        case 2:
            ret = lv * 0.24;
            break;
        case 3:
        case 4:
        case 5:
            ret = blue_add_dodge(uselv, attirelv);
            break;
        default:
            break;
    }
    return ret;
}


