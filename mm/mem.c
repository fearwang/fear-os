#include <mem_layout.h>
#include <debug.h>

#define	NULL	((void *)0)

struct list_head {
	struct list_head *next, *prev;
};


static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(struct list_head *new_lst,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new_lst;
	new_lst->next = next;
	new_lst->prev = prev;
	prev->next = new_lst;
}

static inline void list_add(struct list_head *new_lst, struct list_head *head)
{
	__list_add(new_lst, head, head->next);
}

static inline void list_add_tail(struct list_head *new_lst, struct list_head *head)
{
	__list_add(new_lst, head->prev, head);
}

static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head * entry)
{
	__list_del(entry->prev,entry->next);
}


static inline void list_remove_chain(struct list_head *ch,struct list_head *ct){
	ch->prev->next=ct->next;
	ct->next->prev=ch->prev;
}

static inline void list_add_chain(struct list_head *ch,struct list_head *ct,struct list_head *head){
		ch->prev=head;
		ct->next=head->next;
		head->next->prev=ct;
		head->next=ch;
}

static inline void list_add_chain_tail(struct list_head *ch,struct list_head *ct,struct list_head *head){
		ch->prev=head->prev;
		head->prev->next=ch;
		head->prev=ct;
		ct->next=head;
}


static inline int list_empty(const struct list_head *head)
{
	return head->next == head;
}


#define offsetof(TYPE, MEMBER) ((unsigned int) &((TYPE *)0)->MEMBER)


#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
		(type *)( (char *)__mptr - offsetof(type,member) );})


#define list_entry(ptr,type,member)	\
    container_of(ptr, type, member)



#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)




#define _MEM_END	PLAT_HEAP_MEM_END
#define _MEM_START	PLAT_MEM_START + KERNEL_IMG_MAX_SIZE


#define PAGE_SHIFT	(12)
#define PAGE_SIZE	(1<<PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

#define KERNEL_MEM_END	(_MEM_END)
/*the bigin and end of the kernel mem which is needed to be paged.*/
#define KERNEL_PAGING_START	((_MEM_START+(~PAGE_MASK))&((PAGE_MASK))) //向上取页面大小整数
#define	KERNEL_PAGING_END	(((KERNEL_MEM_END-KERNEL_PAGING_START)/(PAGE_SIZE+sizeof(struct page)))*(PAGE_SIZE)+KERNEL_PAGING_START)

/*page number in need */
#define KERNEL_PAGE_NUM	((KERNEL_PAGING_END-KERNEL_PAGING_START)/PAGE_SIZE)
/*the start and end of the page structure should be storaged in.*/
#define KERNEL_PAGE_END	_MEM_END
#define KERNEL_PAGE_START	(KERNEL_PAGE_END-KERNEL_PAGE_NUM*sizeof(struct page))


/*page flags*/
#define PAGE_AVAILABLE		0x00
#define PAGE_DIRTY			0x01
#define PAGE_PROTECT		0x02
#define PAGE_BUDDY_BUSY		0x04
#define PAGE_IN_CACHE		0x08


struct kmem_cache{
	unsigned int obj_size;
	unsigned int obj_nr;
	unsigned int page_order;
	unsigned int flags;
	struct page *head_page;
	struct page *end_page;
	void *nf_block;
};

struct page {
	unsigned int vaddr;
	unsigned int flags;
	int order;
	struct kmem_cache *cachep;
	struct list_head list;//to string the buddy member
};


//finally I desided to choose a fixed value,which means users could alloc 1M space at most,those who need more than 1M should change the macro to reserve enough space for private use.
#define BUDDY_ORDER_NUM	(9)	//the numbder of order we support
#define BUDDY_MAX_ORDER  (BUDDY_ORDER_NUM - 1)	//the max order 

#define AVERAGE_PAGE_NUM_PER_BUDDY	(KERNEL_PAGE_NUM/MAX_BUDDY_PAGE_NUM)
#define PAGE_NUM_FOR_EACH_BUDDY(j) ((AVERAGE_PAGE_NUM_PER_BUDDY>>(j))*(1<<(j)))

#define PAGE_NUM_FOR_MAX_ORDER	(1<<BUDDY_MAX_ORDER) //the page num of the max order
#define PAGE_NUM_MASK_FOR_MAX_ORDER (PAGE_NUM_FOR_MAX_ORDER-1) //the mask of max page num

static struct list_head page_buddy[BUDDY_ORDER_NUM]; //we need BUDDY_ORDER_NUM list_head  


