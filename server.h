#ifndef _SERVER_H_
#define _SERVER_H_

#include "producto.h"

void server_init(Producto *p);
void server_start();
void server_check_requests();

#endif // _SERVER_H_
