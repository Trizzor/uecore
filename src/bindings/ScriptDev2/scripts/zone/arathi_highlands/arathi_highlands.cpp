/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Arathi Highlands
SD%Complete: 100
SDComment: Quest support: 665
SDCategory: Arathi Highlands
EndScriptData */

/* ContentData
npc_professor_phizzlethorpe
EndContentData */

#include "precompiled.h"
#include "../../npc/npc_escortAI.h"

/*######
## npc_professor_phizzlethorpe
######*/

enum
{
    SAY_PROGRESS_1          = -1000264,
    SAY_PROGRESS_2          = -1000265,
    SAY_PROGRESS_3          = -1000266,
    EMOTE_PROGRESS_4        = -1000267,
    SAY_AGGRO               = -1000268,
    SAY_PROGRESS_5          = -1000269,
    SAY_PROGRESS_6          = -1000270,
    SAY_PROGRESS_7          = -1000271,
    EMOTE_PROGRESS_8        = -1000272,
    SAY_PROGRESS_9          = -1000273,

    QUEST_SUNKEN_TREASURE   = 665,
    ENTRY_VENGEFUL_SURGE    = 2776,
    FACTION_ESCORTEE        = 113,
};

struct MANGOS_DLL_DECL npc_professor_phizzlethorpeAI : public npc_escortAI
{
    npc_professor_phizzlethorpeAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        normFaction = pCreature->getFaction();
        Reset();
    }

    uint32 normFaction;

    void WaypointReached(uint32 i)
    {
        Unit* pUnit = Unit::GetUnit((*m_creature), PlayerGUID);

        if (!pUnit || pUnit->GetTypeId() != TYPEID_PLAYER)
            return;

        switch(i)
        {
            case 4: DoScriptText(SAY_PROGRESS_2, m_creature, pUnit); break;
            case 5: DoScriptText(SAY_PROGRESS_3, m_creature, pUnit); break;
            case 8: DoScriptText(EMOTE_PROGRESS_4, m_creature); break;
            case 9:
                m_creature->SummonCreature(ENTRY_VENGEFUL_SURGE, -2056.41, -2144.01, 20.59, 5.70, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000);
                m_creature->SummonCreature(ENTRY_VENGEFUL_SURGE, -2050.17, -2140.02, 19.54, 5.17, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000);
                break;
            case 10: DoScriptText(SAY_PROGRESS_5, m_creature, pUnit); break;
            case 11: DoScriptText(SAY_PROGRESS_6, m_creature, pUnit); break;
            case 19: DoScriptText(SAY_PROGRESS_7, m_creature, pUnit); break;
            case 20:
                DoScriptText(EMOTE_PROGRESS_8, m_creature);
                DoScriptText(SAY_PROGRESS_9, m_creature, pUnit);
                ((Player*)pUnit)->GroupEventHappens(QUEST_SUNKEN_TREASURE, m_creature);
                break;
        }
    }

    void Reset()
    {
        if (!IsBeingEscorted)
            m_creature->setFaction(normFaction);
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_AGGRO, m_creature, NULL);
    }

    void JustSummoned(Creature *summoned)
    {
        summoned->AI()->AttackStart(m_creature);
    }

    void JustDied(Unit* killer)
    {
        if (IsBeingEscorted)
        {
            if (Unit* pUnit = Unit::GetUnit((*m_creature), PlayerGUID))
            {
                if (pUnit->GetTypeId() == TYPEID_PLAYER)
                    ((Player*)pUnit)->FailQuest(QUEST_SUNKEN_TREASURE);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);
    }
};

bool QuestAccept_npc_professor_phizzlethorpe(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_SUNKEN_TREASURE)
    {
        pCreature->setFaction(FACTION_ESCORTEE);
        DoScriptText(SAY_PROGRESS_1, pCreature, pPlayer);
        ((npc_escortAI*)(pCreature->AI()))->Start(false, true, false, pPlayer->GetGUID());
    }
    return true;
}

CreatureAI* GetAI_npc_professor_phizzlethorpe(Creature* pCreature)
{
    npc_professor_phizzlethorpeAI* phizzleAI = new npc_professor_phizzlethorpeAI(pCreature);

    phizzleAI->FillPointMovementListForCreature();

    return (CreatureAI*)phizzleAI;
}

void AddSC_arathi_highlands()
{
    Script * newscript;

    newscript = new Script;
    newscript->Name = "npc_professor_phizzlethorpe";
    newscript->GetAI = &GetAI_npc_professor_phizzlethorpe;
    newscript->pQuestAccept = &QuestAccept_npc_professor_phizzlethorpe;
    newscript->RegisterSelf();
}