//get the pointer to struct page with the addr
struct page *virt_to_page(unsigned int addr) {
	unsigned int i;
	i = ((addr)-KERNEL_PAGING_START) >> PAGE_SHIFT;
	if(i > KERNEL_PAGE_NUM)
		return NULL;
	return (struct page *)KERNEL_PAGE_START + i;
}

void init_page_buddy(void){
	int i;
	for(i = 0; i < BUDDY_ORDER_NUM; i++) {
		INIT_LIST_HEAD(&page_buddy[i]);
	}
}

//init all struct page, try to make the mem max buddy
void init_page_map(void){
	int i;
	pr_info("the start addr of heap is %x, end addr is %x\n", KERNEL_PAGING_START, KERNEL_PAGING_END);
	pr_info("page number is %d\n", KERNEL_PAGE_NUM);
	pr_info("the single page num is %d\n", KERNEL_PAGE_NUM&(~PAGE_NUM_MASK_FOR_MAX_ORDER));
	pr_info("page array start is %x, end is %x\n", KERNEL_PAGE_START, KERNEL_PAGE_END);
	pr_info("size of struct page is %d\n", sizeof(struct page));
	struct page *pg = (struct page *)KERNEL_PAGE_START;
	init_page_buddy();
	for(i = 0; i < (KERNEL_PAGE_NUM); pg++,i++) {
		/*1.fill struct page first*/
		pg->vaddr = KERNEL_PAGING_START + i*PAGE_SIZE;	
		pg->flags = PAGE_AVAILABLE;
		INIT_LIST_HEAD(&(pg->list));

		/*2.make the memory max buddy as possible*/
		if(i < (KERNEL_PAGE_NUM&(~PAGE_NUM_MASK_FOR_MAX_ORDER))) {	
			/*the following code should be dealt carefully,we would change the order field of a head struct page to the corresponding order,and change others to -1*/
			if((i&PAGE_NUM_MASK_FOR_MAX_ORDER) == 0) {
				pr_info("the addr of this buddy is %x\n", pg->vaddr);
				pg->order = BUDDY_MAX_ORDER;
			} else {
				pg->order = -1;
			}
			list_add_tail(&(pg->list),&page_buddy[BUDDY_MAX_ORDER]);
		/*3.the remainder not enough to merge into a max buddy is done as min buddy*/
		}else{
			pg->order = 0;
			list_add_tail(&(pg->list),&page_buddy[0]);
		}
	}
}

/*we can do these all because the page structure that represents one page aera is continuous*/
#define BUDDY_END(x,order)	((x)+(1<<(order))-1)
#define NEXT_BUDDY_START(x,order)	((x)+(1<<(order)))
#define PREV_BUDDY_START(x,order)	((x)-(1<<(order)))


/*the logic of this function seems good,no bug reported yet*/
//get the page from buddy system,with the paramter 'order'
struct page *get_pages_from_list(int order){
	unsigned int vaddr;
	int neworder = order;
	struct page *pg,*ret;
	struct list_head *tlst,*tlst1;
	pr_info("call get_pages_from_list with order=%d, neworder=%d\n", order, neworder);
	//try from the order we get from the paramter, if there is none, try order++
	for(; neworder < BUDDY_ORDER_NUM; neworder++) {
		pr_info("try order %d\n", neworder);
		if(list_empty(&page_buddy[neworder])) {
			continue;
		} else {
			pg = list_entry(page_buddy[neworder].next,struct page,list);
			pr_info("get a buddy with order %d, pg->vaddr = %x\n", neworder, pg->vaddr);
			tlst = &(BUDDY_END(pg,neworder)->list);//get the last page of the buddy with neworder
			tlst->next->prev = &page_buddy[neworder];//remove chain from the list with neworder
			page_buddy[neworder].next = tlst->next;
			goto OUT_OK;
		}
	}
	return NULL;
	
OUT_OK:
	for(neworder--; neworder >= order; neworder--) {
		tlst1 = &(BUDDY_END(pg,neworder)->list);
		tlst = &(pg->list);

		pg = NEXT_BUDDY_START(pg,neworder);
		list_entry(tlst,struct page,list)->order = neworder;
		//分裂的时候插入 尾部
		list_add_chain_tail(tlst,tlst1,&page_buddy[neworder]);
	}
	pg->flags |= PAGE_BUDDY_BUSY; //split a buddy to two part, take the last one as result
	pg->order = order;
	return pg;
}



