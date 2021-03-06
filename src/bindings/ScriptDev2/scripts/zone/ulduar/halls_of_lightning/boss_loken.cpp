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
SDName: Boss Loken
SDAuthor: ckegg
SD%Complete: 95%
SDComment: Missing intro. Remove hack of Pulsing Shockwave when core supports. Aura is not working (59414)
SDCategory: Halls of Lightning
EndScriptData */

#include "precompiled.h"
#include "def_halls_of_lightning.h"

enum
{
    SAY_AGGRO                              = -1602015,
    SAY_INTRO_1                            = -1602016,
    SAY_INTRO_2                            = -1602017,
    SAY_SLAY_1                             = -1602018,
    SAY_SLAY_2                             = -1602019,
    SAY_SLAY_3                             = -1602020,
    SAY_DEATH                              = -1602021,
    SAY_NOVA_1                             = -1602022,
    SAY_NOVA_2                             = -1602023,
    SAY_NOVA_3                             = -1602024,
    SAY_75HEALTH                           = -1602025,
    SAY_50HEALTH                           = -1602026,
    SAY_25HEALTH                           = -1602027,

    SPELL_ARC_LIGHTNING                    = 52921,
    SPELL_LIGHTNING_NOVA_N                 = 52960,
    SPELL_LIGHTNING_NOVA_H                 = 59835,

    SPELL_PULSING_SHOCKWAVE_N              = 52961,
    SPELL_PULSING_SHOCKWAVE_H              = 59836,
    SPELL_PULSING_SHOCKWAVE_AURA           = 59414
};

/*######
## Boss Loken
######*/
struct MANGOS_DLL_DECL boss_lokenAI : public ScriptedAI
{
    boss_lokenAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
    	pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    	m_bIsHeroic = pCreature->GetMap()->IsHeroic();
        Reset();
    }

    ScriptedInstance *pInstance;

    bool m_bIsHeroic;
    bool m_bIsAura;

    uint8 m_uiHealthCheck[3];

    uint32 m_uiArcLightning_Timer;
    uint32 m_uiLightningNova_Timer;
    uint32 m_uiPulsingShockwave_Timer;
    uint32 m_uiResumePulsingShockwave_Timer;
    uint32 m_uiHealCheck_Timer;

    void Reset()
    {
    	m_bIsAura = false;

        m_uiArcLightning_Timer = 15000;
        m_uiLightningNova_Timer = 20000;
        m_uiPulsingShockwave_Timer = 2000;
        m_uiResumePulsingShockwave_Timer = 15000;
        m_uiHealCheck_Timer = 1000;

        m_uiHealthCheck[0] = 75;
        m_uiHealthCheck[1] = 50;
        m_uiHealthCheck[2] = 25;

        m_creature->RemoveAllAuras();

        if(pInstance)
            pInstance->SetData(DATA_LOKEN_EVENT, NOT_STARTED);
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (!who || m_creature->getVictim())
            return;

        m_creature->SetInCombatWithZone();

        if(pInstance)
            pInstance->SetData(DATA_LOKEN_EVENT, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff) 
    {
        //Return since we have no target
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if(m_bIsAura)
        {
            // workaround for PULSING_SHOCKWAVE
            if (m_uiPulsingShockwave_Timer < uiDiff)
            {
                Map *map = m_creature->GetMap();
                if (map->IsDungeon())
                {
                    Map::PlayerList const &PlayerList = map->GetPlayers();

                    if (PlayerList.isEmpty())
                        return;

                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if (i->getSource()->isAlive() && i->getSource()->isTargetableForAttack())
                        {
                            int32 dmg;
                            float m_fDist = m_creature->GetDistance(i->getSource());

                            if (m_fDist <= 1.0f) // Less than 1 yard
                                dmg = (m_bIsHeroic ? 850 : 800); // need to correct damage
                            else // Further from 1 yard
                                dmg = ((m_bIsHeroic ? 250 : 200) * m_fDist) + (m_bIsHeroic ? 850 : 800); // need to correct damage

                            m_creature->CastCustomSpell(i->getSource(), (m_bIsHeroic ? 59837 : 52942), &dmg, 0, 0, false);
                        }
                }
                m_uiPulsingShockwave_Timer = 2000;
            }else m_uiPulsingShockwave_Timer -= uiDiff;
        }
        else
        {
            if(m_uiResumePulsingShockwave_Timer < uiDiff)
            {
      	        //DoCast(m_creature, m_bIsHeroic ? SPELL_PULSING_SHOCKWAVE_H : SPELL_PULSING_SHOCKWAVE_N); // need core support
                m_bIsAura = true;
                m_uiResumePulsingShockwave_Timer = 0;
            }else m_uiResumePulsingShockwave_Timer -= uiDiff;
        }

        if(m_uiArcLightning_Timer < uiDiff)
        {
            DoCast(SelectUnit(SELECT_TARGET_RANDOM, 0), SPELL_ARC_LIGHTNING);
            m_uiArcLightning_Timer = 15000 + rand()%1000;
        }else m_uiArcLightning_Timer -= uiDiff;

        if(m_uiLightningNova_Timer < uiDiff)
        {
            switch(rand()%3)
            {
                case 0: DoScriptText(SAY_NOVA_1, m_creature);break;
                case 1: DoScriptText(SAY_NOVA_2, m_creature);break;
                case 2: DoScriptText(SAY_NOVA_3, m_creature);break;
            }
            DoCast(m_creature, m_bIsHeroic ? SPELL_LIGHTNING_NOVA_H : SPELL_LIGHTNING_NOVA_N);
            m_bIsAura = false;
            m_uiResumePulsingShockwave_Timer = (m_bIsHeroic ? 4000 : 5000); // Pause Pulsing Shockwave aura
            m_uiLightningNova_Timer = 20000 + rand()%1000;
        }else m_uiLightningNova_Timer -= uiDiff;

        // Health check -----------------------------------------------------------------------------
        if (m_uiHealCheck_Timer < uiDiff)
        {
            for(uint8 i = 0; i < 3; i++)
            {
                if (m_uiHealthCheck[i] && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) <= m_uiHealthCheck[i])
                {
                    switch(i)
                    {
                        case 0: DoScriptText(SAY_75HEALTH, m_creature);break;
                        case 1: DoScriptText(SAY_50HEALTH, m_creature);break;
                        case 2: DoScriptText(SAY_25HEALTH, m_creature);break;
                    }
                    m_uiHealthCheck[i] = 0; // deactive
                }
            }
            m_uiHealCheck_Timer = 1000;
        } else m_uiHealCheck_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if(pInstance)
            pInstance->SetData(DATA_LOKEN_EVENT, DONE);
    }

    void KilledUnit(Unit *victim)
    {
        if(victim == m_creature)
            return;
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature);break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature);break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature);break;
        }
    }
};

CreatureAI* GetAI_boss_loken(Creature *_Creature)
{
    return new boss_lokenAI (_Creature);
}

void AddSC_boss_loken()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_loken";
    newscript->GetAI = GetAI_boss_loken;
    newscript->RegisterSelf();
}
