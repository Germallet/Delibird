#include "broker.h"
#include <commons/log.h>
#include <commons/collections/list.h>

int main()
{
	t_log* logger = log_create("broker.log", "Broker", true, LOG_LEVEL_INFO);
	log_info(logger, "Broker!");
	log_destroy(logger);
}