void put_pages_to_list(struct page *pg,int order){
	struct page *tprev,*tnext;
	if(!(pg->flags&PAGE_BUDDY_BUSY)) {
		printk("something must be wrong when you see this message,that probably means you are forcing to release a page that was not alloc at all\n");
		return;
	}
	pg->flags&=~(PAGE_BUDDY_BUSY);

	for(;order < BUDDY_MAX_ORDER; order++) {
		pr_info("need to combine order=%d\n", order);
		tnext=NEXT_BUDDY_START(pg,order);
		tprev=PREV_BUDDY_START(pg,order);
		if((!(tnext->flags&PAGE_BUDDY_BUSY)) && (tnext->order == order)) {
			pg->order++; //order++ of the first struct page which we will combine
			tnext->order = -1;
			//即将被合并的chain，从原来的list中删除
			list_remove_chain(&(tnext->list),&(BUDDY_END(tnext,order)->list));
			//将二者合并成一个chain
			BUDDY_END(pg,order)->list.next = &(tnext->list);
			tnext->list.prev = &(BUDDY_END(pg,order)->list);
			pr_info("combine with next buddy\n");
			continue;
		} else if ((!(tprev->flags&PAGE_BUDDY_BUSY)) && (tprev->order == order)) {
			pg->order = -1; //we combine with the buddy before us, so we need to set order=-1 of the struct page
			//即将被合并的chain，从原来的list中删除
			list_remove_chain(&(tprev->list),&(BUDDY_END(tprev,order)->list));
			//将二者合并成一个chain
			BUDDY_END(tprev,order)->list.next = &(pg->list);
			pg->list.prev = &(BUDDY_END(tprev,order)->list);
			
			pg = tprev;	//set the new first page of the new chain with order++
			pg->order++;
			pr_info("combine with pre buddy\n");
			continue;
		}else{
			pr_info("combine failed\n");
			break;
		}
	}
	//归还的时候，插入头部()
	list_add_chain(&(pg->list),&(BUDDY_END(pg,pg->order)->list),&page_buddy[pg->order]);
}


void *page_address(struct page *pg){
	return (void *)(pg->vaddr);
}

struct page *alloc_pages(unsigned int flag,int order){
	struct page *pg;
	int i;
	//pr_info("call alloc_pages with order=%d\n", order);
	pg = get_pages_from_list(order);
	if(pg == NULL)
		return NULL;
	for(i=0; i < (1<<order); i++) {
		(pg+i)->flags |= PAGE_DIRTY;
	}
	return pg;
}

void free_pages(struct page *pg,int order){
	int i;
	for(i = 0; i < (1<<order); i++){
		(pg+i)->flags &= ~PAGE_DIRTY;
	}
	put_pages_to_list(pg,order);
}

void *get_free_pages(unsigned int flag,int order){
	struct page * page;
	//pr_info("call get_free_pages with order=%d\n", order);
	page = alloc_pages(flag, order);
	if (!page)
		return NULL;
	return	page_address(page);
}

void put_free_pages(void *addr,int order){
	if(order > BUDDY_MAX_ORDER) {
		pr_error("order > %d\n", BUDDY_MAX_ORDER);
	} 
	if(addr == NULL) {
		pr_error("try to free NULL pointer");
	}
	free_pages(virt_to_page((unsigned int)addr), order);
}



#define KMEM_CACHE_DEFAULT_ORDER	(0)
#define KMEM_CACHE_MAX_ORDER		(5)			//cache can deal with the memory no less than 32*PAGE_SIZE
#define KMEM_CACHE_SAVE_RATE		(0x5a)
#define KMEM_CACHE_PERCENT			(0x64)
#define KMEM_CACHE_MAX_WAST			(PAGE_SIZE-KMEM_CACHE_SAVE_RATE*PAGE_SIZE/KMEM_CACHE_PERCENT)


//根据一定的算法 得到合适的order
int find_right_order(unsigned int size){
	int order;
	for(order = 0; order <= KMEM_CACHE_MAX_ORDER; order++) {
		if(size<=(KMEM_CACHE_MAX_WAST)*(1<<order)) {
			return order;
		}
	}
	if(size > (1<<order))
		return -1;
	return order;
}


int kmem_cache_line_object(void *head,unsigned int size,int order){
	void **pl;
	char *p;
	pl=(void **)head; //head指向kmem cache得到的首地址
	p=(char *)head + size; //指向下个对象的首地址
	int i,s = PAGE_SIZE*(1<<order);
	for(i = 0; s > size; i++, s -= size){
		*pl = (void *)p;//第一个对象的首地址作为一个void* 指向下一个对象的首地址
		pl = (void **)p;//pl指向下一个对象的首地址
		*pl = NULL; //先将下一个对象 指向NULL (待检查)
		p = p+size;
	}
	if(s == size) {
		*pl = NULL;
		i++;
	}
	
	return i;
}

