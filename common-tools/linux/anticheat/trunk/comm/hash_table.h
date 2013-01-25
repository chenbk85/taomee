#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__


#define	HASH_MAX_ROW	(100)


/*
 * 注意1: 一旦hash表被存储过任何有效数据后, 
 * 不可再改变 node_size/row_num/nodes_num/mods 等设定, 否则数据会乱套;
 * 建议是:
 * (1) 事先把 node 的大小设计得足够大;
 * (2) 或者当实在不够时, 先dump出来, 然后再写个转换器, 待新的shm_hash_table建立后,
 * 	   把老的shm_hash_table中restore到新的shm_hash_table中, 然后再开始新的业务逻辑;
 *
 * 注意2: node的结构或状态(eg: 空/占据但过期/占据且有效)交给用户来控制;
 */
struct hash_table_t {
	/*! shm hash_table */
	void		*table;
	/*! 单个node的字节数 */
	size_t		node_size;
	/*! hash_table 的总可用node数 */
	uint32_t	total_node_num;
	/*! table 所关联的shm 最多可支持多少级hash (可用于报警) */
	uint32_t	row_num;
	/*! 每行支持的node数量 */
	uint32_t	nodes_num[HASH_MAX_ROW];
	/*! 每行的mod */
	uint32_t	mods[HASH_MAX_ROW];
	/*! 每行的字节数 */
	size_t		row_nodes_size[HASH_MAX_ROW];
	/*! 每行的起始位置 */
	void		*row_starts[HASH_MAX_ROW];

	/*! 用于搜索时的判断是否是 key 的 node (返回 0 表示key的node是node) */
	int			(*compare)(const void *key, const void *node);
};

/**
 * @briel 计算hash_table所需的内存;
 * @return 0: fail, >0: success
 */
size_t calc_hash_table_size(size_t node_size, uint32_t row_num, uint32_t *nodes_num);

/**
 * @briel 初始化hash表的管理结构;
 */
int hash_table_init(struct hash_table_t *hash_table, void *table, size_t table_size, size_t node_size, uint32_t row_num, uint32_t *nodes_num, uint32_t *row_mode, int(*compare)(const void *key, const void *node));

/**
 * @briel 搜索 key 的 node, short_key 是用 key 算出来的整数;
 */
void *hash_table_lookup_node(struct hash_table_t *hash_table, void *key, uint32_t short_key);

void *hash_table_lookup_node_ex_posinfo(struct hash_table_t *hash_table, void *empty_key, void *key, uint32_t short_key, int *exist, uint32_t *row_idx, uint32_t *col_idx);

void *hash_table_lookup_node_posinfo(struct hash_table_t *hash_table, void *key, uint32_t short_key, uint32_t *row_idx, uint32_t *col_idx);

/**
 * @briel 搜索 key 的 node, short_key 是用 key 算出来的整数;
 * @return
 * 	(1) NULL: 既没有 key 的 node, 又没有空节点;
 * 	(2) 有效指针 且 exist = 0: key 的 node 不存在, 且找到一个空节点;
 * 	(3) 有效指针 且 exist = 1: key 的 node 存在, 且返回这个节点的指针;
 */
void *hash_table_lookup_node_ex(struct hash_table_t *hash_table, void *empty_key, void *key, uint32_t short_key, int *exist);

/**
 * @briel 获取 (row, col) 处的节点, 且把 (row, col) 往高地址移动一个节点;
 * @return
 * 	(1) NULL: 本次的 (row, col) 已超出hash_table的范围;
 * 	(2) 有效指针: 返回的是 (row, col) 处的节点 (注意: 是否是有效节点留给用户判断)
 */
void *hash_table_walk(struct hash_table_t *hash_table, uint32_t *row, uint32_t *col);

/**
 * @briel 获取 (row, col) 处的节点;
 * @return
 * 	(1) NULL: (row, col) 已超出hash_table的范围;
 * 	(2) 有效指针: 返回的是 (row, col) 处的节点 (注意: 是否是有效节点留给用户判断)
 */
void *hash_table_get_node(struct hash_table_t *hash_table, uint32_t row, uint32_t col);



#endif /* __HASH_TABLE_H__ */
