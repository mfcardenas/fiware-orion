/*
*
* Copyright 2015 Telefonica Investigacion y Desarrollo, S.A.U
*
* This file is part of Orion Context Broker.
*
* Orion Context Broker is free software: you can redistribute it and/or
* modify it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* Orion Context Broker is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero
* General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Orion Context Broker. If not, see http://www.gnu.org/licenses/.
*
* For those usages not covered by this license please contact with
* iot_support at tid dot es
*
* Author: Felipe Ortiz
*/

#include "parser.h"
#include "connection_manager.h"
#include "rest/ConnectionInfo.h"
#include "common/Format.h"

#include <pthread.h>
#include <map>
#include <string>

// Max possible code (int size)
#define MAX_CID 32768

static int cidVec[MAX_CID];
static pthread_mutex_t mtx_cid = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mtx_conn = PTHREAD_MUTEX_INITIALIZER;

static std::map<int, ConnectionInfo *> connections;

int connection_manager_get_cid()
{
  pthread_mutex_lock(&mtx_cid);
  for (int i = 0; i < MAX_CID; ++i)
  {
    if (cidVec[i] == 1)
      continue;
    cidVec[i] = 1;
    pthread_mutex_unlock(&mtx_cid);
    return i;
  }
  pthread_mutex_unlock(&mtx_cid);
  return -1;
}

ConnectionInfo *connection_manager_get(int cid, const char *message)
{
  std::string url;
  std::string verb;
  std::string payload;

  pthread_mutex_lock(&mtx_conn);
  std::map<int, ConnectionInfo *>::iterator it;
  it = connections.find(cid);
  if (it != connections.end())
  {
    pthread_mutex_unlock(&mtx_conn);

    ws_parser_parse(url, verb, payload, message);
    it->second->modify(url, verb, payload);
    return it->second;
  }

  pthread_mutex_unlock(&mtx_conn);
  ws_parser_parse(url, verb, payload, message);
  ConnectionInfo *ci = new ConnectionInfo("v2", JSON, true);
  ci->modify(url, verb, payload);

  pthread_mutex_lock(&mtx_conn);
  connections[cid] = ci;
  pthread_mutex_unlock(&mtx_conn);
  return ci;
}

void connection_manager_remove(int cid)
{
  pthread_mutex_lock(&mtx_conn);
  delete connections[cid];
  pthread_mutex_unlock(&mtx_conn);

  pthread_mutex_lock(&mtx_cid);
  cidVec[cid] = 0;
  pthread_mutex_unlock(&mtx_cid);
}