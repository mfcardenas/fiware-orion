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

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include "logMsg/logMsg.h"

void ws_parser_parse(std::string &url, std::string &verb, std::string &payload, const char *msg)
{
  rapidjson::Document doc;
  doc.Parse(msg);

  if (!doc.IsObject())
  {
    url.clear();
    verb.clear();
    payload.clear();
    return;
  }

  url = doc["url"].GetString();
  verb = doc["verb"].GetString();

  if (doc.HasMember("payload") && doc["payload"].IsObject())
  {
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);
    doc["payload"].Accept(writer);
    payload = buff.GetString();
  }
  else
    payload.clear();
}