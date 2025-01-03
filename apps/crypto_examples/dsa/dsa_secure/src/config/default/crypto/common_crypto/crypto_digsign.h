/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    crypto_digsign.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef CRYPTO_DIGSIGN_H
#define CRYPTO_DIGSIGN_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "crypto_common.h"
#include "crypto/common_crypto/crypto_hash.h"

typedef enum
{
    CRYPTO_DIGISIGN_ERROR_ALGONOTSUPPTD = -127,
    CRYPTO_DIGISIGN_ERROR_PRIVKEY = -126,
    CRYPTO_DIGISIGN_ERROR_PRIVKEYLEN = -125,
    CRYPTO_DIGISIGN_ERROR_PUBKEY = -124,
    CRYPTO_DIGISIGN_ERROR_PUBKEYCOMPRESS = -123,
    CRYPTO_DIGISIGN_ERROR_PUBKEYLEN = -122,        
    CRYPTO_DIGISIGN_ERROR_HDLR = -121,
    CRYPTO_DIGISIGN_ERROR_INPUTHASH = -120,
    CRYPTO_DIGISIGN_ERROR_SIGNATURE = -119,
    CRYPTO_DIGISIGN_ERROR_SID = -118,  ////session ID Error
    CRYPTO_DIGISIGN_ERROR_ARG = -117,
    CRYPTO_DIGISIGN_ERROR_CURVE = -116, 
    CRYPTO_DIGISIGN_ERROR_RNG = -115, 
    CRYPTO_DIGISIGN_ERROR_HASHTYPE = -114,        
    CRYPTO_DIGISIGN_ERROR_FAIL = -113,
    CRYPTO_DIGISIGN_SUCCESS = 0,        
}crypto_DigiSign_Status_E;

crypto_DigiSign_Status_E Crypto_DigiSign_Ecdsa_Sign(crypto_HandlerType_E ecdsaHandlerType_en, uint8_t *ptr_inputHash, uint32_t hashLen, uint8_t *ptr_outSig, 
                                                    uint32_t sigLen, uint8_t *ptr_privKey, uint32_t privKeyLen, 
                                                    crypto_EccCurveType_E eccCurveType_En, uint32_t ecdsaSessionId);

crypto_DigiSign_Status_E Crypto_DigiSign_Ecdsa_Verify(crypto_HandlerType_E ecdsaHandlerType_en, uint8_t *ptr_inputHash, uint32_t hashLen, 
                                                        uint8_t *ptr_inputSig, uint32_t sigLen, uint8_t *ptr_pubKey, uint32_t pubKeyLen,
                                                        int8_t *ptr_hashVerifyStat, crypto_EccCurveType_E eccCurveType_En, uint32_t ecdsaSessionId);

crypto_DigiSign_Status_E Crypto_DigiSign_Ecdsa_SignData(crypto_HandlerType_E ecdsaHandlerType_en, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                        uint8_t *ptr_outSig, uint32_t sigLen, uint8_t *ptr_privKey, uint32_t privKeyLen, 
                                                        crypto_Hash_Algo_E hashType_en, crypto_EccCurveType_E eccCurveType_En, uint32_t ecdsaSessionId);

crypto_DigiSign_Status_E Crypto_DigiSign_Ecdsa_VerifyData(crypto_HandlerType_E ecdsaHandlerType_en, uint8_t *ptr_inputData, uint32_t dataLen, 
                                                            uint8_t *ptr_inputSig, uint32_t sigLen, uint8_t *ptr_pubKey, uint32_t pubKeyLen, 
                                                            crypto_Hash_Algo_E hashType_en, int8_t *ptr_hashVerifyStat, crypto_EccCurveType_E eccCurveType_En, 
                                                            uint32_t ecdsaSessionId);

#endif /* CRYPTO_DIGSIGN_H */
