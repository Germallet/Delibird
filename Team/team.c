#include "team.h"
#include <commons/log.h>
#include <commons/collections/list.h>

int main()
{
	t_log* logger = log_create("team.log", "Team", true, LOG_LEVEL_INFO);
	log_info(logger, "Team!");
	log_destroy(logger);
}