//创建指定size的kmem_cache
struct kmem_cache *kmem_cache_create(struct kmem_cache *cache,unsigned int size,unsigned int flags){
	void **nf_block = &(cache->nf_block);

	int order = find_right_order(size);
	pr_info("find order: %d\n",order);
	if(order == -1)
		return NULL;
	if((cache->head_page = alloc_pages(0,order)) == NULL)
		return NULL;
	*nf_block = page_address(cache->head_page);
	//格式化获得的buddy内存
	cache->obj_nr = kmem_cache_line_object(*nf_block, size, order);
	pr_info("the num of obj is: %d\n", cache->obj_nr);
	cache->obj_size = size;
	cache->page_order = order;
	cache->flags = flags;
	cache->end_page = BUDDY_END(cache->head_page,order);
	cache->end_page->list.next = NULL;//一开始只有一个buddy 所以设置为NULL

	return cache;
}

/*FIXME:I dont understand it now*/
void kmem_cache_destroy(struct kmem_cache *cache){
	int order = cache->page_order;
	struct page *pg = cache->head_page;
	struct list_head *list;
	while(1){
		list = BUDDY_END(pg,order)->list.next;//指向kmem cache的下一个buddy
		free_pages(pg,order); //释放一个buddy
		if(list){//下一个buddy存在 接着释放
			pg = list_entry(list,struct page,list);
		}else{
			return;
		}
	}
}

void kmem_cache_free(struct kmem_cache *cache,void *objp){
	*(void **)objp = cache->nf_block;//objp指向要free的地址 objp转换成void** , 则*objp就是其指向的变量(void *)的值设置为目前空闲的地址，即objp指向的变量指向目前空闲的地址
	cache->nf_block = objp;//nf_block指向新的空闲地址
	cache->obj_nr++;
}


void *kmem_cache_alloc(struct kmem_cache *cache,unsigned int flag){
	void *p;
	struct page *pg;
	if(cache == NULL)
		return NULL;
	void **nf_block = &(cache->nf_block);
	unsigned int *nr = &(cache->obj_nr);
	int order = cache->page_order;

	if(!*nr) { //空闲对象个数为0 需要申请buddy
		pr_debug("obj num = 0, need to alloc page\n");
		if((pg = alloc_pages(0,order)) == NULL)
			return NULL;
		*nf_block = page_address(pg);//cache->nf_block指向新申请的内存的起始地址
		cache->end_page->list.next = &pg->list;//新申请的chain，插入到末尾
		cache->end_page = BUDDY_END(pg,order);
		cache->end_page->list.next = NULL;
		//格式化内存
		*nr += kmem_cache_line_object(*nf_block,cache->obj_size,order);
	}
	
	(*nr)--;//空闲对象个数--
	p = *nf_block; //p指向第一个空闲对象的地址
	*nf_block = *(void **)p;//cache->nf_block指向下一个空闲对象
	pg = virt_to_page((unsigned int)p);//地址转换成page结构体
	pg->cachep = cache;		//doubt it??? 
	return p;
}



#define KMALLOC_BIAS_SHIFT			(5)				//32byte minimal
#define KMALLOC_MAX_SIZE			(8192)
#define KMALLOC_MINIMAL_SIZE_BIAS	(1<<(KMALLOC_BIAS_SHIFT)) //32
#define KMALLOC_CACHE_SIZE			(KMALLOC_MAX_SIZE/KMALLOC_MINIMAL_SIZE_BIAS) //128

//这里的计算要注意 假设是32byte，那么index =0 可以满足 不必要去到index=1
#define kmalloc_cache_size_to_index(size)	((((size-1))>>(KMALLOC_BIAS_SHIFT)))

//kmalloc系统事先需要申请的kmem cache
struct kmem_cache kmalloc_cache[KMALLOC_CACHE_SIZE]={{0,0,0,0,NULL,NULL,NULL},};

int kmalloc_init(void){
	int i=0;

	for(i = 0; i < KMALLOC_CACHE_SIZE; i++) {
		if(kmem_cache_create(&kmalloc_cache[i],(i+1)*KMALLOC_MINIMAL_SIZE_BIAS,0) == NULL)
			return -1;
	}
	return 0;
}

void *kmalloc(unsigned int size){
	int index = kmalloc_cache_size_to_index(size);
	//pr_info("kmalloc index = %d\n", index);
	if(index >= KMALLOC_CACHE_SIZE)
		return NULL;
	return kmem_cache_alloc(&kmalloc_cache[index],0);
}

void kfree(void *addr){
	struct page *pg;
	pg = virt_to_page((unsigned int)addr);
	kmem_cache_free(pg->cachep, addr);
}
