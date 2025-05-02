/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Config.h"
#include "Duration.h"
#include "GameEventMgr.h"
#include "Language.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "EventStarter.h"
#include "StringConvert.h"
#include "StringFormat.h"
#include "TaskScheduler.h"
#include "Tokenize.h"
#include "Util.h"
#include "World.h"
#include "WorldSessionMgr.h"

#include <stdlib.h>     //for using the function sleep

namespace
 {
     // Scheduler - for update
     TaskScheduler scheduler;
 }

/*static*/ EventStarter* EventStarter::instance()
{
    static EventStarter instance;
    return &instance;
}

void EventStarter::Init()
{
    _isEnableModule = sConfigMgr->GetOption<bool>("EventStarter.Enabled", false);

    LOG_INFO("module", " ");

    if (!_isEnableModule)
	{
	LOG_INFO("module","> EventStarter: System disabled");
    	LOG_INFO("module", " ");
        return;
	}

    LOG_INFO("module","> EventStarter: System loading");
    LOG_INFO("module", " ");

    _waitTime = sConfigMgr->GetOption<float>("EventStarter.Wait", 60.0);
    sleep( _waitTime );

    StartPersistentGameEvents();
}

void EventStarter::OnUpdate(uint32 diff)
{
    // If module disable, why do the update? hah
    if (!_isEnableModule)
        return;

    scheduler.Update(diff);
}

void EventStarter::StartPersistentGameEvents()
{
    std::string eventList = sConfigMgr->GetOption<std::string>("EventStarter.StartEvents", "");

    std::vector<std::string_view> tokens = Acore::Tokenize(eventList, ' ', false);
    GameEventMgr::GameEventDataMap const& events = sGameEventMgr->GetEventMap();

    for (auto token : tokens)
    {
        if (token.empty())
            continue;

        uint32 eventId = *Acore::StringTo<uint32>(token);
        sGameEventMgr->StartEvent(eventId);

        GameEventData const& eventData = events[eventId];
        LOG_INFO("module", "> EventStarter: Starting event {} ({}).", eventData.Description, eventId);
    }
}
