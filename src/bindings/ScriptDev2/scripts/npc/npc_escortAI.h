/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_ESCORTAI_H
#define SC_ESCORTAI_H

extern UNORDERED_MAP<uint32, std::vector<PointMovement> > PointMovementMap;

struct Escort_Waypoint
{
    Escort_Waypoint(uint32 _id, float _x, float _y, float _z, uint32 _w)
    {
        id = _id;
        x = _x;
        y = _y;
        z = _z;
        WaitTimeMs = _w;
    }

    uint32 id;
    float x;
    float y;
    float z;
    uint32 WaitTimeMs;
};

struct MANGOS_DLL_DECL npc_escortAI : public ScriptedAI
{
    public:
        explicit npc_escortAI(Creature* pCreature) : ScriptedAI(pCreature),
            IsBeingEscorted(false), IsOnHold(false), PlayerGUID(0), m_uiPlayerCheckTimer(1000), m_uiWPWaitTimer(0),
            m_bIsReturning(false), m_bIsActiveAttacker(true), m_bCanDefendSelf(true), m_bIsRunning(false) {}
        ~npc_escortAI() {}

        // Pure Virtual Functions
        virtual void WaypointReached(uint32) = 0;

        virtual void Aggro(Unit*);

        virtual void Reset() = 0;

        // CreatureAI functions
        bool IsVisible(Unit*) const;

        void AttackStart(Unit*);

        void EnterCombat(Unit*);

        void MoveInLineOfSight(Unit*);

        void JustRespawned();

        void EnterEvadeMode();

        void UpdateAI(const uint32);

        void MovementInform(uint32, uint32);

        // EscortAI functions
        void AddWaypoint(uint32 id, float x, float y, float z, uint32 WaitTimeMs = 0);

        void FillPointMovementListForCreature();

        void Start(bool bIsActiveAttacker = true, bool bCanDefendSelf = true, bool bRun = false, uint64 uiPlayerGUID = 0);

        void SetRun(bool bRun = true);

    // EscortAI variables
    protected:
        uint64 PlayerGUID;
        bool IsBeingEscorted;
        bool IsOnHold;

    private:
        uint32 m_uiWPWaitTimer;
        uint32 m_uiPlayerCheckTimer;
 
        std::list<Escort_Waypoint> WaypointList;
        std::list<Escort_Waypoint>::iterator CurrentWP;

        bool m_bIsActiveAttacker;                           //possible obsolete, and should be determined with db only (civilian)
        bool m_bCanDefendSelf;                              //rarely used, is true in 99%
        bool m_bIsReturning;                                //in use when creature leave combat, and are returning to combat start position
        bool m_bIsRunning;                                  //all creatures are walking by default (has flag MONSTER_MOVE_WALK)
};
#endif
