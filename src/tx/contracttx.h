// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2017-2019 The WaykiChain Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TX_CONTRACT_H
#define TX_CONTRACT_H

#include "tx.h"
#include "entities/contract.h"


/**#################### LuaVM Contract Deploy & Invoke Class Definitions ##############################**/
class CLuaContractDeployTx : public CBaseTx {
public:
    CLuaContract contract;  // contract script content

public:

    CLuaContractDeployTx(): CBaseTx(LCONTRACT_DEPLOY_TX) {}
    ~CLuaContractDeployTx() {}

    IMPLEMENT_SERIALIZE(
        READWRITE(VARINT(this->nVersion));
        nVersion = this->nVersion;
        READWRITE(VARINT(nValidHeight));
        READWRITE(txUid);

        READWRITE(contract);
        READWRITE(VARINT(llFees));
        READWRITE(signature);
    )

    TxID ComputeSignatureHash(bool recalculate = false) const {
        if (recalculate || sigHash.IsNull()) {
            CHashWriter ss(SER_GETHASH, 0);
            ss << VARINT(nVersion) << uint8_t(nTxType) << VARINT(nValidHeight) << txUid << contract
               << VARINT(llFees);
            sigHash = ss.GetHash();
        }

        return sigHash;
    }

    virtual uint256 GetHash() const { return ComputeSignatureHash(); }
    virtual std::shared_ptr<CBaseTx> GetNewInstance() const { return std::make_shared<CLuaContractDeployTx>(*this); }
    virtual uint64_t GetFuel(uint32_t nFuelRate);
    virtual map<TokenSymbol, uint64_t> GetValues() const { return {{SYMB::WICC, 0}}; }
    virtual string ToString(CAccountDBCache &view);
    virtual Object ToJson(const CAccountDBCache &AccountView) const;
    virtual bool GetInvolvedKeyIds(CCacheWrapper &cw, set<CKeyID> &keyIds);

    virtual bool CheckTx(int32_t height, CCacheWrapper &cw, CValidationState &state);
    virtual bool ExecuteTx(int32_t height, int32_t index, CCacheWrapper &cw, CValidationState &state);
};

class CLuaContractInvokeTx : public CBaseTx {
public:
    mutable CUserID app_uid;  // app regid or address
    uint64_t bcoins;          // transfer amount
    string arguments;         // arguments to invoke a contract method

public:
    CLuaContractInvokeTx() : CBaseTx(LCONTRACT_INVOKE_TX) {}

    CLuaContractInvokeTx(const CUserID &txUidIn, CUserID appUidIn, uint64_t feesIn,
                uint64_t bcoinsIn, int32_t validHeightIn, string &argumentsIn):
                CBaseTx(LCONTRACT_INVOKE_TX, txUidIn, validHeightIn, feesIn) {
        if (txUidIn.type() == typeid(CRegID))
            assert(!txUidIn.get<CRegID>().IsEmpty()); //FIXME: shouldnot be using assert here, throw an error instead.

        if (appUidIn.type() == typeid(CRegID))
            assert(!appUidIn.get<CRegID>().IsEmpty());

        app_uid   = appUidIn;
        bcoins    = bcoinsIn;
        arguments = argumentsIn;
    }

    ~CLuaContractInvokeTx() {}

    IMPLEMENT_SERIALIZE(
        READWRITE(VARINT(this->nVersion));
        nVersion = this->nVersion;
        READWRITE(VARINT(nValidHeight));
        READWRITE(txUid);
        READWRITE(app_uid);
        READWRITE(VARINT(llFees));
        READWRITE(VARINT(bcoins));
        READWRITE(arguments);
        READWRITE(signature);
    )

    TxID ComputeSignatureHash(bool recalculate = false) const {
        if (recalculate || sigHash.IsNull()) {
            CHashWriter ss(SER_GETHASH, 0);
            ss << VARINT(nVersion) << uint8_t(nTxType) << VARINT(nValidHeight) << txUid << app_uid
               << VARINT(llFees) << VARINT(bcoins) << arguments;
            sigHash = ss.GetHash();
        }
        return sigHash;
    }

    virtual map<TokenSymbol, uint64_t> GetValues() const { return {{SYMB::WICC, bcoins}}; }
    virtual uint256 GetHash() const { return ComputeSignatureHash(); }
    virtual std::shared_ptr<CBaseTx> GetNewInstance() const { return std::make_shared<CLuaContractInvokeTx>(*this); }
    virtual string ToString(CAccountDBCache &view);
    virtual Object ToJson(const CAccountDBCache &AccountView) const;
    virtual bool GetInvolvedKeyIds(CCacheWrapper &cw, set<CKeyID> &keyIds);

    virtual bool CheckTx(int32_t height, CCacheWrapper &cw, CValidationState &state);
    virtual bool ExecuteTx(int32_t height, int32_t index, CCacheWrapper &cw, CValidationState &state);
};

/**#################### Universal Contract Deploy & Invoke Class Definitions ##############################**/
class CUniversalContractDeployTx : public CBaseTx {
public:
    TokenSymbol         fee_symbol;
    TokenSymbol         coin_symbol;
    uint64_t            coin_amount;
    CUniversalContract  contract;  // contract script content

public:
    CUniversalContractDeployTx(): CBaseTx(LCONTRACT_DEPLOY_TX) {}
    ~CUniversalContractDeployTx() {}

