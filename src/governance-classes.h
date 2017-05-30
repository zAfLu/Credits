// Copyright (c) 2009-2017 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Developers
// Copyright (c) 2014-2017 The Dash Core Developers
// Copyright (c) 2017 Credits Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef CREDITS_GOVERNANCE_CLASSES_H
#define CREDITS_GOVERNANCE_CLASSES_H

//#define ENABLE_CREDITS_DEBUG

#include "base58.h"
#include "chainparams.h"
#include "governance.h"
#include "key.h"
#include "script/standard.h"
#include "util.h"

#include <boost/shared_ptr.hpp>

class CGovernanceTrigger;
class CGovernanceTriggerManager;
class CSuperblock;
class CSuperblockManager;

static const int TRIGGER_UNKNOWN            = -1;
static const int TRIGGER_SUPERBLOCK         = 1000;
static const CAmount STATIC_SUPERBLOCK_AMOUNT = 10000 * COIN; //Budget amount fixed at 10000 CRDS

typedef boost::shared_ptr<CSuperblock> CSuperblock_sptr;

// DECLARE GLOBAL VARIABLES FOR GOVERNANCE CLASSES
extern CGovernanceTriggerManager triggerman;

// SPLIT A STRING UP - USED FOR SUPERBLOCK PAYMENTS
std::vector<std::string> SplitBy(std::string strCommand, std::string strDelimit);

/**
*   Trigger Mananger
*
*   - Track governance objects which are triggers
*   - After triggers are activated and executed, they can be removed
*/

class CGovernanceTriggerManager
{
    friend class CSuperblockManager;
    friend class CGovernanceManager;

private:
    typedef std::map<uint256, CSuperblock_sptr> trigger_m_t;
    typedef trigger_m_t::iterator trigger_m_it;
    typedef trigger_m_t::const_iterator trigger_m_cit;

    trigger_m_t mapTrigger;

    std::vector<CSuperblock_sptr> GetActiveTriggers();
    bool AddNewTrigger(uint256 nHash);
    void CleanAndRemove();

public:
    CGovernanceTriggerManager() : mapTrigger() {}
};

/**
*   Superblock Manager
*
*   Class for querying superblock information
*/

class CSuperblockManager
{
private:
    static bool GetBestSuperblock(CSuperblock_sptr& pSuperblockRet, int nBlockHeight);

public:

    static bool IsSuperblockTriggered(int nBlockHeight);

    static void CreateSuperblock(CMutableTransaction& txNewRet, int nBlockHeight, std::vector<CTxOut>& voutSuperblockRet);

    static std::string GetRequiredPaymentsString(int nBlockHeight);
    static bool IsValid(const CTransaction& txNew, int nBlockHeight, CAmount blockReward);
};

/**
*   Governance Object Payment
*
*/

class CGovernancePayment
{
private:
    bool fValid;

public:
    CScript script;
    CAmount nAmount;

    CGovernancePayment()
        :fValid(false),
         script(),
         nAmount(0)
    {}

    CGovernancePayment(CCreditsAddress addrIn, CAmount nAmountIn)
        :fValid(false),
         script(),
         nAmount(0)
    {
        try
        {
            CTxDestination dest = addrIn.Get();
            script = GetScriptForDestination(dest);
            nAmount = nAmountIn;
            fValid = true;
        }
        catch(const std::exception& e)
        {
            LogPrintf("CGovernancePayment Payment not valid: addrIn = %s, nAmountIn = %d, what = %s\n",
                     addrIn.ToString(), nAmountIn, e.what());
        }
        catch(...)
        {
            LogPrintf("CGovernancePayment Payment not valid: addrIn = %s, nAmountIn = %d\n",
                      addrIn.ToString(), nAmountIn);
        }
    }

    bool IsValid() { return fValid; }
};


/**
*   Trigger : Superblock
*
*   - Create payments on the network
*
*   object structure:
*   {
*       "governance_object_id" : last_id,
*       "type" : govtypes.trigger,
*       "subtype" : "superblock",
*       "superblock_name" : superblock_name,
*       "start_epoch" : start_epoch,
*       "payment_addresses" : "addr1|addr2|addr3",
*       "payment_amounts"   : "amount1|amount2|amount3"
*   }
*/

class CSuperblock : public CGovernanceObject
{
private:
    uint256 nGovObjHash;

    int nEpochStart;
    int nStatus;
    std::vector<CGovernancePayment> vecPayments;

    void ParsePaymentSchedule(std::string& strPaymentAddresses, std::string& strPaymentAmounts);

public:

    CSuperblock();
    CSuperblock(uint256& nHash);

    static bool IsValidBlockHeight(int nBlockHeight);
    static CAmount GetPaymentsLimit(int nBlockHeight);

    int GetStatus() { return nStatus; }
    void SetStatus(int nStatusIn) { nStatus = nStatusIn; }

    // IS THIS TRIGGER ALREADY EXECUTED?
    bool IsExecuted() { return nStatus == SEEN_OBJECT_EXECUTED; }
    // TELL THE ENGINE WE EXECUTED THIS EVENT
    void SetExecuted() { nStatus = SEEN_OBJECT_EXECUTED; }

    CGovernanceObject* GetGovernanceObject()
    {
        AssertLockHeld(governance.cs);
        CGovernanceObject* pObj = governance.FindGovernanceObject(nGovObjHash);
        return pObj;
    }

    int CountPayments() { return (int)vecPayments.size(); }
    bool GetPayment(int nPaymentIndex, CGovernancePayment& paymentRet);
    CAmount GetPaymentsTotalAmount();

    bool IsValid(const CTransaction& txNew, int nBlockHeight, CAmount blockReward);
};

#endif // CREDITS_GOVERNANCE_CLASSES_H
