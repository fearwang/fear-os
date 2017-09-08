#ifndef _DEBUG_H_
#define _DEBUG_H_


static int g_debug_level = 1;

#define ERR_LOG  (0)
#define DEBUG_LOG (1)
#define INFO_LOG (2)


#define pr_debug(fmt, args...) do { \
			if (g_debug_level >= DEBUG_LOG) {\
				printk("[DEBUG] "fmt, ##args); \
			} \
		} while (0)

#define pr_error(fmt, args...) do { \
			if (g_debug_level >= ERR_LOG) {\
				printk("[ERROR] "fmt, ##args); \
			} \
		} while (0)

#define pr_info(fmt, args...) do { \
			if (g_debug_level >= INFO_LOG) {\
				printk("[INFO] "fmt, ##args); \
			} \
		} while (0)
			
		
		

#define FUNC_ENTRY()  pr_debug("%s, %d, enter\n", __func__, __LINE__)
#define FUNC_EXIT()  pr_debug("%s, %d, exit\n", __func__, __LINE__)


#endif