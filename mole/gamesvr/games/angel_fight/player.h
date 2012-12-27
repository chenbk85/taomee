/*
 * =====================================================================================
 *
 *       Filename:  player.h
 *
 *    Description:  天使对战玩家基本数据
 *
 *        Version:  1.0
 *        Created:  06/08/2011 03:54:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ericlee, Ericlee@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef PLAYER_H
#define PLAYER_H

#define	ACT_MSG_MAX_SIZE	2048
#define AWARDA_CARD_NUM		5
#define MAX_NAME_LEN		16

#include <vector>
extern "C"
{
#include "../../game.h"
}
#include "creatureinfo.h"

//错误码定义
enum ANGEL_FIGHT_ERROR
{
	AFE_INVALID_CMD			= 10000,
	AFE_INVALID_OPERATE,
	AFE_HAS_OPERATED,
	AFE_INVALID_SKILL,
	AFE_NOT_ENOUGH_MANA,
	AFE_SYSTEM_BUSY,
	AFE_INVALID_TASK_ID,
};

//for db return
struct PlayerAttr_s
{
	char		m_cName[MAX_NAME_LEN];
	uint32_t	m_nColor;
	uint32_t	m_nTotalExp;
	uint32_t	m_nExp;
	uint32_t	m_dummy8;
	uint32_t	m_nLevel;
	int32_t		m_nIntelligence;		//智力
	int32_t		m_nAgile;				//敏捷
	int32_t		m_nStrength;			//力量
	int32_t		m_nHabitus;				//体质
	uint32_t	m_dummy1;
	uint32_t	m_dummy2;
	uint32_t	m_nEnergy;				//能量
	uint32_t	m_dummy4;				//能量上限
	uint32_t	m_nVitality;			//活力
	uint32_t	m_dummy6;				//活力上限
	uint32_t	m_dummy7;				//收集点数
	uint32_t	m_nEquip;				//装备
	int32_t		m_nItemCount;			//附加道具数目
	int32_t		m_nStateCount;			//附加状态数目
}__attribute__((packed));

//增益道具ID
enum ITEM_ADDED_STATE
{
	IAS_STR		= 1,
	IAS_INT,
	IAS_ALI,
	IAS_HAB,
	IAS_EXP_INITIVATE,
	IAS_EXP_PASSIVE,
};

struct DbItemInfo_s
{
	int32_t m_nID;		//物品ID
	int32_t m_nLevel;	//物品等级
};

//发送给前端的事件定义
enum PLAYER_EVENT
{
	PE_BUFF		= 1,
	PE_ATTACK,
	PE_COMBO,
	PF_CRITICAL,
	PE_EVADE,
	PE_BLOCK,
	PE_SKILL,
};

enum ACTION_STATE
{
	AS_WAIT		= 0x0,
	AS_ATTACK	= 0x1,
	AS_BLOCK	= 0x2,
	AS_SKILL	= 0x3,
	AS_REST		= 0x4,
	AS_EVADE	= 0x5,
	AS_MAX		= 0x6,
	AS_AUTO		= 0x7,
	//第二字节用作标志位
	AS_WAIT_DATA	= 0x100,
	AS_WAIT_ANI		= 0x200,
	AS_WAIT_DB		= 0x300,
	AS_FORCE_DWORD	= 0xFFFFFFFF
};

enum PLAYER_STATE
{
	PS_NORMAL	= 0,
	PS_OVER		= 1,
	PS_WIN		= 2,
};

class ItemAttr;

typedef std::vector<int32_t>		VecAddedEffect_t;
typedef VecAddedEffect_t::iterator	VecAddedEffectIt_t;

struct SkillInfo
{
	int32_t id;			//技能ID
	int32_t point;		//剩余使用次数

	SkillInfo( int32_t idx );
	bool operator==( const SkillInfo& skill );
};

typedef std::vector<SkillInfo>		VecSkillID_t;
typedef VecSkillID_t::iterator		VecSkillIDIt_t;


class Player
{
public:
	Player();
	~Player();
	void 			SetSprite( sprite_t* p );
	sprite_t*		GetSprite();
	VecSkillID_t&	GetSkillInfo();
	uint32_t		GetUserID();
	//如果declare为true则仅仅通知玩家的操作选择，而不做具体行动
	int				Action( bool declare );
	int32_t			GetMaxHP();
	int32_t			GetHP();
	int32_t			GetMaxMP();
	int32_t			GetMP();
	int32_t			GetActSPD();

	int32_t			GetLevel();

	ACTION_STATE	GetActionState();
	int32_t			GetASParam();
	int				SetActionState( ACTION_STATE as, int32_t param = 0);

	void			ClearActionState();

	void			SetTar( Player* p );
	Player*			GetTar();

	void			IncHP( int inc );
	void			IncMP( int inc );

	int				GetDamage( int damage );

	void			AddBuff( int32_t idx, int32_t skillID );
	void			RemoveBuff( int32_t idx );
	int				ExecuteBuff();
	//将hp，mp修正到合理区间
	void			ConstraintHpMp();

	int 			GetValidCmd();
	void			SetValidCmd( int cmd );
	//获取怪物数据
	int 			SetInfo( CreatureInfo* my_info );
	//获取经由DB返回的玩家数据
	int 			SetInfo( char* data, int32_t len, bool isPK = false );
	void			ComputeAttr();
	int				GetAtk();
	int				AutoAct();
	int				AINormal();
	int				AIBoss();
	//添加一个队友
	void			AddPartner( Player* p );
	Player*			GetPartner();
	//获取一个活着的队友
	Player*			GetValidPartner();
	void			SetTeamID( int32_t id );
	int32_t			GetTeamID();
	int32_t			GetTeamSize();

	void			GetNoticeInfo( uint8_t* des, int32_t& len, int32_t maxLen );
	void			NoticeBuffClear();
	void			NoticeGetHead( uint8_t** buf, int32_t& len );
	void			NoticeNewPacket( uint8_t** buf, int32_t& len, uint32_t& spaceLeft );
	void			CompletePacket( uint8_t* buf, int32_t cmd, int32_t len, bool head = false );
	void 			SetCreatureID( int32_t id );

	int32_t			GetAddExp();
	int32_t			GetEnergy();
	int32_t			GetVitality();
	int32_t			GetEquipID();
	char*			GetName();
	int32_t			GetColor();
	void			Leave();
	bool			IsLeave();
private:
	int				Attack();
	int				UseSkill();
	void			AddAttr( ItemAttr* attr );

private:

	int32_t			m_nLevel;				//等级

	int32_t			m_nStrength;			//力量
	int32_t			m_nIntelligence;		//智力
	int32_t			m_nAgile;				//敏捷
	int32_t			m_nHabitus;				//体质

	int32_t			m_nAttack;				//攻击力
	int32_t			m_nActSpeed;			//行动速度
	int32_t			m_nEvasion;				//闪避率
	int32_t			m_nBlock;				//格挡率
	int32_t			m_nCritical;			//暴击率
	int32_t			m_nHit;					//命中率
	int32_t			m_nCombo;				//连击
	int32_t			m_nDef;					//防御

	int32_t			m_nHP;					//体力值
	int32_t			m_nMP;					//法力值
	int32_t			m_nMaxHP;
	int32_t			m_nMaxMP;

	int32_t			m_nEnergy;				//能量
	int32_t			m_nVitality;			//活力
	int32_t			m_nEquipID;				//装备ID

	VecSkillID_t	m_vecSkillID;	//技能索引

	struct BuffInfo
	{
		int32_t				m_nRoundLasts;
		int32_t				m_nBuffID;
		VecAddedEffect_t	m_vecEffect;
	};

	VecAddedEffect_t	m_vecAddedEffect;	//附加的技能特效

	typedef std::vector<BuffInfo>	VecBuffInfo_t;
	typedef VecBuffInfo_t::iterator	VecBuffInfoIt_t;

	VecBuffInfo_t	m_vecBuff;				//附加状态

	sprite_t*		m_pSprite;
	int32_t			m_nCreatureID;	//怪物ID

	ACTION_STATE	m_nActionState;	//行动状态
	int32_t			m_nASParam;		//行动状态参数， 如技能编号等
	Player*			m_pActTar;		//行动目标
	int				m_nValidCmd;

	Player*			m_pPartner;		//队友--暂时只适用于怪物

	int32_t			m_nTeamID;
	//发送到客户端的通知消息缓存
	uint8_t			m_noticeBuff[ACT_MSG_MAX_SIZE];
	int32_t			m_nNBSize;
	int32_t			m_nAddExp;		//额外经验
	bool			m_bActioned;
	bool			m_bIsBoss;
	char			m_cName[MAX_NAME_LEN];
	int32_t			m_nColor;
	bool			m_bIsLeave;

public:
	//附加能力
	int32_t			m_nAttackEx;			//攻击力
	int32_t			m_nActSpeedEx;			//行动速度
	int32_t			m_nEvasionEx;			//闪避率
	int32_t			m_nCriticalEx;			//暴击率
	int32_t			m_nBlockEx;				//格挡率
	int32_t			m_nHitEx;				//命中率
	int32_t			m_nComboEx;				//连击
	int32_t			m_nDefEx;				//防御
};

inline void Player::SetSprite( sprite_t* p )
{
	m_pSprite = p;
}

inline sprite_t* Player::GetSprite()
{
	return m_pSprite;
}

inline ACTION_STATE Player::GetActionState()
{
	return m_nActionState;
}

inline int32_t Player::GetASParam()
{
	return m_nASParam;
}

inline int32_t Player::GetMaxHP()
{
	return m_nMaxHP;
}

inline int32_t Player::GetHP()
{
	return m_nHP;
}

inline int32_t Player::GetMaxMP()
{
	return m_nMaxMP;
}

inline int32_t Player::GetMP()
{
	return m_nMP;
}

inline int32_t Player::GetActSPD()
{
	return m_nActSpeed + m_nActSpeedEx;
}

inline void Player::SetTar( Player * p )
{
	Player* pObj = this;
	while( pObj )
	{
		pObj->m_pActTar = p;
		pObj = pObj->m_pPartner;
	}
}

inline Player* Player::GetTar()
{
	if( m_pActTar->GetValidPartner() )
	{
		return m_pActTar->GetValidPartner();
	}
	else
	{
		return m_pActTar;
	}
}

inline int Player::GetValidCmd()
{
	return m_nValidCmd;
}

inline void Player::SetValidCmd( int cmd )
{
	m_nValidCmd = cmd;
}

inline void Player::IncHP( int inc )
{
	m_nHP += inc;
}

inline void Player::IncMP( int inc )
{
	m_nMP += inc;
}

inline int Player::GetDamage( int damage )
{
	if( damage > 0 )
	{
		damage = int ( damage * ( 1.0 - (m_nDef + m_nDefEx) / 100.0 ) );
		if( m_nHP < damage )
		{
			damage = m_nHP;
		}
	}

	return damage;
}

inline int Player::GetAtk()
{
	return m_nAttack + m_nAttackEx;
}

inline VecSkillID_t& Player::GetSkillInfo()
{
	return m_vecSkillID;
}

inline int32_t Player::GetLevel()
{
	return m_nLevel;
}

inline void Player::SetTeamID( int32_t id )
{
	m_nTeamID = id;
}

inline int32_t Player::GetTeamID()
{
	return m_nTeamID;
}

inline Player* Player::GetPartner()
{
	return m_pPartner;
}

inline void Player::SetCreatureID( int32_t id )
{
	m_nCreatureID = id;
}

inline int32_t Player::GetAddExp()
{
	return m_nAddExp;
}

inline int32_t Player::GetEnergy()
{
	return m_nEnergy;
}

inline int32_t Player::GetVitality()
{
	return m_nVitality;
}

inline int32_t Player::GetEquipID()
{
	return m_nEquipID;
}

inline char* Player::GetName()
{
	return m_cName;
}

inline int32_t Player::GetColor()
{
	return m_nColor;
}

inline void Player::Leave()
{
	m_bIsLeave = true;
}

inline bool Player::IsLeave()
{
	return m_bIsLeave;
}

#endif

