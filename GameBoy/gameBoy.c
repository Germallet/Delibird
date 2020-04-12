#include "gameBoy.h"
#include<commons/log.h>
#include<commons/collections/list.h>

int main()
{
	t_log* logger = log_create("gameBoy.log", "GameBoy", true, LOG_LEVEL_INFO);
	log_info(logger, "GameBoy!");
	log_destroy(logger);
}