    IMPLEMENT_SERIALIZE(
        READWRITE(VARINT(this->nVersion));
        nVersion = this->nVersion;
        READWRITE(VARINT(nValidHeight));
        READWRITE(txUid);

        READWRITE(fee_symbol);
        READWRITE(VARINT(llFees));
        READWRITE(coin_symbol);
        READWRITE(VARINT(coin_amount));
        READWRITE(contract);

        READWRITE(signature);
    )

    TxID ComputeSignatureHash(bool recalculate = false) const {
        if (recalculate || sigHash.IsNull()) {
            CHashWriter ss(SER_GETHASH, 0);
            ss << VARINT(nVersion) << uint8_t(nTxType) << VARINT(nValidHeight) << txUid << VARINT(llFees) << fee_symbol
               << coin_symbol << VARINT(coin_amount) << contract;
            sigHash = ss.GetHash();
        }

        return sigHash;
    }

    virtual uint256 GetHash() const { return ComputeSignatureHash(); }
    virtual std::shared_ptr<CBaseTx> GetNewInstance() const { return std::make_shared<CUniversalContractDeployTx>(*this); }
    virtual uint64_t GetFuel(uint32_t nFuelRate);
    virtual map<TokenSymbol, uint64_t> GetValues() const { return {{coin_symbol, coin_amount}}; }
    virtual string ToString(CAccountDBCache &view);
    virtual Object ToJson(const CAccountDBCache &AccountView) const;
    virtual bool GetInvolvedKeyIds(CCacheWrapper &cw, set<CKeyID> &keyIds);

    virtual bool CheckTx(int32_t height, CCacheWrapper &cw, CValidationState &state);
    virtual bool ExecuteTx(int32_t height, int32_t index, CCacheWrapper &cw, CValidationState &state);
};

class CUniversalContractInvokeTx : public CBaseTx {
public:
    mutable CUserID app_uid;  // app regid or address
    string arguments;         // arguments to invoke a contract method
    TokenSymbol fee_symbol;
    TokenSymbol coin_symbol;
    uint64_t coin_amount;  // transfer amount to contract account

public:
    CUniversalContractInvokeTx() : CBaseTx(UCONTRACT_INVOKE_TX) {}

    CUniversalContractInvokeTx(const CUserID &txUidIn, int32_t validHeightIn, uint64_t feesIn,
                CUserID appUidIn, string &argumentsIn, TokenSymbol feeSymbol, TokenSymbol coinSymbol,
                uint64_t coinAmount):
                CBaseTx(UCONTRACT_INVOKE_TX, txUidIn, validHeightIn, feesIn) {
        if (txUidIn.type() == typeid(CRegID))
            assert(!txUidIn.get<CRegID>().IsEmpty()); //FIXME: shouldnot be using assert here, throw an error instead.

        if (appUidIn.type() == typeid(CRegID))
            assert(!appUidIn.get<CRegID>().IsEmpty());

        app_uid     = appUidIn;
        arguments   = argumentsIn;
        fee_symbol  = feeSymbol;
        coin_symbol = coinSymbol;
        coin_amount = coinAmount;
    }

    ~CUniversalContractInvokeTx() {}

    IMPLEMENT_SERIALIZE(
        READWRITE(VARINT(this->nVersion));
        nVersion = this->nVersion;
        READWRITE(VARINT(nValidHeight));
        READWRITE(txUid);

        READWRITE(app_uid);
        READWRITE(arguments);
        READWRITE(VARINT(llFees));
        READWRITE(fee_symbol);
        READWRITE(coin_symbol);
        READWRITE(VARINT(coin_amount));

        READWRITE(signature);
    )

    TxID ComputeSignatureHash(bool recalculate = false) const {
        if (recalculate || sigHash.IsNull()) {
            CHashWriter ss(SER_GETHASH, 0);
            ss << VARINT(nVersion) << uint8_t(nTxType) << VARINT(nValidHeight) << txUid << app_uid << arguments
               << VARINT(llFees) << fee_symbol << coin_symbol << VARINT(coin_amount);
            sigHash = ss.GetHash();
        }
        return sigHash;
    }

    virtual map<TokenSymbol, uint64_t> GetValues() const { return {{coin_symbol, coin_amount}}; }
    virtual uint256 GetHash() const { return ComputeSignatureHash(); }
    virtual std::shared_ptr<CBaseTx> GetNewInstance() const { return std::make_shared<CUniversalContractInvokeTx>(*this); }
    virtual string ToString(CAccountDBCache &view);
    virtual Object ToJson(const CAccountDBCache &AccountView) const;
    virtual bool GetInvolvedKeyIds(CCacheWrapper &cw, set<CKeyID> &keyIds);

    virtual bool CheckTx(int32_t height, CCacheWrapper &cw, CValidationState &state);
    virtual bool ExecuteTx(int32_t height, int32_t index, CCacheWrapper &cw, CValidationState &state);
};
#endif //TX_CONTRACT_H