#include "gameCard.h"
#include<commons/log.h>
#include<commons/collections/list.h>

int main()
{
	t_log* logger = log_create("gameCard.log", "GameCard", true, LOG_LEVEL_INFO);
	log_info(logger, "GameCard!");
	log_destroy(logger);
}
